# InttDoubletMap AI Framework

A systematic, reproducible way to run multiple **InttDoubletMap analysis passes** — each combining HTCondor batch jobs, a ROOT `hadd` merge, and the analysis macro — with a single Python command.

---

## Directory Layout

```
AI_framework/
├── PassConfig.py          ← dataclass with every InttDoubletMap parameter
├── run_pass.py            ← master orchestrator (generate macro → submit → merge → analyse)
├── pass_configs/          ← one .py file per analysis pass
│   ├── data_82405_baseline.py
│   ├── MC_HIJING_baseline.py
│   └── ...                ← add your own here
└── README.md

<output_directory>/        ← created automatically per pass
├── generated_macro/
│   └── Run_PrepareHist_<pass_name>.C   ← auto-generated with all parameters baked in
├── run_condor_<pass_name>.job
├── completed/             ← per-job output root files land here
└── cw_log/                ← condor stdout / stderr / logs
```

---

## Concept: one pass = three steps

| Step | What happens |
|------|-------------|
| **1** | `run_pass.py` generates a fully self-contained `Run_PrepareHist_<pass_name>.C` with **all PassConfig values baked in as C++ literals**, writes a custom `run_condor_<pass_name>.job` pointing to it, and submits the HTCondor jobs. The original `Run_PrepareHist.C` is **never modified**. |
| **2** | The framework polls until ≥ 95 % of jobs have finished, then calls `run_random_merge` (→ `Run_RandomMerge.sh` → `RandomMerge.C` → `hadd`) to produce a single merged `.root` file. |
| **3** | `InttDoubletMap.C` is run on the merged file, producing `Output_<merged_name>.root` in the same directory. |

---

## How PassConfig parameters reach InttDoubletMap

The HTCondor `run_job.sh` wrapper only passes 6 plain arguments to ROOT
(`process_id`, `run_num`, `nEvents`, `output_dir`, `input_dir`, `input_file`).
There is no way to forward complex C++ types like `std::pair<bool,std::pair<int,int>>`
through a shell command line.

The framework solves this by **generating a dedicated ROOT macro** for each pass:

```
PassConfig (Python)
  └─ run_pass.py  →  generates  Run_PrepareHist_<pass_name>.C
                      (all parameters are C++ literals inside this file)
       ↓
condor job  →  run_job.sh  →  root.exe Run_PrepareHist_<pass_name>.C(process,...)
                                  ↓
                           new InttDoubletMap(..., isTriggerSel_in, DeltaPhiCut_in, ...)
```

The generated macro is saved in `<output_directory>/generated_macro/` so you always
have a complete, reproducible record of exactly what parameters each pass ran with.
The original `Run_PrepareHist_template.C` is the source of truth and is **never touched**.

### vtxZReweight — plain bool design

The template uses a **plain `bool` flag** for `vtxZReweight_in`, then constructs
`std::pair<int, TH1D*> vtxZReweight_final` internally:

```cpp
bool vtxZReweight_in = false;  // ← baked in from PassConfig.vtxZReweight
std::pair<int, TH1D*> vtxZReweight_final = {vtxZReweight_in, nullptr};
if (vtxZReweight_final.first) {
  vtxZReweight_final.second = GetTH1D(zvtx_weight_dir, zvtx_weight_file, zvtx_weight_hist);
}
if (vtxZReweight_final.first && vtxZReweight_final.second == nullptr) { return 666; }
```

In the PassConfig you only set:
```python
vtxZReweight    = True,          # or False
zvtx_weight_dir  = "/path/to/VtxZDist/completed",
zvtx_weight_file = "INTTvtxZReWeight.root",
zvtx_weight_hist = "HIJING_noZWeight_NoVtxZQA_Inclusive70_tight",
```

---

## Quick start

### 1 – Copy and edit a config

```bash
cp pass_configs/data_82405_baseline.py pass_configs/my_new_pass.py
# Edit my_new_pass.py – at minimum change:
#   pass_name, input_directory, input_file_name, output_directory
```

### 2 – Dry-run to verify

```bash
cd /sphenix/user/ChengWei/INTT/INTTdNdEtaOO/AI_framework
python3 run_pass.py pass_configs/my_new_pass.py --dry-run
```

### 3 – Submit everything (full pass)

```bash
python3 run_pass.py pass_configs/my_new_pass.py --step all
```

The script will block on Step 2 polling until enough jobs finish, then proceed automatically.

### Run individual steps

```bash
# Only submit condor jobs
python3 run_pass.py pass_configs/my_new_pass.py --step 1

# Only merge (jobs already done, no polling)
python3 run_pass.py pass_configs/my_new_pass.py --step 2 --no-poll

# Only run InttDoubletMap.C on an existing merged file
python3 run_pass.py pass_configs/my_new_pass.py --step 3 \
    --merged-file /path/to/merged.root
```

---

## All CLI flags

```
python3 run_pass.py <config.py> [options]

  --step {1,2,3,all}      Which step(s) to run (default: all)
  --dry-run               Print commands without executing
  --merged-file PATH      Explicit merged file path (for --step 3)
  --no-poll               Skip waiting for condor jobs (for --step 2)
  --min-fraction FLOAT    Fraction of jobs needed before merging (default: 0.95)
  --poll-interval INT     Seconds between completion polls (default: 120)
```

---

## PassConfig reference

Every field in `PassConfig` maps 1-to-1 to the `InttDoubletMap.h` constructor.

### Required fields (no default)

| Field | Type | Description |
|-------|------|-------------|
| `pass_name` | `str` | Unique label for this pass |
| `input_directory` | `str` | Directory of input root files |
| `input_file_name` | `str` | Filename template; use `{process:05d}` for the job index |
| `output_directory` | `str` | Base output directory |

### Analysis parameters

| Field | Default | Maps to |
|-------|---------|---------|
| `run_number` | `-1` | `runnumber_in` |
| `n_events` | `-1` | `run_nEvents_in` |
| `output_file_name_suffix` | `""` | `output_file_name_suffix_in` |
| `vertexXYIncm` | `(-0.03677, 0.1454)` | `vertexXYIncm_in` |
| `data_type` | `2` | `data_type_in` (0=pure_trig, 1=stream_trig, 2=stream_data) |
| `isUsedMBDz` | `False` | `isUsedMBDz_in` |
| `BcoFullDiffCut` | `True` | `BcoFullDiffCut_in` |
| `CentralityBin` | `1` | `CentralityBin_in` (1 = inclusive) |
| `isMinBiasCut` | `True` | `isMinBiasCut_in` |
| `isTriggerSel` | `False` | `isTriggerSel_in` |
| `isMBDChargeCut` | `(False,(0,10000))` | `isMBDChargeCut_in` |
| `isBunchNumber_cut` | `(False,(-10,1000))` | `isBunchNumber_cut_in` |
| `vtxZReweight` | `False` | plain `bool` → `vtxZReweight_in`; the macro builds `pair<int,TH1D*>` internally |
| `zvtx_weight_dir` | `""` | directory for `GetTH1D()` (only used when `vtxZReweight=True`) |
| `zvtx_weight_file` | `""` | root filename for `GetTH1D()` |
| `zvtx_weight_hist` | `""` | TH1D name inside the file |
| `INTT_vtxZ_QA` | `False` | `INTT_vtxZ_QA_in` |
| `VtxZRange` | `(-10,10)` | `VtxZRange_in` |
| `ColMulMask` | `True` | `ColMulMask_in` |
| `ColMulMask_map_dir` | `""` | directory of the MulMap root file |
| `ColMulMask_map_file` | `""` | filename of the MulMap root file |
| `isClusQA` | `(True,(30,40))` | `isClusQA_in` — `(adc_cut, phi_size_cut)` |
| `DeltaPhiCut` | `0.15` | `DeltaPhiCut_in` |
| `HaveGeoOffsetTag` | `False` | `HaveGeoOffsetTag_in` |

### Merge parameters (`run_random_merge`)

| Field | Default | Description |
|-------|---------|-------------|
| `merge_is_shuffle` | `False` | `IsShuffle` arg |
| `merge_n_files` | `1` | `N_merged_files` arg |
| `merge_mega_merge` | `False` | `mega_merge` arg |
| `merge_filename_template` | `""` | Explicit input filename; empty = auto-detect |

### HTCondor parameters

| Field | Default | Description |
|-------|---------|-------------|
| `n_condor_jobs` | `1000` | `Queue N` |
| `job_offset` | `0` | Starting process-ID offset |
| `condor_request_memory` | `"4GB"` | Per-job memory |
| `condor_priority` | `20` | Condor priority |
| `condor_concurrency_limit` | `100` | Max simultaneous jobs |

---

## Adding a new pass

1. Copy an existing config from `pass_configs/`
2. Change `pass_name` and the I/O paths
3. Adjust any analysis parameters that differ from the default
4. Run with `--dry-run` to preview, then `--step all` to execute

---

## Notes

- `run_random_merge` is a bash function defined in `~/.bash_profile`; the orchestrator sources it automatically.
- The merged file is produced by `RandomMerge.C` as `<prefix>_merged.root` (for `merge_n_files=1`) in the `completed/` directory.
- `InttDoubletMap.C` auto-derives its output path as `Output_<merged_basename>.root` in the same directory.
