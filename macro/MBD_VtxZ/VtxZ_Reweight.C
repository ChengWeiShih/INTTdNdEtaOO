#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <iostream>

void VtxZ_Reweight()
{
    // -------------------------------------------------------
    // Paths
    // -------------------------------------------------------
    const TString inputDir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/"
        "dNdEtaOO/test_82391_INTTsurveyOnly/EvtVtxZ/completed/";

    const TString inputFile = inputDir + "MBD_VtxZ_hist_00082391.root";
    const TString outputPDF = inputDir + "VtxZ_Reweight_00082391.pdf";

    // -------------------------------------------------------
    // Open file and retrieve histograms
    // -------------------------------------------------------
    TFile *fIn = TFile::Open(inputFile.Data(), "READ");
    if (!fIn || fIn->IsZombie())
    {
        std::cerr << "ERROR: cannot open " << inputFile << std::endl;
        return;
    }

    TH1D *h1 = dynamic_cast<TH1D *>(fIn->Get("h_MBD_z_vtx"));
    TH2D *h2 = dynamic_cast<TH2D *>(fIn->Get("h2_VtxZ_ChargeSum"));

    if (!h1 || !h2)
    {
        std::cerr << "ERROR: histograms not found in " << inputFile << std::endl;
        fIn->Close();
        return;
    }

    h1->SetDirectory(nullptr);
    h2->SetDirectory(nullptr);
    fIn->Close();

    // -------------------------------------------------------
    // Build reweighted TH2D
    //   For each bin (ix, iy):
    //     reweighted[ix][iy] = h2[ix][iy] / h1[ix]
    //   i.e. each Y-slice is divided bin-by-bin by the TH1D
    // -------------------------------------------------------
    TH2D *h2_rw = dynamic_cast<TH2D *>(h2->Clone("h2_VtxZ_ChargeSum_Reweighted"));
    h2_rw->SetTitle("MBD charge sum / vtx-Z yield;"
                    "MBD z_{vtx} [cm];MBD charge sum [a.u.]");
    h2_rw->SetDirectory(nullptr);

    int nBinsX = h2_rw->GetNbinsX();
    int nBinsY = h2_rw->GetNbinsY();

    for (int ix = 1; ix <= nBinsX; ++ix)
    {
        double h1Val = h1->GetBinContent(ix);
        double h1Err = h1->GetBinError(ix);

        for (int iy = 1; iy <= nBinsY; ++iy)
        {
            double h2Val = h2_rw->GetBinContent(ix, iy);
            double h2Err = h2_rw->GetBinError(ix, iy);

            if (h1Val > 0)
            {
                double rwVal = h2Val / h1Val;
                // error propagation: sqrt( (dh2/h1)^2 + (h2*dh1/h1^2)^2 )
                double rwErr = rwVal * TMath::Sqrt(
                    (h2Val > 0 ? (h2Err / h2Val) * (h2Err / h2Val) : 0) +
                    (h1Err / h1Val) * (h1Err / h1Val));
                h2_rw->SetBinContent(ix, iy, rwVal);
                h2_rw->SetBinError(ix, iy,   rwErr);
            }
            else
            {
                h2_rw->SetBinContent(ix, iy, 0);
                h2_rw->SetBinError(ix, iy,   0);
            }
        }
    }

    std::cout << "Reweighted TH2D built." << std::endl;

    // -------------------------------------------------------
    // Draw and save PDF
    // -------------------------------------------------------
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kRainBow);

    TCanvas *c = new TCanvas("c_rw", "VtxZ Reweighted", 900, 700);
    c->SetLeftMargin(0.12);
    c->SetRightMargin(0.15);   // room for colour-bar
    c->SetBottomMargin(0.12);
    c->SetTopMargin(0.10);
    c->SetLogz();              // log-z scale helps visualise large dynamic range

    h2_rw->GetXaxis()->SetTitleSize(0.05);
    h2_rw->GetYaxis()->SetTitleSize(0.05);
    h2_rw->GetXaxis()->SetTitleOffset(0.90);
    h2_rw->GetYaxis()->SetTitleOffset(1.05);
    h2_rw->Draw("COLZ");

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.040);
    latex.DrawLatex(0.13, 0.93, "Run 82391 — charge sum reweighted by vtx-Z yield");

    c->SaveAs(outputPDF.Data());
    std::cout << "Plot saved to: " << outputPDF << std::endl;

    delete c;
    delete h2_rw;
    delete h2;
    delete h1;
}
