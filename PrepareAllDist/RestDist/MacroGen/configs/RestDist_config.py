"""
Sample config for RestDist pass.
Edit the variables below, then run:
    python3 run_rest_dist.py configs/RestDist_config.py --step 0   # generate files only
    python3 run_rest_dist.py configs/RestDist_config.py --step 1   # generate + submit
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from RestDistConfig import RestDistConfig

# ── Tunable local variables (override with --set VAR=VALUE on command line) ──
_run_number    = 82391
_n_jobs        = 1000
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality"
_pass_name     = "RestDist"

_output_directory = _mother_directory + "/" + _pass_name

cfg = RestDistConfig(
    pass_name       = _pass_name,
    notes           = "RestDist pass",

    # ── Per-job I/O ────────────────────────────────────────────────────────
    run_number      = _run_number,
    n_events        = -1,

    input_directory = _mother_directory + "/EvtVtxZ/completed",
    input_file_name = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_{process:05d}.root",
    output_directory= _output_directory,

    # ── Analysis parameters ─────────────────────────────────────────────────
    output_file_name_suffix = "",
    
    vertexXYIncm    = (-0.05156, 0.13594),
    ApplyEvtBcoFullDiffCut = (True, 80), # note : don't touch
    
    isClusQA        = (True, (30.0, 40.0)),
    isRotated       = False,

    Apply_cut       = True, #note: z-vertex QA
    RequireVtxZRange= (True, (-10.0, 10.0)),
    ApplyVtxZReWeighting = False,
    vtxZReWeighting_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/VtxZDist/completed/vtxZ_comp_WithVtxZQA_001/INTTvtxZReWeight.root",
    map_name        = "HIJING_noZWeight_WithVtxZQA_Inclusive70",

    # ── HTCondor ────────────────────────────────────────────────────────────
    n_condor_jobs   = _n_jobs,
    job_offset      = 0,
    condor_request_memory  = "4GB",
    condor_priority        = 20,
    condor_concurrency_limit = 100,
)
