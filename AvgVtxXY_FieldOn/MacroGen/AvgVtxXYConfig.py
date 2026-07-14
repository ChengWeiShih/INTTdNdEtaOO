"""
AvgVtxXYConfig  –  dataclass defining all parameters for an AvgVtxXY_FieldOn pass.

Usage in a config file:
    from AvgVtxXYConfig import AvgVtxXYConfig
    cfg = AvgVtxXYConfig(
        pass_name       = "data_82391_baseline",
        run_number      = 82391,
        ...
    )
"""

from dataclasses import dataclass, field
from typing import Tuple
import os

PairDouble = Tuple[float, float]
PairInt    = Tuple[int, int]

_FRAMEWORK_DIR = os.path.dirname(os.path.abspath(__file__))

@dataclass
class AvgVtxXYConfig:
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

    number_of_file: int = 40
    """Number of input files per job (passed as nfiles to the macro)."""

    input_directory: str = ""
    input_file_name: str = "Ntuple_{run_num:08d}_standalone"
    """May contain {process:05d} or {run_num:08d} placeholders."""

    output_directory: str = ""
    """Root output directory; completed/ and cw_log/ are created here."""

    # ── Analysis parameters ────────────────────────────────────────────────────
    output_file_name_suffix: str = ""

    MBD_vtxZ_cut:   PairDouble = (-20.0, 20.0)
    """(min, max) MBD vtxZ cut in cm."""

    INTTNClus_cut:  PairInt    = (20, 350)
    """(min, max) INTT cluster count cut."""

    ClusAdc_cut:    float      = 15.0
    ClusPhiSize_cut: float     = 40.0

    HaveGeoOffsetTag: bool     = False
    random_range_XYZ: float    = 0.02
    """Random smearing range in cm."""

    random_seed:    int        = -999
    input_offset_map: str      = "no_map"

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
    """Directory containing Run_AvgVtxXY_FieldOn_template.C and run_job_template.sh."""

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
            f"  number_of_file   : {self.number_of_file}",
            "",
            "── I/O ──────────────────────────────────────────────",
            f"  input_directory  : {self.input_directory}",
            f"  input_file_name  : {self.input_file_name}",
            f"  output_directory : {self.output_directory}",
            f"  suffix           : {self.output_file_name_suffix!r}",
            "",
            "── Analysis parameters ──────────────────────────────",
            f"  MBD_vtxZ_cut     : {self.MBD_vtxZ_cut}  cm",
            f"  INTTNClus_cut    : {self.INTTNClus_cut}",
            f"  ClusAdc_cut      : {self.ClusAdc_cut}",
            f"  ClusPhiSize_cut  : {self.ClusPhiSize_cut}",
            f"  HaveGeoOffsetTag : {self.HaveGeoOffsetTag}",
            f"  random_range_XYZ : {self.random_range_XYZ}  cm",
            f"  random_seed      : {self.random_seed}",
            f"  input_offset_map : {self.input_offset_map}",
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
