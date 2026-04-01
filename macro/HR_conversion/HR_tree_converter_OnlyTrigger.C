int HR_tree_converter_OnlyTrigger(
    int process_id = 0,
    int runnumber = 82699,
    std::string input_directory  = "/sphenix/user/ChengWei/INTT/HitCarriedOver_Streaming/production/macro/test_run_82699_self/completed",
    std::string output_directory = "/sphenix/user/ChengWei/INTT/HitCarriedOver_Streaming/production/macro/test_run_82699_self/conversion"
){

    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    std::string input_filename  = Form("Ntuple_000%d_standalone_ana538_Acts_%s.root",runnumber,job_index.c_str());
    std::string output_filename = Form("Ntuple_convertTrigMbdZ_000%d_standalone_ana538_Acts_%s.root",runnumber,job_index.c_str());

    TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    TTree * tree_in = (TTree *) file_in -> Get("VTX");


    float centrality_mbd_in;
    bool is_min_bias_in;
    float MBD_charge_sum_in;
    uint64_t gl1BunchCrossing_in;
    vector<float> * mbdVertex = 0;
    vector<int> * firedTriggers = 0;

    vector<unsigned long> *clusterKey = 0;
    vector<unsigned int> *cluster_layer = 0;
    vector<int> *cluster_chip = 0;
    vector<int> *cluster_stave = 0;
    vector<float> *cluster_globalX = 0;
    vector<float> *cluster_globalY = 0;
    vector<float> *cluster_globalZ = 0;
    vector<float> *cluster_phi = 0;
    vector<float> *cluster_eta = 0;
    vector<float> *cluster_r = 0;
    vector<int> *cluster_phiSize = 0;
    vector<int> *cluster_zSize = 0;
    vector<int> *cluster_adc = 0;
    vector<int> *cluster_timeBucketID = 0;
    vector<int> *cluster_ladderZId = 0;
    vector<int> *cluster_ladderPhiId = 0;
    vector<float> *cluster_LocalX = 0;
    vector<float> *cluster_LocalY = 0;
    vector<int>   *cluster_FPHXBCO = 0;

    vector<int> *trackerVertexId = 0;
    vector<float> *trackerVertexX = 0;
    vector<float> *trackerVertexY = 0;
    vector<float> *trackerVertexZ = 0;
    vector<float> *trackerVertexChisq = 0;
    vector<int> *trackerVertexNdof = 0;
    vector<int> *trackerVertexNTracks = 0;
    vector<short> *trackerVertexCrossing = 0;
    vector<vector<int> > *trackerVertexTrackIDs = 0;


    tree_in -> SetBranchAddress("centrality_mbd", &centrality_mbd_in);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias_in);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum_in);
    tree_in -> SetBranchAddress("gl1BunchCrossing", &gl1BunchCrossing_in);
    tree_in -> SetBranchAddress("mbdVertex", &mbdVertex);
    tree_in -> SetBranchAddress("firedTriggers", &firedTriggers);

    tree_in -> SetBranchAddress("clusterKey", &clusterKey);
    tree_in -> SetBranchAddress("cluster_layer", &cluster_layer);
    tree_in -> SetBranchAddress("cluster_chip", &cluster_chip);
    tree_in -> SetBranchAddress("cluster_stave", &cluster_stave);
    tree_in -> SetBranchAddress("cluster_globalX", &cluster_globalX);
    tree_in -> SetBranchAddress("cluster_globalY", &cluster_globalY);
    tree_in -> SetBranchAddress("cluster_globalZ", &cluster_globalZ);
    tree_in -> SetBranchAddress("cluster_phi", &cluster_phi);
    tree_in -> SetBranchAddress("cluster_eta", &cluster_eta);
    tree_in -> SetBranchAddress("cluster_r", &cluster_r);
    tree_in -> SetBranchAddress("cluster_phiSize", &cluster_phiSize);
    tree_in -> SetBranchAddress("cluster_zSize", &cluster_zSize);
    tree_in -> SetBranchAddress("cluster_adc", &cluster_adc);
    tree_in -> SetBranchAddress("cluster_timeBucketID", &cluster_timeBucketID);
    tree_in -> SetBranchAddress("cluster_ladderZId", &cluster_ladderZId);
    tree_in -> SetBranchAddress("cluster_ladderPhiId", &cluster_ladderPhiId);
    tree_in -> SetBranchAddress("cluster_LocalX", &cluster_LocalX);
    tree_in -> SetBranchAddress("cluster_LocalY", &cluster_LocalY);
    tree_in -> SetBranchAddress("cluster_FPHXBCO", &cluster_FPHXBCO);
    
    tree_in -> SetBranchAddress("trackerVertexId", &trackerVertexId);
    tree_in -> SetBranchAddress("trackerVertexX", &trackerVertexX);
    tree_in -> SetBranchAddress("trackerVertexY", &trackerVertexY);
    tree_in -> SetBranchAddress("trackerVertexZ", &trackerVertexZ);
    tree_in -> SetBranchAddress("trackerVertexChisq", &trackerVertexChisq);
    tree_in -> SetBranchAddress("trackerVertexNdof", &trackerVertexNdof);
    tree_in -> SetBranchAddress("trackerVertexNTracks", &trackerVertexNTracks);
    tree_in -> SetBranchAddress("trackerVertexCrossing", &trackerVertexCrossing);
    tree_in -> SetBranchAddress("trackerVertexTrackIDs", &trackerVertexTrackIDs);

    system (Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));

    TFile * file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
    TTree * tree_out = new TTree("EventTree", Form("Tree for Run %d",runnumber));
    // note : MBD & centrality relevant
    bool is_min_bias;
    float MBD_centrality;
    float MBD_charge_sum;
    vector<int> firedTriggers_out;

    float MBD_z_vtx = 0;
    float MBD_south_charge_sum = 100;
    float MBD_north_charge_sum = 100;
    float MBD_charge_asymm = 1;
    int InttBcoFullDiff_next = 200;

    // note : trigger tag
    int MBDNSg2 = 1;
    int MBDNSg2_vtxZ10cm = 1;
    int MBDNSg2_vtxZ30cm = 1;
    int MBDNSg2_vtxZ60cm = 1;
    uint64_t BunchNumber;
    int nTrack_vtx;
    int Crossing_out;
    int FPHXBCO_out;

    long long NClus;
    std::vector<float> ClusX; ClusX.clear();
    std::vector<float> ClusY; ClusY.clear();
    std::vector<float> ClusZ; ClusZ.clear();
    std::vector<int> ClusLayer; ClusLayer.clear();
    std::vector<unsigned char> ClusLadderZId; ClusLadderZId.clear();
    std::vector<unsigned char> ClusLadderPhiId; ClusLadderPhiId.clear();
    std::vector<float> ClusLocalX; ClusLocalX.clear();
    std::vector<float> ClusLocalY; ClusLocalY.clear();
    std::vector<int> ClusAdc; ClusAdc.clear();
    std::vector<float> ClusPhiSize; ClusPhiSize.clear();
    std::vector<double> ClusEta_INTTz; ClusEta_INTTz.clear();

    // note : INTT vertex Z
    double INTTvtxZ = std::nan("");
    double INTTvtxZError = 0;
    double NgroupTrapezoidal = 1;
    double NgroupCoarse = 1;
    double TrapezoidalFitWidth = 1;
    double TrapezoidalFWHM = 1;

    tree_out -> Branch("firedTriggers", &firedTriggers_out);
    tree_out -> Branch("MBD_z_vtx", &MBD_z_vtx);
    tree_out -> Branch("is_min_bias", &is_min_bias);
    tree_out -> Branch("MBD_centrality", &MBD_centrality);
    tree_out -> Branch("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_out -> Branch("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_out -> Branch("MBD_charge_sum", &MBD_charge_sum);
    tree_out -> Branch("MBD_charge_asymm", &MBD_charge_asymm);
    tree_out -> Branch("InttBcoFullDiff_next", &InttBcoFullDiff_next);
    tree_out -> Branch("MBDNSg2", &MBDNSg2);
    tree_out -> Branch("MBDNSg2_vtxZ10cm", &MBDNSg2_vtxZ10cm);
    tree_out -> Branch("MBDNSg2_vtxZ30cm", &MBDNSg2_vtxZ30cm);
    tree_out -> Branch("MBDNSg2_vtxZ60cm", &MBDNSg2_vtxZ60cm);
    tree_out -> Branch("BunchNumber", &BunchNumber);
    tree_out -> Branch("Crossing", &Crossing_out);
    tree_out -> Branch("FPHXBCO", &FPHXBCO_out);
    tree_out -> Branch("NClus", &NClus);
    tree_out -> Branch("ClusX", &ClusX);
    tree_out -> Branch("ClusY", &ClusY);
    tree_out -> Branch("ClusZ", &ClusZ);
    tree_out -> Branch("ClusLocalX", &ClusLocalX);
    tree_out -> Branch("ClusLocalY", &ClusLocalY);
    tree_out -> Branch("ClusLayer", &ClusLayer);
    tree_out -> Branch("ClusLadderZId", &ClusLadderZId);
    tree_out -> Branch("ClusLadderPhiId", &ClusLadderPhiId);
    tree_out -> Branch("ClusAdc", &ClusAdc);
    tree_out -> Branch("ClusPhiSize", &ClusPhiSize);
    tree_out -> Branch("ClusEta_INTTz", &ClusEta_INTTz);
    tree_out -> Branch("INTTvtxZ", &INTTvtxZ);
    tree_out -> Branch("INTTvtxZError", &INTTvtxZError);
    tree_out -> Branch("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_out -> Branch("NgroupCoarse", &NgroupCoarse);
    tree_out -> Branch("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_out -> Branch("TrapezoidalFWHM", &TrapezoidalFWHM);
    tree_out -> Branch("nTrack_vtx", &nTrack_vtx);

    for (int i = 0; i < tree_in->GetEntries(); i++){
        // for (int i = 0; i < 100; i++){
        tree_in -> GetEntry(i);

        if (i%10 == 0){
            std::cout << "Processing entry " << i << " / " << tree_in->GetEntries() <<", MBD_charge_sum: "<< MBD_charge_sum << std::endl;
        }

        is_min_bias = is_min_bias_in;
        MBD_centrality = centrality_mbd_in;
        MBD_charge_sum = MBD_charge_sum_in;
        BunchNumber = gl1BunchCrossing_in;
        if (mbdVertex -> size() > 0){
            MBD_z_vtx = mbdVertex -> at(0);
        }
        else{
            MBD_z_vtx = -1000;
            std::cout<<"interesting, no MBD_z_vtx, MBD_charge_sum: "<< MBD_charge_sum_in <<std::endl;
        }

        firedTriggers_out = *(firedTriggers);

        NClus = 0;
        ClusX.clear();
        ClusY.clear();
        ClusZ.clear();
        ClusLocalX.clear();
        ClusLocalY.clear();
        ClusLayer.clear();
        ClusLadderZId.clear();
        ClusLadderPhiId.clear();
        ClusAdc.clear();
        ClusPhiSize.clear();
        

        Crossing_out = 0;
        std::map<int,int> FPHXBCO_check; FPHXBCO_check.clear();

        for (int j = 0; j < cluster_globalX -> size(); j++){
            
            int crossing = cluster_timeBucketID -> at(j);
            
            if (crossing != 0) {continue;}

            ClusX.push_back(cluster_globalX -> at(j));
            ClusY.push_back(cluster_globalY -> at(j));
            ClusZ.push_back(cluster_globalZ -> at(j));
            ClusLocalX.push_back(cluster_LocalX -> at(j));
            ClusLocalY.push_back(cluster_LocalY -> at(j));
            ClusLayer.push_back(cluster_layer -> at(j));
            ClusLadderZId.push_back(cluster_ladderZId -> at(j));
            ClusLadderPhiId.push_back(cluster_ladderPhiId -> at(j));
            ClusAdc.push_back(cluster_adc -> at(j));
            ClusPhiSize.push_back(cluster_phiSize -> at(j));


            if (FPHXBCO_check.find(cluster_FPHXBCO -> at(j)) == FPHXBCO_check.end()){
                FPHXBCO_check[cluster_FPHXBCO -> at(j)] = 1;
            }
            else {
                FPHXBCO_check[cluster_FPHXBCO -> at(j)] += 1;
            }
        }

        NClus = ClusX.size();

        if (FPHXBCO_check.size() != 1){
            std::cout<<"check this, FPHXBCO_check size for crossing: "<<Crossing_out<<" is "<<FPHXBCO_check.size()<<std::endl;
            for (auto &map_pair : FPHXBCO_check){
                std::cout<<map_pair.first<<", "<<map_pair.second<<std::endl;
            }
        }

        FPHXBCO_out =     (FPHXBCO_check.size() == 0) ? -1000 : FPHXBCO_check.begin()->first;

        INTTvtxZ =        -10;

        nTrack_vtx =      -10;

        tree_out -> Fill();
    }

    file_out -> cd();
    tree_out -> Write();
    file_out -> Close();

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_filename.c_str(), output_directory.c_str()));

    return 888;
}