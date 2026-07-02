int quick_check(){

    // std::string directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/per5k";
    // std::string intput_filename = "ntuple_per5k_000*.root";

    std::string directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/EvtVtxZ/completed";
    std::string intput_filename = "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_VtxZReco_00082391_0000*.root";

    TChain * chain = new TChain("EventTree");
    chain -> Add(Form("%s/%s",directory.c_str(), intput_filename.c_str()));

    chain -> SetBranchStatus("*",0);

    chain->SetBranchStatus("is_min_bias", 1);
    chain->SetBranchStatus("is_min_bias_wozdc", 1);
    chain->SetBranchStatus("is_min_bias_private", 1);
    chain->SetBranchStatus("is_min_bias_private_MinDeposit", 1);
    chain->SetBranchStatus("MBD_centrality", 1);
    chain->SetBranchStatus("MBD_centrality_private", 1);
    chain->SetBranchStatus("MBD_centrality_privateFit", 1);
    chain->SetBranchStatus("MBD_z_vtx", 1);
    chain->SetBranchStatus("MBD_south_npmt", 1);
    chain->SetBranchStatus("MBD_north_npmt", 1);
    chain->SetBranchStatus("MBD_south_charge_sum", 1);
    chain->SetBranchStatus("MBD_north_charge_sum", 1);
    chain->SetBranchStatus("MBD_charge_sum", 1);
    chain->SetBranchStatus("MBD_charge_asymm", 1);
    chain->SetBranchStatus("MBD_nhitsoverths_south", 1);
    chain->SetBranchStatus("MBD_nhitsoverths_north", 1);

    chain->Draw("MBD_centrality_private:MBD_centrality_privateFit>>h2D_centralityPriate(100,0,100,101,-0.5,100.5)","is_min_bias_private_MinDeposit == 1","",100000);
    TH1D *h2D_centralityPriate = (TH1D*) gDirectory->Get("h2D_centralityPriate");

    chain->Draw("MBD_centrality_private>>h1D_MBD_centrality_private(101,-0.5,100.5)","is_min_bias_private_MinDeposit == 1","",100000);
    TH1D *h1D_MBD_centrality_private = (TH1D*) gDirectory->Get("h1D_MBD_centrality_private");

    chain->Draw("MBD_centrality_privateFit>>h1D_MBD_centrality_privateFit(100,0,100)","is_min_bias_private_MinDeposit == 1","",100000);
    TH1D *h1D_MBD_centrality_privateFit = (TH1D*) gDirectory->Get("h1D_MBD_centrality_privateFit");

    TFile * file_out = new TFile(Form("%s/Centrality_check.root",directory.c_str()), "RECREATE");
    
    h2D_centralityPriate -> Write();
    h1D_MBD_centrality_private -> Write();
    h1D_MBD_centrality_privateFit -> Write();

    file_out -> Close();

    return 888;
}