"""
Sample config for EvtVtxZTracklet pass.
Edit the variables below, then run:
    python3 run_evt_vtx.py configs/EvtVtxZ_config.py --step 0   # generate files only
    python3 run_evt_vtx.py configs/EvtVtxZ_config.py --step 1   # generate + submit
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from EvtVtxZTrackletConfig import EvtVtxZTrackletConfig

# ── Tunable local variables (override with --set VAR=VALUE on command line) ──
_run_number    = 82391
_n_jobs        = 100
_ClusAdcCut    = 30
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_LoacalPos_PrivateCentrality"
_pass_name     = "EvtVtxZ_test"

_output_directory = _mother_directory + "/" + _pass_name

cfg = EvtVtxZTrackletConfig(
    pass_name       = _pass_name,
    notes           = "EvtVtxZTracklet pass",

    # ── Per-job I/O ────────────────────────────────────────────────────────
    run_number      = _run_number,
    n_events        = -1,

    input_directory = _mother_directory + "/completed",
    input_file_name = "Ntuple_00082391_standalone_ana551_{process:05d}.root",
    output_directory= _output_directory,

    # ── Analysis parameters ─────────────────────────────────────────────────
    output_file_name_suffix = "",
    vertexXYIncm    = (-0.053125, 0.13593800),
    IsFieldOn       = True,
    IsDCACutApplied = True,
    DeltaPhiCutInDegree = ((-1.146, 1.146), (-1000.0, 1000.0)),
    DCAcutIncm      = ((-0.7, 0.7), (-1000.0, 1000.0)),
    ClusAdcCut      = _ClusAdcCut,
    ClusPhiSizeCut  = 40,

    PrintRecoDetails = True,
    DrawEvtVtxZ      = True,

    RunInttBcoFullDiff = True,
    RunVtxZReco        = True,
    RunTrackletPair    = False,
    RunTrackletPairRotate = False,

    HaveGeoOffsetTag = False,

    # ── HTCondor ────────────────────────────────────────────────────────────
    n_condor_jobs   = _n_jobs,
    job_offset      = 0,
    condor_request_memory  = "4GB",
    condor_priority        = 20,
    condor_concurrency_limit = 100,
)
