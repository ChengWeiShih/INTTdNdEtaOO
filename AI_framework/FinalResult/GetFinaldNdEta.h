#ifndef GETFINALDNDETA_H
#define GETFINALDNDETA_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include <TCanvas.h>
#include <TPad.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TLine.h>
#include <TFile.h>
#include <TTree.h>

#include "sPhenixStyle.C"

class GetFinaldNdEta {
    public:
        GetFinaldNdEta(
            int                  Mbin_in,
            std::pair<int,int>   centrality_range_in,
            std::pair<int,int>   zvtx_range_in,
            std::string          sPHENIX_label_in,
            std::string          Collision_system_in,

            std::string          output_directory_in
            // std::pair<double,double> x_range_in = {-1.5, 1.5}
        );

        // note : store baseline TGraphErrors and TH1D
        void SetUpBaselinedNdEta(std::string baseline_legend_name_in, TGraphErrors * input_grE, TH1D * input_h1D);

        // note : store MC TH1D
        void SetUpMCdNdEta(std::string MC_name_in, std::string MC_legend_name_in, TH1D * input_h1D);

        // note : register one systematic variation source.
        //        input_vec is the return of EachVariation::GetMaxRelativeDiff():
        //        each element is {x_point, relative_uncertainty}.
        //        Can be called multiple times.
        void SetUpOneVariation(std::string variation_name, std::string variation_legend, std::vector<std::pair<double,double>> input_vec);

        // note : compute total systematic uncertainty via quadrature sum,
        //        fill h1D_FinalSyst, draw all variations + total on a canvas, save pdf.
        void PrepareFinalSystematicUnc();

        // note : prepare the final dNdEta plot with systematic uncertainty band.
        //        y_axis_max: upper bound of the y axis.
        //        IsDrawMC: if true and h1D_MC_vec is non-empty, overlay MC histograms.
        void PrepareFinaldNdEta(double y_axis_max = -999, bool IsDrawMC = true);

        // note : save all key objects and a summary TTree to a ROOT file.
        //        Output: Mbin{N}_FinaldNdEta.root
        void PrepareOutputRootFile();

    protected:
        // Division : -Constructor parameters-------------------------------------------
        int                  Mbin;
        std::pair<int,int>   centrality_range;
        std::pair<int,int>   zvtx_range;
        std::string          sPHENIX_label;
        std::string          Collision_system;
        std::string          output_directory;
        // std::pair<double,double> x_range;

        // Division : -Baseline---------------------------------------------------------
        TGraphErrors * grE_baseline = nullptr;
        TH1D         * h1D_baseline = nullptr;
        std::string baseline_legend_name;

        // Division : -MC---------------------------------------------------------------
        std::vector<std::string> MC_name_vec;
        std::vector<std::string> MC_legend_vec;
        std::vector<TH1D *>      h1D_MC_vec;

        // Division : -Variations (filled by GetOneVariation)---------------------------
        std::vector<std::string>                              all_variation_name;
        std::vector<std::string>                              all_variation_legend;
        std::vector<std::vector<std::pair<double,double>>>    all_variation_points_vec;
        std::vector<TGraph *>                                 all_variation_gr_vec;

        // Division : -Final systematic uncertainty histogram---------------------------
        TH1D * h1D_FinalSyst = nullptr;

        // note : vector of {x_center, total_relative_syst} filled by PrepareFinalSystematicUnc
        std::vector<std::pair<double,double>> final_syst_vec;

        // Division : -Baseline with systematic uncertainty band------------------------
        TGraphErrors * grE_baseline_Syst = nullptr;

        // Division : -Marker styles and colors (cycle for each variation)--------------
        static const int max_marker_entries = 8;
        int marker_styles[max_marker_entries] = {24, 25, 26, 27, 28, 30, 32, 42};
        // int marker_colors[max_marker_entries] = {2,  4,  8,  6,  7,  28, 46, 30};

        const std::vector<std::string> marker_colors = {
            "#9e0142",
            "#66c2a5",
            "#f46d43",
            "#3288bd",
            "#fee08b",
            "#5e4fa2",
            "#00A1FF",
            "#FF42A1",
            "#000000",
            
            
            "#abdda4",
            "#e6f598",
            "#fdae61",
            "#d53e4f"
        };
        // note : index 0 reserved for the total (black), variations start cycling here
};

#endif
