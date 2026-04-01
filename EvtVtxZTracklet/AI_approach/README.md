# EvtVtxZProtoTracklet_MultiVtx — Workflow Description

Standalone class that reconstructs **multiple INTT z-vertices per event**.
Based on `EvtVtxZProtoTracklet` (all code copied; no inheritance).

---

## Differences vs. `EvtVtxZProtoTracklet`

| Feature | EvtVtxZProtoTracklet | EvtVtxZProtoTracklet_MultiVtx |
|---|---|---|
| Vertices per event | 1 | N (iterative) |
| Scalar branches | `INTTvtxZ`, `INTTvtxZError`, … | Same (= first vertex) |
| New vector branches | — | `vec_INTTvtxZ`, `vec_INTTvtxZError`, `vec_NgroupTrapezoidal`, `vec_NgroupCoarse`, `vec_TrapezoidalFitWidth`, `vec_TrapezoidalFWHM`, `N_INTT_VtxZ` |

---

## Per-Event Workflow

```
MainProcess()
│
├── EvtCleanUp()          — reset all histograms, cluster vectors, multi-vertex state
├── PrepareClusterVec()   — fill inner/outer PostCut cluster vectors from ClusX/Y/Z branches
│
└── GetAllINTTvtxZ()
    │
    ├── Step 1 — GetINTTvtxZ()              [first vertex, identical to original class]
    │   ├── Build phi-maps from PostCut clusters
    │   ├── Form proto-tracklets: loop over inner ×  outer (±5 phi-cells)
    │   │     Apply ΔΦ cut  +  DCA cut (normal values)
    │   │     Fill evt_possible_z (coarse) and line_breakdown_hist (fine, trapezoidal)
    │   ├── Edge rejection (zero out |Z| > 50 cm)
    │   ├── find_Ngroup() on both histograms
    │   └── 7-Gaussian fits → out_INTTvtxZ = mean of fit means
    │                          out_INTTvtxZError = std dev of fit means
    │
    ├── Push first vertex into vec_INTTvtxZ[0], vec_INTTvtxZError[0], …
    │
    └── while (true)  ──── iteration for vertex N ────────────────────────────
        │
        ├── MarkAssociatedClusters(vtxZ, vtxZError)
        │     For every unused inner × outer pair within ±5 phi-cells:
        │       (a) ΔΦ cut   ×2.5 loose
        │       (b) DCA cut  ×2.5 loose
        │       (c) z-range overlap:  z_mid − z_half ≤ vtxZ ≤ z_mid + z_half
        │     If all three pass → mark inner.index & outer.index as USED
        │     (used sets accumulate across all iterations; one cluster ↔ one vertex)
        │
        ├── Build remaining_inner, remaining_outer
        │     (PostCut clusters whose .index is NOT in used sets)
        │
        ├── RunSingleVtxReco(remaining_inner, remaining_outer, vtx_index, eID)
        │     ├── Create local TH1D* local_lbd + local_evtz  (same bin specs as parent)
        │     ├── Build local phi-maps from remaining clusters
        │     ├── Proto-tracklet loop with NORMAL ΔΦ + DCA cuts
        │     │     Fill local_evtz and local_lbd via trapezoidal_line_breakdown
        │     ├── Edge rejection on local_lbd
        │     ├── STOP if  peak bin content ≤ 0.05  → return valid=false
        │     ├── find_Ngroup() + 7-Gaussian fits on local_lbd
        │     ├── Optional: draw canvas  eID_%d_VtxN_%d_EvtZDist[_ZoomIn]
        │     └── return VtxFitResult {vtxZ, vtxZError, Ngroup*, FitWidth, FWHM, valid=true}
        │
        ├── if (!res.valid)  →  break
        │
        ├── Push result into vec_INTTvtxZ[N], vec_INTTvtxZError[N], …
        ├── current_vtxZ = res.vtxZ   (next iteration filters against this vertex)
        └── vtx_index++
    
    N_INTT_VtxZ = vec_INTTvtxZ.size()
    Scalar branch INTTvtxZ = vec_INTTvtxZ[0]  (set by GetINTTvtxZ, unchanged)
```

---

## Key Constants

| Constant | Value | Purpose |
|---|---|---|
| `assoc_cut_factor` | 2.5 | Multiplier for loose ΔΦ / DCA cuts in `MarkAssociatedClusters` |
| `min_peak_content` | 0.05 | Stop iterating when histogram peak ≤ this value |

---

## Compile

```bash
# from EvtVtxZTracklet/
source /opt/sphenix/core/bin/sphenix_setup.sh -n
g++ -std=c++17 -I. -I.. $(root-config --cflags) \
    -c AI_approach/EvtVtxZProtoTracklet_MultiVtx.cpp \
    -o AI_approach/EvtVtxZProtoTracklet_MultiVtx.o
```

## Minimal Usage

```cpp
#include "AI_approach/EvtVtxZProtoTracklet_MultiVtx.h"

auto* reco = new EvtVtxZProtoTracklet_MultiVtx(
    0, 82391, -1,                       // process_id, runnumber, nEvents (-1 = all)
    input_dir,  input_file,
    output_dir, "_MultiVtx",
    {0., 0.},                           // beam XY in cm
    true,  true,                        // IsFieldOn, IsDCACutApplied
    {{-1.,1.},{-1000.,1000.}},          // DeltaPhiCutInDegree
    {{-1.,1.},{-1000.,1000.}}           // DCAcutIncm
);
reco->MainProcess();
reco->EndRun();
```

After running, open the output ROOT file and check:
```
vec_INTTvtxZ        — Z positions of all reconstructed vertices
N_INTT_VtxZ        — number of vertices found per event
vec_INTTvtxZ[0]    — always equals the scalar branch INTTvtxZ
```
