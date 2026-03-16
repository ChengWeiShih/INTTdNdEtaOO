#include <TFile.h>
#include <TProfile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <cmath>
#include <TLatex.h>

void plot_npart() {
    TFile *fileIn = TFile::Open("MC_NormalizeddNdEta_output.root", "READ");
    if (!fileIn || fileIn->IsZombie()) {
        std::cout << "Failed to open MC_NormalizeddNdEta_output.root" << std::endl;
        return;
    }

    TProfile *prof_npart_count = (TProfile*)fileIn->Get("prof_npart_count");
    if (!prof_npart_count) {
        std::cout << "Failed to get prof_npart_count" << std::endl;
        fileIn->Close();
        return;
    }

    TGraphErrors *graph = new TGraphErrors();
    int nPoints = 0;

    for (int i = 1; i <= prof_npart_count->GetNbinsX(); ++i) {
        // We only consider bins that have some entries
        if (prof_npart_count->GetBinEntries(i) > 0) {
            double x = prof_npart_count->GetBinCenter(i);
            double y = prof_npart_count->GetBinContent(i);
            double err = prof_npart_count->GetBinError(i);

            // "drop the data points with x-axis value = 32 and = 31"
            if (std::abs(x - 31.5) < 0.1 || std::abs(x - 32.5) < 0.1 || std::abs(x - 0.5) < 0.1) {
                continue;
            }

            std::cout<<"x = "<<x<<", y = "<<y<<", err = "<<err<<std::endl;

            graph->SetPoint(nPoints, x, y);
            graph->SetPointError(nPoints, 0, 0);
            nPoints++;
        }
    }

    TCanvas *c1 = new TCanvas("c1", "TCanvas", 700, 600);
    c1->SetLeftMargin(0.15); // Add space for y-axis title
    c1->cd();

    graph->SetTitle(";#LTN_{part}#GT;(1/#LTN_{part}#GT) dN_{ch}/d#eta#cbar_{ |#eta|<0.5}");
    graph->GetYaxis()->SetTitleOffset(1.5);
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlack);
    graph->SetLineColor(kOrange + 1);
    graph->SetLineWidth(5);
    graph->SetLineStyle(9);
    
    // Use AL to draw axes and line
    graph->Draw("ALC");

    graph->GetXaxis()->SetLimits(0, 32);
    graph->SetMinimum(0);
    graph->SetMaximum(4);

    c1->SetTickx(1);
    c1->SetTicky(1);

    TLatex *latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);
    // latex->SetTextAlign(33); // align top right
    latex->DrawLatex(0.22, 0.80, "HIJING event generator");
    latex->DrawLatex(0.22, 0.75, "O+O collisions at #sqrt{s_{NN}} = 200 GeV");

    c1->SaveAs("HIJING_OO_normalizeddNdEta_npart.pdf");

    // Clean up
    fileIn->Close();
}
