"""
ColumnCheckConfig  –  dataclass defining all parameters for an ColumnCheck pass.

Usage in a config file:
    from ColumnCheckConfig import ColumnCheckConfig
    cfg = ColumnCheckConfig(
        pass_name       = "data_82391_baseline",
        run_number      = 82391,
        ...
    )
"""

from dataclasses import dataclass, field
from typing import Tuple
import os

PairDouble = Tuple[float, float]
PairBoolPairDouble = Tuple[bool, Tuple[float, float]]

_FRAMEWORK_DIR = os.path.dirname(os.path.abspath(__file__))

@dataclass
class ColumnCheckConfig:
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
    input_file_name: str = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_{process:05d}.root"
    """May contain {process:05d} or {run_num:08d} placeholders."""

    output_directory: str = ""
    """Root output directory; completed/ and cw_log/ are created here."""

    # ── Analysis parameters ────────────────────────────────────────────────────
    output_file_name_suffix: str = ""
    
    SetMbinFloat: float = 55.0
    VtxZRange: PairDouble = (-30.0, 30.0)
    IsZClustering: bool = False
    vertexXYIncm: PairDouble = (-0.05156, 0.13594)
    isClusQA: PairBoolPairDouble = (True, (30.0, 40.0))
    ColMulMask: bool = False

    # ── HTCondor ───────────────────────────────────────────────────────────────
    n_condor_jobs:            int = 100
    job_offset:               int = 0
    condor_request_memory:    str = "4GB"
    condor_priority:          int = 20
    condor_concurrency_limit: int = 100

    # ── sPHENIX build tag ──────────────────────────────────────────────────────
    sphenix_build: str = "ana.538"

    # ── Template locations (defaults point to MacroGen/) ──────────────────────
    macro_template_dir: str = _FRAMEWORK_DIR
    """Directory containing Run_ColumnCheck_template.C and run_job_template.sh."""

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
            f"  SetMbinFloat     : {self.SetMbinFloat}",
            f"  VtxZRange        : {self.VtxZRange}",
            f"  IsZClustering    : {self.IsZClustering}",
            f"  vertexXYIncm     : {self.vertexXYIncm}",
            f"  isClusQA         : {self.isClusQA}",
            f"  ColMulMask       : {self.ColMulMask}",
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
