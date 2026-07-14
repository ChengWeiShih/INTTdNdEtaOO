"""
RestDistConfig  –  dataclass defining all parameters for an RestDist pass.

Usage in a config file:
    from RestDistConfig import RestDistConfig
    cfg = RestDistConfig(
        pass_name       = "data_82391_baseline",
        run_number      = 82391,
        ...
    )
"""

from dataclasses import dataclass, field
from typing import Tuple
import os

PairDouble = Tuple[float, float]
PairBoolInt = Tuple[bool, int]
PairBoolPairDouble = Tuple[bool, Tuple[float, float]]

_FRAMEWORK_DIR = os.path.dirname(os.path.abspath(__file__))

@dataclass
class RestDistConfig:
    # ── Pass identity ──────────────────────────────────────────────────────────
    pass_name: str = "unnamed_pass"
    """Short unique name used for all generated file names."""

    notes: str = ""
    """Free-text notes."""

    # ── Per-job I/O ────────────────────────────────────────────────────────────
    run_number: int = -1
    """Run number; -1 = MC."""

    n_events: int = -1
    """Events per job; -1 = all."""

    input_directory: str = ""
    input_file_name: str = "MC_EvtVtxZProtoTracklet_FieldOff_VtxZReco_{process:05d}.root"
    """May contain {process:05d} or {run_num:08d} placeholders."""

    output_directory: str = ""
    """Root output directory; completed/ and cw_log/ are created here."""

    # ── Analysis parameters ────────────────────────────────────────────────────
    output_file_name_suffix: str = ""
    vertexXYIncm: PairDouble = (-0.0218978, 0.223183)
    Apply_cut: bool = True
    ApplyVtxZReWeighting: bool = True
    ApplyEvtBcoFullDiffCut: PairBoolInt = (False, 61)
    RequireVtxZRange: PairBoolPairDouble = (True, (-10.0, 10.0))
    isClusQA: PairBoolPairDouble = (True, (35.0, 40.0))
    isRotated: bool = True
    vtxZReWeighting_input_directory: str = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/VtxZDist/completed/vtxZ_comp_WithVtxZQA_001/INTTvtxZReWeight.root"
    map_name: str = "HIJING_noZWeight_WithVtxZQA_Inclusive70"

    # ── HTCondor ───────────────────────────────────────────────────────────────
    n_condor_jobs:            int = 146
    job_offset:               int = 0
    condor_request_memory:    str = "4GB"
    condor_priority:          int = 20
    condor_concurrency_limit: int = 100

    # ── sPHENIX build tag ──────────────────────────────────────────────────────
    sphenix_build: str = "ana.538"

    # ── Template locations (defaults point to MacroGen/) ──────────────────────
    macro_template_dir: str = _FRAMEWORK_DIR
    """Directory containing Run_RestDist_template.C and run_job_template.sh."""

    # ── Derived helpers ────────────────────────────────────────────────────────
    def completed_dir(self) -> str:
        return os.path.join(self.output_directory, "completed")

    def condor_log_dir(self) -> str:
        return os.path.join(self.output_directory, "cw_log")

    def generated_dir(self) -> str:
        return os.path.join(self.output_directory, "generated_macro")

    def summary(self) -> str:
        lines = [
            "=" * 60,
            f"  Pass : {self.pass_name}",
            "=" * 60,
            f"  Run number       : {self.run_number}",
            f"  N events         : {self.n_events}",
            "",
            "── I/O ──────────────────────────────────────────────",
            f"  input_directory  : {self.input_directory}",
            f"  input_file_name  : {self.input_file_name}",
            f"  output_directory : {self.output_directory}",
            f"  suffix           : {self.output_file_name_suffix!r}",
            "",
            "── Analysis parameters ──────────────────────────────",
            f"  vertexXYIncm          : {self.vertexXYIncm}",
            f"  Apply_cut             : {self.Apply_cut}",
            f"  ApplyVtxZReWeighting  : {self.ApplyVtxZReWeighting}",
            f"  ApplyEvtBcoFullDiffCut: {self.ApplyEvtBcoFullDiffCut}",
            f"  RequireVtxZRange      : {self.RequireVtxZRange}",
            f"  isClusQA              : {self.isClusQA}",
            f"  isRotated             : {self.isRotated}",
            f"  vtxZ_reweight_dir     : {self.vtxZReWeighting_input_directory}",
            f"  map_name              : {self.map_name}",
            "",
            "── HTCondor ─────────────────────────────────────────",
            f"  n_condor_jobs    : {self.n_condor_jobs}",
            f"  job_offset       : {self.job_offset}",
            f"  memory / job     : {self.condor_request_memory}",
            f"  priority         : {self.condor_priority}",
            "",
            "── Notes ────────────────────────────────────────────",
            f"  {self.notes}",
            "=" * 60,
        ]
        return "\n".join(lines)
