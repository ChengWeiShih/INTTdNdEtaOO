#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLegend.h>
#include <iostream>

void FitMBD_VtxZ()
{
    // -------------------------------------------------------
    // Input file (output of MBD_VtxZ_hist.C)
    // -------------------------------------------------------
    const TString inputDir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/"
        "dNdEtaOO/test_82391_INTTsurveyOnly/EvtVtxZ/completed/";

    const TString inputFile  = inputDir + "MBD_VtxZ_hist_00082391.root";
    const TString outputPDF  = inputDir + "FitMBD_VtxZ_00082391.pdf";

    // -------------------------------------------------------
    // Open file and retrieve histogram
    // -------------------------------------------------------
    TFile *fIn = TFile::Open(inputFile.Data(), "READ");
    if (!fIn || fIn->IsZombie())
    {
        std::cerr << "ERROR: cannot open " << inputFile << std::endl;
        return;
    }

    TH1D *h = dynamic_cast<TH1D *>(fIn->Get("h_MBD_z_vtx"));
    if (!h)
    {
        std::cerr << "ERROR: histogram h_MBD_z_vtx not found in " << inputFile << std::endl;
        fIn->Close();
        return;
    }
    h->SetDirectory(nullptr);   // detach from file so it survives fIn->Close()
    fIn->Close();

    // -------------------------------------------------------
    // Determine initial Gaussian parameters
    //   mean  -> centre of the bin with the highest content
    //   sigma -> 10 cm (user request)
    //   norm  -> bin content at the peak bin * bin width
    // -------------------------------------------------------
    const double fitLo = -10.0;
    const double fitHi = 10.0;

    int    peakBin  = h->GetMaximumBin();
    double initMean = h->GetBinCenter(peakBin);
    double initSig  = 10.0;                         // cm
    double initNorm = h->GetBinContent(peakBin);

    std::cout << "Peak bin centre (initial mean) : " << initMean << " cm" << std::endl;

    // -------------------------------------------------------
    // Build Gaussian TF1 and fit
    // -------------------------------------------------------
    TF1 *fGaus = new TF1("fGaus", "gaus", fitLo, fitHi);
    fGaus->SetParameters(initNorm, initMean, initSig);
    fGaus->SetParNames("Norm", "Mean", "Sigma");
    fGaus->SetLineColor(kRed);
    fGaus->SetLineWidth(2);

    // "R"  -> fit within the range defined in TF1
    // "S"  -> return the fit result (for error reporting)
    // "0"  -> do not draw automatically
    h->Fit(fGaus, "RS0", "", fitLo, fitHi);

    double fitMean  = fGaus->GetParameter(1);
    double fitSigma = fGaus->GetParameter(2);
    double fitNormR = fGaus->GetParameter(0);
    double chi2     = fGaus->GetChisquare();
    int    ndf      = fGaus->GetNDF();

    std::cout << "\n--- Fit results ---" << std::endl;
    std::cout << "  Norm  = " << fitNormR << std::endl;
    std::cout << "  Mean  = " << fitMean  << " cm" << std::endl;
    std::cout << "  Sigma = " << fitSigma << " cm" << std::endl;
    std::cout << "  chi2/NDF = " << chi2 << " / " << ndf << std::endl;

    // -------------------------------------------------------
    // Draw – two-pad layout
    // -------------------------------------------------------
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    // Pad split fractions
    const double padSplit = 0.30;   // lower pad fraction
    const double lMargin  = 0.13;
    const double rMargin  = 0.05;

    TCanvas *c = new TCanvas("c_MBD_vtxZ", "MBD vtx Z fit", 800, 800);
    c->SetFillColor(0);

    // ---- Upper pad (histogram + fit) ----
    TPad *pTop = new TPad("pTop", "pTop", 0, padSplit, 1, 1);
    pTop->SetLeftMargin(lMargin);
    pTop->SetRightMargin(rMargin);
    pTop->SetBottomMargin(0.02);
    pTop->SetTopMargin(0.08);
    pTop->Draw();
    pTop->cd();

    h->SetLineColor(kBlack);
    h->SetLineWidth(2);
    // suppress x-axis labels on upper pad
    h->GetXaxis()->SetLabelSize(0);
    h->GetXaxis()->SetTitleSize(0);
    h->GetYaxis()->SetTitle("Events");
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleOffset(0.85);
    h->GetYaxis()->SetLabelSize(0.06);
    h->Draw("HIST");

    // Draw-only TF1 spanning the full x-axis
    double xLo = h->GetXaxis()->GetXmin();
    double xHi = h->GetXaxis()->GetXmax();
    TF1 *fGausDraw = new TF1("fGausDraw", "gaus", xLo, xHi);
    fGausDraw->SetParameters(fGaus->GetParameter(0),
                             fGaus->GetParameter(1),
                             fGaus->GetParameter(2));
    fGausDraw->SetLineColor(kRed);
    fGausDraw->SetLineWidth(2);
    fGausDraw->Draw("SAME");

    // Fit-range marker lines (dashed)
    TLine *lLo = new TLine(fitLo, 0, fitLo, h->GetMaximum() * 0.95);
    TLine *lHi = new TLine(fitHi, 0, fitHi, h->GetMaximum() * 0.95);
    for (TLine *l : {lLo, lHi})
    {
        l->SetLineColor(kBlue);
        l->SetLineStyle(2);
        l->SetLineWidth(2);
        l->Draw();
    }

    // Legend
    TLegend *leg = new TLegend(0.53, 0.55, 0.93, 0.90);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.055);
    leg->AddEntry(h,         "Data (run 82391)", "l");
    leg->AddEntry(fGausDraw, "Gaussian fit",      "l");
    leg->AddEntry((TObject *)nullptr,
                  Form("Mean  = %.2f cm",  fitMean),  "");
    leg->AddEntry((TObject *)nullptr,
                  Form("Sigma = %.2f cm",  fitSigma), "");
    leg->AddEntry((TObject *)nullptr,
                  Form("#chi^{2}/NDF = %.1f / %d", chi2, ndf), "");
    leg->Draw();

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.05);
    latex.SetTextColor(kBlue);
    latex.DrawLatex(0.14, 0.91,
                    Form("Fit range: [%.0f, %.0f] cm", fitLo, fitHi));

    // ---- Lower pad (ratio: data / fit) ----
    c->cd();
    TPad *pBot = new TPad("pBot", "pBot", 0, 0, 1, padSplit);
    pBot->SetLeftMargin(lMargin);
    pBot->SetRightMargin(rMargin);
    pBot->SetTopMargin(0.04);
    pBot->SetBottomMargin(0.30);
    pBot->Draw();
    pBot->cd();

    int nBins = h->GetNbinsX();
    TH1D *hRatio = new TH1D("hRatio", "", nBins, xLo, xHi);
    hRatio->Sumw2();

    for (int i = 1; i <= nBins; ++i)
    {
        double binCentre  = h->GetBinCenter(i);
        double dataVal    = h->GetBinContent(i);
        double dataErr    = h->GetBinError(i);
        double fitVal     = fGausDraw->Eval(binCentre);

        if (fitVal > 0)
        {
            hRatio->SetBinContent(i, dataVal / fitVal);
            hRatio->SetBinError(i,   dataErr / fitVal);
        }
        else
        {
            hRatio->SetBinContent(i, 0);
            hRatio->SetBinError(i,   0);
        }
    }

    hRatio->SetLineColor(kBlack);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerSize(0.7);
    hRatio->SetMarkerColor(kBlack);

    hRatio->GetXaxis()->SetTitle("MBD z_{vtx} [cm]");
    hRatio->GetXaxis()->SetTitleSize(0.13);
    hRatio->GetXaxis()->SetTitleOffset(0.90);
    hRatio->GetXaxis()->SetLabelSize(0.11);

    hRatio->GetYaxis()->SetTitle("Data / Fit");
    hRatio->GetYaxis()->SetTitleSize(0.11);
    hRatio->GetYaxis()->SetTitleOffset(0.55);
    hRatio->GetYaxis()->SetLabelSize(0.10);
    hRatio->GetYaxis()->SetNdivisions(505);
    hRatio->GetYaxis()->SetRangeUser(0.0, 2.0);

    hRatio->Draw("E1");

    // Reference line at ratio = 1
    TLine *refLine = new TLine(xLo, 1.0, xHi, 1.0);
    refLine->SetLineColor(kRed);
    refLine->SetLineStyle(2);
    refLine->SetLineWidth(2);
    refLine->Draw();

    // -------------------------------------------------------
    // Save PDF
    // -------------------------------------------------------
    c->SaveAs(outputPDF.Data());
    std::cout << "\nPlot saved to: " << outputPDF << std::endl;

    delete c;
    delete fGaus;
    delete fGausDraw;
    delete hRatio;
    delete h;
}
