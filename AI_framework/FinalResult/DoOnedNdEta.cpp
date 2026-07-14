#include "DoOnedNdEta.h"

DoOnedNdEta::DoOnedNdEta(
    std::string class_name_in,
    std::pair<double,double> eta_range_in,

    std::string data_input_directory_in,
    std::string data_hist_name_in,

    std::string MC_input_directory_in,
    std::string MC_correction_hist_name_in,
    std::string MC_truth_dNdEta_hist_name_in,
    bool        IsSymmetrize_in,
    bool        IsMirror_in
):
    class_name(class_name_in),
    eta_range(eta_range_in),
    data_input_directory(data_input_directory_in),
    data_hist_name(data_hist_name_in),
    MC_input_directory(MC_input_directory_in),
    MC_correction_hist_name(MC_correction_hist_name_in),
    MC_truth_dNdEta_hist_name(MC_truth_dNdEta_hist_name_in),
    IsSymmetrize(IsSymmetrize_in),
    IsMirror(IsMirror_in)
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

    // Division : -Symmetrize h1D_dNdEta if requested-------------------------------------------
    // note : for each bin at center +A, find the mirror bin at center -A and average them.
    if (IsSymmetrize) {
        int nBins = h1D_dNdEta->GetNbinsX();
        for (int i = 1; i <= nBins; i++) {
            double center_i = h1D_dNdEta->GetBinCenter(i);
            if (center_i <= 0. || fabs(center_i) < 0.000001) { continue; } // note : only process positive-η bins

            // note : find the mirror bin (center == -center_i)
            int mirror_bin = h1D_dNdEta->FindBin(-center_i);
            if (mirror_bin < 1 || mirror_bin > nBins) { 
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", fail to symmetry the bin: "<<i<<", the bin center: "<<center_i<<", the mirror bin: "<<mirror_bin<<std::endl;
                exit(1); 
            }

            double c_pos = h1D_dNdEta->GetBinContent(i);
            double c_neg = h1D_dNdEta->GetBinContent(mirror_bin);
            double e_pos = h1D_dNdEta->GetBinError(i);
            double e_neg = h1D_dNdEta->GetBinError(mirror_bin);

            // note : inverse-variance weighted combination
            // note : w = 1/sigma^2,  combined = sum(w*c) / sum(w),  sigma_combined = 1/sqrt(sum(w))
            double avg   = 0.;
            double e_avg = 0.;

            if (e_pos > 0. && e_neg > 0.) {
                double w_pos = 1. / (e_pos * e_pos);
                double w_neg = 1. / (e_neg * e_neg);
                double w_sum = w_pos + w_neg;
                avg   = (w_pos * c_pos + w_neg * c_neg) / w_sum;
                e_avg = 1. / std::sqrt(w_sum);
            } else if (e_pos > 0.) {
                // note : only pos bin has a valid error – use it alone
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", e_neg=0 – using pos bin alone"<<std::endl;
                avg   = c_pos;
                e_avg = e_pos;
            } else if (e_neg > 0.) {
                // note : only neg bin has a valid error – use it alone
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", e_pos=0 – using neg bin alone"<<std::endl;
                avg   = c_neg;
                e_avg = e_neg;
            } else {
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", both errors=0 – using plain average"<<std::endl;
                // note : both errors are 0, fall back to plain average
                avg   = (c_pos + c_neg) / 2.;
                e_avg = 0.;
            }

            h1D_dNdEta->SetBinContent(i,          avg);
            h1D_dNdEta->SetBinError(i,            e_avg);
            h1D_dNdEta->SetBinContent(mirror_bin, avg);
            h1D_dNdEta->SetBinError(mirror_bin,   e_avg);
        }
    }

    // Division : -Mirror h1D_dNdEta if requested (reflect eta -> -eta)------------------
    // note : take a snapshot of the current values, then for every bin set
    //        content/error = content/error of the mirror bin in the snapshot.
    if (IsMirror) {
        TH1D * h_snap = (TH1D*) h1D_dNdEta->Clone(("h_mirror_snap_"+class_name).c_str());
        h_snap->SetDirectory(0);

        int nBins = h1D_dNdEta->GetNbinsX();
        for (int i = 1; i <= nBins; i++) {
            double center_i  = h1D_dNdEta->GetBinCenter(i);
            int    mirror_bin = h_snap->FindBin(-center_i);

            if (mirror_bin < 1 || mirror_bin > nBins) {
                std::cout << "In DoOnedNdEta [IsMirror]: class_name: " << class_name
                          << ", no mirror bin for center = " << center_i << std::endl;
                exit(1);
            }

            h1D_dNdEta->SetBinContent(i, h_snap->GetBinContent(mirror_bin));
            h1D_dNdEta->SetBinError(i,   h_snap->GetBinError(mirror_bin));
        }

        // delete h_snap;
    }
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
