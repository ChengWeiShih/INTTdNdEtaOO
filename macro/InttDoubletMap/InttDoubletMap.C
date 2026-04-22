#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TString.h>
#include <TSystem.h>
#include <iostream>

void InttDoubletMap(TString inputPath = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/CarryOverHit/82420/InttDoublets/FPHX_range_10/completed/Data_INTTDoublets_StreamTrig_FphxBcoRange106to116_VtxZRangeN15to15_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082420_merged.root")
{
    // =====================================================================
    // Input / Output setup
    // =====================================================================
    // TString inputPath = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405/InttDoublets/baseline/completed/Data_INTTDoublets_Streaming_VtxZRangeN10to10_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082405_00000.root";
    // TString inputPath = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405/InttDoublets/baseline/completed/Data_INTTDoublets_StreamTrig_CentralityRange1to100_VtxZRangeN10to10_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082405_00000.root";
    std::pair<double, double> DeltaPhi_cut = {-0.15, 0.15};

    // Derive output filename: <basename>_output.root in same directory
    TString inputDir  = gSystem->DirName(inputPath);
    TString inputBase = gSystem->BaseName(inputPath);
    inputBase.ReplaceAll(".root", "");
    TString outputPath = Form("%s/Output_%s.root", inputDir.Data(), inputBase.Data());

    TFile *fin = TFile::Open(inputPath, "READ");
    if (!fin || fin->IsZombie()) {
        std::cout << "Error: Cannot open input file: " << inputPath << std::endl;
        return;
    }
    std::cout << "Opened: " << inputPath << std::endl;

    // =====================================================================
    // Read histograms from the input file
    // =====================================================================

    // --- 2D histograms ---
    TH2D *h2D_GoodProtoTracklet_EtaPhi          = (TH2D*)fin->Get("h2D_GoodProtoTracklet_EtaPhi");
    TH2D *h2D_GoodProtoTracklet_EtaPhi_rotated  = (TH2D*)fin->Get("h2D_GoodProtoTracklet_EtaPhi_rotated");
    TH2D *h2D_Clus_ColumnZID_LayerPhiID         = (TH2D*)fin->Get("h2D_Clus_ColumnZID_LayerPhiID");

    // --- 1D histograms ---
    TH1D *h1D_GoodProtoTracklet_Eta             = (TH1D*)fin->Get("h1D_GoodProtoTracklet_Eta");
    TH1D *h1D_GoodProtoTracklet_Eta_rotated     = (TH1D*)fin->Get("h1D_GoodProtoTracklet_Eta_rotated");
    TH1D *h1D_nEvent                            = (TH1D*)fin->Get("h1D_nEvent");
    TH1D *h1D_InttVtxZ                          = (TH1D*)fin->Get("h1D_InttVtxZ");
    TH1D *h1D_BunchNumber                       = (TH1D*)fin->Get("h1D_BunchNumber");
    TH1D *h1D_ClusEtaInttZ                      = (TH1D*)fin->Get("h1D_ClusEtaInttZ");
    TH1D *h1D_ClusEtaInttZ_normalized           = (h1D_ClusEtaInttZ == nullptr) ? nullptr : (TH1D*) h1D_ClusEtaInttZ -> Clone("h1D_ClusEtaInttZ_normalized");
    if (h1D_ClusEtaInttZ_normalized){
        h1D_ClusEtaInttZ_normalized -> SetTitle("h1D_ClusEtaInttZ_normalized;#eta;INTT cluster count (per evt. per #eta)");
    }

    // --- Truth histograms (may not exist for data) ---
    TH1D *h1D_Truth_ChargedHadron         = (TH1D*)fin->Get("h1D_Truth_ChargedHadron");
    TH2D *h2D_Truth_ChargedHadron_EtaPhi  = (TH2D*)fin->Get("h2D_Truth_ChargedHadron_EtaPhi");
    TH1D *h1D_Truth_ChargedHadron_Eta     = (TH1D*)fin->Get("h1D_Truth_ChargedHadron_Eta");

    // Basic sanity checks on required histograms
    if (!h2D_GoodProtoTracklet_EtaPhi || !h2D_GoodProtoTracklet_EtaPhi_rotated) { std::cout << "Error: Missing h2D_GoodProtoTracklet_EtaPhi histograms" << std::endl; return; }
    if (!h1D_GoodProtoTracklet_Eta    || !h1D_GoodProtoTracklet_Eta_rotated)     { std::cout << "Error: Missing h1D_GoodProtoTracklet_Eta histograms" << std::endl; return; }
    if (!h1D_nEvent)                      { std::cout << "Error: Missing h1D_nEvent" << std::endl; return; }
    if (!h1D_InttVtxZ)                    { std::cout << "Error: Missing h1D_InttVtxZ" << std::endl; return; }
    if (!h2D_Clus_ColumnZID_LayerPhiID)                     { std::cout << "Error: Missing h2D_Clus_ColumnZID_LayerPhiID" << std::endl; return; }
    if (!h1D_BunchNumber)                     { std::cout << "No h1D_BunchNumber"<<std::endl;}
    if (!h1D_ClusEtaInttZ)                     { std::cout << "No h1D_ClusEtaInttZ"<<std::endl;}

    double nEvents = h1D_nEvent->GetBinContent(1);
    std::cout << "Number of events: " << nEvents << std::endl;
    std::cout << "h1D_GoodProtoTracklet_Eta NbinsX(): "<< h1D_GoodProtoTracklet_Eta->GetNbinsX() << std::endl;

    // =====================================================================
    // Task 1: h2D subtraction  EtaPhi - EtaPhi_rotated
    // =====================================================================
    TH2D *h2D_GoodProtoTracklet_EtaPhi_Subtracted = (TH2D*)h2D_GoodProtoTracklet_EtaPhi->Clone("h2D_GoodProtoTracklet_EtaPhi_Subtracted");
    h2D_GoodProtoTracklet_EtaPhi_Subtracted->SetTitle("h2D_GoodProtoTracklet_EtaPhi - rotated;Pair #eta;Phi [radian]");
    h2D_GoodProtoTracklet_EtaPhi_Subtracted->Add(h2D_GoodProtoTracklet_EtaPhi_rotated, -1.0);

    // =====================================================================
    // Task 2: h1D subtraction  Eta - Eta_rotated
    // =====================================================================
    TH1D *h1D_GoodProtoTracklet_Eta_Subtracted = (TH1D*)h1D_GoodProtoTracklet_Eta->Clone("h1D_GoodProtoTracklet_Eta_Subtracted");
    h1D_GoodProtoTracklet_Eta_Subtracted->SetTitle("h1D_GoodProtoTracklet_Eta - rotated;#eta;Entries");
    h1D_GoodProtoTracklet_Eta_Subtracted->Add(h1D_GoodProtoTracklet_Eta_rotated, -1.0);

    // =====================================================================
    // Task 6: Normalized dN/deta  = (Eta_sub) / binWidth / nEvents
    // =====================================================================
    TH1D *h1D_GoodProtoTracklet_Eta_Subtracted_normalized = (TH1D*)h1D_GoodProtoTracklet_Eta_Subtracted->Clone("h1D_GoodProtoTracklet_Eta_Subtracted_normalized");
    h1D_GoodProtoTracklet_Eta_Subtracted_normalized->SetTitle("h1D_GoodProtoTracklet_Eta_Subtracted_normalized;#eta;INTT doublets (per evt. & per #eta)");
    for (int i = 1; i <= h1D_GoodProtoTracklet_Eta_Subtracted_normalized->GetNbinsX(); i++) {
        double binW = h1D_GoodProtoTracklet_Eta_Subtracted_normalized->GetBinWidth(i);
        double val  = h1D_GoodProtoTracklet_Eta_Subtracted_normalized->GetBinContent(i);
        double err  = h1D_GoodProtoTracklet_Eta_Subtracted_normalized->GetBinError(i);
        h1D_GoodProtoTracklet_Eta_Subtracted_normalized->SetBinContent(i, val / binW / nEvents);
        h1D_GoodProtoTracklet_Eta_Subtracted_normalized->SetBinError(i, err / binW / nEvents);
    }

    if (h1D_ClusEtaInttZ_normalized){
        for (int i = 1; i <= h1D_ClusEtaInttZ_normalized->GetNbinsX(); i++) {
            double binW = h1D_ClusEtaInttZ_normalized->GetBinWidth(i);
            double val  = h1D_ClusEtaInttZ_normalized->GetBinContent(i);
            double err  = h1D_ClusEtaInttZ_normalized->GetBinError(i);
            h1D_ClusEtaInttZ_normalized->SetBinContent(i, val / binW / nEvents);
            h1D_ClusEtaInttZ_normalized->SetBinError(i, err / binW / nEvents);
        }   
    }

    // =====================================================================
    // Task 8: DeltaPhi_EtaXXX canvases and integral summary
    // =====================================================================
    // Summary TH1D with same binning as h1D_GoodProtoTracklet_Eta
    TH1D *h1D_DeltaPhi_IntegralEta = (TH1D*)h1D_GoodProtoTracklet_Eta->Clone("h1D_DeltaPhi_IntegralEta");
    h1D_DeltaPhi_IntegralEta->Reset();
    h1D_DeltaPhi_IntegralEta->SetTitle(Form("DeltaPhi integral [%.2f, %.2f] vs #eta;#eta;Integral", DeltaPhi_cut.first, DeltaPhi_cut.second));

    int nEtaBin_DeltaPhi = h1D_GoodProtoTracklet_Eta->GetNbinsX(); // 0..26
    for (int eta = 0; eta < nEtaBin_DeltaPhi; eta++)
    {
        TString nameBase = Form("h1D_DeltaPhi_Eta%d", eta);
        TH1D *hSig = (TH1D*)fin->Get(nameBase);
        TH1D *hRot = (TH1D*)fin->Get(nameBase + "_rotated");
        if (!hSig || !hRot) {
            std::cout << "Warning: Missing " << nameBase << " or rotated. Skipping." << std::endl;
            continue;
        }

        TH1D *hSub = (TH1D*)hSig->Clone(nameBase + "_Subtracted");
        hSub->Add(hRot, -1.0);

        // Canvas: signal, rotated, subtraction
        TCanvas *c = new TCanvas(Form("c_DeltaPhi_Eta%d", eta), Form("DeltaPhi Eta%d", eta), 800, 600);
        hSig->SetLineColor(kBlue);
        hSig->SetLineWidth(2);
        hRot->SetLineColor(kRed);
        hRot->SetLineWidth(2);
        hSub->SetLineColor(kBlack);
        hSub->SetLineWidth(2);
        hSub->SetLineStyle(2);

        double yMax = std::max({hSig->GetMaximum(), hRot->GetMaximum(), hSub->GetMaximum()}) * 1.3;
        double yMin = std::min({hSig->GetMinimum(), hRot->GetMinimum(), hSub->GetMinimum()}) * 1.1;
        if (yMin > 0) yMin = 0;
        hSig->GetYaxis()->SetRangeUser(yMin, yMax);
        hSig->SetStats(0);
        hSig->Draw("hist");
        hRot->Draw("hist same");
        hSub->Draw("hist same");

        TLegend *leg = new TLegend(0.55, 0.70, 0.88, 0.88);
        leg->AddEntry(hSig, "Signal", "l");
        leg->AddEntry(hRot, "Rotated", "l");
        leg->AddEntry(hSub, "Signal - Rotated", "l");
        leg->SetBorderSize(0);
        leg->Draw();

        // Compute integral from -0.15 to 0.15
        int binLo = hSub->FindBin(DeltaPhi_cut.first);
        int binHi = hSub->FindBin(DeltaPhi_cut.second);
        double integral = hSub->Integral(binLo, binHi);

        // Fill into summary hist at bin = eta + 1
        if (eta + 1 <= h1D_DeltaPhi_IntegralEta->GetNbinsX()) {
            h1D_DeltaPhi_IntegralEta->SetBinContent(eta + 1, integral);
        }
    }

    TH1D *h1D_DeltaPhi_IntegralEta_normalized = (TH1D*)h1D_DeltaPhi_IntegralEta->Clone("h1D_DeltaPhi_IntegralEta_normalized");
    h1D_DeltaPhi_IntegralEta_normalized->SetTitle("h1D_DeltaPhi_IntegralEta_normalized;#eta;INTT doublets (per evt, per #eta)");
    for (int i = 1; i <= h1D_DeltaPhi_IntegralEta_normalized->GetNbinsX(); i++) {
        double binW = h1D_DeltaPhi_IntegralEta_normalized->GetBinWidth(i);
        double val  = h1D_DeltaPhi_IntegralEta_normalized->GetBinContent(i);
        double err  = h1D_DeltaPhi_IntegralEta_normalized->GetBinError(i);
        h1D_DeltaPhi_IntegralEta_normalized->SetBinContent(i, val / binW / nEvents);
        h1D_DeltaPhi_IntegralEta_normalized->SetBinError(i, err / binW / nEvents);
    }

    // =====================================================================
    // Task 9: DeltaPhi_PhibinXXX_EtaYYY_VtxZ0 canvases and integral summary
    // =====================================================================
    TH2D *h2D_IntegralEtaPhi = (TH2D*)h2D_GoodProtoTracklet_EtaPhi->Clone("h2D_DeltaPhi_IntegralEtaPhi");
    h2D_IntegralEtaPhi->Reset();
    h2D_IntegralEtaPhi->SetTitle(Form("DeltaPhi integral [%.2f, %.2f];Pair #eta;Phi [radian]", DeltaPhi_cut.first, DeltaPhi_cut.second));

    for (int eta = 0; eta <= 10; eta++)
    {
        for (int phi = 0; phi <= 15; phi++)
        {
            TString nameBase = Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ0", phi, eta);
            TH1D *hSig = (TH1D*)fin->Get(nameBase);
            TH1D *hRot = (TH1D*)fin->Get(nameBase + "_rotated");
            if (!hSig || !hRot) {
                std::cout << "Warning: Missing " << nameBase << " or rotated. Skipping." << std::endl;
                continue;
            }

            TH1D *hSub = (TH1D*)hSig->Clone(nameBase + "_Subtracted");
            hSub->Add(hRot, -1.0);

            // Canvas
            TCanvas *c = new TCanvas(
                Form("c_DeltaPhi_Phibin%d_Eta%d_VtxZ0", phi, eta),
                Form("DeltaPhi Phibin%d Eta%d VtxZ0", phi, eta), 800, 600);

            hSig->SetLineColor(kBlue);
            hSig->SetLineWidth(2);
            hRot->SetLineColor(kRed);
            hRot->SetLineWidth(2);
            hSub->SetLineColor(kBlack);
            hSub->SetLineWidth(2);
            hSub->SetLineStyle(2);

            double yMax = std::max({hSig->GetMaximum(), hRot->GetMaximum(), hSub->GetMaximum()}) * 1.3;
            double yMin = std::min({hSig->GetMinimum(), hRot->GetMinimum(), hSub->GetMinimum()}) * 1.1;
            if (yMin > 0) yMin = 0;
            hSig->GetYaxis()->SetRangeUser(yMin, yMax);
            
            hSig->SetStats(0);
            hSig->Draw("hist");
            hRot->Draw("hist same");
            hSub->Draw("hist same");

            TLegend *leg = new TLegend(0.55, 0.70, 0.88, 0.88);
            leg->AddEntry(hSig, "Signal", "l");
            leg->AddEntry(hRot, "Rotated", "l");
            leg->AddEntry(hSub, "Signal - Rotated", "l");
            leg->SetBorderSize(0);
            leg->Draw();

            // Integral from -0.15 to 0.15
            int binLo = hSub->FindBin(DeltaPhi_cut.first);
            int binHi = hSub->FindBin(DeltaPhi_cut.second);
            double integral = hSub->Integral(binLo, binHi);

            // Fill into 2D summary: (eta+1, phi+1)  ->  (YYY+1, XXX+1)
            h2D_IntegralEtaPhi->SetBinContent(eta + 1, phi + 1, integral);
        }
    }

    // =====================================================================
    // Task 13: Normalized truth dN/deta (if available)
    // =====================================================================
    TH1D *h1D_TruedNdEta_perEvt = nullptr;
    if (h1D_Truth_ChargedHadron_Eta) {
        h1D_TruedNdEta_perEvt = (TH1D*)h1D_Truth_ChargedHadron_Eta->Clone("h1D_Truth_ChargedHadron_dNdEta");
        h1D_TruedNdEta_perEvt->SetTitle("True dN/d#eta;#eta;dN/d#eta");
        for (int i = 1; i <= h1D_TruedNdEta_perEvt->GetNbinsX(); i++) {
            double binW = h1D_TruedNdEta_perEvt->GetBinWidth(i);
            double val  = h1D_TruedNdEta_perEvt->GetBinContent(i);
            double err  = h1D_TruedNdEta_perEvt->GetBinError(i);
            h1D_TruedNdEta_perEvt->SetBinContent(i, val / binW / nEvents);
            h1D_TruedNdEta_perEvt->SetBinError(i, err / binW / nEvents);
        }
    }

    // =====================================================================
    // Write output
    // =====================================================================
    TFile *fout = new TFile(outputPath, "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cout << "Error: Cannot create output file: " << outputPath << std::endl;
        return;
    }

    if (h1D_BunchNumber) {h1D_BunchNumber->Write();}
    if (h1D_ClusEtaInttZ) {h1D_ClusEtaInttZ->Write();}
    if (h1D_ClusEtaInttZ_normalized) {h1D_ClusEtaInttZ_normalized->Write();}

    // Task 3
    h1D_nEvent->Write();

    // Task 4
    h2D_Clus_ColumnZID_LayerPhiID->Write();

    // Task 5
    h1D_InttVtxZ->Write();

    // Task 2
    h1D_GoodProtoTracklet_Eta_Subtracted->Write();

    // Task 6
    h1D_GoodProtoTracklet_Eta_Subtracted_normalized->Write();

    // Task 8 summary
    h1D_DeltaPhi_IntegralEta->Write();

    // Task 1
    h2D_GoodProtoTracklet_EtaPhi_Subtracted->Write();

    // Task 9 summary
    h2D_IntegralEtaPhi->Write();

    // Task 8 canvases
    for (int eta = 0; eta < nEtaBin_DeltaPhi; eta++) {
        TCanvas *c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(Form("c_DeltaPhi_Eta%d", eta));
        if (c) c->Write();
    }

    // Task 9 canvases
    for (int eta = 0; eta <= 10; eta++) {
        for (int phi = 0; phi <= 15; phi++) {
            TCanvas *c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(Form("c_DeltaPhi_Phibin%d_Eta%d_VtxZ0", phi, eta));
            if (c) c->Write();
        }
    }

    // Task 10, 11, 12: Truth histograms (if available)
    if (h1D_Truth_ChargedHadron)    h1D_Truth_ChargedHadron->Write();
    if (h2D_Truth_ChargedHadron_EtaPhi)    h2D_Truth_ChargedHadron_EtaPhi->Write();
    if (h1D_Truth_ChargedHadron_Eta) h1D_Truth_ChargedHadron_Eta->Write();

    // Task 13: Normalized truth dN/deta
    if (h1D_TruedNdEta_perEvt) h1D_TruedNdEta_perEvt->Write();

    fout->Close();
    fin->Close();

    std::cout << "Output saved to: " << outputPath << std::endl;
}
