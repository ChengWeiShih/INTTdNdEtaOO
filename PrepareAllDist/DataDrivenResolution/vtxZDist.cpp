#include "vtxZDist.h"

vtxZDist::vtxZDist(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,

    bool Apply_cut_in,
    bool ApplyVtxZReWeighting_in,
    std::pair<bool, int> ApplyEvtBcoFullDiffCut_in,
    bool isMBDAsymmCut_in,
    bool CheckDataDrivenVtxZResolution_in
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    Apply_cut(Apply_cut_in),
    ApplyVtxZReWeighting(ApplyVtxZReWeighting_in),
    ApplyEvtBcoFullDiffCut(ApplyEvtBcoFullDiffCut_in),
    isMBDAsymmCut(isMBDAsymmCut_in),
    CheckDataDrivenVtxZResolution(CheckDataDrivenVtxZResolution_in)
{
    PrepareOutPutFileName();
    PrepareInputFile();

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;
    nCentrality_bin = centrality_edges.size() - 1;

    PrepareOutputFile();
    PrepareHist();

    // f1_MBDAsymmCut1 = new TF1("f1_MBDAsymmCut1","pol1",0,2000);
    // f1_MBDAsymmCut1 -> SetParameters(MBDAsymmCutSlope1_pair.first,MBDAsymmCutSlope1_pair.second);
    
    // f1_MBDAsymmCut2 = new TF1("f1_MBDAsymmCut2","pol1",0,2000);
    // f1_MBDAsymmCut2 -> SetParameters(MBDAsymmCutSlope2_pair.first,MBDAsymmCutSlope2_pair.second);

}

void vtxZDist::PrepareOutPutFileName()
{
    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    std::string runnumber_str = std::to_string( runnumber );
    if (runnumber != -1){
        int runnumber_str_len = 8;
        runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');
    }

    if (output_file_name_suffix.size() > 0 && output_file_name_suffix[0] != '_') {
        output_file_name_suffix = "_" + output_file_name_suffix;
    }

    output_filename = "vtxZDist";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;

    output_filename += (Apply_cut) ? "_VtxZQA" : "_NoVtxZQA";
    output_filename += (ApplyVtxZReWeighting) ? "_VtxZReWeighting" : "";
    output_filename += (runnumber != -1 && ApplyEvtBcoFullDiffCut.first) ? Form("_EvtBcoFullDiffCut%d", ApplyEvtBcoFullDiffCut.second) : "";
    output_filename += (isMBDAsymmCut) ? "_MBDAsymmCut" : "";
    output_filename += (CheckDataDrivenVtxZResolution) ? "_DDVtxZResolution" : "";

    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

std::map<std::string, int> vtxZDist::GetInputTreeBranches(TTree * m_tree_in)
{
    std::map<std::string, int> branch_map;
    TObjArray * branch_list = m_tree_in -> GetListOfBranches();
    for (int i = 0; i < branch_list -> GetEntries(); i++)
    {
        TBranch * branch = dynamic_cast<TBranch*>(branch_list->At(i));
        branch_map[branch -> GetName()] = 1;
    }
    return branch_map;
}

void vtxZDist::PrepareInputFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    tree_in = (TTree*)file_in->Get(tree_name.c_str());

    std::map<std::string, int> branch_map = GetInputTreeBranches(tree_in);

    tree_in -> SetBranchStatus("*", 0);

    tree_in -> SetBranchStatus("is_min_bias_wozdc", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("MBD_charge_sum", 1);

    tree_in -> SetBranchStatus("INTTvtxZ", 1);
    tree_in -> SetBranchStatus("INTTvtxZError", 1);
    tree_in -> SetBranchStatus("NgroupTrapezoidal", 1);
    tree_in -> SetBranchStatus("NgroupCoarse", 1);
    tree_in -> SetBranchStatus("TrapezoidalFitWidth", 1);
    tree_in -> SetBranchStatus("TrapezoidalFWHM", 1);

    if (CheckDataDrivenVtxZResolution){
        tree_in -> SetBranchStatus("Left_INTTvtxZ", 1);
        tree_in -> SetBranchStatus("Left_INTTvtxZError", 1);
        tree_in -> SetBranchStatus("Left_NgroupTrapezoidal", 1);
        tree_in -> SetBranchStatus("Left_NgroupCoarse", 1);
        tree_in -> SetBranchStatus("Left_TrapezoidalFitWidth", 1);
        tree_in -> SetBranchStatus("Left_TrapezoidalFWHM", 1);

        tree_in -> SetBranchStatus("Right_INTTvtxZ", 1);
        tree_in -> SetBranchStatus("Right_INTTvtxZError", 1);
        tree_in -> SetBranchStatus("Right_NgroupTrapezoidal", 1);
        tree_in -> SetBranchStatus("Right_NgroupCoarse", 1);
        tree_in -> SetBranchStatus("Right_TrapezoidalFitWidth", 1);
        tree_in -> SetBranchStatus("Right_TrapezoidalFWHM", 1);
    }

    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("NClus_Layer1", 1);

    tree_in -> SetBranchStatus("MBD_south_charge_sum", 1);
    tree_in -> SetBranchStatus("MBD_north_charge_sum", 1);
    
    // note : for data
    if (branch_map.find("MBDNSg2") != branch_map.end()) {
        tree_in -> SetBranchStatus("MBDNSg2", 1);
        m_withTrig = true;
    }
    if (branch_map.find("MBDNSg2_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchStatus("MBDNSg2_vtxZ10cm", 1);}
    if (branch_map.find("MBDNSg2_vtxZ30cm") != branch_map.end()) {tree_in -> SetBranchStatus("MBDNSg2_vtxZ30cm", 1);}
    
    if (branch_map.find("InttBcoFullDiff_next") != branch_map.end()) {tree_in -> SetBranchStatus("InttBcoFullDiff_next", 1); }
    
    // note :for MC
    if (branch_map.find("NTruthVtx") != branch_map.end()) {tree_in -> SetBranchStatus("NTruthVtx", 1);}
    if (branch_map.find("TruthPV_trig_z") != branch_map.end()) {tree_in -> SetBranchStatus("TruthPV_trig_z", 1);}

    // Division : ---SetBranchAddress-----------------------------------------------------------------------------------------------
    tree_in -> SetBranchAddress("is_min_bias_wozdc", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);

    tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
    tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);
    tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);
    tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);

    if (CheckDataDrivenVtxZResolution){
        tree_in -> SetBranchAddress("Left_INTTvtxZ", &Left_INTTvtxZ);
        tree_in -> SetBranchAddress("Left_INTTvtxZError", &Left_INTTvtxZError);
        tree_in -> SetBranchAddress("Left_NgroupTrapezoidal", &Left_NgroupTrapezoidal);
        tree_in -> SetBranchAddress("Left_NgroupCoarse", &Left_NgroupCoarse);
        tree_in -> SetBranchAddress("Left_TrapezoidalFitWidth", &Left_TrapezoidalFitWidth);
        tree_in -> SetBranchAddress("Left_TrapezoidalFWHM", &Left_TrapezoidalFWHM);

        tree_in -> SetBranchAddress("Right_INTTvtxZ", &Right_INTTvtxZ);
        tree_in -> SetBranchAddress("Right_INTTvtxZError", &Right_INTTvtxZError);
        tree_in -> SetBranchAddress("Right_NgroupTrapezoidal", &Right_NgroupTrapezoidal);
        tree_in -> SetBranchAddress("Right_NgroupCoarse", &Right_NgroupCoarse);
        tree_in -> SetBranchAddress("Right_TrapezoidalFitWidth", &Right_TrapezoidalFitWidth);
        tree_in -> SetBranchAddress("Right_TrapezoidalFWHM", &Right_TrapezoidalFWHM);
    }

    tree_in -> SetBranchAddress("NClus", &NClus);
    tree_in -> SetBranchAddress("NClus_Layer1", &NClus_Layer1);

    // note : for data
    if (branch_map.find("MBDNSg2") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);}
    if (branch_map.find("MBDNSg2_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ10cm", &MBDNSg2_vtxZ10cm);}
    if (branch_map.find("MBDNSg2_vtxZ30cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ30cm", &MBDNSg2_vtxZ30cm);}

    if (branch_map.find("InttBcoFullDiff_next") != branch_map.end()) {tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next); }

    // note : for MC
    if (branch_map.find("NTruthVtx") != branch_map.end()) {tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);}
    if (branch_map.find("TruthPV_trig_z") != branch_map.end()) {tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);}
}

void vtxZDist::PrepareOutputFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
}

void vtxZDist::PrepareHist()
{
    h1D_centrality_bin = new TH1D("h1D_centrality_bin","h1D_centrality_bin;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%
    
    h1D_map.clear();
    
    h1D_map.insert(
        std::make_pair(
            "h1D_INTTz_Inclusive100",
            new TH1D("h1D_INTTz_Inclusive100","h1D_INTTz_Inclusive100;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_INTTz_Inclusive70",
            new TH1D("h1D_INTTz_Inclusive70","h1D_INTTz_Inclusive70;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_INTTz_Inclusive70_coarse",
            new TH1D("h1D_INTTz_Inclusive70_coarse","h1D_INTTz_Inclusive70_coarse;INTT z-vertex [cm];Entries", nVtxZ_bin/2, vtxZ_range.first, vtxZ_range.second)
        )
    );

    if (CheckDataDrivenVtxZResolution){

        h1D_map.insert(
            std::make_pair(
                "h1D_LeftINTTz_Inclusive70",
                new TH1D("h1D_LeftINTTz_Inclusive70","h1D_LeftINTTz_Inclusive70;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
            )
        );

        h1D_map.insert(
            std::make_pair(
                "h1D_LeftINTTz_Inclusive70_coarse",
                new TH1D("h1D_LeftINTTz_Inclusive70_coarse","h1D_LeftINTTz_Inclusive70_coarse;INTT z-vertex [cm];Entries", nVtxZ_bin/2, vtxZ_range.first, vtxZ_range.second)
            )
        );

        h1D_map.insert(
            std::make_pair(
                "h1D_RightINTTz_Inclusive70",
                new TH1D("h1D_RightINTTz_Inclusive70","h1D_RightINTTz_Inclusive70;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
            )
        );

        h1D_map.insert(
            std::make_pair(
                "h1D_RightINTTz_Inclusive70_coarse",
                new TH1D("h1D_RightINTTz_Inclusive70_coarse","h1D_RightINTTz_Inclusive70_coarse;INTT z-vertex [cm];Entries", nVtxZ_bin/2, vtxZ_range.first, vtxZ_range.second)
            )
        );

        h1D_map.insert(
            std::make_pair(
                "h1D_LeftRightINTTz_diff_Inclusive70",
                new TH1D("h1D_LeftRightINTTz_diff_Inclusive70","h1D_LeftRightINTTz_diff_Inclusive70;z-vertex dif (INTTLeft - INTTRight) [cm];Entries", 500, -10, 10)
            )
        );

        h1D_map.insert(
            std::make_pair(
                "h1D_Full_LeftRightINTTz_diff_Inclusive70",
                new TH1D("h1D_Full_LeftRightINTTz_diff_Inclusive70","h1D_Full_LeftRightINTTz_diff_Inclusive70;z-vertex dif (Full - Avg_INTTLeftINTTRight) [cm];Entries", 2000, -5.0, 5.0)
            )
        );
    }

    h1D_map.insert(
        std::make_pair(
            "h1D_INTTz_Inclusive100_tight",
            new TH1D("h1D_INTTz_Inclusive100_tight","h1D_INTTz_Inclusive100_tight;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_INTTz_Inclusive70_tight",
            new TH1D("h1D_INTTz_Inclusive70_tight","h1D_INTTz_Inclusive70_tight;INTT vtxZ [cm];Entries", nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
        )
    );

    // for (int i = 0; i < nCentrality_bin; i++)
    // {
    //     h1D_map.insert(
    //         std::make_pair(
    //             Form("h1D_INTTz_Mbin%d", i),
    //             new TH1D(Form("h1D_INTTz_Mbin%d", i), Form("h1D_INTTz_Mbin%d;INTT vtxZ [cm];Entries", i), nVtxZ_bin, vtxZ_range.first, vtxZ_range.second)
    //         )
    //     );
    // }


    // h1D_map.insert(
    //     std::make_pair(
    //         "h1D_INTTzNarrow_Inclusive100",
    //         new TH1D("h1D_INTTzNarrow_Inclusive100","h1D_INTTzNarrow_Inclusive100;INTT vtxZ [cm];Entries", nVtxZ_bin_narrow, vtxZ_range_narrow.first, vtxZ_range_narrow.second)
    //     )
    // );

    // h1D_map.insert(
    //     std::make_pair(
    //         "h1D_INTTzNarrow_Inclusive70",
    //         new TH1D("h1D_INTTzNarrow_Inclusive70","h1D_INTTzNarrow_Inclusive70;INTT vtxZ [cm];Entries", nVtxZ_bin_narrow, vtxZ_range_narrow.first, vtxZ_range_narrow.second)
    //     )
    // );

    // for (int i = 0; i < nCentrality_bin; i++)
    // {
    //     h1D_map.insert(
    //         std::make_pair(
    //             Form("h1D_INTTzNarrow_Mbin%d", i),
    //             new TH1D(Form("h1D_INTTzNarrow_Mbin%d", i), Form("h1D_INTTzNarrow_Mbin%d;INTT vtxZ [cm];Entries", i), nVtxZ_bin_narrow, vtxZ_range_narrow.first, vtxZ_range_narrow.second)
    //         )
    //     );
    // }


    // note : MBD-INTT vtxZ Diff
    h1D_map.insert(
        std::make_pair(
            "h1D_VtxZDiff_MBD_INTT_Inclusive100",
            new TH1D("h1D_VtxZDiff_MBD_INTT_Inclusive100","h1D_VtxZDiff_MBD_INTT_Inclusive100;MBD vtxZ - INTT vtxZ [cm];Entries", 100, -15, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_VtxZDiff_MBD_INTT_Inclusive70",
            new TH1D("h1D_VtxZDiff_MBD_INTT_Inclusive70","h1D_VtxZDiff_MBD_INTT_Inclusive70;MBD vtxZ - INTT vtxZ [cm];Entries", 100, -15, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_VtxZDiff_MBD_INTT_HighNClus",
            new TH1D("h1D_VtxZDiff_MBD_INTT_HighNClus","h1D_VtxZDiff_MBD_INTT_HighNClus;MBD vtxZ - INTT vtxZ [cm];Entries", 100, -15, 15)
        )
    );



    // note : for the INTT vtxZ profile
    h1D_map.insert(
        std::make_pair(
            "h1D_TrapezoidalFitWidth",
            new TH1D("h1D_TrapezoidalFitWidth","h1D_TrapezoidalFitWidth;INTT vtxZ Dist Fit Width [cm];Entries", 100, 0, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_TrapezoidalFWHM",
            new TH1D("h1D_TrapezoidalFWHM","h1D_TrapezoidalFWHM;INTT vtxZ Dist FWHM [cm];Entries", 100, 0, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_INTTvtxZError",
            new TH1D("h1D_INTTvtxZError","h1D_INTTvtxZError;INTT vtxZ StdDev [cm];Entries", 100, 0, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_NgroupTrapezoidal",
            new TH1D("h1D_NgroupTrapezoidal","h1D_NgroupTrapezoidal;N group (Trapezoidal);Entries", 15, 0, 15)
        )
    );

    h1D_map.insert(
        std::make_pair(
            "h1D_NgroupCoarse",
            new TH1D("h1D_NgroupCoarse","h1D_NgroupCoarse;N group (Coarse);Entries", 15, 0, 15)
        )
    );

    if (runnumber == -1)
    {
        h1D_map.insert(
            std::make_pair(
                "h1D_INTTvtxZ_resolution_Inclusive100", 
                new TH1D("h1D_INTTvtxZ_resolution_Inclusive100", "h1D_INTTvtxZ_resolution_Inclusive100;INTTvtxZ - Truth_{z} [cm];Entries (/0.1)",100, -2.5,2.5)
            )
        );
        h1D_map.insert(
            std::make_pair(
                "h1D_INTTvtxZ_resolution_Inclusive70", 
                new TH1D("h1D_INTTvtxZ_resolution_Inclusive70", "h1D_INTTvtxZ_resolution_Inclusive70;INTTvtxZ - Truth_{z} [cm];Entries (/0.1)",100, -2.5,2.5)
            )
        );
        h1D_map.insert(
            std::make_pair(
                "h1D_INTTvtxZ_resolution_HighNClus", 
            new TH1D("h1D_INTTvtxZ_resolution_HighNClus", "h1D_INTTvtxZ_resolution_HighNClus;INTTvtxZ - Truth_{z} [cm];Entries (/0.1)",100, -2.5, 2.5)
        )
        );
    }

    for (auto &hist : h1D_map)
    {
        std::string YTitle = hist.second -> GetYaxis() -> GetTitle();
        std::string XTitle = hist.second -> GetXaxis() -> GetTitle();

        std::string YTitle_post;

        if (YTitle.find("Entries") != std::string::npos) // note : found the (Entries)
        {
            YTitle_post = Form("Entries  (/%.2f)", hist.second -> GetBinWidth(1));
            hist.second -> GetYaxis() -> SetTitle(YTitle_post.c_str());
        }
    }

    // Division: ---For TH2D---------------------------------------------------------------------------------------
    h2D_map.clear();

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTz_MBDz_Inclusive100",
            new TH2D("h2D_INTTz_MBDz_Inclusive100","h2D_INTTz_MBDz_Inclusive100;INTT vtxZ [cm]; MBD vtxZ [cm]",200, vtxZ_range.first, vtxZ_range.second, 200, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTz_MBDz_Inclusive70",
            new TH2D("h2D_INTTz_MBDz_Inclusive70","h2D_INTTz_MBDz_Inclusive70;INTT vtxZ [cm]; MBD vtxZ [cm]",200, vtxZ_range.first, vtxZ_range.second, 200, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTz_MBDz_Inclusive70_narrow",
            new TH2D("h2D_INTTz_MBDz_Inclusive70_narrow","h2D_INTTz_MBDz_Inclusive70_narrow;INTT z-vertex [cm]; MBD z-vertex [cm]",225, -45, 45, 225, -45, 45)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTz_MBDz_NClus150_narrow",
            new TH2D("h2D_INTTz_MBDz_NClus150_narrow","h2D_INTTz_MBDz_NClus150_narrow;INTT z-vertex [cm]; MBD z-vertex [cm]",225, -45, 45, 225, -45, 45)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTz_MBDz_HighNClus",
            new TH2D("h2D_INTTz_MBDz_HighNClus","h2D_INTTz_MBDz_HighNClus;INTT vtxZ [cm]; MBD vtxZ [cm]",200, vtxZ_range.first, vtxZ_range.second, 200, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_MBDvtxZ_MBDINTTvtxZDiff_HighNClus",
            new TH2D("h2D_MBDvtxZ_MBDINTTvtxZDiff_HighNClus","h2D_MBDvtxZ_MBDINTTvtxZDiff_HighNClus;MBD vtxZ [cm]; MBDz - INTTz [cm]",200, vtxZ_range.first, vtxZ_range.second, 200, -15, 15)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_NClus_INTTvtxZ",
            new TH2D("h2D_NClus_INTTvtxZ","h2D_NClus_INTTvtxZ;NClus (INTT);INTT vtxZ [cm]",200, 0, 2000, 240, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_NClus_TrapezoidalFitWidth",
            new TH2D("h2D_NClus_TrapezoidalFitWidth","h2D_NClus_TrapezoidalFitWidth;NClus;INTT vtxZ Dist Fit Width [cm]", 100, 0, 2000, 100, 0, 15)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_NClus_TrapezoidalFWHM",
            new TH2D("h2D_NClus_TrapezoidalFWHM","h2D_NClus_TrapezoidalFWHM;NClus;INTT vtxZ Dist FWHM [cm]", 100, 0, 2000, 100, 0, 15)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_NClus_INTTvtxZError",
            new TH2D("h2D_NClus_INTTvtxZError","h2D_NClus_INTTvtxZError;NClus;INTT vtxZ StdDev [cm]", 100, 0, 2000, 100, 0, 15)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_confirm_InttNClus_MbdChargeSum",
            new TH2D("h2D_confirm_InttNClus_MbdChargeSum","h2D_confirm_InttNClus_MbdChargeSum;INTT NClus;MBD charge sum", 200, 0, 2000, 200, 0, 2500)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_confirm_InttInnerOuterNClus",
            new TH2D("h2D_confirm_InttInnerOuterNClus","h2D_confirm_InttInnerOuterNClus;NClus (inner);NClus (outer)", 200, 0, 1500, 200, 0, 1500)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_TrapezoidalFitWidth_TrapezoidalFWHM",
            new TH2D("h2D_TrapezoidalFitWidth_TrapezoidalFWHM","h2D_TrapezoidalFitWidth_TrapezoidalFWHM;INTT vtxZ Dist Fit Width [cm];INTT vtxZ Dist FWHM [cm]", 200,0,100,200,0,100)
        )
    );

    h2D_map.insert(
        std::make_pair(
            "h2D_INTTvtxZ_TrapezoidalFWHM",
            new TH2D("h2D_INTTvtxZ_TrapezoidalFWHM","h2D_INTTvtxZ_TrapezoidalFWHM;INTT vtxZ [cm];INTT vtxZ Dist Fit Width [cm]", 200, vtxZ_range.first, vtxZ_range.second,200,0,100)
        )
    );

    h2D_map.insert(std::make_pair("h2D_MBDcharge_south_north",   new TH2D("h2D_MBDcharge_south_north","h2D_MBDcharge_south_north;MBD charge sum (south);MBD charge sum (north)",300,0,1500,300,0,1500)));

    if (CheckDataDrivenVtxZResolution){

        h2D_map.insert(
            std::make_pair(
                "h2D_NClus_LeftINTTz",
                new TH2D("h2D_NClus_LeftINTTz","h2D_NClus_LeftINTTz;NClus (total);INTT vtxZ [cm]", 200, 0, 2000, 240, -60, 60)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_NClus_RightINTTz",
                new TH2D("h2D_NClus_RightINTTz","h2D_NClus_RightINTTz;NClus (total);INTT vtxZ [cm]", 200, 0, 2000, 240, -60, 60)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_NClus_LeftRightINTTz_diff",
                new TH2D("h2D_NClus_LeftRightINTTz_diff","h2D_NClus_LeftRightINTTz_diff;NClus (total);z-vertex dif (INTTLeft - INTTRight) [cm]", 200, 0, 2000, 500, -10, 10)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_NClus_Full_LeftRightINTTz_diff",
                new TH2D("h2D_NClus_Full_LeftRightINTTz_diff","h2D_NClus_Full_LeftRightINTTz_diff;NClus (total);z-vertex dif (Full - Avg_INTTLeftINTTRight) [cm]", 200, 0, 2000, 2000, -5.0, 5.0)
            )
        );
    }

    // note : for MC
    if (runnumber == -1)
    {
        h2D_map.insert(
            std::make_pair(
                "h2D_NClus_Zresolution", 
                new TH2D("h2D_NClus_Zresolution","h2D_NClus_Zresolution;NClus;INTTvtxZ - Truth_{z} [cm]",100,0,2000,200,-4,4)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_truthZ_Zresolution_Inclusive100", 
                new TH2D("h2D_truthZ_Zresolution_Inclusive100","h2D_truthZ_Zresolution_Inclusive100;Truth_{z} [cm];INTTvtxZ - Truth_{z} [cm]",100,-50,50,200,-4,4)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_truthZ_Zresolution_HighNClus", 
                new TH2D("h2D_truthZ_Zresolution_HighNClus","h2D_truthZ_Zresolution_HighNClus;Truth_{z} [cm];INTTvtxZ - Truth_{z} [cm]",100,-50,50,200,-4,4)
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_GoodRecoZ_TruthZ_Centrality",
                new TH2D("h2D_GoodRecoZ_TruthZ_Centrality","h2D_GoodRecoZ_TruthZ_Centrality;Truth_{z} [cm];Centrality [%]",nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0])
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_TruthCount_TruthZ_Centrality",
                new TH2D("h2D_TruthCount_TruthZ_Centrality","h2D_TruthCount_TruthZ_Centrality;Truth_{z} [cm];Centrality [%]",nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0])
            )
        );

        h2D_map.insert(
            std::make_pair(
                "h2D_RecoZEffi_TruthZ_Centrality",
                new TH2D("h2D_RecoZEffi_TruthZ_Centrality","h2D_RecoZEffi_TruthZ_Centrality;Truth_{z} [cm];Centrality [%]",nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0])
            )
        );

    }

}

void vtxZDist::PrepareEvent()
{
    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        if (i % 500 == 0) {std::cout << "Processing Event " << i << " / " << run_nEvents << std::endl;}

        // =======================================================================================================================================================
        // note : hard cut

        // note : for data
        if (runnumber != -1 && ApplyEvtBcoFullDiffCut.first && InttBcoFullDiff_next <= ApplyEvtBcoFullDiffCut.second) {continue;}
        // if (runnumber != -1 && MBDNSg2 != 1) {continue;} // todo: assume MC no trigger

        // note : for MC
        // if (runnumber == -1 && NTruthVtx != 1) {continue;}

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (INTTvtxZ != INTTvtxZ) {continue;}
        if (MBD_z_vtx < cut_GlobalMBDvtxZ.first || MBD_z_vtx > cut_GlobalMBDvtxZ.second) {continue;} // todo: the hard cut 60 cm 

        // =======================================================================================================================================================
        // note : optional cut
        if (Apply_cut && (MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {continue;}
        if (Apply_cut && (TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){continue;}
        if (Apply_cut && (TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){continue;}
        if (Apply_cut && (INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){continue;}

        
        // if (isMBDAsymmCut && (MBD_north_charge_sum > f1_MBDAsymmCut1->Eval(MBD_south_charge_sum) || MBD_north_charge_sum < f1_MBDAsymmCut2->Eval(MBD_south_charge_sum))) {continue;}

        // =======================================================================================================================================================
        double INTTvtxZWeighting;
        if (ApplyVtxZReWeighting && h1D_INTT_vtxZ_reweighting != nullptr){
            INTTvtxZWeighting = h1D_INTT_vtxZ_reweighting -> GetBinContent(h1D_INTT_vtxZ_reweighting -> FindBin(INTTvtxZ));
        }
        else if (ApplyVtxZReWeighting && h1D_INTT_vtxZ_reweighting == nullptr){
            std::cout << "ApplyVtxZReWeighting is true, but h1D_INTT_vtxZ_reweighting is nullptr" << std::endl;
            exit(1); 
        }
        else {
            INTTvtxZWeighting = 1.0;
        }

        int Mbin = h1D_centrality_bin -> Fill(MBD_centrality, INTTvtxZWeighting);
        Mbin = (Mbin == -1) ? -1 : Mbin - 1;
        if (Mbin == -1) {
            std::cout << "Mbin == -1, MBD_centrality = " << MBD_centrality << std::endl;
            continue;
        }

        if (i % 100 == 0) {std::cout << "MBD_centrality: "<< MBD_centrality <<", Mbin: " << Mbin << ", nCentrality_bin: " << nCentrality_bin << std::endl;}

        h2D_map["h2D_MBDcharge_south_north"] -> Fill(MBD_south_charge_sum, MBD_north_charge_sum, INTTvtxZWeighting);

        // note : inclusive100
        h2D_map["h2D_confirm_InttNClus_MbdChargeSum"] -> Fill(NClus, MBD_charge_sum, INTTvtxZWeighting);
        h2D_map["h2D_confirm_InttInnerOuterNClus"] -> Fill(NClus_Layer1, NClus - NClus_Layer1, INTTvtxZWeighting);

        h1D_map["h1D_INTTz_Inclusive100"] -> Fill(INTTvtxZ, INTTvtxZWeighting);
        h1D_map["h1D_VtxZDiff_MBD_INTT_Inclusive100"] -> Fill(MBD_z_vtx - INTTvtxZ, INTTvtxZWeighting);
        h2D_map["h2D_INTTz_MBDz_Inclusive100"] -> Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);

        h1D_map["h1D_TrapezoidalFitWidth"] -> Fill(TrapezoidalFitWidth, INTTvtxZWeighting);
        h1D_map["h1D_TrapezoidalFWHM"] -> Fill(TrapezoidalFWHM, INTTvtxZWeighting);
        h1D_map["h1D_INTTvtxZError"] -> Fill(INTTvtxZError, INTTvtxZWeighting);
        h1D_map["h1D_NgroupTrapezoidal"] -> Fill(NgroupTrapezoidal, INTTvtxZWeighting);
        h1D_map["h1D_NgroupCoarse"] -> Fill(NgroupCoarse, INTTvtxZWeighting);
        
        h2D_map["h2D_NClus_INTTvtxZ"] -> Fill(NClus, INTTvtxZ, INTTvtxZWeighting);
        h2D_map["h2D_NClus_TrapezoidalFitWidth"] -> Fill(NClus, TrapezoidalFitWidth, INTTvtxZWeighting);
        h2D_map["h2D_NClus_TrapezoidalFWHM"] -> Fill(NClus, TrapezoidalFWHM, INTTvtxZWeighting);
        h2D_map["h2D_NClus_INTTvtxZError"] -> Fill(NClus, INTTvtxZError, INTTvtxZWeighting);

        h2D_map["h2D_TrapezoidalFitWidth_TrapezoidalFWHM"] -> Fill(TrapezoidalFitWidth, TrapezoidalFWHM, INTTvtxZWeighting);
        h2D_map["h2D_INTTvtxZ_TrapezoidalFWHM"] -> Fill(INTTvtxZ, TrapezoidalFWHM, INTTvtxZWeighting);

        // h1D_map[Form("h1D_INTTz_Mbin%d", Mbin)] -> Fill(INTTvtxZ, INTTvtxZWeighting);

        if (CheckDataDrivenVtxZResolution){
            h2D_map["h2D_NClus_LeftINTTz"] -> Fill(NClus, Left_INTTvtxZ, INTTvtxZWeighting);
            h2D_map["h2D_NClus_RightINTTz"] -> Fill(NClus, Right_INTTvtxZ, INTTvtxZWeighting);
            h2D_map["h2D_NClus_LeftRightINTTz_diff"] -> Fill(NClus, Left_INTTvtxZ - Right_INTTvtxZ, INTTvtxZWeighting);
            h2D_map["h2D_NClus_Full_LeftRightINTTz_diff"] -> Fill(NClus, INTTvtxZ - (Left_INTTvtxZ + Right_INTTvtxZ)/2.0, INTTvtxZWeighting);
        }

    
        // todo: the Semi_inclusive_bin 
        if (Mbin <= Semi_inclusive_bin){ 
            h1D_map["h1D_INTTz_Inclusive70"] -> Fill(INTTvtxZ, INTTvtxZWeighting);
            h1D_map["h1D_INTTz_Inclusive70_coarse"] -> Fill(INTTvtxZ, INTTvtxZWeighting);
            h1D_map["h1D_VtxZDiff_MBD_INTT_Inclusive70"] -> Fill(MBD_z_vtx - INTTvtxZ, INTTvtxZWeighting);
            h2D_map["h2D_INTTz_MBDz_Inclusive70"] -> Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);
            h2D_map["h2D_INTTz_MBDz_Inclusive70_narrow"] -> Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);

            if (CheckDataDrivenVtxZResolution) {
                h1D_map["h1D_LeftINTTz_Inclusive70"] -> Fill(Left_INTTvtxZ, INTTvtxZWeighting);
                h1D_map["h1D_LeftINTTz_Inclusive70_coarse"] -> Fill(Left_INTTvtxZ, INTTvtxZWeighting);
                h1D_map["h1D_RightINTTz_Inclusive70"] -> Fill(Right_INTTvtxZ, INTTvtxZWeighting);
                h1D_map["h1D_RightINTTz_Inclusive70_coarse"] -> Fill(Right_INTTvtxZ, INTTvtxZWeighting);
                h1D_map["h1D_LeftRightINTTz_diff_Inclusive70"] -> Fill(Left_INTTvtxZ - Right_INTTvtxZ, INTTvtxZWeighting);
                h1D_map["h1D_Full_LeftRightINTTz_diff_Inclusive70"] -> Fill(INTTvtxZ - (Left_INTTvtxZ + Right_INTTvtxZ)/2.0, INTTvtxZWeighting);
            }
        }

        if (NClus > 150){
            h2D_map["h2D_INTTz_MBDz_NClus150_narrow"] -> Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);
        }

        // if (MBD_centrality <= 92){
        //     h2D_map["h2D_INTTz_MBDz_Inclusive92_narrow"] -> Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);
        // }
        
        if (NClus > HighNClus){
            h1D_map["h1D_VtxZDiff_MBD_INTT_HighNClus"]->Fill(MBD_z_vtx - INTTvtxZ, INTTvtxZWeighting);
            h2D_map["h2D_INTTz_MBDz_HighNClus"]->Fill(INTTvtxZ, MBD_z_vtx, INTTvtxZWeighting);
            h2D_map["h2D_MBDvtxZ_MBDINTTvtxZDiff_HighNClus"]->Fill(MBD_z_vtx, MBD_z_vtx - INTTvtxZ, INTTvtxZWeighting);
        }
    

        // note : for MC
        // todo : no vtxZ reweighting for the resolution study
        if (runnumber == -1){
            h1D_map["h1D_INTTvtxZ_resolution_Inclusive100"] -> Fill(INTTvtxZ - TruthPV_trig_z);
            h2D_map["h2D_truthZ_Zresolution_Inclusive100"] -> Fill(TruthPV_trig_z, INTTvtxZ - TruthPV_trig_z);
            h2D_map["h2D_NClus_Zresolution"]  -> Fill(NClus, INTTvtxZ - TruthPV_trig_z);

            if (NClus > HighNClus){
                h1D_map["h1D_INTTvtxZ_resolution_HighNClus"] -> Fill(INTTvtxZ - TruthPV_trig_z);
                h2D_map["h2D_truthZ_Zresolution_HighNClus"] -> Fill(TruthPV_trig_z, INTTvtxZ - TruthPV_trig_z);
            }

            // if (Mbin <= Semi_inclusive_bin){
            //     h1D_map["h1D_INTTvtxZ_resolution_Inclusive70"] -> Fill(INTTvtxZ - TruthPV_trig_z);
            // }

            // todo : no weight
            // if (fabs(INTTvtxZ - TruthPV_trig_z) <= cut_GoodRecoVtxZ) {h2D_map["h2D_GoodRecoZ_TruthZ_Centrality"] -> Fill(TruthPV_trig_z, MBD_centrality);}
            // h2D_map["h2D_TruthCount_TruthZ_Centrality"] -> Fill(TruthPV_trig_z, MBD_centrality);
        }

        // =======================================================================================================================================================
        // todo: the vtxZ range cut for the analysis is based on INTT only 
        if (INTTvtxZ < cut_AnaVtxZ.first || INTTvtxZ > cut_AnaVtxZ.second) {continue;} 
        // if (MBD_z_vtx < cut_AnaVtxZ.first || MBD_z_vtx > cut_AnaVtxZ.second) {continue;}

        h1D_map["h1D_INTTz_Inclusive100_tight"] -> Fill(INTTvtxZ, INTTvtxZWeighting);

        // if (Mbin <= Semi_inclusive_bin){
        //     h1D_map["h1D_INTTz_Inclusive70_tight"] -> Fill(INTTvtxZ, INTTvtxZWeighting);
        // }
 
    } // note : end of the event loop
}

void vtxZDist::EndRun()
{
    file_out -> cd();
    for (auto &hist : h1D_map)
    {
        hist.second -> Write();
    }

    if (runnumber == -1) {h2D_map["h2D_RecoZEffi_TruthZ_Centrality"] -> Divide(h2D_map["h2D_GoodRecoZ_TruthZ_Centrality"], h2D_map["h2D_TruthCount_TruthZ_Centrality"]);}

    for (auto &hist : h2D_map)
    {
        hist.second -> Write();
    }

    if (h1D_INTT_vtxZ_reweighting != nullptr){
        h1D_INTT_vtxZ_reweighting -> Write();
    }

    h1D_centrality_bin -> Write();

    file_out -> Close();
}