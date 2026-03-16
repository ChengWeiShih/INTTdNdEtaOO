#include "sPhenixStyle.C"
#include <TLine.h>

int MakePlot_INTTzEffi_CustomColors()
{
    // --- Configuration ---
    std::string input_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/VtxZDist/completed"; 
    std::string input_file_name = "MC_vtxZDist_NoVtxZQA_merged.root";
    std::string target_plot_truth_name = "h2D_TruthCount_TruthZ_Centrality";
    std::string target_plot2_reco_name = "h2D_GoodRecoZ_TruthZ_Centrality";
    std::string output_file_name = "h2D_RecoZEffi_WithLines";

    SetsPhenixStyle();

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    TH2D * h2D_truth = (TH2D*)file_in->Get(target_plot_truth_name.c_str());
    TH2D * h2D_reco = (TH2D*)file_in->Get(target_plot2_reco_name.c_str());

    TH2D * final = (TH2D*)h2D_reco->Clone("final");
    final->Sumw2();
    final->Divide(h2D_reco, h2D_truth, 1, 1);

    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        {0.18, 0.96, "HIJING, |VtxZDiff| #leq 3 cm"},
    };

    // --- Custom Color Palette ---
    // 0-92% Red, 92-98% Blue, 98-100%+ Green
    Int_t colors[] = {kRed, kAzure+1, kGreen+2}; 
    gStyle->SetPalette(3, colors);
    Double_t contours[] = {0.0, 0.92, 0.98, 1.1}; 
    final->SetContour(4, contours);

    // --- Canvas Setup ---
    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);
    c1->SetRightMargin(0.15);
    
    final->GetXaxis()->SetTitle("Truth Z [cm]");
    final->GetYaxis()->SetTitle("Centrality Interval [%]");
    final->GetZaxis()->SetTitle("Efficiency");
    final->GetZaxis()->SetRangeUser(0, 1.0);
    
    gStyle->SetPaintTextFormat("1.3f");
    
    final -> SetMarkerSize(0.4);    
    final->Draw("COLZ TEXT90 E"); // Draw base plot first

    // --- DRAWING THE CELL LINES ---
    // We get the axis to find the low and high edges of every bin
    TAxis *xAxis = final->GetXaxis();
    TAxis *yAxis = final->GetYaxis();

    TLine *line = new TLine();
    line->SetLineColor(kBlack);
    line->SetLineWidth(1);
    line->SetLineStyle(1); // Solid line

    // Draw vertical lines (constant X at bin edges)
    for (int i = 1; i <= xAxis->GetNbins() + 1; ++i) {
        double x = xAxis->GetBinLowEdge(i);
        line->DrawLine(x, yAxis->GetXmin(), x, yAxis->GetXmax());
    }

    // Draw horizontal lines (constant Y at bin edges)
    for (int j = 1; j <= yAxis->GetNbins() + 1; ++j) {
        double y = yAxis->GetBinLowEdge(j);
        line->DrawLine(xAxis->GetXmin(), y, xAxis->GetXmax(), y);
    }

    // --- Annotations ---
    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX}} Simulation"); 

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    c1->Print(Form("%s/%s.pdf", input_file_directory.c_str(), output_file_name.c_str()));
    return 0;
}