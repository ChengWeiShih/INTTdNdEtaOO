#include <TFile.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include "sPhenixStyle.C"
// #include <gROOT.h>

void MakePlot_SingledNdEta() {
    // 1. Load sPhenix style
    // gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();

    // 2. Open the ROOT file
    TFile *file = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p15/completed/vtxZ_-10_10cm_MBin0/Folder_BaseLine/Run_0/DataMc2Comp.root");
    if (!file || file->IsZombie()) {
        std::cout << "Error: Could not open the ROOT file." << std::endl;
        return;
    }

    // 3. Access the TCanvas
    TCanvas *source_canvas = (TCanvas*)file->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");
    if (!source_canvas) {
        std::cout << "Error: Could not find canvas 'h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC' in the file." << std::endl;
        file->Close();
        return;
    }

    // 4. Get the TH1D histograms from the canvas
    TH1D *h1D_TruedNdEta = (TH1D*)source_canvas->GetPrimitive("h1D_TruedNdEta");
    TH1D *h_data = (TH1D*)source_canvas->GetPrimitive("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    if (!h1D_TruedNdEta || !h_data) {
        std::cout << "Error: Could not find one or both histograms in the canvas." << std::endl;
        file->Close();
        return;
    }

    // 5. Create a new TCanvas
    TCanvas *c1 = new TCanvas("c1", "c1", 950, 800);

    // Style and draw options
    h1D_TruedNdEta->SetLineColor(kRed);
    h1D_TruedNdEta->SetLineWidth(3);
    h_data->SetMarkerStyle(kFullCircle);
    h_data->SetMarkerSize(1.5);
    h_data->SetMarkerColor(kBlack);
    h_data->SetLineColor(kBlack);

    // Filter h_data points based on eta < \pm 1.1
    for (int i = 1; i <= h_data->GetNbinsX(); ++i) {
        double eta = h_data->GetBinCenter(i);
        if (std::abs(eta) >= 1.1) {
            h_data->SetBinContent(i, -999); // Set to a value out of range or just 0
            h_data->SetBinError(i, 0);
        }
    }

    // Set Y-axis range and titles
    h1D_TruedNdEta->GetYaxis()->SetRangeUser(20, 60);
    h1D_TruedNdEta->SetTitle(";#eta; dN_{ch}/d#eta");
    
    // 8. Set the eta range by setting the X-axis range for drawing
    // h1D_TruedNdEta->GetXaxis()->SetRangeUser(-1.1, 1.1);

    // Draw the histograms
    h1D_TruedNdEta->Draw("hist");
    h_data->Draw("p same");

    // 6. Create the TLegend
    TLegend *leg = new TLegend(0.22, 0.75, 0.8, 0.91);
    leg->SetHeader("Most central collision events");
    leg->AddEntry(h1D_TruedNdEta, "HIJING, O+O collisions", "l");
    leg->AddEntry(h_data, "O+O collision data", "p");
    leg->SetBorderSize(0);
    leg->Draw();

    // 7. Add the TLatex object
    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX}} Internal, for SPDR application");

    // Save the plot as a PDF file
    c1->SaveAs("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p15/completed/vtxZ_-10_10cm_MBin0/Folder_BaseLine/Run_0/dNdEta_comparison.pdf");

    // Clean up
    file->Close();
}
