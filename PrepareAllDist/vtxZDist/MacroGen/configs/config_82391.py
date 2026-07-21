"""
Sample config for vtxZDist pass.
Edit the variables below, then run:
    python3 run_vtxz_dist.py configs/VtxZDist_config.py --step 0   # generate files only
    python3 run_vtxz_dist.py configs/VtxZDist_config.py --step 1   # generate + submit
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from VtxZDistConfig import VtxZDistConfig

# note : ── Tunable local variables (override with --set VAR=VALUE on command line) ──
_run_number    = 82391
_n_jobs        = 1000
# _mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391/test_82391_LoacalPos_PrivateCentrality"
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality"
_pass_name     = "VtxZDist"

_output_directory = _mother_directory + "/" + _pass_name

cfg = VtxZDistConfig(
    pass_name       = _pass_name,
    notes           = "vtxZDist pass",

    # ── Per-job I/O ────────────────────────────────────────────────────────
    run_number      = _run_number,
    n_events        = -1,

    input_directory = _mother_directory + "/EvtVtxZ/completed",
    input_file_name = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_{process:05d}.root",
    output_directory= _output_directory,

    # ── Analysis parameters ─────────────────────────────────────────────────
    output_file_name_suffix = "",
    Apply_cut               = False, # note : don't touch
    ApplyVtxZReWeighting    = False, # note : don't touch, regardless data or MC
    ApplyEvtBcoFullDiffCut  = (True, 80), # note : don't touch, regardless data or MC
    IsVtxZQACutKnown        = True,

    # ── HTCondor ────────────────────────────────────────────────────────────
    n_condor_jobs   = _n_jobs,
    job_offset      = 0,
    condor_request_memory  = "4GB",
    condor_priority        = 20,
    condor_concurrency_limit = 100,
)
