// note : Test macro for EachVariation
// note : Run with: root -l -b -q test_EachVariation.C

#include "EachVariation.cpp"

// note : helper – build a flat TGraphErrors (all y values equal to `level`)
//        nPoints points uniformly spaced in [x_lo, x_hi]
//        y_err = 0.05 * level
TGraphErrors * MakeFlatGraph(
    int    nPoints,
    double x_lo, double x_hi,
    double level,
    const char * name
)
{
    TGraphErrors * gr = new TGraphErrors(nPoints);
    gr->SetName(name);

    double step = (x_hi - x_lo) / (nPoints - 1);
    for (int i = 0; i < nPoints; i++) {
        double x   = x_lo + i * step;
        double err = 0.01 * level;
        gr->SetPoint(i, x, level);
        gr->SetPointError(i, 0., err);
    }
    return gr;
}

// note : helper – build a dummy TH1D with all bins set to `level`
TH1D * MakeFlatHist(int nBins, double x_lo, double x_hi, double level, const char * name)
{
    TH1D * h = new TH1D(name, name, nBins, x_lo, x_hi);
    h->Sumw2(true);
    for (int i = 1; i <= nBins; i++) {
        h->SetBinContent(i, level);
        h->SetBinError(i, 0.05 * level);
    }
    return h;
}

int test_EachVariation()
{
    // Division : -Settings-------------------------------------------------------------
    const int    nPoints        = 30;
    const double x_lo           = -1.5;
    const double x_hi           =  1.5;
    const double baseline_level =  20.;

    std::string output_dir = "./test_EachVariation_output";

    // Division : -Build baseline and 3 variations--------------------------------------
    // note : baseline – flat at 20
    TH1D        * h_baseline  = MakeFlatHist(nPoints, x_lo, x_hi, baseline_level, "h_baseline");
    TGraphErrors* gr_baseline = MakeFlatGraph(nPoints, x_lo, x_hi, baseline_level, "grE_baseline");

    // note : variation 1 – +7.5%
    double level_var1 = baseline_level * 1.075;
    TH1D        * h_var1  = MakeFlatHist(nPoints, x_lo, x_hi, level_var1, "h_var1");
    TGraphErrors* gr_var1 = MakeFlatGraph(nPoints, x_lo, x_hi, level_var1, "grE_var1");

    // note : variation 2 – -5%
    double level_var2 = baseline_level * 0.95;
    TH1D        * h_var2  = MakeFlatHist(nPoints, x_lo, x_hi, level_var2, "h_var2");
    TGraphErrors* gr_var2 = MakeFlatGraph(nPoints, x_lo, x_hi, level_var2, "grE_var2");

    // note : variation 3 – -9%
    double level_var3 = baseline_level * 0.91;
    TH1D        * h_var3  = MakeFlatHist(nPoints, x_lo, x_hi, level_var3, "h_var3");
    TGraphErrors* gr_var3 = MakeFlatGraph(nPoints, x_lo, x_hi, level_var3, "grE_var3");

    // Division : -Build input_dNdEta vector--------------------------------------------
    std::vector<std::tuple<std::string, TH1D*, TGraphErrors*>> input_dNdEta = {
        { "Baseline",            h_baseline, gr_baseline },
        { "Variation 1 (+7.5%)", h_var1,     gr_var1     },
        { "Variation 2 (-5%)",   h_var2,     gr_var2     },
        { "Variation 3 (-9%)",   h_var3,     gr_var3     }
    };

    // Division : -Construct EachVariation and let it draw------------------------------
    EachVariation * EV = new EachVariation(
        0,
        {0,  10},                                         // centrality_range
        {-15, 15},                                        // zvtx_range
        "Internal",
        "O+O #sqrt{s_{NN}} = 200 GeV",         // sPHENIX_label
        output_dir,                                       // output_directory

        "test_variation",                                 // variation_name
        "Test: flat-level variations",                    // variation_description
        input_dNdEta,                                     // input_dNdEta

        {x_lo, x_hi},                                    // x_range
        {"Pseudorapidity #eta", "dN_{ch}/d#eta"}          // plot_title
    );

    std::vector<std::pair<double,double>> max_rel_diff = EV->GetMaxRelativeDiff();

    std::cout << "Max relative difference: " << std::endl;
    for (int i = 0; i < max_rel_diff.size(); i++) {
        std::cout << "Point " << i << ": " << max_rel_diff[i].first << " , " << max_rel_diff[i].second << std::endl;
    }

    std::cout << "EachVariation test finished. Output saved to: " << output_dir << std::endl;

    delete EV;
    return 0;
}
