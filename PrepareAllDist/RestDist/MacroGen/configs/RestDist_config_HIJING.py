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
_run_number    = -1
_n_jobs        = 800
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex"
_pass_name     = "RestDist"

_output_directory = _mother_directory + "/" + _pass_name

cfg = RestDistConfig(
    pass_name       = _pass_name,
    notes           = "HIJING RestDist pass",

    # ── Per-job I/O ────────────────────────────────────────────────────────
    run_number      = _run_number,
    n_events        = -1,

    input_directory = _mother_directory + "/EvtVtxZ/completed",
    input_file_name = "MC_EvtVtxZProtoTracklet_FieldOn_VtxZReco_{process:05d}.root",
    output_directory= _output_directory,

    # ── Analysis parameters ─────────────────────────────────────────────────
    output_file_name_suffix = "",
    
    vertexXYIncm    = (-0.05469, 0.13906),
    ApplyEvtBcoFullDiffCut = (True, 80), # note : don't touch
    
    isClusQA        = (True, (15.0, 40.0)),
    isRotated       = False,

    Apply_cut       = True, #note: z-vertex QA
    RequireVtxZRange= (True, (-10.0, 10.0)),
    ApplyVtxZReWeighting = True,
    vtxZReWeighting_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/VtxZDist/completed/vtxZ_comp_NoVtxZQA/INTTvtxZReWeight.root",
    map_name        = "HIJING_noZWeight_NoVtxZQA_Inclusive70_tight",

    # ── HTCondor ────────────────────────────────────────────────────────────
    n_condor_jobs   = _n_jobs,
    job_offset      = 0,
    condor_request_memory  = "4GB",
    condor_priority        = 20,
    condor_concurrency_limit = 100,
)
