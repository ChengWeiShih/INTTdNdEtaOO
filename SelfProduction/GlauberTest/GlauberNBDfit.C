// ============================================================
//  GlauberNBDfit.C
//
//  Negative Binomial Distribution (NBD) convolution with the
//  Glauber Npart distribution, fitted to the MBD charge sum.
//
//  Usage (from ROOT prompt or command line):
//    root -l 'GlauberNBDfit.C'
//    or
//    root -l -q 'GlauberNBDfit.C'
// ============================================================

#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TGraph.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>

// ---------------------------------------------------------------
// Global pointer to the Npart histogram (used inside TF1 evaluator)
// ---------------------------------------------------------------
TH1D *g_h1D_OO_Npart = nullptr;

// ---------------------------------------------------------------
//  NBD probability:  P(n | mu, k)
//
//    P(n; mu, k) = Gamma(n+k) / [Gamma(k) * n!]
//                 * (mu/k)^n / (1 + mu/k)^(n+k)
//
//  Parameters:
//    mu  – mean multiplicity per NN collision
//    k   – inverse over-dispersion (k -> inf => Poisson)
// ---------------------------------------------------------------
double NBD(double n, double mu, double k)
{
    if (mu <= 0.0 || k <= 0.0) return 0.0;
    double r  = k / (mu + k);          // p in the standard form
    double lg = TMath::LnGamma(n + k)
              - TMath::LnGamma(k)
              - TMath::LnGamma(n + 1.0)
              + n * TMath::Log(1.0 - r)
              + k * TMath::Log(r);
    return (lg < -300.0) ? 0.0 : TMath::Exp(lg);
}

// ---------------------------------------------------------------
//  NBD-Glauber convolution evaluated at charge value x
//
//  The total MBD charge for a given Npart is the sum of Npart
//  independent NBD contributions.  For Npart identical NBD(mu,k)
//  random variables the resulting distribution is NBD(Npart*mu, Npart*k).
//
//  Full model:
//    f(x) = norm * Sum_{Npart} P(Npart) * NBD(x ; Npart*mu , Npart*k)
//
//  TF1 parameters:
//    [0]  norm  – overall normalization
//    [1]  mu    – mean charge per NN collision
//    [2]  k     – NBD shape parameter
// ---------------------------------------------------------------
double NBDGlauberConv(double *x_arr, double *par)
{
    double x    = x_arr[0];
    double norm = par[0];
    double mu   = par[1];
    double k    = par[2];

    if (!g_h1D_OO_Npart) return 0.0;
    if (mu <= 0.0 || k <= 0.0) return 0.0;

    double val = 0.0;
    int nbins  = g_h1D_OO_Npart->GetNbinsX();

    for (int ib = 1; ib <= nbins; ++ib)
    {
        double Npart    = g_h1D_OO_Npart->GetBinCenter(ib);
        double wNpart   = g_h1D_OO_Npart->GetBinContent(ib);
        if (wNpart <= 0.0 || Npart <= 0.0) continue;

        // For the sum of Npart NBD(mu,k) variates -> NBD(Npart*mu, Npart*k)
        double mu_eff = Npart * mu;
        double k_eff  = Npart * k;

        val += wNpart * NBD(x, mu_eff, k_eff);
    }

    return norm * val;
}

// ---------------------------------------------------------------
//  Main macro
// ---------------------------------------------------------------
void GlauberNBDfit()
{
    // ===== Output directory =====
    // std::string output_directory = "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/MC_HIJING_run00037_20260318";
    std::string output_directory = "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/run82391";

    // ===== Style =====
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPadLeftMargin(0.14);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetTitleFontSize(0.045);

    // ===== 1) Open Glauber file and get Npart histogram =====
    const char *glauberFile =
        "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/Npart_output.root";

    TFile *f_glauber = TFile::Open(glauberFile, "READ");
    if (!f_glauber || f_glauber->IsZombie()) {
        std::cerr << "[ERROR] Cannot open: " << glauberFile << std::endl;
        return;
    }

    g_h1D_OO_Npart = dynamic_cast<TH1D*>(f_glauber->Get("h1D_OO_Npart"));
    if (!g_h1D_OO_Npart) {
        std::cerr << "[ERROR] Cannot retrieve h1D_OO_Npart from Glauber file." << std::endl;
        return;
    }
    g_h1D_OO_Npart->SetDirectory(nullptr);   // detach from file
    std::cout << "[INFO] Loaded h1D_OO_Npart  (bins="
              << g_h1D_OO_Npart->GetNbinsX()
              << ", Npart_max=" << g_h1D_OO_Npart->GetXaxis()->GetXmax()
              << ")" << std::endl;

    // Normalize Npart histogram to unity (probability weights)
    double totalNpart = g_h1D_OO_Npart->Integral();
    if (totalNpart > 0.0)
        g_h1D_OO_Npart->Scale(1.0 / totalNpart);

    f_glauber->Close();

    // ===== 2) Open MBD file and get charge-sum histogram =====
    const char *mbdFile =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/"
        "test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/quick_hist/completed/"
        "HistOut_82391_merged.root";

    // const char *mbdFile = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260318/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/quick_hist/completed/HIJING_HistOut_37_merged.root";

    TFile *f_mbd = TFile::Open(mbdFile, "READ");
    if (!f_mbd || f_mbd->IsZombie()) {
        std::cerr << "[ERROR] Cannot open: " << mbdFile << std::endl;
        return;
    }

    TH1D *h1D_MBD_charge_sum = dynamic_cast<TH1D*>(f_mbd->Get("h1D_MBD_charge_sum"));
    if (!h1D_MBD_charge_sum) {
        std::cerr << "[ERROR] Cannot retrieve h1D_MBD_charge_sum from MBD file." << std::endl;
        return;
    }
    h1D_MBD_charge_sum->SetDirectory(nullptr);  // detach from file
    std::cout << "[INFO] Loaded h1D_MBD_charge_sum  (bins="
              << h1D_MBD_charge_sum->GetNbinsX()
              << ", entries=" << h1D_MBD_charge_sum->GetEntries()
              << ")" << std::endl;
    f_mbd->Close();

    // ===== 3) Build the NBD-Glauber TF1 =====
    //   Fit range: skip the very low-charge region (< 5) to avoid
    //   trigger-turn-on bias; upper edge at histogram maximum.
    // double fitXmin =   40.0; // note: for data
    double fitXmin =   40.0;
    double fitXmax = 250.0;

    TF1 *f_NBDGlauber = new TF1("f_NBDGlauber",
                                 NBDGlauberConv,
                                 fitXmin, fitXmax,
                                 3);   // 3 free parameters

    // Parameter names
    f_NBDGlauber->SetParName(0, "Norm");
    f_NBDGlauber->SetParName(1, "#mu (mean charge/NN)");
    f_NBDGlauber->SetParName(2, "k (NBD shape)");

    // Initial guesses
    //   norm  ~ total integral of data histogram
    //   mu    ~ rough mean charge / <Npart> per arm, O+O -> Npart ~2-8
    //   k     ~ start at 1 (geometric-like), range (0,inf)
    double dataIntegral = h1D_MBD_charge_sum->Integral(
        h1D_MBD_charge_sum->FindBin(fitXmin),
        h1D_MBD_charge_sum->FindBin(fitXmax));

    f_NBDGlauber->SetParameter(0, dataIntegral);
    f_NBDGlauber->SetParameter(1, 5.0);     // mu
    f_NBDGlauber->SetParameter(2, 1.0);     // k

    // Limits (keep physical)
    f_NBDGlauber->SetParLimits(0, 0.0, dataIntegral * 100.0);
    f_NBDGlauber->SetParLimits(1, 0.01, 200.0);
    f_NBDGlauber->SetParLimits(2, 0.01, 200.0);

    f_NBDGlauber->SetLineColor(kRed);
    f_NBDGlauber->SetLineWidth(2);

    // ===== 4) Fit =====
    std::cout << "[INFO] Starting NBD-Glauber fit ..." << std::endl;
    h1D_MBD_charge_sum->Fit(f_NBDGlauber, "RL0", "", fitXmin, fitXmax);
    // Options: R=use range from TF1, L=likelihood, 0=don't draw yet

    // Second pass with IMPROVE for better minimum
    h1D_MBD_charge_sum->Fit(f_NBDGlauber, "RL0", "", fitXmin, fitXmax);

    std::cout << "[INFO] Fit complete." << std::endl;
    std::cout << "       Norm = " << f_NBDGlauber->GetParameter(0) << std::endl;
    std::cout << "       mu   = " << f_NBDGlauber->GetParameter(1) << std::endl;
    std::cout << "       k    = " << f_NBDGlauber->GetParameter(2) << std::endl;

    // ===== 5) Build deterministic CDF from fitted TF1 and write centrality cuts =====
    //
    //  Ported faithfully from dosimpleNBMfit_data.cc :: findCentBounds().
    //
    //  The CDF is built LEFT-TO-RIGHT:  cdf[i] = Integral(0 -> x_i)  (trapezoid)
    //  so it is monotone-increasing with x (standard CDF).
    //
    //  For centrality percentile ip (1..nDiv), the charge cut is where:
    //    cdf(x_cut) = (1 - ip/nDiv) * fitTotal
    //  i.e. the fraction of the fit integral ABOVE x_cut equals ip/nDiv.
    //  => 0% centrality is at xMax (highest charge), 100% at xMin.
    // -----------------------------------------------------------------------

    const int    nCdfSteps = 200000;
    const double fullRangeMax = 400.0;
    const double dxCdf = fullRangeMax / nCdfSteps;

    // --- Step 1: build left-to-right CDF ---
    std::vector<double> xCdfArr(nCdfSteps + 1, 0.);
    std::vector<double> cdfArr (nCdfSteps + 1, 0.);
    {
        double prevY = f_NBDGlauber->Eval(0.);
        for (int i = 1; i <= nCdfSteps; ++i)
        {
            const double xi = i * dxCdf;
            const double yi = f_NBDGlauber->Eval(xi);
            xCdfArr[i] = xi;
            cdfArr[i]  = cdfArr[i - 1] + 0.5 * (prevY + yi) * dxCdf;
            prevY = yi;
        }
    }

    // --- Helper: interpolated CDF value at arbitrary x ---
    auto cdfAt = [&](double x) -> double
    {
        if (x <= 0.)            return 0.;
        if (x >= fullRangeMax)  return cdfArr.back();
        const int    idx  = std::min((int)(x / dxCdf), nCdfSteps - 1);
        const double frac = (x - xCdfArr[idx]) / dxCdf;
        return cdfArr[idx] + frac * (cdfArr[idx + 1] - cdfArr[idx]);
    };

    // --- Helper: integral of fit between two x values via CDF ---
    auto integralFromCdf = [&](double xlo, double xhi) -> double
    {
        if (xhi < xlo) std::swap(xlo, xhi);
        xlo = std::max(0.0, std::min(fullRangeMax, xlo));
        xhi = std::max(0.0, std::min(fullRangeMax, xhi));
        return cdfAt(xhi) - cdfAt(xlo);
    };

    const double fitTotalForCent = integralFromCdf(0., fullRangeMax);
    std::cout << "[INFO] CDF total (trapezoid) = " << fitTotalForCent << std::endl;

    // --- findCentBounds: identical logic to reference macro ---
    // xb[0]    = fullRangeMax  (0% boundary, highest charge)
    // xb[ip]   = charge cut where ip/nDiv of the fit area is above it
    // xb[nDiv] -> 0            (100% boundary)
    auto findCentBounds = [&](int nDiv) -> std::vector<double>
    {
        std::vector<double> xb(nDiv + 1, 0.);
        xb[0] = fullRangeMax;
        for (int ip = 1; ip <= nDiv; ip++)
        {
            const double targetCdf = (1.0 - ip / (double)nDiv) * fitTotalForCent;
            // cdfArr is monotone increasing; find first element >= targetCdf
            auto it = std::lower_bound(cdfArr.begin(), cdfArr.end(), targetCdf);
            if (it == cdfArr.begin())      xb[ip] = 0.;
            else if (it == cdfArr.end())   xb[ip] = fullRangeMax;
            else
            {
                const int    idx  = it - cdfArr.begin();
                const double c1v  = cdfArr[idx - 1];
                const double c2v  = cdfArr[idx];
                const double x1v  = xCdfArr[idx - 1];
                const double x2v  = xCdfArr[idx];
                const double frac = (c2v > c1v) ? (targetCdf - c1v) / (c2v - c1v) : 0.;
                xb[ip] = x1v + frac * (x2v - x1v);
            }
        }
        return xb;
    };

    // --- Write 5% boundaries ---
    {
        auto xb = findCentBounds(20);
        std::ofstream out(output_directory + "/GlauberNBDfit_centrality_bounds_5pct.txt");
        out << "# percent_low percent_high x_cut_low x_cut_high\n";
        out << std::setprecision(17);
        for (int ip = 1; ip <= 20; ip++)
            out << (ip - 1) * 5 << " " << ip * 5
                << " " << xb[ip] << " " << xb[ip - 1] << "\n";
        out.close();
        std::cout << "[INFO] 5% centrality bounds -> " << output_directory << "/GlauberNBDfit_centrality_bounds_5pct.txt" << std::endl;
    }

    // --- Write 1% boundaries ---
    auto xb1 = findCentBounds(100);
    {
        std::ofstream out1(output_directory + "/GlauberNBDfit_centrality_bounds_1pct.txt");
        out1 << "# percent_low percent_high x_cut_low x_cut_high\n";
        out1 << std::setprecision(17);
        for (int ip = 1; ip <= 100; ip++)
            out1 << (ip - 1) << " " << ip
                 << " " << xb1[ip] << " " << xb1[ip - 1] << "\n";
        out1.close();
        std::cout << "[INFO] 1% centrality bounds -> " << output_directory << "/GlauberNBDfit_centrality_bounds_1pct.txt" << std::endl;
    }

    // std::vector<std::pair<double,double>> location_fraction; location_fraction.clear();
    // int sampling_step = 100000;
    // double dx_location = 400.0/sampling_step;
    // for (int i = 0; i < sampling_step; i++){
    //     double x =i * dx_location;
    //     double y = f_NBDGlauber->Integral(x, 400)/f_NBDGlauber->Integral(0,400);

    //     if (i%10000 == 0) std::cout << "[DEBUG] x = " << x << ", y = " << y << std::endl;
    //     location_fraction.push_back(std::make_pair(x,y));
    // }

    // for (int i = 1; i <= 100; i++){ // note : centrality 
        
    //     std::pair<double, double> higher_pair = {-999,-999}; 
    //     std::pair<double, double> lower_pair = {-999,-999}; 

    //     for (int pair_i = 0; pair_i < location_fraction.size() - 1; pair_i++){
    //         if (
    //             location_fraction[pair_i].second > (i/100.) &&
    //             location_fraction[pair_i+1].second < (i/100.)
    //         ){
    //             std::cout << "[DEBUG] centrality i = " << i << ", lower_pair = " << location_fraction[pair_i].first << ", " << location_fraction[pair_i].second << ", higher_pair = " << location_fraction[pair_i+1].first << ", " << location_fraction[pair_i+1].second << std::endl;
    //             lower_pair = location_fraction[pair_i];
    //             higher_pair = location_fraction[pair_i+1];
    //             break;
    //         }
    //     }

    // }

    // ===== 5b) Centrality-uniformity validation TGraph =====
    //
    //  For each 1% centrality bin ip (1..100):
    //    x = percent_high  (second column of the output file, i.e. ip)
    //    y = integralFromCdf(x_cut_low, x_cut_high) / fitTotalForCent
    //      = fraction of the fit integral contained in that bin
    //  If the cuts are correct every y should equal exactly 0.01 (1%).
    // -----------------------------------------------------------------------
    TGraph *gr_centVal = new TGraph(100);
    for (int ip = 1; ip <= 100; ip++)
    {
        const double x_cut_low  = xb1[ip];       // third  column
        const double x_cut_high = xb1[ip - 1];   // fourth column
        const double binInteg    = integralFromCdf(x_cut_low, x_cut_high);
        // const double fraction    = (fitTotalForCent > 0.) ? binInteg / fitTotalForCent : 0.;
        const double fraction    = (f_NBDGlauber->Integral(0, 400) > 0.) ? binInteg / f_NBDGlauber->Integral(0, 400) : 0.;
        gr_centVal->SetPoint(ip - 1, (double)ip, fraction);
    }

    gr_centVal->SetMarkerStyle(kFullCircle);
    gr_centVal->SetMarkerSize(0.7);
    gr_centVal->SetMarkerColor(kBlack);
    gr_centVal->SetLineColor(kBlack);
    gr_centVal->SetLineWidth(1);

    TCanvas *c_val = new TCanvas("c_val", "Centrality Uniformity Validation", 800, 600);
    c_val->SetLeftMargin(0.13);
    c_val->SetRightMargin(0.05);
    c_val->SetBottomMargin(0.13);
    c_val->SetTopMargin(0.08);
    c_val->SetTicks(1, 1);
    c_val->SetGridx();
    c_val->SetGridy();

    // Draw with axes; GetHistogram() only exists after Draw("AP")
    gr_centVal->Draw("AP");

    gr_centVal->GetXaxis()->SetTitle("Centrality percentile (%)");
    gr_centVal->GetYaxis()->SetTitle("Fit integral fraction per 1% bin");
    gr_centVal->GetXaxis()->SetRangeUser(-0.5, 100.5);
    gr_centVal->GetYaxis()->SetRangeUser(0.0, 0.05);
    gr_centVal->GetXaxis()->CenterTitle();
    gr_centVal->GetYaxis()->CenterTitle();
    gr_centVal->GetXaxis()->SetTitleFont(43);  gr_centVal->GetXaxis()->SetTitleSize(19);
    gr_centVal->GetXaxis()->SetLabelFont(43);  gr_centVal->GetXaxis()->SetLabelSize(17);
    gr_centVal->GetYaxis()->SetTitleFont(43);  gr_centVal->GetYaxis()->SetTitleSize(19);
    gr_centVal->GetYaxis()->SetLabelFont(43);  gr_centVal->GetYaxis()->SetLabelSize(17);
    gr_centVal->GetYaxis()->SetTitleOffset(1.6);
    gr_centVal->SetTitle("");

    // Expected value: 1% = 0.01
    TLine *lexp = new TLine(-0.5, 0.01, 100.5, 0.01);
    lexp->SetLineStyle(2);
    lexp->SetLineColor(kRed + 1);
    lexp->SetLineWidth(2);
    lexp->Draw();

    TLatex valLabel;
    valLabel.SetNDC();
    valLabel.SetTextFont(43);
    valLabel.SetTextSize(17);
    valLabel.DrawLatex(0.14, 0.94, "O+O #sqrt{s_{NN}} = 200 GeV  |  Run 82391  |  NBD-Glauber: centrality uniformity check");

    c_val->Update();
    c_val->SaveAs((output_directory + "/GlauberNBDfit_cent_validation.pdf").c_str());
    std::cout << "[INFO] Centrality validation plot -> " << output_directory << "/GlauberNBDfit_cent_validation.pdf" << std::endl;


    // ===== 6) Draw and save  (two-pad canvas) =====
    // Split: upper pad 70%, lower pad 30%
    const double splitFrac = 0.30;   // fraction of canvas height for ratio pad

    TCanvas *c1 = new TCanvas("c1", "NBD-Glauber Fit to MBD Charge Sum", 900, 800);
    c1->SetFillColor(0);

    // ---- Upper pad (main plot) ----
    TPad *pad1 = new TPad("pad1", "pad1", 0.0, splitFrac, 1.0, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->SetTopMargin(0.10);
    pad1->SetLeftMargin(0.14);
    pad1->SetRightMargin(0.05);
    pad1->SetLogy();
    pad1->SetGridx();
    pad1->SetGridy();
    pad1->Draw();
    pad1->cd();

    // --- Data histogram style (upper pad) ---
    h1D_MBD_charge_sum->SetTitle("");
    h1D_MBD_charge_sum->SetStats(0);
    h1D_MBD_charge_sum->SetLineColor(kBlack);
    h1D_MBD_charge_sum->SetMarkerStyle(20);
    h1D_MBD_charge_sum->SetMarkerSize(0.6);
    h1D_MBD_charge_sum->SetMarkerColor(kBlack);
    // X-axis: suppress labels/title on upper pad (shared with ratio pad)
    h1D_MBD_charge_sum->GetXaxis()->SetLabelSize(0.0);
    h1D_MBD_charge_sum->GetXaxis()->SetTitleSize(0.0);
    h1D_MBD_charge_sum->GetXaxis()->SetRangeUser(0.0, 400.0);
    // Y-axis: scale up sizes to compensate for smaller pad height
    double scaleUp = 1.0 / (1.0 - splitFrac);
    h1D_MBD_charge_sum->GetYaxis()->SetTitle("Counts");
    h1D_MBD_charge_sum->GetYaxis()->SetTitleSize(0.045 * scaleUp);
    h1D_MBD_charge_sum->GetYaxis()->SetLabelSize(0.035 * scaleUp);
    h1D_MBD_charge_sum->GetYaxis()->SetTitleOffset(0.9);
    h1D_MBD_charge_sum->SetMinimum(0.001);

    h1D_MBD_charge_sum->Draw("EP");

    // Draw fit function on top
    f_NBDGlauber->SetNpx(10000);
    f_NBDGlauber->SetRange(0, 400);
    f_NBDGlauber->Draw("SAME");

    // --- Legend ---
    TLegend *leg = new TLegend(0.40, 0.68, 0.94, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.040 * scaleUp);
    leg->AddEntry(h1D_MBD_charge_sum, "Data: MBD Charge Sum", "ep");
    leg->AddEntry(f_NBDGlauber,        "NBD #otimes Glauber (O+O) fit", "l");
    leg->Draw();

    // --- Fit parameter labels ---
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.033 * scaleUp);
    latex.SetTextFont(42);

    double chi2   = f_NBDGlauber->GetChisquare();
    int    ndf    = f_NBDGlauber->GetNDF();
    double norm_v = f_NBDGlauber->GetParameter(0);
    double norm_e = f_NBDGlauber->GetParError(0);
    double mu_v   = f_NBDGlauber->GetParameter(1);
    double mu_e   = f_NBDGlauber->GetParError(1);
    double k_v    = f_NBDGlauber->GetParameter(2);
    double k_e    = f_NBDGlauber->GetParError(2);

    latex.DrawLatex(0.550, 0.62, Form("#chi^{2}/NDF = %.1f / %d = %.2f", chi2, ndf,
                                      (ndf > 0 ? chi2 / ndf : 0.0)));
    latex.DrawLatex(0.550, 0.55, Form("Norm = %.2e #pm %.2e", norm_v, norm_e));
    latex.DrawLatex(0.550, 0.48, Form("#mu = %.3f #pm %.3f", mu_v, mu_e));
    latex.DrawLatex(0.550, 0.41, Form("k = %.3f #pm %.3f", k_v, k_e));

    // --- Title label ---
    TLatex title;
    title.SetNDC();
    title.SetTextSize(0.042 * scaleUp);
    title.SetTextFont(62);
    title.DrawLatex(0.14, 0.92, "O+O #sqrt{s_{NN}} = 200 GeV  |  Run 82391  |  NBD-Glauber Fit");

    pad1->Update();

    // ---- Lower pad (Data / Fit ratio) ----
    c1->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, splitFrac);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.30);
    pad2->SetLeftMargin(0.14);
    pad2->SetRightMargin(0.05);
    pad2->SetGridx();
    pad2->SetGridy();
    pad2->Draw();
    pad2->cd();

    // Build ratio histogram: Data / Fit
    TH1D *h_ratio = (TH1D*)h1D_MBD_charge_sum->Clone("h_ratio");
    h_ratio->SetTitle("");
    h_ratio->SetStats(0);
    int nbins_ratio = h_ratio->GetNbinsX();
    for (int ib = 1; ib <= nbins_ratio; ++ib)
    {
        double xc   = h_ratio->GetBinCenter(ib);
        double data = h1D_MBD_charge_sum->GetBinContent(ib);
        double err  = h1D_MBD_charge_sum->GetBinError(ib);
        double fit  = f_NBDGlauber->Eval(xc);
        if (fit > 0.0) {
            h_ratio->SetBinContent(ib, data / fit);
            h_ratio->SetBinError(ib,   err  / fit);
        } else {
            h_ratio->SetBinContent(ib, 0.0);
            h_ratio->SetBinError(ib,   0.0);
        }
    }

    double fit_integral = f_NBDGlauber->Integral(0, 400) / h1D_MBD_charge_sum->GetBinWidth(1);
    double data_integral = h1D_MBD_charge_sum->Integral(0, h1D_MBD_charge_sum->GetNbinsX());
    std::cout << "[INFO] Fit integral = " << fit_integral << std::endl;
    std::cout << "[INFO] Data integral = " << data_integral << std::endl;
    std::cout << "[INFO] Ratio of integrals = " <<  data_integral / fit_integral << std::endl;

    // Ratio pad axis styling
    double scaleDown = 1.0 / splitFrac;
    h_ratio->GetXaxis()->SetTitle("MBD Charge Sum");
    h_ratio->GetXaxis()->SetTitleSize(0.045 * scaleDown);
    h_ratio->GetXaxis()->SetLabelSize(0.035 * scaleDown);
    h_ratio->GetXaxis()->SetTitleOffset(1.0);
    h_ratio->GetXaxis()->SetRangeUser(0.0, 400.0);
    h_ratio->GetYaxis()->SetTitle("Data / Fit");
    h_ratio->GetYaxis()->SetTitleSize(0.040 * scaleDown);
    h_ratio->GetYaxis()->SetLabelSize(0.035 * scaleDown);
    h_ratio->GetYaxis()->SetTitleOffset(0.55);
    h_ratio->GetYaxis()->SetNdivisions(505);
    h_ratio->GetYaxis()->SetRangeUser(0.9, 1.1);
    h_ratio->SetMarkerStyle(20);
    h_ratio->SetMarkerSize(0.6);
    h_ratio->SetMarkerColor(kBlack);
    h_ratio->SetLineColor(kBlack);

    h_ratio->Draw("EP");

    // Unity line
    TLine *unity = new TLine(0.0, 1.0, 400.0, 1.0);
    unity->SetLineColor(kRed);
    unity->SetLineWidth(2);
    unity->SetLineStyle(2);   // dashed
    unity->Draw("SAME");

    pad2->Update();

    c1->cd();
    c1->Update();

    std::string outPDF = output_directory + "/GlauberNBDfit_MBDChargeSum.pdf";
    c1->SaveAs(outPDF.c_str());
    std::cout << "[INFO] Saved plot to: " << outPDF << std::endl;

    // ===== 7) Write all canvases to an output ROOT file =====
    std::string outROOT = output_directory + "/GlauberNBDfit_output.root";
    TFile *fout = TFile::Open(outROOT.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "[ERROR] Cannot create output ROOT file: " << outROOT << std::endl;
    } else {
        fout->cd();

        // --- TTree of fit parameters ---
        Double_t t_Norm,    t_Norm_err;
        Double_t t_mu,      t_mu_err;
        Double_t t_k,       t_k_err;
        Double_t t_chi2,    t_ndf,   t_chi2ndf;
        Double_t t_fitXmin, t_fitXmax;

        TTree *fitTree = new TTree("fitParams", "NBD-Glauber fit parameters");
        fitTree->Branch("Norm",      &t_Norm,     "Norm/D");
        fitTree->Branch("Norm_err",  &t_Norm_err, "Norm_err/D");
        fitTree->Branch("mu",        &t_mu,       "mu/D");
        fitTree->Branch("mu_err",    &t_mu_err,   "mu_err/D");
        fitTree->Branch("k",         &t_k,        "k/D");
        fitTree->Branch("k_err",     &t_k_err,    "k_err/D");
        fitTree->Branch("chi2",      &t_chi2,     "chi2/D");
        fitTree->Branch("ndf",       &t_ndf,      "ndf/D");
        fitTree->Branch("chi2ndf",   &t_chi2ndf,  "chi2ndf/D");
        fitTree->Branch("fitXmin",   &t_fitXmin,  "fitXmin/D");
        fitTree->Branch("fitXmax",   &t_fitXmax,  "fitXmax/D");

        // Retrieve from already-computed variables (norm_v/e, mu_v/e, k_v/e, chi2, ndf)
        t_Norm     = norm_v;   t_Norm_err = norm_e;
        t_mu       = mu_v;     t_mu_err   = mu_e;
        t_k        = k_v;      t_k_err    = k_e;
        t_chi2     = chi2;
        t_ndf      = (Double_t)ndf;
        t_chi2ndf  = (ndf > 0) ? chi2 / ndf : 0.;
        t_fitXmin  = fitXmin;
        t_fitXmax  = fitXmax;

        fitTree->Fill();
        fitTree->Write();
        std::cout << "[INFO] TTree 'fitParams' written to: " << outROOT << std::endl;

        // --- Canvases ---
        c1->Write();       // main fit + Data/Fit ratio canvas
        c_val->Write();    // centrality uniformity validation canvas
        fout->Close();
        std::cout << "[INFO] Canvases written to: " << outROOT << std::endl;
    }
}
