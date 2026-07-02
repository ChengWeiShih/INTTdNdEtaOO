"""
pass_configs/MC_HIJING_baseline.py
====================================
Configuration for a HIJING MC

Edit the fields below to match your setup, then run:

  python3 run_pass.py pass_configs/MC_HIJING_baseline.py --dry-run
  python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from PassConfig import PassConfig

# ── Change these to shift output_directory, CentralityBin, cluster cuts, and DeltaPhiCut ──
_centrality_bin   = 0
_cluster_adc      = 15 # 15 for MC
_cluster_phi_size = 40
_delta_phi_cut    = 0.15
_subfoldername    = "baseline"

_mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex"

cfg = PassConfig(

    # ── Pass identification ──────────────────────────────────────────────────
    pass_name       = "MC_HIJING_baseline_test",
    notes           = "Baseline, MC HIJING, test",

    # ── I/O ─────────────────────────────────────────────────────────────────
    input_directory  = _mother_directory + "/EvtVtxZ/completed",
    input_file_name  = "MC_EvtVtxZProtoTracklet_FieldOn_VtxZReco_{process:05d}.root",
    output_directory = _mother_directory + "/InttDoublets_WithAlignP/%s_%d" % (_subfoldername, _centrality_bin),

    # ── Run metadata ─────────────────────────────────────────────────────────
    run_number  = -1, # use -1 for MC
    n_events    = -1,       # -1 → all events in each file

    # ── InttDoubletMap parameters ────────────────────────────────────────────
    output_file_name_suffix = "",
    vertexXYIncm    = (-0.05469, 0.13906),   # beam-spot (x,y) in cm
    
    data_type       = 0,                    # 2 = streaming_data
    isUsedMBDz      = False,
    
    BcoFullDiffCut  = True,
    CentralityBin   = _centrality_bin,
    isMinBiasCut    = True,
    isTriggerSel    = False,
    isMBDChargeCut  = (False, (0, 10000)),
    isBunchNumber_cut = (False, (-10, 1000)),
    
    vtxZReweight    = True,        # plain bool: True = enable vtxZ reweighting
    # zvtx_weight_dir  = _mother_directory + "/VtxZDist/completed/vtxZ_comp_NoVtxZQA",           # only needed when vtxZReweight=True
    zvtx_weight_dir  = _mother_directory + "/VtxZDist/completed/AlignP_vtxZ_comp_NoVtxZQA",           # only needed when vtxZReweight=True
    zvtx_weight_file = "INTTvtxZReWeight.root",
    zvtx_weight_hist = "HIJING_noZWeight_NoVtxZQA_Inclusive70_tight",
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
    n_condor_jobs       = 800,
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
