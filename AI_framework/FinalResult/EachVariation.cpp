#include "EachVariation.h"

EachVariation::EachVariation(
    int                  Mbin_in,
    std::pair<int,int>   centrality_range_in,
    std::pair<int,int>   zvtx_range_in,
    std::string          sPHENIX_label_in,
    std::string          Collision_system_in,
    std::string          output_directory_in,

    std::string          variation_name_in,
    std::string          variation_description_in,
    std::vector<std::tuple<std::string, TH1D*, TGraphErrors*>> input_dNdEta_in,

    std::pair<double,double>           x_range_in,
    std::pair<std::string,std::string> plot_title_in
):
    Mbin(Mbin_in),
    centrality_range(centrality_range_in),
    zvtx_range(zvtx_range_in),
    sPHENIX_label(sPHENIX_label_in),
    Collision_system(Collision_system_in),
    output_directory(output_directory_in),
    variation_name(variation_name_in),
    variation_description(variation_description_in),
    // input_dNdEta(input_dNdEta_in),
    // input_dNdEta(0),
    x_range(x_range_in),
    plot_title(plot_title_in)
{
    input_dNdEta.clear();

    for (int i = 0; i < (int)input_dNdEta_in.size(); i++) {

        input_dNdEta.emplace_back(
            std::get<0>(input_dNdEta_in[i]),
            (TH1D*) (std::get<1>(input_dNdEta_in[i]))->Clone(Form("h1D_Mbin%d_%s_%d",Mbin,variation_name.c_str(),i)),
            (TGraphErrors*) (std::get<2>(input_dNdEta_in[i]))->Clone(Form("grE_Mbin%d_%s_%d",Mbin,variation_name.c_str(),i))
        );
    }


    // Division : -Sanity checks-----------------------------------------------------------
    if (input_dNdEta.size() < 2) {
        std::cout << "Error [EachVariation] : input_dNdEta must have at least 2 entries." << std::endl;
        exit(1);
    }
    if (input_dNdEta.size() > (size_t)max_entries) {
        std::cout << "Error [EachVariation] : input_dNdEta can have at most " << max_entries << " entries." << std::endl;
        exit(1);
    }

    // note : check that baseline TGraphErrors has > 0 points
    int n_baseline_points = std::get<2>(input_dNdEta[0])->GetN();
    if (n_baseline_points <= 0) {
        std::cout << "Error [EachVariation] : baseline TGraphErrors (index 0) has 0 points." << std::endl;
        exit(1);
    }

    // note : check that all TGraphErrors have the same number of points as the baseline
    for (int i = 1; i < (int)input_dNdEta.size(); i++) {
        int n_points_i = std::get<2>(input_dNdEta[i])->GetN();
        if (n_points_i != n_baseline_points) {
            std::cout << "Error [EachVariation] : TGraphErrors at index " << i
                      << " (\"" << std::get<0>(input_dNdEta[i]) << "\") has "
                      << n_points_i << " points, but baseline has "
                      << n_baseline_points << " points." << std::endl;
            exit(1);
        }
    }

    DrawPlot();
}

// Division : -Helper: average of y points of a TGraphErrors---------------------------
double EachVariation::GetGraphYAverage(TGraphErrors * gr)
{
    int n = gr->GetN();
    if (n == 0) { return 1.; }

    double sum = 0.;
    for (int i = 0; i < n; i++) {
        double x, y;
        gr->GetPoint(i, x, y);
        sum += y;
    }
    return sum / (double)n;
}

// Division : -Public: largest absolute relative deviation per point-----------------
std::vector<std::pair<double,double>> EachVariation::GetMaxRelativeDiff()
{
    TGraphErrors * gr_baseline = std::get<2>(input_dNdEta[0]);
    int n = gr_baseline->GetN();

    std::vector<std::pair<double,double>> max_rel_diff(n, {0.,0.});

    for (int j = 0; j < n; j++) {
        double x_base, y_base;
        gr_baseline->GetPoint(j, x_base, y_base);

        max_rel_diff[j].first = x_base;

        for (int i = 1; i < (int)input_dNdEta.size(); i++) {
            TGraphErrors * gr_var = std::get<2>(input_dNdEta[i]);

            double x_var, y_var;
            gr_var->GetPoint(j, x_var, y_var);

            double rel_diff = (y_base != 0.) ? fabs(y_var - y_base) / fabs(y_base) : 0.;
            if (y_base == 0.){
                std::cout<<"Warning [EachVariation] : y_base is 0 at point j = "<<j<<". This should not happen." <<std::endl;
                exit(1);
            }

            if (rel_diff > max_rel_diff[j].second) {
                max_rel_diff[j].second = rel_diff;
            }
        }
    }

    return max_rel_diff;
}

// Division : -Main drawing function---------------------------------------------------
void EachVariation::DrawPlot()
{
    // -- Canvas setup -----------------------------------------------------------------
    TCanvas * c1 = new TCanvas(
        ("c1_" + variation_name).c_str(),
        ("c1_" + variation_name).c_str(),
        800, 1050
    );
    c1->Draw();

    // note : top panel (major) takes ~65% of the canvas height
    TPad * pad_top = new TPad(
        ("pad_top_" + variation_name).c_str(), "",
        0., 0.35, 1., 1.
    );
    TPad * pad_bot = new TPad(
        ("pad_bot_" + variation_name).c_str(), "",
        0., 0.,  1., 0.35
    );

    // note : large top margin reserves space for the legend above the frame
    pad_top->SetTopMargin(0.26);
    pad_top->SetBottomMargin(0.02);
    pad_top->SetLeftMargin(0.13);
    pad_top->SetRightMargin(0.04);
    pad_top->SetTicks(1, 1); // note : ticks on top and right axes

    pad_bot->SetTopMargin(0.02);
    pad_bot->SetBottomMargin(0.28);
    pad_bot->SetLeftMargin(0.13);
    pad_bot->SetRightMargin(0.04);
    pad_bot->SetTicks(1, 1); // note : ticks on top and right axes

    pad_top->Draw();
    pad_bot->Draw();

    // -- Determine y-axis range of top panel from baseline average --------------------
    TGraphErrors * gr_baseline = std::get<2>(input_dNdEta[0]);
    double y_avg = GetGraphYAverage(gr_baseline);
    double y_top_min = y_avg * 0.8;
    double y_top_max = y_avg * 1.2;

    // ---------------------------------------------------------------------------------
    // TOP PANEL
    // ---------------------------------------------------------------------------------
    pad_top->cd();

    // note : draw a dummy frame to set axes
    TH1F * h_frame_top = pad_top->DrawFrame(
        x_range.first,  y_top_min,
        x_range.second, y_top_max
    );
    h_frame_top->GetXaxis()->SetLabelSize(0.);       // shared x-axis, no labels on top
    h_frame_top->GetXaxis()->SetTitleSize(0.);
    h_frame_top->GetYaxis()->SetTitle(plot_title.second.c_str());
    h_frame_top->GetYaxis()->SetTitleSize(0.055);
    h_frame_top->GetYaxis()->SetTitleOffset(1.1);
    h_frame_top->GetYaxis()->SetLabelSize(0.05);

    // note : draw all TGraphErrors
    // note : legend placed in the top-margin strip (NDC y > 1 - TopMargin = 0.74)
    TLegend * leg = new TLegend(0.06, 0.76, 0.97, 0.99);
    leg->SetHeader(Form("%s, z-vertex: [%d, %d] cm, Cent.: %d-%d%%", variation_description.c_str(), zvtx_range.first, zvtx_range.second, centrality_range.first, centrality_range.second));
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    leg->SetTextSize(0.038);
    // leg->SetNColumns(2);

    for (int i = 0; i < (int)input_dNdEta.size(); i++) {
        std::string  label = std::get<0>(input_dNdEta[i]);
        TGraphErrors * gr  = std::get<2>(input_dNdEta[i]);

        gr->SetMarkerStyle(marker_styles[i]);
        gr->SetMarkerColor(TColor::GetColor(marker_colors[i].c_str()));
        gr->SetLineColor(TColor::GetColor(marker_colors[i].c_str()));
        gr->SetMarkerSize(1.5);

        gr->Draw("p same");
        leg->AddEntry(gr, label.c_str(), "p");
    }

    leg->Draw();

    // note : sPHENIX label + info
    TLatex latex_top;
    latex_top.SetNDC();
    latex_top.SetTextFont(62);
    latex_top.SetTextSize(0.035);
    latex_top.DrawLatex(0.15, 0.68, Form("#it{sPHENIX} %s   %s", sPHENIX_label.c_str(), Collision_system.c_str()));

    latex_top.SetTextFont(42);
    latex_top.SetTextSize(0.042);
    // latex_top.DrawLatex()
    // latex_top.DrawLatex(0.15, 0.61, Form("Au+Au, Cent. %d-%d%%", centrality_range.first, centrality_range.second));
    // latex_top.DrawLatex(0.15, 0.55, Form("|Vtx_{Z}| < %d cm",   zvtx_range.second));
    // latex_top.DrawLatex(0.15, 0.49, variation_description.c_str());

    pad_top->Update();

    // ---------------------------------------------------------------------------------
    // BOTTOM PANEL
    // ---------------------------------------------------------------------------------
    pad_bot->cd();

    TH1F * h_frame_bot = pad_bot->DrawFrame(
        x_range.first,  0.9,
        x_range.second, 1.1
    );
    h_frame_bot->GetXaxis()->SetTitle(plot_title.first.c_str());
    h_frame_bot->GetXaxis()->SetTitleSize(0.11);
    h_frame_bot->GetXaxis()->SetTitleOffset(1.1);
    h_frame_bot->GetXaxis()->SetLabelSize(0.09);
    h_frame_bot->GetYaxis()->SetTitle("Ratio to baseline");
    h_frame_bot->GetYaxis()->SetTitleSize(0.09);
    h_frame_bot->GetYaxis()->SetTitleOffset(0.65);
    h_frame_bot->GetYaxis()->SetLabelSize(0.08);
    h_frame_bot->GetYaxis()->SetNdivisions(505);

    // note : dashed horizontal line at y = 1
    TLine * line_unity = new TLine(x_range.first, 1., x_range.second, 1.);
    line_unity->SetLineStyle(2); // dashed
    line_unity->SetLineColor(1);
    line_unity->SetLineWidth(2);
    line_unity->Draw();

    // note : ratio graphs: each variation / baseline
    int n_baseline = gr_baseline->GetN();

    for (int i = 1; i < (int)input_dNdEta.size(); i++) {
        TGraphErrors * gr_var = std::get<2>(input_dNdEta[i]);

        TGraph * gr_ratio = new TGraph();
        gr_ratio->SetName(Form("gr_ratio_%s_%d", variation_name.c_str(), i));

        for (int j = 0; j < n_baseline; j++) {
            double x_base, y_base;
            gr_baseline->GetPoint(j, x_base, y_base);

            double x_var, y_var;
            gr_var->GetPoint(j, x_var, y_var);

            // note : check that x positions match between variation and baseline
            if (fabs(x_base - x_var) > 0.0005) {
                std::cout << "Error [EachVariation] : \""
                          << std::get<0>(input_dNdEta[i])
                          << "\" point " << j
                          << " has x_var = " << x_var
                          << " which differs from x_base = " << x_base
                          << " by more than 0.0005. quite the operation." << std::endl;
                exit(1);
            }

            double ratio = (y_base != 0.) ? y_var / y_base : 0.;
            gr_ratio->SetPoint(j, x_base, ratio);
        }

        gr_ratio->SetMarkerStyle(marker_styles[i]);
        gr_ratio->SetMarkerColor(TColor::GetColor(marker_colors[i].c_str()));
        gr_ratio->SetLineColor(TColor::GetColor(marker_colors[i].c_str()));
        gr_ratio->SetMarkerSize(1.5);

        gr_ratio->Draw("p same");
    }

    pad_bot->Update();

    // -- Save canvas ------------------------------------------------------------------
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", output_directory.c_str(), output_directory.c_str()));

    c1->SaveAs(Form("%s/Mbin%d_%s.pdf",  output_directory.c_str(), Mbin, variation_name.c_str()));
    c1->SaveAs(Form("%s/Mbin%d_%s.png",  output_directory.c_str(), Mbin, variation_name.c_str()));

    delete c1;
}
