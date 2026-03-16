// EventInterval.C
// -----------------------------------------------------------------------
// Samples every 20th file from the EvtVtxZ run-82391 dataset,
// reads InttBcoFullDiff_next (unit: 106 ns = 1 BCO clock tick),
// computes the average interval per file, converts to an event-rate
// frequency (Hz), and draws Frequency vs. file-index on a TCanvas
// saved as EventInterval.pdf.
// -----------------------------------------------------------------------

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TROOT.h>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

void EventInterval()
{
    // ------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------
    const std::string input_dir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/"
        "dNdEtaOO/test_82391_INTTsurveyOnly/EvtVtxZ/completed/";
    const std::string file_prefix =
        "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_";
    const std::string file_suffix = ".root";

    const int    file_first  = 0;
    const int    file_last   = 499;
    const int    file_step   = 10;          // sample every 20 files
    const double bco_ns      = 106.0;       // 1 BCO tick = 106 ns

    const std::string out_pdf = "EventInterval.pdf";

    // ------------------------------------------------------------------
    // Loop over sampled files
    // ------------------------------------------------------------------
    std::vector<double> v_index;      // x-axis: file index (0, 20, 40, …)
    std::vector<double> v_freq;       // y-axis: frequency in Hz

    // Global counters across all sampled files (min-bias events only)
    Long64_t g_total   = 0;   // all min-bias events
    Long64_t g_le61    = 0;   // InttBcoFullDiff_next in [0, 61]
    Long64_t g_le80    = 0;   // InttBcoFullDiff_next in [0, 80]

    for (int idx = file_first; idx <= file_last; idx += file_step)
    {
        // Build zero-padded filename  e.g. 00082391_00000.root
        char padded[16];
        snprintf(padded, sizeof(padded), "%05d", idx);
        std::string fname = input_dir + file_prefix + padded + file_suffix;

        TFile *f = TFile::Open(fname.c_str(), "READ");
        if (!f || f->IsZombie())
        {
            std::cerr << "[WARN] Cannot open " << fname << " — skipping.\n";
            if (f) delete f;
            continue;
        }

        TTree *tree = dynamic_cast<TTree*>(f->Get("EventTree"));
        if (!tree)
        {
            std::cerr << "[WARN] No EventTree in " << fname << " — skipping.\n";
            f->Close(); delete f;
            continue;
        }

        // Attach only the needed branches
        Int_t bco_diff   = 0;
        Bool_t is_min_bias = false;
        tree->SetBranchStatus("*", 0);
        tree->SetBranchStatus("InttBcoFullDiff_next", 1);
        tree->SetBranchStatus("is_min_bias",          1);
        tree->SetBranchAddress("InttBcoFullDiff_next", &bco_diff);
        tree->SetBranchAddress("is_min_bias",          &is_min_bias);

        Long64_t nEntries = tree->GetEntries();
        if (nEntries == 0)
        {
            std::cerr << "[WARN] Empty tree in " << fname << " — skipping.\n";
            f->Close(); delete f;
            continue;
        }

        double sum = 0.0;
        Long64_t count = 0;
        for (Long64_t iev = 0; iev < nEntries; ++iev)
        {
            tree->GetEntry(iev);
            if (is_min_bias != 1) continue;   // require minimum-bias
            if (bco_diff <= 0 || bco_diff > 100000) continue;   // skip unphysical values

            // Global fraction counters (all min-bias, no upper cut)
            ++g_total;
            if (bco_diff >= 0 && bco_diff <= 61)  ++g_le61;
            if (bco_diff >= 0 && bco_diff <= 80)  ++g_le80;

            sum += static_cast<double>(bco_diff);
            ++count;
        }

        f->Close();
        delete f;

        if (count == 0)
        {
            std::cerr << "[WARN] All entries non-positive in " << fname << " — skipping.\n";
            continue;
        }

        double avg_bco   = sum / static_cast<double>(count);
        double avg_ns    = avg_bco * bco_ns;           // average interval in ns
        double freq_hz   = 1.0e9 / avg_ns;             // Hz  (1 s = 1e9 ns)

        std::cout << Form("[INFO] idx=%3d  nEvt=%lld  <BCO>=%.2f  <dt>=%.2f ns  f=%.2f kHz",
                          idx, count, avg_bco, avg_ns, freq_hz / 1.0e3)
                  << "\n";

        v_index.push_back(static_cast<double>(idx));
        v_freq.push_back(freq_hz / 1.0e3);   // store in kHz for readability
    }

    // ------------------------------------------------------------------
    // Global fraction summary
    // ------------------------------------------------------------------
    std::cout << "\n[SUMMARY] Over all sampled files (min-bias events only):\n";
    std::cout << Form("  Total min-bias events         : %lld", g_total) << "\n";
    std::cout << Form("  BCO in [0, 61]  count/fraction: %lld / %.4f",
                      g_le61, g_total > 0 ? (double)g_le61 / g_total : 0.0) << "\n";
    std::cout << Form("  BCO in [0, 80]  count/fraction: %lld / %.4f",
                      g_le80, g_total > 0 ? (double)g_le80 / g_total : 0.0) << "\n";

    if (v_index.empty())
    {
        std::cerr << "[ERROR] No valid files processed. No plot produced.\n";
        return;
    }

    // ------------------------------------------------------------------
    // Plot
    // ------------------------------------------------------------------
    gStyle->SetOptStat(0);
    gStyle->SetPadLeftMargin(0.14);
    gStyle->SetPadBottomMargin(0.12);

    TCanvas *c = new TCanvas("c_EventInterval", "Event Rate vs File Index", 900, 600);
    c->SetGrid();

    TGraph *gr = new TGraph(
        static_cast<int>(v_index.size()),
        v_index.data(),
        v_freq.data()
    );

    gr->SetTitle("Event Rate vs. File Index (Run 82391)");
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(0.9);
    gr->SetMarkerColor(kBlue + 1);
    gr->SetLineColor(kBlue + 1);
    gr->SetLineWidth(1);

    gr->GetXaxis()->SetTitle("File Index");
    gr->GetYaxis()->SetTitle("Event Rate [kHz]");
    gr->GetXaxis()->SetTitleSize(0.05);
    gr->GetYaxis()->SetTitleSize(0.05);
    gr->GetXaxis()->SetLabelSize(0.04);
    gr->GetYaxis()->SetLabelSize(0.04);

    gr->Draw("APL");

    TLatex tex;
    tex.SetNDC();
    tex.SetTextSize(0.035);
    tex.DrawLatex(0.16, 0.92, "INTT BCO Full Diff  (1 BCO = 106 ns)   run 82391");

    c->SaveAs(out_pdf.c_str());
    std::cout << "[INFO] Saved plot to " << out_pdf << "\n";
}
