#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>
#include <TAxis.h>

#include <cmath>
#include <iostream>
#include <string>

void MakeFileIndexInttDoublet(
    const std::string input_directory =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391/"
        "test_82391_LoacalPos_PrivateCentrality/InttDoublet_zweight_trigcorr/"
        "StabilityBaseline_70/completed/"
        "merged_files_Data_INTTDoublets_Trigger_BcoFullDiffCut_CentralityRange1to70_"
        "MinBiasEvt_IsTrigEffiWeight_VtxZRangeN10to10_ColMulMask_ClusQAAdc30PhiSize40_"
        "DeltaPhiCut0p150_00082391_",
    const std::string input_prefix =
        "Data_INTTDoublets_Trigger_BcoFullDiffCut_CentralityRange1to70_"
        "MinBiasEvt_IsTrigEffiWeight_VtxZRangeN10to10_ColMulMask_ClusQAAdc30PhiSize40_"
        "DeltaPhiCut0p150_00082391_",
    const std::string output_directory =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391/"
        "test_82391_LoacalPos_PrivateCentrality/InttDoublet_zweight_trigcorr/"
        "StabilityBaseline_70/completed",
    const std::string output_filename = "FileIndex_InttDoublet_00082391.root",
    const int file_first = 0,
    const int file_last = 999,
    const double eta_min = -1.1,
    const double eta_max = 1.1)
{
    const std::string file_suffix = ".root";

    TH2D *h2D_FileIndex_InttDoublet = new TH2D(
        "h2D_FileIndex_InttDoublet",
        "INTT doublet stability;File index;INTT doublet phi [radian];INTT doublet count (Integral |#eta| < 1.1) per MB events",
        1200, 0, 1200,
        40, -3.2, 3.2);
    h2D_FileIndex_InttDoublet->Sumw2();
    h2D_FileIndex_InttDoublet->GetXaxis()->SetTitle("File index");
    h2D_FileIndex_InttDoublet->GetYaxis()->SetTitle("INTT doublet phi [radian]");
    h2D_FileIndex_InttDoublet->GetZaxis()->SetTitle("INTT doublet count (Integral |#eta| < 1.1) per MB events");

    Long64_t n_file_opened = 0;
    Long64_t n_file_filled = 0;
    Long64_t n_file_missing = 0;
    Long64_t n_file_bad_event_count = 0;

    for (int file_idx = file_first; file_idx <= file_last; ++file_idx)
    {
        const std::string fname = Form("%s/%s%05d%s",
                                       input_directory.c_str(),
                                       input_prefix.c_str(),
                                       file_idx,
                                       file_suffix.c_str());

        TFile *fin = TFile::Open(fname.c_str(), "READ");
        if (!fin || fin->IsZombie())
        {
            std::cerr << "[WARN] Cannot open " << fname << " -- skipping." << std::endl;
            if (fin) delete fin;
            ++n_file_missing;
            continue;
        }
        ++n_file_opened;

        TH1D *h1D_eta_bin = dynamic_cast<TH1D *>(fin->Get("h1D_eta_bin"));
        TH1D *h1D_phi_bin = dynamic_cast<TH1D *>(fin->Get("h1D_phi_bin"));
        TH1D *h1D_nEvent = dynamic_cast<TH1D *>(fin->Get("h1D_nEvent"));
        TH1D *h1D_BunchNumber = dynamic_cast<TH1D *>(fin->Get("h1D_BunchNumber"));
        TH1D *h1D_MBDChargeSum = dynamic_cast<TH1D *>(fin->Get("h1D_MBDChargeSum"));
        TH1D *h1D_ClusEtaInttZ = dynamic_cast<TH1D *>(fin->Get("h1D_ClusEtaInttZ"));
        TH1D *h1D_centrality = dynamic_cast<TH1D *>(fin->Get("h1D_centrality"));
        TH2D *h2D_Clus_ColumnZID_LayerPhiID = dynamic_cast<TH2D *>(fin->Get("h2D_Clus_ColumnZID_LayerPhiID"));
        TH2D *h2D_GoodProtoTracklet_EtaPhi = dynamic_cast<TH2D *>(fin->Get("h2D_GoodProtoTracklet_EtaPhi"));
        TH2D *h2D_GoodProtoTracklet_EtaPhi_rotated = dynamic_cast<TH2D *>(fin->Get("h2D_GoodProtoTracklet_EtaPhi_rotated"));

        if (!h1D_eta_bin || !h1D_phi_bin || !h1D_nEvent || !h1D_BunchNumber ||
            !h1D_MBDChargeSum || !h1D_ClusEtaInttZ || !h1D_centrality ||
            !h2D_Clus_ColumnZID_LayerPhiID ||
            !h2D_GoodProtoTracklet_EtaPhi || !h2D_GoodProtoTracklet_EtaPhi_rotated)
        {
            std::cerr << "[WARN] Missing at least one required histogram in "
                      << fname << " -- skipping." << std::endl;
            fin->Close();
            delete fin;
            continue;
        }

        const double n_mb_event = h1D_nEvent->GetBinContent(3);
        if (n_mb_event <= 0)
        {
            std::cerr << "[WARN] h1D_nEvent bin 3 is <= 0 in "
                      << fname << " -- skipping." << std::endl;
            fin->Close();
            delete fin;
            ++n_file_bad_event_count;
            continue;
        }

        TH2D *h2D_GoodProtoTracklet_EtaPhi_Subtracted =
            dynamic_cast<TH2D *>(h2D_GoodProtoTracklet_EtaPhi->Clone(
                Form("h2D_GoodProtoTracklet_EtaPhi_Subtracted_%05d", file_idx)));
        h2D_GoodProtoTracklet_EtaPhi_Subtracted->SetDirectory(nullptr);
        h2D_GoodProtoTracklet_EtaPhi_Subtracted->Add(h2D_GoodProtoTracklet_EtaPhi_rotated, -1.0);

        TAxis *eta_axis = h2D_GoodProtoTracklet_EtaPhi_Subtracted->GetXaxis();
        const int eta_bin_min = eta_axis->FindBin(eta_min + 1.0e-9);
        const int eta_bin_max = eta_axis->FindBin(eta_max - 1.0e-9);

        TH1D *h1D_phi_subtracted_integrated =
            h2D_GoodProtoTracklet_EtaPhi_Subtracted->ProjectionY(
                Form("h1D_phi_subtracted_integrated_%05d", file_idx),
                eta_bin_min,
                eta_bin_max);
        h1D_phi_subtracted_integrated->SetDirectory(nullptr);
        h1D_phi_subtracted_integrated->Scale(1.0 / n_mb_event);

        const int output_x_bin = h2D_FileIndex_InttDoublet->GetXaxis()->FindBin(file_idx + 0.5);
        for (int phi_bin = 1; phi_bin <= h2D_FileIndex_InttDoublet->GetNbinsY(); ++phi_bin)
        {
            h2D_FileIndex_InttDoublet->SetBinContent(
                output_x_bin,
                phi_bin,
                h1D_phi_subtracted_integrated->GetBinContent(phi_bin));
            h2D_FileIndex_InttDoublet->SetBinError(
                output_x_bin,
                phi_bin,
                h1D_phi_subtracted_integrated->GetBinError(phi_bin));
        }

        delete h1D_phi_subtracted_integrated;
        delete h2D_GoodProtoTracklet_EtaPhi_Subtracted;

        fin->Close();
        delete fin;
        ++n_file_filled;
    }

    const std::string output_path = Form("%s/%s", output_directory.c_str(), output_filename.c_str());
    TFile *fout = TFile::Open(output_path.c_str(), "RECREATE");
    if (!fout || fout->IsZombie())
    {
        std::cerr << "[ERROR] Cannot create output file " << output_path << std::endl;
        if (fout) delete fout;
        delete h2D_FileIndex_InttDoublet;
        return;
    }

    h2D_FileIndex_InttDoublet->Write();
    fout->Close();
    delete fout;

    std::cout << "[INFO] Files opened: " << n_file_opened << std::endl;
    std::cout << "[INFO] Files filled: " << n_file_filled << std::endl;
    std::cout << "[INFO] Files missing/unreadable: " << n_file_missing << std::endl;
    std::cout << "[INFO] Files with bad MB event count: " << n_file_bad_event_count << std::endl;
    std::cout << "[INFO] Wrote " << output_path << std::endl;

    delete h2D_FileIndex_InttDoublet;
}
