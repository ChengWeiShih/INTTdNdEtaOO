"""
pass_configs/data_82405_baseline.py
====================================
Example configuration for a real-data pass on run 82405.

Edit the fields below to match your setup, then run:

  python3 run_pass.py pass_configs/data_82405_baseline.py --dry-run
  python3 run_pass.py pass_configs/data_82405_baseline.py --step all
"""

import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from PassConfig import PassConfig

cfg = PassConfig(

    # ── Pass identification ──────────────────────────────────────────────────
    pass_name       = "data_82405_baseline",
    notes           = "Baseline streaming-data pass, run 82405, no cuts",

    # ── I/O ─────────────────────────────────────────────────────────────────
    input_directory  = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405",
    input_file_name  = "82405_converter_{process:05d}.root",
    output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405/InttDoublets",

    # ── Run metadata ─────────────────────────────────────────────────────────
    run_number  = 82405,
    n_events    = -1,       # -1 → all events in each file

    # ── InttDoubletMap parameters ────────────────────────────────────────────
    output_file_name_suffix = "",
    vertexXYIncm    = (-0.03677, 0.1454),   # beam-spot (x,y) in cm
    data_type       = 2,                    # 2 = streaming_data
    isUsedMBDz      = False,
    BcoFullDiffCut  = False,
    CentralityBin   = 100,
    isMinBiasCut    = False,
    isTriggerSel    = False,
    isMBDChargeCut  = (False, (0, 0)),
    isBunchNumber_cut = (False, (0, 0)),
    vtxZReweight    = False,        # plain bool: True = enable vtxZ reweighting
    zvtx_weight_dir  = "",           # only needed when vtxZReweight=True
    zvtx_weight_file = "",
    zvtx_weight_hist = "",
    INTT_vtxZ_QA   = False,
    VtxZRange       = (-10.0, 10.0),
    ColMulMask      = False,
    isClusQA        = (True, (0.0, 128.0)),  # data: no cuts baseline
    DeltaPhiCut     = 0.15,
    HaveGeoOffsetTag = False,

    # ── ColMulMask map (only needed when ColMulMask=True) ─────────────────────
    # ColMulMask_map_dir  = "/path/to/ColumnCheck/baseline/completed/MulMap/completed",
    # ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin55_VtxZ-30to30cm_ClusQAAdc30PhiSize40_00082391.root",
    ColMulMask_map_dir  = "",
    ColMulMask_map_file = "",

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
