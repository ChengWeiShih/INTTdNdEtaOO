#ifndef EACHVARIATION_H
#define EACHVARIATION_H

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <numeric>

#include <TCanvas.h>
#include <TPad.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TAxis.h>

class EachVariation {
    public:
        EachVariation(
            int                  Mbin_in,
            std::pair<int,int>   centrality_range_in,
            std::pair<int,int>   zvtx_range_in,
            std::string          sPHENIX_label_in,
            std::string          Collision_system_in,
            std::string          output_directory_in,

            std::string          variation_name_in,
            std::string          variation_description_in,
            std::vector<std::tuple<std::string, TH1D*, TGraphErrors*>> input_dNdEta_in,

            std::pair<double,double>           x_range_in   = {-1.5, 1.5},
            std::pair<std::string,std::string> plot_title_in = {"Pseudorapidity #eta", "dN_{ch}/d#eta"}
        );

        // note : returns a vector of size N (= baseline->GetN()).
        // note : each element is the largest absolute relative deviation
        //        across all variations at that point index:
        //        max_i ( |y_var_i[j] - y_base[j]| / |y_base[j]| )
        std::vector<std::pair<double,double>> GetMaxRelativeDiff();

    protected:
        // Division : -For constructor-----------------------------------------------------------
        int                  Mbin;
        std::pair<int,int>   centrality_range;
        std::pair<int,int>   zvtx_range;
        std::string          sPHENIX_label;
        std::string          Collision_system;
        std::string          output_directory;

        std::string          variation_name;
        std::string          variation_description;
        std::vector<std::tuple<std::string, TH1D*, TGraphErrors*>> input_dNdEta;

        std::pair<double,double>           x_range;
        std::pair<std::string,std::string> plot_title;

        // Division : -Marker styles and colors (up to 8 entries)-------------------------------
        // note : index 0 = baseline
        static const int max_entries = 8;
        int marker_styles[max_entries] = {24, 25, 26, 27, 28, 30, 32, 42};

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

        // Division : -Internal helpers---------------------------------------------------------
        double GetGraphYAverage(TGraphErrors * gr);
        void   DrawPlot();
};

#endif
