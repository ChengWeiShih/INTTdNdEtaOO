"""
Sample config for run 82391 AvgVtxXY_FieldOn pass.
Edit the variables below, then run:
    python3 run_avg_vtx.py configs/data_82391_baseline.py --step 0   # generate files only
    python3 run_avg_vtx.py configs/data_82391_baseline.py --step 1   # generate + submit
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from AvgVtxXYConfig import AvgVtxXYConfig

# ── Tunable local variables (override with --set VAR=VALUE on command line) ──
_run_number    = 82391
_n_jobs        = 1
_number_of_file = 5
_ClusAdc_cut   = 30
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality"
_pass_name = "AvgVtxXY_test"

_output_directory = _mother_directory + "/" + _pass_name

cfg = AvgVtxXYConfig(
    pass_name       = _pass_name,
    notes           = "Baseline AvgVtxXY pass for run 82391",

    # ── Per-job I/O ────────────────────────────────────────────────────────
    run_number      = _run_number,
    n_events        = -1,
    number_of_file  = _number_of_file,

    input_directory = _mother_directory + "/completed",
    input_file_name = "Ntuple_00082391_standalone_ana551",
    output_directory= _output_directory,

    # ── Analysis parameters ─────────────────────────────────────────────────
    output_file_name_suffix = "",
    MBD_vtxZ_cut    = (-20.0, 20.0),
    INTTNClus_cut   = (20, 350),
    ClusAdc_cut     = _ClusAdc_cut,
    ClusPhiSize_cut = 40.0,
    HaveGeoOffsetTag= False,
    random_range_XYZ= 0.02,
    random_seed     = -999,
    input_offset_map= "no_map",

    # ── HTCondor ────────────────────────────────────────────────────────────
    n_condor_jobs   = _n_jobs,
    job_offset      = 0,
    condor_request_memory  = "12GB",
    condor_priority        = 20,
    condor_concurrency_limit = 100,
)
