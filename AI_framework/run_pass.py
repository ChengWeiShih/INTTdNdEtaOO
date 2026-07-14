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
from typing import Optional

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

def load_config(config_path: str, pre_inject: Optional[dict] = None) -> PassConfig:
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
# Step 1 helpers – header generation + template copy
# ═══════════════════════════════════════════════════════════════════════════════

# Placeholder string that must appear in Run_PrepareHist_template.C:
#   #include "header_to_be_replace"
_HEADER_PLACEHOLDER = "header_to_be_replace"


def generate_params_header(cfg: PassConfig, out_dir: str) -> str:
    """
    Write a C++ namespace header file into out_dir that contains every
    InttDoubletMap parameter from PassConfig as a compile-time constant.

    The generated header is #included by the copied Run_PrepareHist_<pass>.C
    and exposes a namespace ``PassParams`` that the template macro reads.

    Returns the absolute path to the generated header file.
    """
    macro_name  = f"Run_PrepareHist_{cfg.pass_name}"
    header_name = f"{macro_name}_params.h"
    header_path = os.path.join(out_dir, header_name)

    default_filename = cfg.input_file_name.replace("{process:05d}", "00000")

    lines = [
        "// ======================================================================",
        "// AUTO-GENERATED by run_pass.py  –  DO NOT EDIT BY HAND",
        f"// Pass  : {cfg.pass_name}",
        f"// Notes : {cfg.notes or '(none)'}",
        "//",
        "// Re-generate by re-running Step 1:",
        "//   python3 run_pass.py <config.py> --step 1",
        "// ======================================================================",
        "",
        "#pragma once",
        "#include <string>",
        "#include <utility>",
        "",
        "namespace PassParams {",
        "",
        "  // ── per-job arguments (defaults used when running interactively) ──",
        f'  const int         default_process_id      = 0;',
        f'  const int         default_run_num         = {cfg.run_number};',
        f'  const int         default_nevents         = {cfg.n_events};',
        f'  const std::string default_input_directory = "{cfg.input_directory}";',
        f'  const std::string default_input_filename  = "{default_filename}";',
        f'  const std::string default_output_directory= "{cfg.output_directory}";',
        "",
        "  // ── TrigEffiWeight ──────────────────────────────────────────────────",
        f'  const bool        IsTrigEffiWeight        = {_cpp_bool(cfg.IsTrigEffiWeight)};',
        f'  const std::string TrigEffiWeight_dir        = "{cfg.TrigEffiWeight_dir}";',
        f'  const std::string TrigEffiWeight_file       = "{cfg.TrigEffiWeight_file}";',
        f'  const std::string TrigEffiWeight_hist       = "{cfg.TrigEffiWeight_hist}";',
        "",
        "  // ── vtxZ reweight ──────────────────────────────────────────────────",
        f'  const bool        vtxZReweight            = {_cpp_bool(cfg.vtxZReweight)};',
        f'  const std::string zvtx_weight_dir         = "{cfg.zvtx_weight_dir}";',
        f'  const std::string zvtx_weight_file        = "{cfg.zvtx_weight_file}";',
        f'  const std::string zvtx_weight_hist        = "{cfg.zvtx_weight_hist}";',
        "",
        "  // ── ColMulMask map ────────────────────────────────────────────────",
        f'  const std::string ColMulMask_map_dir      = "{cfg.ColMulMask_map_dir}";',
        f'  const std::string ColMulMask_map_file     = "{cfg.ColMulMask_map_file}";',
        "",
        "  // ── InttDoubletMap constructor parameters ───────────────────────────",
        f'  const std::string output_file_name_suffix = "{cfg.output_file_name_suffix}";',
        f'  const std::pair<double,double> vertexXYIncm       = {_cpp_pair_double(cfg.vertexXYIncm)};',
        f'  const int         data_type               = {cfg.data_type};  // 0=pure_trig 1=stream_trig 2=stream_data',
        f'  const bool        isUsedMBDz              = {_cpp_bool(cfg.isUsedMBDz)};',
        f'  const bool        BcoFullDiffCut          = {_cpp_bool(cfg.BcoFullDiffCut)};',
        f'  const int         CentralityBin           = {cfg.CentralityBin};',
        f'  const bool        isMinBiasCut            = {_cpp_bool(cfg.isMinBiasCut)};',
        f'  const bool        isTriggerSel            = {_cpp_bool(cfg.isTriggerSel)};',
        f'  const std::pair<bool,std::pair<int,int>>    isMBDChargeCut    = {_cpp_pair_bool_int(cfg.isMBDChargeCut)};',
        f'  const std::pair<bool,std::pair<int,int>>    isBunchNumber_cut = {_cpp_pair_bool_int(cfg.isBunchNumber_cut)};',
        f'  const bool        INTT_vtxZ_QA            = {_cpp_bool(cfg.INTT_vtxZ_QA)};',
        f'  const std::pair<double,double> VtxZRange          = {_cpp_pair_double(cfg.VtxZRange)};',
        f'  const bool        ColMulMask              = {_cpp_bool(cfg.ColMulMask)};',
        f'  const std::pair<bool,std::pair<double,double>> isClusQA = {_cpp_pair_bool_double(cfg.isClusQA)};  // (adc, phi_size)',
        f'  const double      DeltaPhiCut             = {cfg.DeltaPhiCut};',
        f'  const bool        HaveGeoOffsetTag        = {_cpp_bool(cfg.HaveGeoOffsetTag)};',
        "",
        "}  // namespace PassParams",
        "",
    ]

    Path(out_dir).mkdir(parents=True, exist_ok=True)
    with open(header_path, "w") as fh:
        fh.write("\n".join(lines))

    log(f"Generated header  : {header_path}")
    return header_path


def copy_and_patch_template(cfg: PassConfig, header_path: str,
                            out_dir: str) -> str:
    """
    Copy Run_PrepareHist_template.C from cfg.macro_dir into out_dir,
    rename it to Run_PrepareHist_<pass_name>.C, and replace the line

        #include "header_to_be_replace"

    with the absolute path to the generated header.

    Returns the absolute path to the patched macro file.
    """
    import shutil

    macro_name   = f"Run_PrepareHist_{cfg.pass_name}"
    template_src = os.path.join(cfg.macro_dir, "Run_PrepareHist_template.C")
    macro_dst    = os.path.join(out_dir, f"{macro_name}.C")

    if not os.path.isfile(template_src):
        raise FileNotFoundError(
            f"Template not found: {template_src}\n"
            f"Set cfg.macro_dir to the directory containing "
            f"Run_PrepareHist_template.C"
        )

    Path(out_dir).mkdir(parents=True, exist_ok=True)
    shutil.copy2(template_src, macro_dst)
    log(f"Copied template   : {template_src}")
    log(f"             → {macro_dst}")

    # Patch: replace  #include "header_to_be_replace"
    #   with          #include "/abs/path/to/<pass>_params.h"
    abs_header = os.path.abspath(header_path)
    # Use Python for the replacement (avoids sed quoting issues with paths)
    with open(macro_dst) as fh:
        src = fh.read()

    old = f'#include "{_HEADER_PLACEHOLDER}"'
    new = f'#include "{abs_header}"'
    if old not in src:
        log(
            f"WARNING: placeholder '{old}' not found in template. "
            f"Add the line  {old}  to {template_src}",
            "WARN",
        )
    else:
        src = src.replace(old, new, 1)
        
        # Patch the function name to match the file name
        old_func = "int Run_PrepareHist_template("
        new_func = f"int {macro_name}("
        if old_func in src:
            src = src.replace(old_func, new_func, 1)
            log(f"Patched func name : 'Run_PrepareHist_template' → '{macro_name}'")
        else:
            log(f"WARNING: function signature '{old_func}' not found in template.", "WARN")
            
        with open(macro_dst, "w") as fh:
            fh.write(src)
        log(f"Patched include   : '{_HEADER_PLACEHOLDER}' → {abs_header}")

    return macro_dst





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
    header_path    = os.path.join(generated_dir, f"{macro_name}_params.h")

    if not dry:
        hdr = generate_params_header(cfg, generated_dir)
        copy_and_patch_template(cfg, hdr, generated_dir)
    else:
        log(f"[dry] Would generate header : {header_path}")
        log(f"[dry] Would copy+patch template → {macro_out_path}")

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

def copy_and_patch_analysis_template(cfg: PassConfig, header_path: str,
                                     out_dir: str) -> str:
    """
    Copy InttDoubletMap_template.C from cfg.analysis_macro_dir into out_dir,
    rename it to InttDoubletMap_<pass_name>.C, and apply two patches:

      1. Replace  #include "header_to_be_replace"
         with     #include "/abs/path/to/<pass>_params.h"

      2. Replace  void InttDoubletMap_template(
         with     void InttDoubletMap_<pass_name>(

    Returns the absolute path to the patched macro file.
    """
    import shutil

    analysis_name = f"InttDoubletMap_{cfg.pass_name}"
    template_src  = os.path.join(cfg.analysis_macro_dir,
                                 "InttDoubletMap_template.C")
    macro_dst     = os.path.join(out_dir, f"{analysis_name}.C")

    if not os.path.isfile(template_src):
        raise FileNotFoundError(
            f"Analysis template not found: {template_src}\n"
            f"Set cfg.analysis_macro_dir to the directory containing "
            f"InttDoubletMap_template.C"
        )

    Path(out_dir).mkdir(parents=True, exist_ok=True)
    shutil.copy2(template_src, macro_dst)
    log(f"Copied analysis template : {template_src}")
    log(f"                       → {macro_dst}")

    abs_header = os.path.abspath(header_path)
    with open(macro_dst) as fh:
        src = fh.read()

    # Patch 1: #include placeholder → absolute header path
    old_inc = f'#include "{_HEADER_PLACEHOLDER}"'
    new_inc = f'#include "{abs_header}"'
    if old_inc not in src:
        log(
            f"WARNING: placeholder '{old_inc}' not found in analysis template. "
            f"Add the line  {old_inc}  to {template_src}",
            "WARN",
        )
    else:
        src = src.replace(old_inc, new_inc, 1)
        log(f"Patched include   : '{_HEADER_PLACEHOLDER}' → {abs_header}")

    # Patch 2: function name template → pass-specific name
    old_func = "void InttDoubletMap_template("
    new_func = f"void {analysis_name}("
    if old_func in src:
        src = src.replace(old_func, new_func, 1)
        log(f"Patched func name : 'InttDoubletMap_template' → '{analysis_name}'")
    else:
        log(f"WARNING: function signature '{old_func}' not found in analysis template.", "WARN")

    with open(macro_dst, "w") as fh:
        fh.write(src)

    return macro_dst


def step3_analysis(cfg: PassConfig, merged_file: str, dry: bool = False) -> None:
    log("── Step 3: Run InttDoubletMap analysis macro ────────────────", "STEP")

    # The header was already written to generated_dir by step1_submit.
    # We reuse it here so both macros always share the same params.
    generated_dir  = os.path.join(cfg.output_directory, "generated_macro")
    macro_name     = f"Run_PrepareHist_{cfg.pass_name}"
    header_path    = os.path.join(generated_dir, f"{macro_name}_params.h")
    analysis_name  = f"InttDoubletMap_{cfg.pass_name}"
    analysis_dst   = os.path.join(generated_dir, f"{analysis_name}.C")

    if not dry:
        if not os.path.isfile(header_path):
            log(
                f"Header not found at {header_path}. "
                f"Run Step 1 first to generate it.",
                "ERR",
            )
            sys.exit(1)
        copy_and_patch_analysis_template(cfg, header_path, generated_dir)
    else:
        log(f"[dry] Would copy+patch analysis template → {analysis_dst}")

    cmd = (
        f"source /opt/sphenix/core/bin/sphenix_setup.sh -n {cfg.sphenix_build} && "
        f"root.exe -l -b -q '{analysis_dst}(\"{merged_file}\")'"
    )
    rc = run(cmd, dry=dry)
    if rc != 0:
        log(f"Analysis macro failed (exit {rc})", "ERR")
        sys.exit(rc)

    out_dir  = os.path.dirname(merged_file)
    out_base = "Output_" + os.path.splitext(os.path.basename(merged_file))[0] + ".root"
    log(f"Analysis output  : {os.path.join(out_dir, out_base)}")


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
