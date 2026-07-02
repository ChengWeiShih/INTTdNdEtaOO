#include "GetFinaldNdEta.h"

// Division : -Constructor--------------------------------------------------------------
GetFinaldNdEta::GetFinaldNdEta(
    int                  Mbin_in,
    std::pair<int,int>   centrality_range_in,
    std::pair<int,int>   zvtx_range_in,
    std::string          sPHENIX_label_in,
    std::string          Collision_system_in,

    std::string          output_directory_in
    // std::pair<double,double> x_range_in
):
    Mbin(Mbin_in),
    centrality_range(centrality_range_in),
    zvtx_range(zvtx_range_in),
    sPHENIX_label(sPHENIX_label_in),
    Collision_system(Collision_system_in),
    output_directory(output_directory_in)
    // x_range(x_range_in)
{

    MC_name_vec.clear();
    MC_legend_vec.clear();
    h1D_MC_vec.clear();

    all_variation_name.clear();
    all_variation_legend.clear();
    all_variation_points_vec.clear();
    all_variation_gr_vec.clear();

    final_syst_vec.clear();
}

// Division : -SetUpBaselinedNdEta------------------------------------------------------
void GetFinaldNdEta::SetUpBaselinedNdEta(std::string baseline_legend_name_in, TGraphErrors * input_grE, TH1D * input_h1D)
{
    if (!input_grE) {
        std::cout << "Error [GetFinaldNdEta::SetUpBaselinedNdEta] : input_grE is nullptr." << std::endl;
        exit(1);
    }
    if (!input_h1D) {
        std::cout << "Error [GetFinaldNdEta::SetUpBaselinedNdEta] : input_h1D is nullptr." << std::endl;
        exit(1);
    }
    if (input_grE->GetN() <= 0) {
        std::cout << "Error [GetFinaldNdEta::SetUpBaselinedNdEta] : input_grE has 0 points." << std::endl;
        exit(1);
    }

    baseline_legend_name = baseline_legend_name_in;

    grE_baseline = (TGraphErrors*)input_grE->Clone(Form("grE_baseline_Mbin%d", Mbin));
    h1D_baseline = (TH1D*)input_h1D->Clone(Form("h1D_baseline_Mbin%d", Mbin));
}

// Division : -SetUpMCdNdEta-----------------------------------------------------------
void GetFinaldNdEta::SetUpMCdNdEta(std::string MC_name_in, std::string MC_legend_name_in, TH1D * input_h1D)
{
    if (!input_h1D) {
        std::cout << "Error [GetFinaldNdEta::SetUpMCdNdEta] : input_h1D is nullptr." << std::endl;
        exit(1);
    }

    MC_name_vec.push_back(MC_name_in);
    MC_legend_vec.push_back(MC_legend_name_in);
    h1D_MC_vec.push_back((TH1D*) input_h1D->Clone(Form("h1D_MC_%s_Mbin%d", MC_name_in.c_str(), Mbin)));
    int mc_idx = (int)MC_name_vec.size() - 1; // note : index of the newly pushed entry
    h1D_MC_vec.back()->SetLineWidth(2);
    h1D_MC_vec.back()->SetLineColor(TColor::GetColor(marker_colors[mc_idx % (int)marker_colors.size()].c_str()));
}

// Division : -SetUpOneVariation----------------------------------------------------------
void GetFinaldNdEta::SetUpOneVariation(
    std::string variation_name,
    std::string variation_legend,
    std::vector<std::pair<double,double>> input_vec
)
{
    // note : 3.1 – size must match baseline TGraphErrors point count
    if (!grE_baseline) {
        std::cout << "Error [GetFinaldNdEta::SetUpOneVariation] : call SetUpBaselinedNdEta first." << std::endl;
        exit(1);
    }
    int n_baseline = grE_baseline->GetN();
    if ((int)input_vec.size() != n_baseline) {
        std::cout << "Error [GetFinaldNdEta::SetUpOneVariation] : \""
                  << variation_legend << "\" has " << input_vec.size()
                  << " points, but baseline TGraphErrors has " << n_baseline
                  << " points." << std::endl;
        exit(1);
    }

    for (int point_i = 0; point_i < int(input_vec.size()); point_i++) {
        if (std::abs(input_vec[point_i].first - grE_baseline->GetX()[point_i]) > 0.0005) {
            std::cout << "For variation " << variation_legend << ","
                      << " Error [GetFinaldNdEta::SetUpOneVariation] : "
                      << "x values of the " << point_i << "-th point do not match, "
                      << input_vec[point_i].first << " != " << grE_baseline->GetX()[point_i] << std::endl;
            exit(1);
        }
    }

    // note : 3.2 – store label and point vector
    all_variation_name.push_back(variation_name);
    all_variation_legend.push_back(variation_legend);
    all_variation_points_vec.push_back(input_vec);

    // note : 3.3/3.4 – build TGraph and set marker properties
    int var_index = (int)all_variation_gr_vec.size(); // 0-based index of this variation

    TGraph * gr = new TGraph();
    gr->SetName(Form("gr_var_%d_Mbin%d", var_index, Mbin));

    for (int j = 0; j < (int)input_vec.size(); j++) {
        gr->SetPoint(j, input_vec[j].first, input_vec[j].second);
    }

    int style_idx = var_index % max_marker_entries;
    int color_idx = var_index % (int)marker_colors.size();
    gr->SetMarkerStyle(marker_styles[style_idx]);
    gr->SetMarkerColor(TColor::GetColor(marker_colors[color_idx].c_str()));
    gr->SetLineColor(TColor::GetColor(marker_colors[color_idx].c_str()));
    gr->SetLineWidth(2);
    gr->SetMarkerSize(1.5);

    all_variation_gr_vec.push_back(gr);
}

// Division : -PrepareFinalSystematicUnc------------------------------------------------
void GetFinaldNdEta::PrepareFinalSystematicUnc()
{
    // note : must have baseline
    if (!grE_baseline || !h1D_baseline) {
        std::cout << "Error [GetFinaldNdEta::PrepareFinalSystematicUnc] : call SetUpBaselinedNdEta first." << std::endl;
        exit(1);
    }
    // note : must have at least one variation
    if (all_variation_points_vec.empty()) {
        std::cout << "Error [GetFinaldNdEta::PrepareFinalSystematicUnc] : no variations registered. Call SetUpOneVariation first." << std::endl;
        exit(1);
    }

    // note : 5.1 – verify all variation vectors have the same size
    int n_points = (int)all_variation_points_vec[0].size();
    for (int i = 1; i < (int)all_variation_points_vec.size(); i++) {
        if ((int)all_variation_points_vec[i].size() != n_points) {
            std::cout << "Error [GetFinaldNdEta::PrepareFinalSystematicUnc] : variation \""
                      << all_variation_name[i] << "\" has " << all_variation_points_vec[i].size()
                      << " points, but first variation has " << n_points << " points." << std::endl;
            exit(1);
        }
    }

    // note : 5.2 – clone h1D_baseline to create h1D_FinalSyst
    h1D_FinalSyst = (TH1D *) h1D_baseline->Clone(Form("h1D_FinalSyst_Mbin%d", Mbin));
    // note : 5.22 – reset all content/errors/stats
    h1D_FinalSyst->Reset("ICESM");
    h1D_FinalSyst->SetLineColor(kBlack);
    // h1D_FinalSyst->SetLineStyle(kSolid);
    h1D_FinalSyst->SetLineWidth(3);

    // note : 5.3 – quadrature sum of all relative uncertainties per point
    for (int j = 0; j < n_points; j++) {
        double x_j      = all_variation_points_vec[0][j].first;
        double quad_sum = 0.;

        for (int i = 0; i < (int)all_variation_points_vec.size(); i++) {
            double rel_unc = all_variation_points_vec[i][j].second;
            quad_sum += rel_unc * rel_unc;
        }
        quad_sum = std::sqrt(quad_sum);

        // note : find the histogram bin whose center is closest to x_j
        int bin = h1D_FinalSyst->FindBin(x_j);
        h1D_FinalSyst->SetBinContent(bin, quad_sum);

        // note : store in final_syst_vec
        final_syst_vec.push_back({x_j, quad_sum});

        std::cout << "eta center = " << x_j << ", quad_sum = " << quad_sum << std::endl;
    }

    // note : 5.4 – canvas
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", output_directory.c_str(), output_directory.c_str()));

    TCanvas * c1 = new TCanvas(
        Form("c1_FinalSyst_Mbin%d", Mbin),
        Form("c1_FinalSyst_Mbin%d", Mbin),
        600, 600
    );
    c1->Draw();
    c1->SetTicks(1, 1);
    c1->SetLeftMargin(0.13);
    c1->SetRightMargin(0.04);
    c1->SetTopMargin(0.08);
    c1->SetBottomMargin(0.13);

    // note : determine a sensible y-axis max from the individual variations
    double y_max = h1D_FinalSyst->GetBinContent(h1D_FinalSyst->GetMaximumBin()) * 1.8;
    // for (auto & gr : all_variation_gr_vec) {
    //     for (int j = 0; j < gr->GetN(); j++) {
    //         double xp, yp;
    //         gr->GetPoint(j, xp, yp);
    //         if (yp > y_max) { y_max = yp; }
    //     }
    // }
    // // note : also consider the total
    // for (int b = 1; b <= h1D_FinalSyst->GetNbinsX(); b++) {
    //     double yb = h1D_FinalSyst->GetBinContent(b);
    //     if (yb > y_max) { y_max = yb; }
    // }
    
    // y_max *= 1.3; // note : leave headroom for legend

    // note : draw frame
    TH1F * h_frame = (TH1F *) c1->DrawFrame(h1D_FinalSyst->GetXaxis()->GetXmin(), 0., h1D_FinalSyst->GetXaxis()->GetXmax(), y_max);
    h_frame->GetXaxis()->SetTitle("Pseudorapidity #eta");
    h_frame->GetXaxis()->SetTitleSize(0.055);
    h_frame->GetXaxis()->SetTitleOffset(1.0);
    h_frame->GetXaxis()->SetLabelSize(0.05);
    h_frame->GetYaxis()->SetTitle("Relative uncertainty");
    h_frame->GetYaxis()->SetTitleSize(0.050);
    h_frame->GetYaxis()->SetTitleOffset(1.2);
    h_frame->GetYaxis()->SetLabelSize(0.05);

    // note : legend
    TLegend * leg = new TLegend(0.15, 0.68, 0.93, 0.86);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.03);
    leg->SetMargin(0.05);

    leg->AddEntry(h1D_FinalSyst, "Total uncertainty", "l");

    // note : draw individual variation TGraphs
    for (int i = 0; i < (int)all_variation_gr_vec.size(); i++) {
        all_variation_gr_vec[i]->Draw("p same");
        leg->AddEntry(all_variation_gr_vec[i], all_variation_legend[i].c_str(), "p");
    }

    // note : draw total systematic uncertainty (hist, thick black line)
    h1D_FinalSyst->Draw("hist same");

    leg->Draw();

    // note : sPHENIX label
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(62);
    latex.SetTextSize(0.042);
    latex.DrawLatex(0.14, 0.9350, Form("#it{sPHENIX} #bf{%s}", sPHENIX_label.c_str()));

    latex.SetTextFont(42);
    latex.SetTextSize(0.038);
    latex.DrawLatex(0.15, 0.87, Form("z-vertex: [%d, %d] cm, Cent.: %d-%d%%", zvtx_range.first, zvtx_range.second, centrality_range.first, centrality_range.second) );

    latex.SetTextFont(62);
    latex.SetTextSize(0.042);
    latex.SetTextAlign(31);
    // latex.Update();
    latex.DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#bf{%s}", Collision_system.c_str()));

    c1->Update();

    c1->SaveAs(Form("%s/Mbin%d_FinalSyst.pdf", output_directory.c_str(), Mbin));
    c1->SaveAs(Form("%s/Mbin%d_FinalSyst.png", output_directory.c_str(), Mbin));

    // delete c1;
}

// Division : -PrepareFinaldNdEta-------------------------------------------------------
void GetFinaldNdEta::PrepareFinaldNdEta(double y_axis_max, bool IsDrawMC)
{
    // note : 2.1 – apply sPHENIX style
    // gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();

    if (!grE_baseline || !h1D_baseline) {
        std::cout << "Error [GetFinaldNdEta::PrepareFinaldNdEta] : call SetUpBaselinedNdEta first." << std::endl;
        exit(1);
    }
    if (final_syst_vec.empty()) {
        std::cout << "Error [GetFinaldNdEta::PrepareFinaldNdEta] : call PrepareFinalSystematicUnc first." << std::endl;
        exit(1);
    }

    if (final_syst_vec.size() != grE_baseline->GetN()) {
        std::cout << "Error [GetFinaldNdEta::PrepareFinaldNdEta] : final_syst_vec size does not match grE_baseline size." << std::endl;
        exit(1);
    }

    y_axis_max = (y_axis_max == -999) ? h1D_baseline -> GetBinContent(h1D_baseline->GetMaximumBin()) * 2.5 : y_axis_max;

    // note : 2.2 – build grE_baseline_Syst as a copy of grE_baseline,
    //             overwrite y-errors with absolute systematic uncertainties
    grE_baseline_Syst = (TGraphErrors *) grE_baseline->Clone(Form("grE_baseline_Syst_Mbin%d", Mbin));
    int n = grE_baseline_Syst->GetN();
    for (int j = 0; j < n; j++) {
        double xp, yp;
        grE_baseline_Syst->GetPoint(j, xp, yp);
        // note : find matching entry in final_syst_vec by x position
        double abs_syst = 0.;

        if (fabs(final_syst_vec[j].first - xp) > 0.0005) {
            std::cout << "Error [GetFinaldNdEta::PrepareFinaldNdEta] : x position does not match. point in x = " << xp << ", point in final_syst_vec: " << final_syst_vec[j].first << ", relative syst. = " << final_syst_vec[j].second << std::endl;
            exit(1);
        }

        abs_syst = final_syst_vec[j].second * yp;

        grE_baseline_Syst->SetPointError(j, h1D_baseline->GetBinWidth(1) / 2., abs_syst);
    }

    // note : 2.3 – style of grE_baseline_Syst
    grE_baseline_Syst->SetMarkerStyle(20);
    grE_baseline_Syst->SetMarkerSize(1);
    grE_baseline_Syst->SetMarkerColor(1);
    grE_baseline_Syst->SetFillColorAlpha(1, 0.5);
    grE_baseline_Syst->SetLineColorAlpha(1, 0);
    grE_baseline_Syst->SetLineWidth(0);

    // note : 2.4 – canvas
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", output_directory.c_str(), output_directory.c_str()));

    TCanvas * c_final = new TCanvas(
        Form("c_finaldNdEta_Mbin%d", Mbin),
        Form("c_finaldNdEta_Mbin%d", Mbin),
        960, 800
    );
    c_final->Draw();
    c_final->SetTicks(1, 1);

    // note : 2.4 – draw frame: x from h1D_baseline axis, y from [0, y_axis_max]
    TH1F * h_frame = (TH1F *) c_final->DrawFrame(
        h1D_baseline->GetXaxis()->GetXmin(), 0.,
        h1D_baseline->GetXaxis()->GetXmax(), y_axis_max
    );
    h_frame->GetXaxis()->SetTitle("Pseudorapidity #eta");
    h_frame->GetXaxis()->SetTitleSize(0.055);
    h_frame->GetXaxis()->SetTitleOffset(1.0);
    h_frame->GetXaxis()->SetLabelSize(0.05);
    h_frame->GetYaxis()->SetTitle("dN_{ch}/d#eta");
    h_frame->GetYaxis()->SetTitleSize(0.055);
    h_frame->GetYaxis()->SetTitleOffset(1.1);
    h_frame->GetYaxis()->SetLabelSize(0.05);

    // note : labels
    TLatex latex_final;
    latex_final.SetNDC();
    latex_final.SetTextFont(62);
    latex_final.SetTextSize(0.042);
    latex_final.DrawLatex(0.17, 0.955, Form("#it{sPHENIX} #bf{%s}", sPHENIX_label.c_str()));

    latex_final.SetTextFont(42);
    latex_final.SetTextSize(0.038);
    latex_final.DrawLatex(0.2, 0.89, Collision_system.c_str());
    latex_final.DrawLatex(0.2, 0.84, Form("Cent. %d-%d%%, z-vertex: [%d, %d] cm",
        centrality_range.first, centrality_range.second,
        zvtx_range.first, zvtx_range.second));

    // note : 2.6 – legend
    TLegend * leg_final = new TLegend(0.2, 0.62, 0.90, 0.80);
    leg_final->SetBorderSize(0);
    leg_final->SetFillStyle(0);
    leg_final->SetTextSize(0.036);
    leg_final->SetMargin(0.05);
    leg_final->AddEntry(grE_baseline_Syst, baseline_legend_name.c_str(), "fp");

    // note : 2.6 – overlay MC histograms if requested
    if (!h1D_MC_vec.empty() && IsDrawMC) {
        for (int i = 0; i < (int)h1D_MC_vec.size(); i++) {
            h1D_MC_vec[i]->Draw("hist same");
            leg_final->AddEntry(h1D_MC_vec[i], MC_legend_vec[i].c_str(), "l");
        }
    }

    // note : 2.5 – draw grE_baseline_Syst (systematic band) then data points
    grE_baseline_Syst->Draw("p2 same"); // note : "2" draws the filled error band

    leg_final->Draw();

    c_final->Update();

    c_final->SaveAs(Form("%s/Mbin%d_finaldNdEta.pdf", output_directory.c_str(), Mbin));
    c_final->SaveAs(Form("%s/Mbin%d_finaldNdEta.png", output_directory.c_str(), Mbin));
}

// Division : -PrepareOutputRootFile----------------------------------------------------
void GetFinaldNdEta::PrepareOutputRootFile()
{
    // note : require baseline to exist
    if (!grE_baseline || !h1D_baseline) {
        std::cout << "Error [GetFinaldNdEta::PrepareOutputRootFile] : call SetUpBaselinedNdEta first." << std::endl;
        exit(1);
    }

    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", output_directory.c_str(), output_directory.c_str()));

    std::string root_file_path = Form("%s/Mbin%d_FinaldNdEta.root", output_directory.c_str(), Mbin);
    TFile * file_out = new TFile(root_file_path.c_str(), "RECREATE");

    if (!file_out || file_out->IsZombie()) {
        std::cout << "Error [GetFinaldNdEta::PrepareOutputRootFile] : cannot open output file: " << root_file_path << std::endl;
        exit(1);
    }

    // note : 1. grE_baseline
    grE_baseline->Write();

    // note : 2. h1D_baseline
    h1D_baseline->Write();

    // note : 3. grE_baseline_Syst (may be nullptr if PrepareFinaldNdEta was not called)
    if (grE_baseline_Syst) { grE_baseline_Syst->Write(); }

    // note : 4. all variation TGraphs
    for (int i = 0; i < (int)all_variation_gr_vec.size(); i++) {
        all_variation_gr_vec[i]->Write();
    }

    // note : 5. h1D_FinalSyst
    if (h1D_FinalSyst) { h1D_FinalSyst->Write(); }

    // note : 6. MC TH1Ds
    for (int i = 0; i < (int)h1D_MC_vec.size(); i++) {
        h1D_MC_vec[i]->Write();
    }

    // Division : -TTree----------------------------------------------------------------
    // note : 5. TTree branches
    std::vector<double> v_x_pos;
    std::vector<double> v_baseline_y_pos;
    std::vector<double> v_AbsUnc_y;
    std::vector<double> v_RelativeUnc_y;

    // note : one vector per variation (indexed by all_variation_name)
    int n_var = (int)all_variation_name.size();
    std::vector<std::vector<double>> v_var_rel_unc(n_var);

    TTree * tree = new TTree("tree", "tree");
    tree->Branch("x_pos",         &v_x_pos);
    tree->Branch("baseline_y_pos",&v_baseline_y_pos);
    tree->Branch("AbsUnc_y",      &v_AbsUnc_y);
    tree->Branch("RelativeUnc_y", &v_RelativeUnc_y);

    for (int i = 0; i < n_var; i++) {
        tree->Branch(("RelativeUnc_"+all_variation_name[i]).c_str(), &v_var_rel_unc[i]);
    }

    // note : fill all vectors from grE_baseline_Syst (or grE_baseline if Syst not built)
    TGraphErrors * gr_src = (grE_baseline_Syst) ? grE_baseline_Syst : grE_baseline;
    int n_pts = gr_src->GetN();

    for (int j = 0; j < n_pts; j++) {
        double xp, yp;
        gr_src->GetPoint(j, xp, yp);
        double abs_unc = gr_src->GetErrorY(j);
        double rel_unc = (yp != 0.) ? abs_unc / std::fabs(yp) : 0.;

        v_x_pos.push_back(xp);
        v_baseline_y_pos.push_back(yp);
        v_AbsUnc_y.push_back(abs_unc);
        v_RelativeUnc_y.push_back(rel_unc);
    }

    // note : fill per-variation relative uncertainties
    for (int i = 0; i < n_var; i++) {
        for (int j = 0; j < (int)all_variation_points_vec[i].size(); j++) {
            v_var_rel_unc[i].push_back(all_variation_points_vec[i][j].second);
        }
    }

    tree->Fill(); // note : single row — all data packed in vectors
    tree->Write();

    file_out->Close();
    delete file_out;

    std::cout << "Output ROOT file saved: " << root_file_path << std::endl;
}
