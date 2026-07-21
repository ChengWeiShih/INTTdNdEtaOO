#include <TFile.h>
#include <TH2D.h>
#include <TString.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

void DrawInnerClusterPhiTime(
    const std::string input_dir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391/"
        "test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/EvtVtxZ/completed",
    const std::string output_file = "InnerClusterPhiTime_00082391.root",
    const int file_first = 0,
    const int file_last = 999,
    const double bco_period_ns = 106.0)
{
    const std::string file_prefix =
        "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_";
    const std::string file_suffix = ".root";
    const std::string tree_name = "EventTree";

    TH2D *h2D_crossing_count = new TH2D(
        "h2D_crossing_count",
        "Selected triggered crossings;time [min];INTT Cluster phi angle [radian];number of triggered crossings",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_inner_cluster_count = new TH2D(
        "h2D_inner_cluster_count",
        "Inner cluster counts;time [min];INTT Cluster phi angle [radian];number of clusters (inner)",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_inner_cluster_per_crossing = new TH2D(
        "h2D_inner_cluster_per_crossing",
        "Inner clusters per triggered crossing;time [min];INTT Cluster phi angle [radian];number of clusters (inner) per triggered crossing",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_outer_cluster_count = new TH2D(
        "h2D_outer_cluster_count",
        "Outer cluster counts;time [min];INTT Cluster phi angle [radian];number of clusters (outer)",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_outer_cluster_per_crossing = new TH2D(
        "h2D_outer_cluster_per_crossing",
        "Outer clusters per triggered crossing;time [min];INTT Cluster phi angle [radian];number of clusters (outer) per triggered crossing",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_all_cluster_count = new TH2D(
        "h2D_all_cluster_count",
        "Inner plus outer cluster counts;time [min];INTT Cluster phi angle [radian];number of clusters (inner + outer)",
        300, 0, 60, 200, -3.2, 3.2);
    TH2D *h2D_all_cluster_per_crossing = new TH2D(
        "h2D_all_cluster_per_crossing",
        "Inner plus outer clusters per triggered crossing;time [min];INTT Cluster phi angle [radian];number of clusters (inner + outer) per triggered crossing",
        300, 0, 60, 200, -3.2, 3.2);

    h2D_crossing_count->Sumw2();
    h2D_inner_cluster_count->Sumw2();
    h2D_inner_cluster_per_crossing->Sumw2();
    h2D_outer_cluster_count->Sumw2();
    h2D_outer_cluster_per_crossing->Sumw2();
    h2D_all_cluster_count->Sumw2();
    h2D_all_cluster_per_crossing->Sumw2();

    ULong64_t first_GL1Packet_BCO = std::numeric_limits<ULong64_t>::max();
    const double bco_to_min = bco_period_ns * 1.0e-9 / 60.0;

    Long64_t n_file_opened = 0;
    Long64_t n_event_total = 0;
    Long64_t n_event_selected = 0;
    Long64_t n_inner_cluster = 0;
    Long64_t n_outer_cluster = 0;
    Long64_t n_all_cluster = 0;

    for (int file_idx = file_first; file_idx <= file_last; ++file_idx)
    {
        const std::string fname = Form("%s/%s%05d%s",
                                       input_dir.c_str(),
                                       file_prefix.c_str(),
                                       file_idx,
                                       file_suffix.c_str());

        TFile *fin = TFile::Open(fname.c_str(), "READ");
        if (!fin || fin->IsZombie())
        {
            std::cerr << "[WARN] Cannot open " << fname << " -- skipping." << std::endl;
            if (fin) delete fin;
            continue;
        }

        TTree *tree = dynamic_cast<TTree *>(fin->Get(tree_name.c_str()));
        if (!tree)
        {
            std::cerr << "[WARN] Cannot find " << tree_name << " in " << fname << " -- skipping." << std::endl;
            fin->Close();
            delete fin;
            continue;
        }

        if (!tree->GetBranch("GL1Packet_BCO") ||
            !tree->GetBranch("is_min_bias_private_MinDeposit") ||
            !tree->GetBranch("InttBcoFullDiff_next") ||
            !tree->GetBranch("ClusPhi") ||
            !tree->GetBranch("ClusLayer") ||
            !tree->GetBranch("MBD_z_vtx") || 
            !tree->GetBranch("ClusAdc")
        )
        {
            std::cerr << "[WARN] Missing one of GL1Packet_BCO, is_min_bias, InttBcoFullDiff_next, ClusPhi, ClusLayer in "
                      << fname << " -- skipping." << std::endl;
            fin->Close();
            delete fin;
            continue;
        }

        ULong_t GL1Packet_BCO = 0;
        bool is_min_bias = 0;
        int InttBcoFullDiff_next = 0;
        std::vector<float> *ClusPhi = nullptr;
        std::vector<int> *ClusLayer = nullptr;
        float MBD_z_vtx;
        std::vector<int> *ClusAdc = nullptr;

        tree->SetBranchStatus("*", 0);
        tree->SetBranchStatus("GL1Packet_BCO", 1);
        tree->SetBranchStatus("is_min_bias_private_MinDeposit", 1);
        tree->SetBranchStatus("InttBcoFullDiff_next", 1);
        tree->SetBranchStatus("ClusPhi", 1);
        tree->SetBranchStatus("ClusLayer", 1);
        tree->SetBranchStatus("MBD_z_vtx", 1);
        tree->SetBranchStatus("ClusAdc", 1);
        
        tree->SetBranchAddress("GL1Packet_BCO", &GL1Packet_BCO);
        tree->SetBranchAddress("is_min_bias_private_MinDeposit", &is_min_bias);
        tree->SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);
        tree->SetBranchAddress("ClusPhi", &ClusPhi);
        tree->SetBranchAddress("ClusLayer", &ClusLayer);
        tree->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
        tree->SetBranchAddress("ClusAdc", &ClusAdc);

        ++n_file_opened;
        const Long64_t n_entries = tree->GetEntries();
        for (Long64_t entry = 0; entry < n_entries; ++entry)
        {
            tree->GetEntry(entry);
            ++n_event_total;

            if (entry % 2000 == 0){
                std::cout<<"file: "<<file_idx<<", entry: "<<entry<<std::endl;
            }

            if (first_GL1Packet_BCO == std::numeric_limits<ULong64_t>::max())
            {
                first_GL1Packet_BCO = GL1Packet_BCO;
            }
            if (GL1Packet_BCO < first_GL1Packet_BCO) continue;

            if (is_min_bias != 1) continue;
            if (InttBcoFullDiff_next <= 80) continue;
            if (fabs(MBD_z_vtx) < 10) continue;

            const double time_min = static_cast<double>(GL1Packet_BCO - first_GL1Packet_BCO) * bco_to_min;
            if (time_min < 0 || time_min >= 60.0) continue;

            ++n_event_selected;
            for (int phi_bin = 1; phi_bin <= h2D_crossing_count->GetNbinsY(); ++phi_bin)
            {
                h2D_crossing_count->Fill(time_min, h2D_crossing_count->GetYaxis()->GetBinCenter(phi_bin));
            }

            if (!ClusPhi || !ClusLayer) continue;
            const size_t n_clus = std::min(ClusLayer->size(), ClusPhi->size());
            for (size_t iclus = 0; iclus < n_clus; ++iclus)
            {

                if (ClusAdc->at(iclus) <= 35) {continue;}
                
                const int layer = ClusLayer->at(iclus);
                const bool is_inner = (layer == 3 || layer == 4);
                const bool is_outer = (layer == 5 || layer == 6);
                if (!is_inner && !is_outer) continue;

                const double phi = ClusPhi->at(iclus);
                h2D_all_cluster_count->Fill(time_min, phi);
                ++n_all_cluster;
                if (is_inner)
                {
                    h2D_inner_cluster_count->Fill(time_min, phi);
                    ++n_inner_cluster;
                }
                if (is_outer)
                {
                    h2D_outer_cluster_count->Fill(time_min, phi);
                    ++n_outer_cluster;
                }
            }
        }

        fin->Close();
        delete fin;
    }

    h2D_inner_cluster_per_crossing->Add(h2D_inner_cluster_count);
    h2D_inner_cluster_per_crossing->Divide(h2D_crossing_count);
    h2D_outer_cluster_per_crossing->Add(h2D_outer_cluster_count);
    h2D_outer_cluster_per_crossing->Divide(h2D_crossing_count);
    h2D_all_cluster_per_crossing->Add(h2D_all_cluster_count);
    h2D_all_cluster_per_crossing->Divide(h2D_crossing_count);

    TFile *fout = TFile::Open(output_file.c_str(), "RECREATE");
    if (!fout || fout->IsZombie())
    {
        std::cerr << "[ERROR] Cannot create output file " << output_file << std::endl;
        return;
    }

    h2D_crossing_count->Write();
    h2D_inner_cluster_count->Write();
    h2D_inner_cluster_per_crossing->Write();
    h2D_outer_cluster_count->Write();
    h2D_outer_cluster_per_crossing->Write();
    h2D_all_cluster_count->Write();
    h2D_all_cluster_per_crossing->Write();
    fout->Close();
    delete fout;

    std::cout << "[INFO] Files opened: " << n_file_opened << std::endl;
    std::cout << "[INFO] Events read: " << n_event_total << std::endl;
    std::cout << "[INFO] Selected crossings: " << n_event_selected << std::endl;
    std::cout << "[INFO] Inner clusters counted: " << n_inner_cluster << std::endl;
    std::cout << "[INFO] Outer clusters counted: " << n_outer_cluster << std::endl;
    std::cout << "[INFO] Inner plus outer clusters counted: " << n_all_cluster << std::endl;
    if (first_GL1Packet_BCO != std::numeric_limits<ULong64_t>::max())
    {
        std::cout << "[INFO] First GL1Packet_BCO: " << first_GL1Packet_BCO << std::endl;
    }
    std::cout << "[INFO] Wrote " << output_file << std::endl;
}
