#ifndef DOONEDNDETA_H
#define DOONEDNDETA_H

#include <iostream>
#include <string>
#include <utility>

#include <TFile.h>
#include <TH1D.h>
#include <TGraphErrors.h>

class DoOnedNdEta {
    public:
        DoOnedNdEta(
            std::string class_name_in,
            std::pair<double,double> eta_range_in,

            std::string data_input_directory_in,
            std::string data_hist_name_in,

            std::string MC_input_directory_in,
            std::string MC_correction_hist_name_in,
            std::string MC_truth_dNdEta_hist_name_in = ""
        );

        TH1D *        GetdNdEtaHist() { return h1D_dNdEta; }
        TGraphErrors * GetdNdEtagrE() { return grE_dNdEta; }
        TH1D *        GetMCTruthHist() { return h1D_MC_truth; }

    protected:
        // Division : -For constructor-----------------------------------------------------------
        std::string class_name;
        std::pair<double,double> eta_range;

        std::string data_input_directory;
        std::string data_hist_name;

        std::string MC_input_directory;
        std::string MC_correction_hist_name;
        std::string MC_truth_dNdEta_hist_name;

        // Division : -Output objects-----------------------------------------------------------
        TH1D *        h1D_dNdEta = nullptr;
        TGraphErrors * grE_dNdEta = nullptr;

        TH1D *        h1D_MC_truth = nullptr;
};

#endif
