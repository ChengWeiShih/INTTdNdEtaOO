"""
Sample config for an EvtVtxZLeftRight pass.

Edit the variables below, then run:
    python3 run_evt_vtxz_left_right.py configs/EvtVtxZLeftRight_config.py --step 0
    python3 run_evt_vtxz_left_right.py configs/EvtVtxZLeftRight_config.py --step 1
"""

import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from EvtVtxZLeftRightConfig import EvtVtxZLeftRightConfig

_run_number = 82391
_n_jobs = 1
_pass_name = "EvtVtxZLeftRight_test"
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly"
_output_directory = _mother_directory + "/" + _pass_name

cfg = EvtVtxZLeftRightConfig(
    pass_name = _pass_name,
    notes = "EvtVtxZLeftRight generated pass",

    # -- Per-job I/O ---------------------------------------------------------
    run_number = _run_number,
    n_events = -1,
    input_directory = _mother_directory + "/EvtVtxZ/completed",
    input_file_name = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_{process:05d}.root",
    output_directory = _output_directory,

    # -- Analysis parameters -------------------------------------------------
    output_file_name_suffix = "",
    vertexXYIncm = (-0.0059375, -0.0025),
    IsFieldOn = True,
    IsDCACutApplied = True,
    DeltaPhiCutInDegree = ((-1.146, 1.146), (-1000.0, 1000.0)),
    DCAcutIncm = ((-0.7, 0.7), (-1000.0, 1000.0)),
    ClusAdcCut = 15,
    ClusPhiSizeCut = 40,
    PrintRecoDetails = True,
    DrawEvtVtxZ = True,
    RunInttBcoFullDiff = True,
    RunVtxZReco = True,
    RunTrackletPair = False,
    RunTrackletPairRotate = False,
    HaveGeoOffsetTag = False,

    # -- HTCondor ------------------------------------------------------------
    n_condor_jobs = _n_jobs,
    job_offset = 0,
    condor_request_memory = "8GB",
    condor_priority = 20,
    condor_concurrency_limit = 100,
    sphenix_build = "ana.538",
)
