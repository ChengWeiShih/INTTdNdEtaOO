#include "DoOnedNdEta.h"

DoOnedNdEta::DoOnedNdEta(
    std::string class_name_in,
    std::pair<double,double> eta_range_in,

    std::string data_input_directory_in,
    std::string data_hist_name_in,

    std::string MC_input_directory_in,
    std::string MC_correction_hist_name_in,
    std::string MC_truth_dNdEta_hist_name_in
):
    class_name(class_name_in),
    eta_range(eta_range_in),
    data_input_directory(data_input_directory_in),
    data_hist_name(data_hist_name_in),
    MC_input_directory(MC_input_directory_in),
    MC_correction_hist_name(MC_correction_hist_name_in),
    MC_truth_dNdEta_hist_name(MC_truth_dNdEta_hist_name_in)
{
    // Division : -Get TH1Ds from files-----------------------------------------------------------
    TFile * file_data = TFile::Open(data_input_directory.c_str());
    if (!file_data || file_data->IsZombie()) {
        std::cout << "Error : cannot open data file: " << data_input_directory << std::endl;
        exit(1);
    }

    TH1D * h1D_data_raw = (TH1D*) file_data->Get(data_hist_name.c_str());
    if (!h1D_data_raw) {
        std::cout << "Error : cannot get histogram \"" << data_hist_name << "\" from " << data_input_directory << std::endl;
        exit(1);
    }
    h1D_data_raw->Sumw2(true);

    TFile * file_MC = TFile::Open(MC_input_directory.c_str());
    if (!file_MC || file_MC->IsZombie()) {
        std::cout << "Error : cannot open MC file: " << MC_input_directory << std::endl;
        exit(1);
    }

    TH1D * h1D_MC_correction = (TH1D*) file_MC->Get(MC_correction_hist_name.c_str());
    if (!h1D_MC_correction) {
        std::cout << "Error : cannot get histogram \"" << MC_correction_hist_name << "\" from " << MC_input_directory << std::endl;
        exit(1);
    }
    h1D_MC_correction->Sumw2(true);

    if (MC_truth_dNdEta_hist_name.size() !=0){
        h1D_MC_truth = (TH1D*) file_MC->Get(MC_truth_dNdEta_hist_name.c_str());
        if (!h1D_MC_truth) {
            std::cout << "Error : cannot get histogram \"" << MC_truth_dNdEta_hist_name << "\" from " << MC_input_directory << std::endl;
            exit(1);
        }
        h1D_MC_truth->Sumw2(true);
    }

    // Division : -Prepare h1D_dNdEta = data / MC_correction-----------------------------------------------------------
    h1D_dNdEta = (TH1D*) h1D_data_raw->Clone(("h1D" + class_name).c_str());
    h1D_dNdEta->SetDirectory(0); // note : detach from TFile ownership so it survives file close
    h1D_dNdEta->Sumw2(true);
    h1D_dNdEta->Divide(h1D_MC_correction);

    // Division : -Prepare grE_dNdEta from bins within eta_range-----------------------------------------------------------
    grE_dNdEta = new TGraphErrors();
    grE_dNdEta->SetName(("grE" + class_name).c_str());

    int point_index = 0;
    for (int i = 1; i <= h1D_dNdEta->GetNbinsX(); i++) {
        double bin_center = h1D_dNdEta->GetBinCenter(i);

        if (bin_center < eta_range.first || bin_center > eta_range.second) {
            h1D_dNdEta->SetBinContent(i, 0.);
            h1D_dNdEta->SetBinError(i, 0.);
            continue;
        }

        double bin_content = h1D_dNdEta->GetBinContent(i);
        double bin_error   = h1D_dNdEta->GetBinError(i);

        grE_dNdEta->SetPoint(point_index, bin_center, bin_content);
        grE_dNdEta->SetPointError(point_index, 0., bin_error);

        point_index++;
    }

    // Division : -Close files-----------------------------------------------------------
    // file_data->Close();
    // file_MC->Close();
}
