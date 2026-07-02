// note : Test macro for GetFinaldNdEta
// note : Run with: root -l -b -q test_GetFinaldNdEta.C

#include "GetFinaldNdEta.cpp"

int test_GetFinaldNdEta()
{
    std::string output_dir = "./test_GetFinaldNdEta_output";

    // Division : -Build baseline TGraphErrors (10 points, x = -1.0 to 0.8, y = 10)---
    const int    n_points   = 10;
    const double x_start    = -1.0;
    const double x_step     =  0.2;
    const double y_baseline = 10.;

    TGraphErrors * grE_baseline = new TGraphErrors(n_points);
    grE_baseline->SetName("grE_baseline");
    for (int i = 0; i < n_points; i++) {
        double x = x_start + i * x_step;
        grE_baseline->SetPoint(i, x, y_baseline);
        grE_baseline->SetPointError(i, 0., 0);
    }

    // Division : -Build baseline TH1D (27 bins, -2.7 to 2.7)--------------------------
    TH1D * h1D_baseline = new TH1D("h1D_baseline", "h1D_baseline", 27, -2.7, 2.7);
    h1D_baseline->Sumw2(true);
    for (int b = 1; b <= h1D_baseline->GetNbinsX(); b++) {
        double x = h1D_baseline->GetBinCenter(b);
        if (x >= x_start && x <= x_start + (n_points - 1) * x_step + 1e-9) {
            h1D_baseline->SetBinContent(b, y_baseline);
            h1D_baseline->SetBinError(b, 0.05 * y_baseline);
        }
    }

    TH1D* h1D_MC_test = new TH1D("h1D_MC_test","",53,-5.3,5.3);
    for (int i = 0; i < h1D_MC_test->GetNbinsX(); i++) {
        h1D_MC_test->SetBinContent(i+1, 1*i+1 - 10);
        // h1D_MC_test->SetBinError(i+1, 0);
    }

    // Division : -Build variation vectors----------------------------------------------
    // note : x values match the TGraphErrors
    double x_vals[n_points];
    for (int i = 0; i < n_points; i++) { x_vals[i] = x_start + i * x_step; }

    // note : var_1 relative uncertainties
    double y_var1[n_points] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.3};

    // note : var_2 relative uncertainties
    double y_var2[n_points] = {0.1, 0.2, 0.4, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.3};

    std::vector<std::pair<double,double>> vec_var1, vec_var2;
    for (int i = 0; i < n_points; i++) {
        vec_var1.push_back({ x_vals[i], y_var1[i] });
        vec_var2.push_back({ x_vals[i], y_var2[i] });
    }

    // Division : -Construct GetFinaldNdEta---------------------------------------------
    GetFinaldNdEta * GFD = new GetFinaldNdEta(
        0,                                              // Mbin
        {0, 10},                                        // centrality_range
        {-15, 15},                                      // zvtx_range
        "Internal",                                     // sPHENIX_label
        "O+O #sqrt{s_{NN}} = 200 GeV",                  // Collision_system
        output_dir                                      // output_directory
    );

    // Division : -Set up baseline------------------------------------------------------
    GFD->SetUpBaselinedNdEta("Data (INTT-only, combinatoric approach)", grE_baseline, h1D_baseline);
    GFD->SetUpMCdNdEta("HIJING", "HIJING (generator)", h1D_MC_test);

    // Division : -Register variations--------------------------------------------------
    GFD->SetUpOneVariation("var_1", "Variation 1 (VtxZ range)", vec_var1);
    GFD->SetUpOneVariation("var_2", "Variation 2 (#Delta#phi cut)", vec_var2);

    // Division : -Compute and draw total systematic uncertainty------------------------
    GFD->PrepareFinalSystematicUnc();

    // GFD->PrepareFinaldNdEta(50, true);
    GFD->PrepareFinaldNdEta();

    GFD->PrepareOutputRootFile();

    std::cout << "GetFinaldNdEta test finished. Output saved to: " << output_dir << std::endl;

    delete GFD;
    return 0;
}
