"""
pass_configs/data_82391_baseline.py
====================================
Configuration for a real-data pass on run 82391.

Edit the fields below to match your setup, then run:

  python3 run_pass.py pass_configs/data_82391_baseline.py --dry-run
  python3 run_pass.py pass_configs/data_82391_baseline.py --step all
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from PassConfig import PassConfig

# ── Change these to shift output_directory, CentralityBin, cluster cuts, and DeltaPhiCut ──
_centrality_bin   = 1
_cluster_adc      = 30
_cluster_phi_size = 40
_delta_phi_cut    = 0.15
_subfoldername    = "baseline"

# _mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality"
_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_LoacalPos_PrivateCentrality"

cfg = PassConfig(

    # ── Pass identification ──────────────────────────────────────────────────
    pass_name       = "data_82391_baseline_test",
    notes           = "Baseline, run 82391, test",

    # ── I/O ─────────────────────────────────────────────────────────────────
    input_directory  = _mother_directory + "/EvtVtxZ/completed",
    input_file_name  = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_{process:05d}.root",
    output_directory = _mother_directory + "/InttDoublets/%s_%d" % (_subfoldername, _centrality_bin),

    # ── Run metadata ─────────────────────────────────────────────────────────
    run_number  = 82391,
    n_events    = -1,       # -1 → all events in each file

    # ── InttDoubletMap parameters ────────────────────────────────────────────
    output_file_name_suffix = "",
    # vertexXYIncm    = (-0.05156, 0.13594),   # beam-spot (x,y) in cm, survey only
    vertexXYIncm    = (-0.04844, 0.11719),   # beam-spot (x,y) in cm, w/ alignment parameter
    
    data_type       = 0,                    # 2 = streaming_data
    isUsedMBDz      = False,
    
    BcoFullDiffCut  = True,
    CentralityBin   = _centrality_bin,
    isMinBiasCut    = True,
    isTriggerSel    = False,
    isMBDChargeCut  = (False, (0, 10000)),
    isBunchNumber_cut = (False, (-10, 1000)),
    
    vtxZReweight    = False,        # plain bool: True = enable vtxZ reweighting
    zvtx_weight_dir  = "",           # only needed when vtxZReweight=True
    zvtx_weight_file = "",
    zvtx_weight_hist = "",
    INTT_vtxZ_QA   = False,
    VtxZRange       = (-10.0, 10.0),
    
    ColMulMask      = True,
    isClusQA        = (True, (_cluster_adc, _cluster_phi_size)),
    DeltaPhiCut     = _delta_phi_cut,
    
    HaveGeoOffsetTag = False,

    # ── ColMulMask map (only needed when ColMulMask=True) ─────────────────────
    
    # ColMulMask_map_dir  = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/ColumnCheck/baseline/completed/MulMap/completed",
    ColMulMask_map_dir  = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/ColumnCheck/baseline/completed/MulMap_WithAlignP/completed",
    ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin55_VtxZ-30to30cm_ClusQAAdc30PhiSize40_00082391.root",

    # ── HTCondor ─────────────────────────────────────────────────────────────
    n_condor_jobs       = 1000,
    job_offset          = 0,
    condor_request_memory = "4GB",
    condor_priority     = 20,
    condor_concurrency_limit = 100,

    # ── Merge ─────────────────────────────────────────────────────────────────
    # Leave merge_filename_template = "" → auto-detect from completed/ directory
    merge_is_shuffle       = False,
    merge_n_files          = 1,
    merge_mega_merge       = False,
    merge_filename_template = "",

    # ── Build tag ────────────────────────────────────────────────────────────
    sphenix_build = "ana.538",
)
