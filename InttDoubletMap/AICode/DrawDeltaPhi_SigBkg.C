// DrawDeltaPhi_SigBkg.C
// 
// For each signal TH1D (h1D_DeltaPhi_Phibin*_Eta*_VtxZ*) in the input file,
// find the matching background histogram (same name + "_rotated"),
// draw both on a TCanvas, and write the canvas to an output ROOT file.
//
// Input file:  /sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/
//              test_82391_INTTsurveyOnly/InttDoubletMap/baseline/completed/
//              Data_DoubletMap_BcoFullDiffCut_ClusQAAdc30PhiSize40_00082391_00323.root
//
// Output file: /sphenix/user/ChengWei/INTT/INTTdNdEtaOO/InttDoubletMap/AICode/
//              output_DeltaPhi_SigBkg.root
//
// Usage (in ROOT):
//   root -l -b -q 'DrawDeltaPhi_SigBkg.C'

void DrawDeltaPhi_SigBkg()
{
    // ---------------------------------------------------------------
    // Paths
    // ---------------------------------------------------------------
    // const char* input_path = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405/baseline/completed/Data_DoubletMap_BcoFullDiffCut_ClusQAAdc0PhiSize128_00082405_00000.root";
    // const char* input_path = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/InttDoubletMap/baseline/completed/merged_files_MC_DoubletMap_ClusQAAdc0PhiSize128__001/MC_DoubletMap_ClusQAAdc0PhiSize128_merge10files.root";
    const char* input_path = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/InttDoubletMap/baseline/completed/MC_DoubletMap_ClusQAAdc0PhiSize128_merged.root";
        // "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/"
        // "test_82391_INTTsurveyOnly/InttDoubletMap/baseline/completed/"
        // "Data_DoubletMap_BcoFullDiffCut_ClusQAAdc30PhiSize40_00082391_merged.root";


    const char* output_path =
        "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/InttDoubletMap/AICode/"
        "MCHIJING_output_DeltaPhi_SigBkg.root";

    // ---------------------------------------------------------------
    // Open input file
    // ---------------------------------------------------------------
    TFile* fin = TFile::Open(input_path, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "[ERROR] Cannot open input file: " << input_path << std::endl;
        return;
    }
    std::cout << "[INFO] Opened input file: " << input_path << std::endl;

    // ---------------------------------------------------------------
    // Open output file
    // ---------------------------------------------------------------
    TFile* fout = TFile::Open(output_path, "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "[ERROR] Cannot open output file: " << output_path << std::endl;
        fin->Close();
        return;
    }
    std::cout << "[INFO] Output will be written to: " << output_path << std::endl;

    // ---------------------------------------------------------------
    // Create h2D_DeltaPhi_DoubletYield with same binning as h2D_GoodProtoTracklet_EtaPhi
    // ---------------------------------------------------------------
    TH2D* h2D_ref = (TH2D*)fin->Get("h2D_GoodProtoTracklet_EtaPhi");
    if (!h2D_ref) {
        std::cerr << "[ERROR] Cannot find h2D_GoodProtoTracklet_EtaPhi — needed for binning." << std::endl;
        fout->Close(); fin->Close(); return;
    }

    TH2D* h2D_yield = new TH2D("h2D_DeltaPhi_DoubletYield",
                                "Doublet Yield (sig#minusbkg integral #pm0.15 rad);Phi bin;Eta bin",
                                h2D_ref->GetNbinsX(), h2D_ref->GetXaxis()->GetXmin(), h2D_ref->GetXaxis()->GetXmax(),
                                h2D_ref->GetNbinsY(), h2D_ref->GetYaxis()->GetXmin(), h2D_ref->GetYaxis()->GetXmax());
    h2D_yield->SetDirectory(0);

    // Copy axis labels if any
    h2D_yield->GetXaxis()->SetTitle(h2D_ref->GetXaxis()->GetTitle());
    h2D_yield->GetYaxis()->SetTitle(h2D_ref->GetYaxis()->GetTitle());
    h2D_yield->GetZaxis()->SetTitle("Integral of (Signal #minus Bkg) within #pm0.15 rad");

    // ---------------------------------------------------------------
    // Iterate over all keys in the input file
    // For each TH1D whose name does NOT end in "_rotated",
    // look for its "_rotated" counterpart.
    // ---------------------------------------------------------------
    TIter next(fin->GetListOfKeys());
    TKey* key = nullptr;

    int n_canvas = 0;

    while ((key = (TKey*)next()))
    {
        // Only consider TH1D objects
        if (std::string(key->GetClassName()) != "TH1D") continue;

        TString hname = key->GetName();

        // Skip the rotated (background) histograms — we'll fetch them by name
        if (hname.EndsWith("_rotated")) continue;

        // Only process the DeltaPhi histograms
        if (!hname.BeginsWith("h1D_DeltaPhi_")) continue;

        TString hname_rotated = hname + "_rotated";

        // Fetch signal histogram
        TH1D* h_sig = (TH1D*)fin->Get(hname);
        if (!h_sig) {
            std::cerr << "[WARN] Cannot fetch signal histogram: " << hname << std::endl;
            continue;
        }

        // Fetch background (rotated) histogram
        TH1D* h_bkg = (TH1D*)fin->Get(hname_rotated);
        if (!h_bkg) {
            std::cerr << "[WARN] Cannot fetch background histogram: " << hname_rotated << std::endl;
            continue;
        }

        // -----------------------------------------------------------
        // Compute signal - background
        // -----------------------------------------------------------
        TString hname_diff = hname + "_SigMinusBkg";
        TH1D* h_diff = (TH1D*)h_sig->Clone(hname_diff);
        h_diff->SetDirectory(0);   // detach from input file
        h_diff->Add(h_bkg, -1.0); // h_diff = h_sig - h_bkg

        // -----------------------------------------------------------
        // Parse Phibin and Eta indices from hname
        // Name format: h1D_DeltaPhi_Phibin{N}_Eta{M}_VtxZ{K}
        // -----------------------------------------------------------
        int phibin_idx = -1, eta_idx = -1;
        sscanf(hname.Data(), "h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ", &phibin_idx, &eta_idx);

        std::cout<<"reading TH1D: "<<hname<<" phibin: "<<phibin_idx<<" eta: "<<eta_idx<<std::endl;

        // -----------------------------------------------------------
        // Integrate h_diff within ±0.15 rad and fill DoubletYield TH2D
        // TH1D::Integral(bin1, bin2) sums over bin centers in [xlow, xhigh]
        // -----------------------------------------------------------
        if (phibin_idx >= 0 && eta_idx >= 0) {
            int bin_lo = h_diff->GetXaxis()->FindBin(-0.15);
            int bin_hi = h_diff->GetXaxis()->FindBin( 0.15);
            double integral = h_diff->Integral(bin_lo, bin_hi);

            // In the TH2D: X axis = Phibin, Y axis = Eta
            // Use bin number = index + 1  (ROOT bins are 1-indexed)
            int xbin = eta_idx   + 1;
            int ybin = phibin_idx + 1;
            h2D_yield->SetBinContent(xbin, ybin, integral);
        } else {
            std::cerr << "[WARN] Could not parse Phibin/Eta from: " << hname << std::endl;
        }


        // -----------------------------------------------------------
        // Create TCanvas
        // -----------------------------------------------------------
        TString canvas_name  = "c_" + hname;
        TString canvas_title = hname + " Signal vs Background";

        TCanvas* c = new TCanvas(canvas_name, canvas_title, 800, 600);
        c->SetLeftMargin(0.12);
        c->SetBottomMargin(0.12);

        // --- Style: signal (blue solid) ---
        h_sig->SetLineColor(kBlue + 1);
        h_sig->SetLineWidth(2);
        h_sig->SetFillColorAlpha(kBlue - 9, 0.35);
        h_sig->SetStats(0);
        h_sig->SetTitle(canvas_title);
        h_sig->GetXaxis()->SetTitle("#Delta#phi");
        h_sig->GetYaxis()->SetTitle("Counts");

        // --- Style: background (red dashed) ---
        h_bkg->SetLineColor(kRed + 1);
        h_bkg->SetLineWidth(2);
        h_bkg->SetLineStyle(2);
        h_bkg->SetFillColorAlpha(kRed - 9, 0.25);
        h_bkg->SetStats(0);

        // --- Style: signal - background (green) ---
        h_diff->SetLineColor(kGreen + 2);
        h_diff->SetLineWidth(2);
        h_diff->SetLineStyle(1);
        h_diff->SetFillStyle(0);
        h_diff->SetStats(0);

        // Auto-scale Y to accommodate all three histograms
        double ymax = std::max({h_sig->GetMaximum(), h_bkg->GetMaximum(), h_diff->GetMaximum()});
        double ymin = std::min({0.0, h_diff->GetMinimum()});
        h_sig->SetMaximum(ymax * 1.25);
        h_sig->SetMinimum(ymin < 0 ? ymin * 1.25 : 0.0);

        h_sig->Draw("HIST");
        h_bkg->Draw("HIST SAME");
        h_diff->Draw("HIST SAME");

        // --- Legend ---
        TLegend* leg = new TLegend(0.62, 0.68, 0.92, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->AddEntry(h_sig,  "Signal",              "lf");
        leg->AddEntry(h_bkg,  "Background (rotated)", "lf");
        leg->AddEntry(h_diff, "Signal #minus Bkg",    "l");
        leg->Draw();

        c->Update();

        // -----------------------------------------------------------
        // Write canvas to output file
        // -----------------------------------------------------------
        fout->cd();
        c->Write();

        // Clean up to avoid memory build-up over many histograms
        delete leg;
        delete h_diff;
        delete c;

        n_canvas++;
    }

    // ---------------------------------------------------------------
    // Write h2D_DeltaPhi_DoubletYield to output file
    // ---------------------------------------------------------------
    fout->cd();
    h2D_yield->Write();
    delete h2D_yield;
    std::cout << "[INFO] Wrote h2D_DeltaPhi_DoubletYield to output file." << std::endl;

    // ---------------------------------------------------------------
    // TH2D subtraction: h2D_GoodProtoTracklet_EtaPhi - _rotated
    // ---------------------------------------------------------------
    TH2D* h2D_sig = (TH2D*)fin->Get("h2D_GoodProtoTracklet_EtaPhi");
    TH2D* h2D_bkg = (TH2D*)fin->Get("h2D_GoodProtoTracklet_EtaPhi_rotated");

    if (h2D_sig && h2D_bkg) {
        TH2D* h2D_diff = (TH2D*)h2D_sig->Clone("h2D_GoodProtoTracklet_EtaPhi_SigMinusBkg");
        h2D_diff->SetDirectory(0);
        h2D_diff->Add(h2D_bkg, -1.0);
        h2D_diff->SetTitle("h2D GoodProtoTracklet EtaPhi (Signal #minus Background)");
        h2D_diff->GetXaxis()->SetTitle(h2D_sig->GetXaxis()->GetTitle());
        h2D_diff->GetYaxis()->SetTitle(h2D_sig->GetYaxis()->GetTitle());
        h2D_diff->GetZaxis()->SetTitle("Signal #minus Background");

        fout->cd();
        h2D_diff->Write();
        delete h2D_diff;

        std::cout << "[INFO] Wrote h2D_GoodProtoTracklet_EtaPhi_SigMinusBkg to output file." << std::endl;
    } else {
        std::cerr << "[WARN] Could not find h2D_GoodProtoTracklet_EtaPhi pair — skipping TH2D subtraction." << std::endl;
    }

    // ---------------------------------------------------------------
    // 1-D Eta yield histograms
    // ---------------------------------------------------------------
    {
        // Fetch the needed histograms from the input file
        TH1D* h_pt_sig = (TH1D*)fin->Get("h1D_GoodProtoTracklet_Eta");
        TH1D* h_pt_bkg = (TH1D*)fin->Get("h1D_GoodProtoTracklet_Eta_rotated");
        TH1D* h_nEvt   = (TH1D*)fin->Get("h1D_nEvent");
        TH1D* h_truth  = (TH1D*)fin->Get("h1D_Truth_ChargedHadron_Eta");

        if (!h_pt_sig || !h_pt_bkg || !h_nEvt || !h_truth) {
            std::cerr << "[WARN] Missing one or more histograms for eta-yield calculation — skipping." << std::endl;
        } else {
            double nEvent = h_nEvt->GetBinContent(1);
            if (nEvent <= 0) {
                std::cerr << "[WARN] h1D_nEvent bin(1) <= 0 (" << nEvent << ") — skipping eta-yield." << std::endl;
            } else {
                // ------------------------------------------------------------------
                // Hist1: (sig - bkg) / bin-width,  per eta bin
                // ------------------------------------------------------------------
                TH1D* h_EtaYield_per_BinWidth =
                    (TH1D*)h_pt_sig->Clone("h1D_EtaYield_SigMinusBkg_per_BinWidth");
                h_EtaYield_per_BinWidth->SetDirectory(0);
                h_EtaYield_per_BinWidth->Add(h_pt_bkg, -1.0);   // sig - bkg
                // Divide each bin by its width
                for (int ib = 1; ib <= h_EtaYield_per_BinWidth->GetNbinsX(); ++ib) {
                    double bw = h_EtaYield_per_BinWidth->GetBinWidth(ib);
                    h_EtaYield_per_BinWidth->SetBinContent(ib, h_EtaYield_per_BinWidth->GetBinContent(ib) / bw);
                    h_EtaYield_per_BinWidth->SetBinError  (ib, h_EtaYield_per_BinWidth->GetBinError(ib)   / bw);
                }
                h_EtaYield_per_BinWidth->SetTitle("(Signal #minus Bkg) / Bin Width;#eta;Counts / Bin Width");

                // ------------------------------------------------------------------
                // Hist2: Hist1 / nEvent
                // ------------------------------------------------------------------
                TH1D* h_EtaYield_per_BinWidth_per_Event =
                    (TH1D*)h_EtaYield_per_BinWidth->Clone("h1D_EtaYield_SigMinusBkg_per_BinWidth_per_Event");
                h_EtaYield_per_BinWidth_per_Event->SetDirectory(0);
                h_EtaYield_per_BinWidth_per_Event->Scale(1.0 / nEvent);
                h_EtaYield_per_BinWidth_per_Event->SetTitle("(Signal #minus Bkg) / Bin Width / N_{evt};#eta;Per event INTT signal doublets");

                // ------------------------------------------------------------------
                // Hist3: Truth / bin-width / nEvent
                // ------------------------------------------------------------------
                TH1D* h_Truth_dNdEta =
                    (TH1D*)h_truth->Clone("h1D_Truth_dNdEta");
                h_Truth_dNdEta->SetDirectory(0);
                for (int ib = 1; ib <= h_Truth_dNdEta->GetNbinsX(); ++ib) {
                    double bw = h_Truth_dNdEta->GetBinWidth(ib);
                    h_Truth_dNdEta->SetBinContent(ib, h_Truth_dNdEta->GetBinContent(ib) / bw / nEvent);
                    h_Truth_dNdEta->SetBinError  (ib, h_Truth_dNdEta->GetBinError(ib)   / bw / nEvent);
                }
                h_Truth_dNdEta->SetTitle("MC Truth dN/d#eta / N_{evt};#eta;Per event Charged hadron count");

                // ------------------------------------------------------------------
                // Write all three histograms to the output file
                // ------------------------------------------------------------------
                fout->cd();
                h_EtaYield_per_BinWidth->Write();
                h_EtaYield_per_BinWidth_per_Event->Write();
                h_Truth_dNdEta->Write();
                std::cout << "[INFO] Wrote h1D_EtaYield_SigMinusBkg_per_BinWidth, "
                          << "h1D_EtaYield_SigMinusBkg_per_BinWidth_per_Event, "
                          << "h1D_Truth_dNdEta to output file." << std::endl;

                // ------------------------------------------------------------------
                // Canvas: Hist2 (reco dN/deta) vs Hist3 (truth dN/deta)
                // ------------------------------------------------------------------
                TCanvas* cEta = new TCanvas("c_EtaYield_RecoVsTruth",
                                            "Reco dN/d#eta vs Truth dN/d#eta", 800, 600);
                cEta->SetLeftMargin(0.13);
                cEta->SetBottomMargin(0.12);

                // Style: Hist2 (reco) — blue
                h_EtaYield_per_BinWidth_per_Event->SetLineColor(kBlue + 1);
                h_EtaYield_per_BinWidth_per_Event->SetLineWidth(2);
                h_EtaYield_per_BinWidth_per_Event->SetMarkerColor(kBlue + 1);
                h_EtaYield_per_BinWidth_per_Event->SetMarkerStyle(20);
                h_EtaYield_per_BinWidth_per_Event->SetMarkerSize(0.8);
                h_EtaYield_per_BinWidth_per_Event->SetStats(0);

                // Style: Hist3 (truth) — red dashed
                h_Truth_dNdEta->SetLineColor(kRed + 1);
                h_Truth_dNdEta->SetLineWidth(2);
                h_Truth_dNdEta->SetLineStyle(2);
                h_Truth_dNdEta->SetMarkerColor(kRed + 1);
                h_Truth_dNdEta->SetMarkerStyle(24);
                h_Truth_dNdEta->SetMarkerSize(0.8);
                h_Truth_dNdEta->SetStats(0);

                // Fixed axis ranges
                double xmin_eta = h_truth->GetXaxis()->GetXmin();
                double xmax_eta = h_truth->GetXaxis()->GetXmax();
                h_EtaYield_per_BinWidth_per_Event->SetMaximum(25.0);
                h_EtaYield_per_BinWidth_per_Event->SetMinimum(0.0);

                h_EtaYield_per_BinWidth_per_Event->GetXaxis()->SetTitle("#eta");
                h_EtaYield_per_BinWidth_per_Event->GetXaxis()->SetRangeUser(xmin_eta, xmax_eta);
                h_EtaYield_per_BinWidth_per_Event->GetYaxis()->SetTitle("Per event yields");

                h_EtaYield_per_BinWidth_per_Event->Draw("E P");
                h_Truth_dNdEta->Draw("E P SAME");

                TLegend* legEta = new TLegend(0.55, 0.72, 0.92, 0.88);
                legEta->SetBorderSize(0);
                legEta->SetFillStyle(0);
                legEta->AddEntry(h_EtaYield_per_BinWidth_per_Event, "Reco (Sig#minusBkg)/N_{evt}", "lp");
                legEta->AddEntry(h_Truth_dNdEta,                    "MC Truth/N_{evt}",             "lp");
                legEta->Draw();

                cEta->Update();
                fout->cd();
                cEta->Write();
                std::cout << "[INFO] Wrote canvas c_EtaYield_RecoVsTruth to output file." << std::endl;

                delete legEta;
                delete cEta;
                delete h_EtaYield_per_BinWidth;
                delete h_EtaYield_per_BinWidth_per_Event;
                delete h_Truth_dNdEta;
            }
        }
    }

    // ---------------------------------------------------------------
    // Save h1D_nEvent directly to output file
    // ---------------------------------------------------------------
    TH1D* h1D_nEvent = (TH1D*)fin->Get("h1D_nEvent");
    if (h1D_nEvent) {
        h1D_nEvent->SetDirectory(0);
        fout->cd();
        h1D_nEvent->Write();
        std::cout << "[INFO] Wrote h1D_nEvent to output file." << std::endl;
    } else {
        std::cerr << "[WARN] Could not find h1D_nEvent — skipping." << std::endl;
    }

    // ---------------------------------------------------------------
    // Close files
    // ---------------------------------------------------------------
    fout->Close();
    fin->Close();

    std::cout << "[INFO] Done! Wrote " << n_canvas
              << " canvases to: " << output_path << std::endl;
}
