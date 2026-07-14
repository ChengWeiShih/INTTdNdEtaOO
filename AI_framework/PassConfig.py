"""
PassConfig.py
=============
Defines the PassConfig dataclass — a single Python object that captures
every input needed to run one full "pass" of the InttDoubletMap pipeline:

  Step 1  – submit HTCondor jobs running Run_PrepareHist.C
  Step 2  – wait for jobs, then merge with run_random_merge
            (calls Run_RandomMerge.sh → RandomMerge.C → hadd)
  Step 3  – run InttDoubletMap.C on the merged file

run_random_merge signature (from ~/.bash_profile):
  run_random_merge <IsShuffle> <N_merged_files> <input_dir> <input_filename> <mega_merge>
  e.g.:  run_random_merge 0 1 $PWD AAA_00000.root 0

All InttDoubletMap.h constructor fields are represented here 1-to-1.

Usage
-----
from PassConfig import PassConfig

cfg = PassConfig(
    pass_name      = "data_82405_baseline",
    input_directory = "...",
    input_file_name = "82405_converter_{process:05d}.root",
    output_directory = "...",
    run_number      = 82405,
    data_type       = 2,
    ...
)
"""

from dataclasses import dataclass, field
from typing import Optional, Tuple


# ---------------------------------------------------------------------------
# Type aliases (mirrors C++ types)
# ---------------------------------------------------------------------------
PairDouble      = Tuple[float, float]       # std::pair<double,double>
PairInt         = Tuple[int,   int]         # std::pair<int,int>
PairBoolDouble  = Tuple[bool, PairDouble]   # std::pair<bool,std::pair<double,double>>
PairBoolInt     = Tuple[bool, PairInt]      # std::pair<bool,std::pair<int,int>>


# ---------------------------------------------------------------------------
@dataclass
class PassConfig:
    """
    Full configuration for one InttDoubletMap analysis pass.

    ── Required fields (must be set by the user) ───────────────────────────
    """

    # ── Pass identification ─────────────────────────────────────────────────
    pass_name: str
    """Unique human-readable label (used in logs and output names)."""

    # ── I/O paths ───────────────────────────────────────────────────────────
    input_directory: str
    """Directory containing the input root files (converter / proto-tracklet)."""

    input_file_name: str
    """
    Root-file name template.
    Use '{process:05d}' as a placeholder for the job process index, e.g.:
      '82405_converter_{process:05d}.root'
      'MC_EvtVtxZProtoTracklet_FieldOn_VtxZReco_{process:05d}.root'
    This template is expanded by run_pass.py when writing the condor job file
    and by run_job.sh when ROOT opens the file.
    For a single file (no per-job numbering), just give the exact filename.
    """

    output_directory: str
    """
    Base output directory for this pass.
    Sub-directories (baseline/completed, cw_log, …) are created automatically.
    """

    # ── Optional fields (all have sensible defaults) ─────────────────────────

    # ── Run metadata ─────────────────────────────────────────────────────────
    run_number: int = -1
    """Run number forwarded to InttDoubletMap (use -1 for MC)."""

    n_events: int = -1
    """Events per condor job (-1 → process all events in the file)."""

    # ── InttDoubletMap.h constructor parameters ───────────────────────────────
    output_file_name_suffix: str = ""
    """Optional suffix appended to the auto-generated output file name."""

    vertexXYIncm: PairDouble = (-0.03677, 0.1454)
    """Beam-spot position (x, y) in cm."""

    data_type: int = 2
    """0 = pure_trigger, 1 = streaming_trigger, 2 = streaming_data."""

    isUsedMBDz: bool = False
    """Use MBD-z for the event vertex instead of INTT-z."""

    BcoFullDiffCut: bool = True
    """Apply the BCO full-difference cut."""

    CentralityBin: int = 1
    """Number of centrality bins (1 = inclusive, 100 = 1%-wide bins)."""

    isMinBiasCut: bool = True
    """Apply minimum-bias event selection."""

    isTriggerSel: bool = False
    """Apply trigger selection."""

    isMBDChargeCut: PairBoolInt = (False, (0, 10000))
    """(enable, (min_charge, max_charge))  MBD charge cut."""

    isBunchNumber_cut: PairBoolInt = (False, (-10, 1000))
    """(enable, (min_bunch, max_bunch))  bunch-number cut."""

    IsTrigEffiWeight: bool = False
    TrigEffiWeight_dir: str = ""
    TrigEffiWeight_file: str = ""
    TrigEffiWeight_hist: str = ""



    vtxZReweight: bool = False
    """
    Simple bool flag: True = enable vtxZ reweighting, False = disable.
    The macro constructs std::pair<int,TH1D*> internally from this flag
    and loads the histogram via GetTH1D(zvtx_weight_dir, zvtx_weight_file,
    zvtx_weight_hist).
    """

    zvtx_weight_dir: str = ""
    """Directory containing the vtxZ reweight root file."""

    zvtx_weight_file: str = ""
    """Filename of the vtxZ reweight root file (e.g. 'INTTvtxZReWeight.root')."""

    zvtx_weight_hist: str = ""
    """Name of the TH1D inside the file (e.g. 'HIJING_noZWeight_NoVtxZQA_Inclusive70_tight')."""

    INTT_vtxZ_QA: bool = False
    """Fill INTT vertex-Z QA histograms."""

    VtxZRange: PairDouble = (-10.0, 10.0)
    """Accepted vertex-Z window (min, max) in cm."""

    ColMulMask: bool = True
    """Apply the column-multiplicity mask."""

    isClusQA: PairBoolDouble = (True, (30.0, 40.0))
    """(enable, (adc_cut, phi_size_cut))  cluster QA cuts."""

    DeltaPhiCut: float = 0.15
    """Maximum |ΔΦ| for a proto-tracklet pair (radians)."""

    HaveGeoOffsetTag: bool = False
    """Input files carry geometry-offset tags."""

    # ── Column-multiplicity mask map (used only when ColMulMask=True) ────────
    ColMulMask_map_dir: str = ""
    """
    Full path to the directory containing the MulMap root file.
    Example:
      "/path/to/ColumnCheck/baseline/completed/MulMap/completed"
    """

    ColMulMask_map_file: str = ""
    """
    Filename of the MulMap root file.
    Example:
      "MulMap_BcoFullDiffCut_Mbin55_VtxZ-30to30cm_ClusQAAdc30PhiSize40_00082391.root"
    """

    # ── HTCondor settings ────────────────────────────────────────────────────
    n_condor_jobs: int = 1000
    """Number of HTCondor jobs to submit (one per input file)."""

    job_offset: int = 0
    """Starting process-ID offset (useful when re-submitting missing jobs)."""

    condor_request_memory: str = "4GB"
    """Memory request per condor job."""

    condor_priority: int = 20
    """HTCondor job priority."""

    condor_concurrency_limit: int = 100
    """Maximum number of jobs running concurrently."""

    # ── Merge settings (run_random_merge) ────────────────────────────────────
    merge_is_shuffle: bool = False
    """IsShuffle argument of run_random_merge (0=False, 1=True)."""

    merge_n_files: int = 1
    """N_merged_files argument — how many merged output files to produce."""

    merge_mega_merge: bool = False
    """
    mega_merge argument — if True and merge_n_files > 1, hadd all partial
    merged files into a single final file.
    """

    merge_filename_template: str = ""
    """
    The input_filename argument passed to run_random_merge.
    RandomMerge.C strips everything after the last '_' and then globs
    '<stripped>_0*.root', so you only need to provide any valid file in the
    completed/ directory (e.g. 'Data_INTTDoublets_..._00000.root').

    Leave empty ("") and the framework will auto-detect the first *.root
    file it finds in the completed/ directory.
    """

    # ── Path to sPHENIX environment ──────────────────────────────────────────
    sphenix_build: str = "ana.538"
    """sPHENIX build tag (passed to sphenix_setup.sh -n <tag>)."""

    # ── Macro / library paths ────────────────────────────────────────────────
    macro_dir: str = (
        "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/InttDoubletMap/macro"
    )
    """Directory containing Run_PrepareHist.C, run_condor.job, run_job.sh."""

    analysis_macro_dir: str = (
        "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/macro/InttDoubletMap"
    )
    """Directory containing InttDoubletMap_template.C (post-merge analysis step)."""

    # ── Notes ─────────────────────────────────────────────────────────────────
    notes: str = ""
    """Free-text notes stored alongside this config."""

    # ── Derived helpers (computed from stored fields, not set by user) ────────
    def completed_dir(self) -> str:
        """
        Path to the 'completed' sub-directory.
        The template uses output_directory directly (no /baseline appended),
        so completed files land in <output_directory>/completed/.
        """
        return f"{self.output_directory}/completed"

    def condor_log_dir(self) -> str:
        """Path to the HTCondor log sub-directory."""
        return f"{self.output_directory}/cw_log"

    def summary(self) -> str:
        """Return a human-readable one-page summary of this configuration."""
        lines = [
            "=" * 60,
            f"  Pass : {self.pass_name}",
            "=" * 60,
            f"  Run number             : {self.run_number}",
            f"  N events / job         : {self.n_events}",
            "",
            "── I/O ─────────────────────────────────────────────────",
            f"  input_directory        : {self.input_directory}",
            f"  input_file_name        : {self.input_file_name}",
            f"  output_directory       : {self.output_directory}",
            f"  output suffix          : {self.output_file_name_suffix}",
            "",
            "── Analysis parameters ──────────────────────────────────",
            f"  data_type              : {self.data_type}  (0=pure_trig, 1=stream_trig, 2=stream_data)",
            f"  vertexXY (cm)          : {self.vertexXYIncm}",
            f"  VtxZ range (cm)        : {self.VtxZRange}",
            f"  isUsedMBDz             : {self.isUsedMBDz}",
            f"  BcoFullDiffCut         : {self.BcoFullDiffCut}",
            f"  CentralityBin          : {self.CentralityBin}",
            f"  isMinBiasCut           : {self.isMinBiasCut}",
            f"  isTriggerSel           : {self.isTriggerSel}",
            f"  isMBDChargeCut         : {self.isMBDChargeCut}",
            f"  isBunchNumber_cut      : {self.isBunchNumber_cut}",
            f"  vtxZReweight ----------: {self.vtxZReweight}",
            f"    zvtx_weight_dir      : {self.zvtx_weight_dir}",
            f"    zvtx_weight_file     : {self.zvtx_weight_file}",
            f"    zvtx_weight_hist     : {self.zvtx_weight_hist}",
            f"  IsTrigEffiWeight ------: {self.IsTrigEffiWeight}",
            f"    TrigEffiWeight_dir   : {self.TrigEffiWeight_dir}",
            f"    TrigEffiWeight_file  : {self.TrigEffiWeight_file}",
            f"    TrigEffiWeight_hist  : {self.TrigEffiWeight_hist}",
            f"  INTT_vtxZ_QA           : {self.INTT_vtxZ_QA}",
            f"  ColMulMask             : {self.ColMulMask}",
            f"  isClusQA               : {self.isClusQA}",
            f"  DeltaPhiCut            : {self.DeltaPhiCut}",
            f"  HaveGeoOffsetTag       : {self.HaveGeoOffsetTag}",
            "",
            "── HTCondor ─────────────────────────────────────────────",
            f"  n_condor_jobs          : {self.n_condor_jobs}",
            f"  job_offset             : {self.job_offset}",
            f"  memory / job           : {self.condor_request_memory}",
            f"  priority               : {self.condor_priority}",
            f"  concurrency limit      : {self.condor_concurrency_limit}",
            "",
            "── Merge (run_random_merge) ──────────────────────────────",
            f"  IsShuffle              : {int(self.merge_is_shuffle)}",
            f"  N_merged_files         : {self.merge_n_files}",
            f"  mega_merge             : {int(self.merge_mega_merge)}",
            f"  filename template      : {self.merge_filename_template or '(auto-detect)'}",
            "",
            "── Notes ────────────────────────────────────────────────",
            f"  {self.notes}" if self.notes else "  (none)",
            "=" * 60,
        ]
        return "\n".join(lines)
