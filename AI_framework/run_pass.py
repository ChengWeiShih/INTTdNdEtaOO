#!/usr/bin/env python3
"""
run_pass.py  –  InttDoubletMap single-pass orchestrator
========================================================
Runs one full analysis pass for the InttDoubletMap pipeline:

  Step 1 : Generate a custom Run_PrepareHist_<pass_name>.C with all
           PassConfig values baked in, generate a custom run_condor.job,
           and submit HTCondor jobs that execute the generated macro.
  Step 2 : Poll for job completion, then call run_random_merge
           (Run_RandomMerge.sh → RandomMerge.C → hadd)
  Step 3 : Run InttDoubletMap.C on the merged file

The original Run_PrepareHist.C is NEVER modified.

Usage
-----
  # Dry-run (print what would happen, don't submit)
  python3 run_pass.py  pass_configs/data_82405_baseline.py  --dry-run

  # Submit condor jobs only (Step 1)
  python3 run_pass.py  pass_configs/data_82405_baseline.py  --step 1

  # Merge only (Step 2) – jobs already done
  python3 run_pass.py  pass_configs/data_82405_baseline.py  --step 2

  # Analysis macro only (Step 3) – merge already done
  python3 run_pass.py  pass_configs/data_82405_baseline.py  --step 3

  # Full pass (Steps 1 → 2 → 3), polling for job completion
  python3 run_pass.py  pass_configs/data_82405_baseline.py  --step all

The config file must define a variable named `cfg` of type PassConfig.
"""

import argparse
import glob
import importlib.util
import os
import re
import subprocess
import sys
import textwrap
import time
from pathlib import Path

# ── Make sure PassConfig is importable regardless of CWD ─────────────────────
FRAMEWORK_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(FRAMEWORK_DIR))
from PassConfig import PassConfig  # noqa: E402


# ═══════════════════════════════════════════════════════════════════════════════
# Helpers
# ═══════════════════════════════════════════════════════════════════════════════

def log(msg: str, level: str = "INFO") -> None:
    tag = {"INFO": "\033[32m[INFO]\033[0m",
           "WARN": "\033[33m[WARN]\033[0m",
           "ERR":  "\033[31m[ERR ]\033[0m",
           "STEP": "\033[36m[STEP]\033[0m"}.get(level, "[LOG ]")
    print(f"{tag} {msg}", flush=True)


def _coerce_value(s: str):
    """Auto-coerce a CLI string to int, float, bool, or leave as str."""
    if s.lower() == "true":  return True
    if s.lower() == "false": return False
    try: return int(s)
    except ValueError: pass
    try: return float(s)
    except ValueError: pass
    return s


def run(cmd: str, dry: bool = False, cwd: str = None) -> int:
    """Print and optionally execute a shell command. Returns exit code."""
    log(f"  $ {cmd}")
    if dry:
        return 0
    result = subprocess.run(cmd, shell=True, cwd=cwd)
    return result.returncode

def load_config(config_path: str, pre_inject: dict | None = None) -> PassConfig:
    """
    Load a PassConfig from a Python file that defines a variable ``cfg``.

    pre_inject : optional dict of ``{var_name: value}`` injected by ``--set``.
                 Each variable's assignment line in the config file is replaced
                 with the override value, so the ``cfg = PassConfig(...)`` call
                 sees the new value.  Config files must therefore define each
                 overridable variable as a plain top-level assignment
                 (e.g. ``_centrality_bin = 1``).
    """
    import re as _re

    config_path = os.path.abspath(config_path)
    with open(config_path) as fh:
        source = fh.read()

    if pre_inject:
        for k, v in pre_inject.items():
            log(f"[--set] {k} = {v!r}")
            # Replace the first occurrence of "VAR = <anything>" with the
            # override value.  This handles both "VAR = 1" and "VAR = expr".
            pattern = rf"^({_re.escape(k)}\s*=\s*)(.+)$"
            replacement = rf"\g<1>{v!r}"
            new_source, n = _re.subn(pattern, replacement, source,
                                     count=1, flags=_re.MULTILINE)
            if n == 0:
                log(f"[--set] WARNING: '{k}' not found in {config_path}; "
                    f"injecting as new variable", "WARN")
                source = f"{k} = {v!r}\n" + source
            else:
                source = new_source

    code = compile(source, config_path, "exec")
    ns: dict = {"__file__": config_path, "__name__": "pass_config"}
    exec(code, ns)  # noqa: S102

    cfg_obj = ns.get("cfg")
    if cfg_obj is None:
        raise AttributeError(
            f"Config file '{config_path}' must define a variable named 'cfg'."
        )
    if not isinstance(cfg_obj, PassConfig):
        raise TypeError(
            f"'cfg' in '{config_path}' must be a PassConfig instance."
        )
    return cfg_obj




# ── C++ literal helpers ───────────────────────────────────────────────────────

def _cpp_bool(v: bool) -> str:
    return "true" if v else "false"

def _cpp_pair_double(v) -> str:
    return f"{{{v[0]}, {v[1]}}}"

def _cpp_pair_int(v) -> str:
    return f"{{{v[0]}, {v[1]}}}"

def _cpp_pair_bool_double(v) -> str:
    """(bool, (double, double))  →  {true/false, {a, b}}"""
    return f"{{{_cpp_bool(v[0])}, {{{v[1][0]}, {v[1][1]}}}}}"

def _cpp_pair_bool_int(v) -> str:
    """(bool, (int, int))  →  {true/false, {a, b}}"""
    return f"{{{_cpp_bool(v[0])}, {{{v[1][0]}, {v[1][1]}}}}}"


# ═══════════════════════════════════════════════════════════════════════════════
# Generate a per-pass Run_PrepareHist_<pass_name>.C
# ═══════════════════════════════════════════════════════════════════════════════

def generate_run_prepare_hist(cfg: PassConfig, out_dir: str) -> str:
    """
    Write a fully self-contained Run_PrepareHist_<pass_name>.C into out_dir,
    modelled exactly on Run_PrepareHist_template.C.

    The generated macro:
      - Has an int return type (returns 888 on success, 666 if vtxZ reweight
        file is missing)
      - Loads the vtxZ reweight histogram via GetTH1D() if enabled
      - Uses output_directory directly (no /baseline sub-dir appended)
      - Accepts only the 6 per-job arguments from run_job.sh; all
        InttDoubletMap parameters are baked in as C++ literals.

    The original Run_PrepareHist.C and Run_PrepareHist_template.C are NEVER
    modified.

    Returns the absolute path to the generated file.
    """

    macro_name = f"Run_PrepareHist_{cfg.pass_name}"
    out_path   = os.path.join(out_dir, f"{macro_name}.C")

    # Absolute paths to the shared header and shared library
    intt_dir    = os.path.dirname(cfg.macro_dir)   # …/InttDoubletMap
    header_path = os.path.join(intt_dir, "InttDoubletMap.h")
    so_path     = os.path.join(intt_dir, "libInttDoubletMap.so")

    # ── vtxZReweight block ── matches template lines 63-86 ───────────────────
    # vtxZReweight_in is now a plain bool in the template;
    # the macro builds pair<int,TH1D*> vtxZReweight_final internally.
    vz_enable = cfg.vtxZReweight   # plain bool

    vtx_decl = textwrap.dedent(f"""\
        std::string zvtx_weight_dir  = "{cfg.zvtx_weight_dir}";
        std::string zvtx_weight_file = "{cfg.zvtx_weight_file}";
        std::string zvtx_weight_hist = "{cfg.zvtx_weight_hist}";

        // note : plain bool flag – macro constructs the pair internally
        bool vtxZReweight_in = {_cpp_bool(vz_enable)};

        std::pair<int, TH1D*> vtxZReweight_final = {{vtxZReweight_in, nullptr}};
    """)

    vtx_load = textwrap.dedent("""\
        if (vtxZReweight_final.first) {
          vtxZReweight_final.second = GetTH1D(
            zvtx_weight_dir, zvtx_weight_file, zvtx_weight_hist
          );
        }

        if (vtxZReweight_final.first && vtxZReweight_final.second == nullptr) {
          std::cout << "no vtxZReweight map, kill the job" << std::endl;
          return 666;
        }
    """)

    # ── ColMulMask block ──────────────────────────────────────────────────────
    colmul_decl = textwrap.dedent(f"""\
        std::string ColMulMask_map_mother_dir = "{cfg.ColMulMask_map_dir}";
        std::string ColMulMask_map_file       = "{cfg.ColMulMask_map_file}";
    """)

    colmul_set = textwrap.dedent("""\
        if (ColMulMask_in) {
          TLHN->SetGoodColMap(
            GetGoodColMap(ColMulMask_map_mother_dir, ColMulMask_map_file,
                          TLHN->GetGoodColMapName())
          );
        }
    """) if cfg.ColMulMask else "// note : ColMulMask disabled for this pass\n"

    # ── Default filename for the function signature (replace placeholder) ─────
    default_filename = cfg.input_file_name.replace("{process:05d}", "00000")

    # ── Build the C++ source ──────────────────────────────────────────────────
    content = textwrap.dedent(f"""\
        // =====================================================================
        // AUTO-GENERATED by run_pass.py  –  DO NOT EDIT BY HAND
        // Template : Run_PrepareHist_template.C
        // Pass     : {cfg.pass_name}
        // Notes    : {cfg.notes or "(none)"}
        //
        // All InttDoubletMap parameters are baked in as C++ literals.
        // To change any parameter, edit the PassConfig and re-run:
        //   python3 run_pass.py <config.py> --step 1
        // =====================================================================

        #include "{header_path}"

        R__LOAD_LIBRARY({so_path})

        TH2D * GetGoodColMap(std::string ColMulMask_map_dir_in,
                             std::string ColMulMask_map_file_in,
                             std::string map_name_in)
        {{
          TFile * f = TFile::Open(Form("%s/%s",
            ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
          TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
          return h;
        }}

        TH1D * GetTH1D(std::string h1D_input_directory,
                       std::string h1D_filename,
                       std::string h1D_name)
        {{
          TFile * f1 = TFile::Open(Form("%s/%s",
            h1D_input_directory.c_str(), h1D_filename.c_str()));
          TH1D * h1 = (TH1D*)f1->Get(h1D_name.c_str());
          return h1;
        }}

        // note : function name must match the filename (ROOT requirement)
        int {macro_name}(
          int    process_id       = 0,
          int    run_num          = {cfg.run_number},
          int    nevents          = {cfg.n_events},
          string input_directory  = "{cfg.input_directory}",
          string input_filename   = "{default_filename}",
          string output_directory = "{cfg.output_directory}"
        )
        {{

          // ── vtxZ reweight paths + flag ───────────────────────────────────
          {textwrap.indent(vtx_decl.strip(), '  ')}

          // ── vtxZ reweight load + guard ───────────────────────────────────
          {textwrap.indent(vtx_load.strip(), '  ')}

          // ── ColMulMask map paths ─────────────────────────────────────────
          {textwrap.indent(colmul_decl.strip(), '  ')}

          // ── fixed parameters from PassConfig ────────────────────────────
          // todo : modify here via PassConfig – do not edit this file directly
          std::string output_file_name_suffix = "{cfg.output_file_name_suffix}";
          std::pair<double, double> vertexXYIncm = {_cpp_pair_double(cfg.vertexXYIncm)};

          int  data_type_in     = {cfg.data_type};  // 0=pure_trig, 1=stream_trig, 2=stream_data
          bool isUsedMBDz_in    = {_cpp_bool(cfg.isUsedMBDz)};

          bool BcoFullDiffCut_in = {_cpp_bool(cfg.BcoFullDiffCut)};
          int  CentralityBin_in  = {cfg.CentralityBin};
          bool isMinBiasCut_in   = {_cpp_bool(cfg.isMinBiasCut)};
          bool isTriggerSel_in   = {_cpp_bool(cfg.isTriggerSel)};
          std::pair<bool, std::pair<int,int>> isMBDChargeCut_in   = {_cpp_pair_bool_int(cfg.isMBDChargeCut)};
          std::pair<bool, std::pair<int,int>> isBunchNumber_cut_in = {_cpp_pair_bool_int(cfg.isBunchNumber_cut)};

          bool INTT_vtxZ_QA_in  = {_cpp_bool(cfg.INTT_vtxZ_QA)};
          std::pair<double, double> VtxZRange_in = {_cpp_pair_double(cfg.VtxZRange)};

          bool ColMulMask_in    = {_cpp_bool(cfg.ColMulMask)};
          std::pair<bool, std::pair<double,double>> isClusQA_in = {_cpp_pair_bool_double(cfg.isClusQA)};  // note : {{adc, phi size}}
          double DeltaPhiCut_in = {cfg.DeltaPhiCut};

          bool HaveGeoOffsetTag_in = {_cpp_bool(cfg.HaveGeoOffsetTag)};

          // ── output directory ─────────────────────────────────────────────
          std::string final_output_directory = output_directory;
          system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

          // ── construct InttDoubletMap ─────────────────────────────────────
          InttDoubletMap * TLHN = new InttDoubletMap(
            process_id,
            run_num,
            nevents,
            input_directory,
            input_filename,
            final_output_directory,

            output_file_name_suffix,
            vertexXYIncm,

            data_type_in,
            isUsedMBDz_in,

            BcoFullDiffCut_in,
            CentralityBin_in,
            isMinBiasCut_in,
            isTriggerSel_in,
            isMBDChargeCut_in,
            isBunchNumber_cut_in,

            vtxZReweight_final,
            INTT_vtxZ_QA_in,
            VtxZRange_in,

            ColMulMask_in,
            isClusQA_in,
            DeltaPhiCut_in,

            HaveGeoOffsetTag_in
          );

          {textwrap.indent(colmul_set.strip(), '  ')}

          string final_output_file_name = TLHN->GetOutputFileName();
          cout << "final_output_file_name: " << final_output_file_name << endl;
          system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
            final_output_directory.c_str(), final_output_file_name.c_str(),
            final_output_directory.c_str(), final_output_file_name.c_str()));

          TLHN->MainProcess();
          TLHN->EndRun();

          system(Form("mv %s/%s %s/completed",
            final_output_directory.c_str(), final_output_file_name.c_str(),
            final_output_directory.c_str()));

          return 888;
        }}
    """)

    Path(out_dir).mkdir(parents=True, exist_ok=True)
    with open(out_path, "w") as f:
        f.write(content)

    log(f"Generated macro : {out_path}")
    return out_path


# ═══════════════════════════════════════════════════════════════════════════════
# Step 1 – Generate macro + condor job file, then submit
# ═══════════════════════════════════════════════════════════════════════════════

def generate_condor_job(cfg: PassConfig, job_file_path: str,
                        generated_macro_path: str) -> str:
    """Write a customised run_condor.job that calls the generated macro."""

    macro_name = f"Run_PrepareHist_{cfg.pass_name}"
    macro_dir  = os.path.dirname(generated_macro_path)

    # Translate Python {process:05d} placeholder → Condor macro
    condor_filename = re.sub(
        r"\{process:05d\}",
        "$INT(process_shift,%05d)",
        cfg.input_file_name
    )

    # run_job.sh calls:
    #   root.exe -q -b ${macro_name}.C(process,runnumber,nEvents,"input_dir","input_file","output_dir")
    # It does NOT add the .C extension itself, so macro_name must be the bare stem.

    content = textwrap.dedent(f"""\
        Universe           = vanilla
        Initialdir         = {macro_dir}
        Executable         = {cfg.macro_dir}/run_job.sh
        PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)
        concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{cfg.condor_concurrency_limit}
        request_memory     = {cfg.condor_request_memory}
        Priority           = {cfg.condor_priority}
        job_lease_duration = 86400

        runnumber          = {cfg.run_number}
        nEvents            = {cfg.n_events}
        input_directory    = {cfg.input_directory}
        job_offset         = {cfg.job_offset}
        process_shift      = $(Process) + $(job_offset)
        input_filename     = {condor_filename}
        output_directory   = {cfg.output_directory}
        macro_name         = {macro_name}

        Output             = $(output_directory)/cw_log/condor_$INT(process_shift,%05d).out
        Error              = $(output_directory)/cw_log/condor_$INT(process_shift,%05d).err
        Log                = $(output_directory)/cw_log/condor_$INT(process_shift,%05d).log
        Arguments          = "$INT(process_shift) $(runnumber) $(nEvents) $(output_directory) $(input_directory) $(input_filename) $(macro_name)"
        Queue {cfg.n_condor_jobs}
    """)

    Path(job_file_path).parent.mkdir(parents=True, exist_ok=True)
    with open(job_file_path, "w") as f:
        f.write(content)
    log(f"Wrote condor job  : {job_file_path}")
    return job_file_path


def step1_submit(cfg: PassConfig, dry: bool = False) -> None:
    log("── Step 1: Generate macro + submit HTCondor jobs ────────────", "STEP")

    # Create output subdirectories
    completed_dir = cfg.completed_dir()
    log_dir       = cfg.condor_log_dir()
    for d in [completed_dir, log_dir]:
        if not dry:
            Path(d).mkdir(parents=True, exist_ok=True)
            log(f"Ensured directory : {d}")
        else:
            log(f"[dry] Would create: {d}")

    # The generated macro lives inside the output directory so each pass
    # keeps its own complete, reproducible copy of the macro it ran.
    generated_dir  = os.path.join(cfg.output_directory, "generated_macro")
    macro_name     = f"Run_PrepareHist_{cfg.pass_name}"
    macro_out_path = os.path.join(generated_dir, f"{macro_name}.C")

    if not dry:
        generate_run_prepare_hist(cfg, generated_dir)
    else:
        log(f"[dry] Would generate: {macro_out_path}")

    # Write the custom condor job file
    job_file = os.path.join(cfg.output_directory, f"run_condor_{cfg.pass_name}.job")
    if not dry:
        generate_condor_job(cfg, job_file, macro_out_path)
    else:
        log(f"[dry] Would write condor job: {job_file}")

    # Submit
    rc = run(f"condor_submit {job_file}", dry=dry)
    if rc != 0:
        log(f"condor_submit failed (exit {rc})", "ERR")
        sys.exit(rc)
    log("Jobs submitted successfully.")


# ═══════════════════════════════════════════════════════════════════════════════
# Step 2 – Poll + merge
# ═══════════════════════════════════════════════════════════════════════════════

def count_completed(cfg: PassConfig) -> int:
    """Count root files in the completed/ directory (excluding merged files)."""
    pattern = os.path.join(cfg.completed_dir(), "*.root")
    return len([f for f in glob.glob(pattern)
                if "_merged" not in os.path.basename(f)])


def poll_for_completion(cfg: PassConfig,
                        min_fraction: float = 0.95,
                        poll_interval_s: int = 120) -> None:
    """Block until ≥ min_fraction of n_condor_jobs files appear in completed/."""
    target = int(cfg.n_condor_jobs * min_fraction)
    log(f"Waiting for ≥ {target}/{cfg.n_condor_jobs} files "
        f"(≥ {min_fraction*100:.0f}%) in {cfg.completed_dir()}")
    while True:
        n = count_completed(cfg)
        log(f"  {n}/{cfg.n_condor_jobs} files present in completed/")
        if n >= target:
            log("Completion threshold reached – proceeding to merge.")
            break
        log(f"  Sleeping {poll_interval_s}s …")
        time.sleep(poll_interval_s)


def detect_merge_filename(cfg: PassConfig) -> str:
    """Auto-detect the first *.root file in completed/ to pass to run_random_merge."""
    pattern = os.path.join(cfg.completed_dir(), "*.root")
    files = sorted(f for f in glob.glob(pattern)
                   if "_merged" not in os.path.basename(f))
    if not files:
        raise FileNotFoundError(
            f"No *.root files found in {cfg.completed_dir()} – "
            "cannot auto-detect merge filename."
        )
    chosen = os.path.basename(files[0])
    log(f"Auto-detected merge filename: {chosen}")
    return chosen


def step2_merge(cfg: PassConfig, dry: bool = False,
                no_poll: bool = False,
                min_fraction: float = 0.95,
                poll_interval_s: int = 120) -> str:
    """Run run_random_merge. Returns path to the merged root file."""
    log("── Step 2: Merge with run_random_merge ──────────────────────", "STEP")

    if not no_poll:
        poll_for_completion(cfg, min_fraction, poll_interval_s)

    if cfg.merge_filename_template:
        merge_input_file = cfg.merge_filename_template
    else:
        if dry:
            log("[dry] Would auto-detect merge filename; using placeholder.")
            merge_input_file = "PLACEHOLDER_00000.root"
        else:
            merge_input_file = detect_merge_filename(cfg)

    completed  = cfg.completed_dir()
    is_shuffle = int(cfg.merge_is_shuffle)
    n_merged   = cfg.merge_n_files
    mega_merge = int(cfg.merge_mega_merge)

    # run_random_merge is a bash function defined in ~/.bash_profile
    cmd = (
        f"source ~/.bash_profile && "
        f"run_random_merge {is_shuffle} {n_merged} {completed} "
        f"{merge_input_file} {mega_merge}"
    )
    rc = run(cmd, dry=dry)
    if rc != 0:
        log(f"run_random_merge failed (exit {rc})", "ERR")
        sys.exit(rc)

    if not dry:
        base   = os.path.splitext(merge_input_file)[0]
        prefix = base.rsplit("_", 1)[0]
        if n_merged == 1:
            merged_file = os.path.join(completed, f"{prefix}_merged.root")
        else:
            merged_file = os.path.join(completed,
                f"{prefix}_merged.root" if mega_merge else f"{prefix}_merged_001.root")

        if not os.path.isfile(merged_file):
            log(f"Expected merged file not found: {merged_file}", "WARN")
            candidates = glob.glob(os.path.join(completed, f"{prefix}_merged*.root"))
            if candidates:
                merged_file = sorted(candidates)[0]
                log(f"Using: {merged_file}")
            else:
                log("No merged file found!", "ERR")
                sys.exit(1)

        log(f"Merged file: {merged_file}")
        return merged_file
    else:
        return os.path.join(cfg.completed_dir(), "PLACEHOLDER_merged.root")


# ═══════════════════════════════════════════════════════════════════════════════
# Step 3 – Run InttDoubletMap.C
# ═══════════════════════════════════════════════════════════════════════════════

def step3_analysis(cfg: PassConfig, merged_file: str, dry: bool = False) -> None:
    log("── Step 3: Run InttDoubletMap.C ─────────────────────────────", "STEP")

    macro = cfg.analysis_macro
    cmd = (
        f"source /opt/sphenix/core/bin/sphenix_setup.sh -n {cfg.sphenix_build} && "
        f"root.exe -l -b -q '{macro}(\"{merged_file}\")'"
    )
    rc = run(cmd, dry=dry)
    if rc != 0:
        log(f"InttDoubletMap.C failed (exit {rc})", "ERR")
        sys.exit(rc)

    out_dir  = os.path.dirname(merged_file)
    out_base = "Output_" + os.path.splitext(os.path.basename(merged_file))[0] + ".root"
    log(f"Analysis output: {os.path.join(out_dir, out_base)}")


# ═══════════════════════════════════════════════════════════════════════════════
# Main
# ═══════════════════════════════════════════════════════════════════════════════

def main() -> None:
    parser = argparse.ArgumentParser(
        description="InttDoubletMap single-pass orchestrator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(__doc__),
    )
    parser.add_argument("config",
                        help="Path to a pass config .py file (must define 'cfg').")
    parser.add_argument("--step", choices=["1", "2", "3", "all"],
                        default="all",
                        help="Which step(s) to run (default: all).")
    parser.add_argument("--dry-run", action="store_true",
                        help="Print commands without executing anything.")
    parser.add_argument("--merged-file",
                        help="Explicit path to merged file for Step 3.")
    parser.add_argument("--no-poll", action="store_true",
                        help="Skip polling for condor completion in Step 2.")
    parser.add_argument("--min-fraction", type=float, default=0.95,
                        help="Fraction of jobs needed before merging (default: 0.95).")
    parser.add_argument("--poll-interval", type=int, default=120,
                        help="Seconds between completion polls (default: 120).")
    parser.add_argument(
        "--set", metavar="VAR=VALUE", action="append", default=[],
        help=(
            "Override a config-file variable before loading "
            "(e.g. --set _centrality_bin=5). "
            "Values are auto-coerced to int/float/bool/str. "
            "Can be repeated: --set A=1 --set B=2."
        ),
    )
    args = parser.parse_args()

    # ── parse --set overrides ─────────────────────────────────────────────────────
    pre_inject: dict = {}
    for item in args.set:
        if "=" not in item:
            log(f"--set '{item}' ignored: expected VAR=VALUE format", "WARN")
            continue
        var, _, raw = item.partition("=")
        pre_inject[var.strip()] = _coerce_value(raw.strip())

    dry = args.dry_run
    if dry:
        log("DRY-RUN mode – no commands will be executed.", "WARN")

    cfg = load_config(args.config, pre_inject=pre_inject or None)
    print()
    print(cfg.summary())
    print()

    steps       = ["1", "2", "3"] if args.step == "all" else [args.step]
    merged_file = args.merged_file

    for step in steps:
        if step == "1":
            step1_submit(cfg, dry=dry)

        elif step == "2":
            merged_file = step2_merge(
                cfg, dry=dry,
                no_poll=args.no_poll,
                min_fraction=args.min_fraction,
                poll_interval_s=args.poll_interval,
            )

        elif step == "3":
            if merged_file is None:
                if not dry:
                    completed  = cfg.completed_dir()
                    candidates = sorted(
                        glob.glob(os.path.join(completed, "*_merged*.root"))
                    )
                    if not candidates:
                        log("No merged file found. Run Step 2 first, or pass "
                            "--merged-file.", "ERR")
                        sys.exit(1)
                    merged_file = candidates[0]
                    log(f"Auto-detected merged file: {merged_file}")
                else:
                    merged_file = os.path.join(cfg.completed_dir(),
                                               "PLACEHOLDER_merged.root")
            step3_analysis(cfg, merged_file, dry=dry)

    log("Pass complete.", "STEP")


if __name__ == "__main__":
    main()
