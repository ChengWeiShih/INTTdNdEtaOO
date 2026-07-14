"""
EvtVtxZLeftRightConfig - dataclass defining one EvtVtxZLeftRight pass.

Usage in a config file:
    from EvtVtxZLeftRightConfig import EvtVtxZLeftRightConfig
    cfg = EvtVtxZLeftRightConfig(
        pass_name  = "data_82391_left_right",
        run_number = 82391,
        ...
    )
"""

from dataclasses import dataclass
from typing import Tuple
import os

PairDouble = Tuple[float, float]
PairPairDouble = Tuple[PairDouble, PairDouble]

_FRAMEWORK_DIR = os.path.dirname(os.path.abspath(__file__))


@dataclass
class EvtVtxZLeftRightConfig:
    # -- Pass identity -------------------------------------------------------
    pass_name: str = "unnamed_pass"
    notes: str = ""

    # -- Per-job I/O ---------------------------------------------------------
    run_number: int = -1
    n_events: int = -1

    input_directory: str = ""
    input_file_name: str = "Data_EvtVtxZProtoTracklet_{process:05d}.root"
    """May contain {process:05d} and/or {run_num:08d} placeholders."""

    output_directory: str = ""

    # -- Analysis parameters -------------------------------------------------
    output_file_name_suffix: str = ""

    vertexXYIncm: PairDouble = (-0.0059375, -0.0025)
    IsFieldOn: bool = True
    IsDCACutApplied: bool = True
    DeltaPhiCutInDegree: PairPairDouble = ((-1.146, 1.146), (-1000.0, 1000.0))
    DCAcutIncm: PairPairDouble = ((-0.7, 0.7), (-1000.0, 1000.0))
    ClusAdcCut: int = 15
    ClusPhiSizeCut: int = 40

    PrintRecoDetails: bool = True
    DrawEvtVtxZ: bool = True

    RunInttBcoFullDiff: bool = True
    RunVtxZReco: bool = True
    RunTrackletPair: bool = False
    RunTrackletPairRotate: bool = False

    HaveGeoOffsetTag: bool = False

    # -- HTCondor ------------------------------------------------------------
    n_condor_jobs: int = 100
    job_offset: int = 0
    condor_request_memory: str = "8GB"
    condor_priority: int = 20
    condor_concurrency_limit: int = 100

    # -- sPHENIX build tag ---------------------------------------------------
    sphenix_build: str = "ana.538"

    # -- Template locations --------------------------------------------------
    macro_template_dir: str = _FRAMEWORK_DIR

    # -- Derived helpers -----------------------------------------------------
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
            "-- I/O ------------------------------------------------",
            f"  input_directory  : {self.input_directory}",
            f"  input_file_name  : {self.input_file_name}",
            f"  output_directory : {self.output_directory}",
            f"  suffix           : {self.output_file_name_suffix!r}",
            "",
            "-- Analysis parameters --------------------------------",
            f"  vertexXYIncm          : {self.vertexXYIncm}",
            f"  IsFieldOn             : {self.IsFieldOn}",
            f"  IsDCACutApplied       : {self.IsDCACutApplied}",
            f"  DeltaPhiCutInDegree   : {self.DeltaPhiCutInDegree}",
            f"  DCAcutIncm            : {self.DCAcutIncm}",
            f"  ClusAdcCut            : {self.ClusAdcCut}",
            f"  ClusPhiSizeCut        : {self.ClusPhiSizeCut}",
            f"  PrintRecoDetails      : {self.PrintRecoDetails}",
            f"  DrawEvtVtxZ           : {self.DrawEvtVtxZ}",
            f"  RunInttBcoFullDiff    : {self.RunInttBcoFullDiff}",
            f"  RunVtxZReco           : {self.RunVtxZReco}",
            f"  RunTrackletPair       : {self.RunTrackletPair}",
            f"  RunTrackletPairRotate : {self.RunTrackletPairRotate}",
            f"  HaveGeoOffsetTag      : {self.HaveGeoOffsetTag}",
            "",
            "-- HTCondor -------------------------------------------",
            f"  n_condor_jobs    : {self.n_condor_jobs}",
            f"  job_offset       : {self.job_offset}",
            f"  memory / job     : {self.condor_request_memory}",
            f"  priority         : {self.condor_priority}",
            "",
            "-- Notes ----------------------------------------------",
            f"  {self.notes}",
            "=" * 60,
        ]
        return "\n".join(lines)
