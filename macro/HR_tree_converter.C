int HR_tree_converter(){

    TFile * file_in = TFile::Open("/sphenix/user/hjheng/sPHENIXRepo/TrackingAnalysis/Silicon_MBD_Vertexing/Silicon_MBD_Comparisons/VertexCompare_run_82405/files/outputVTX_Acts_Default.root");
    TTree * tree_in = (TTree *) file_in -> Get("VTX");

    // int gl1BunchCrossing;

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

    vector<int> *trackerVertexId = 0;
    vector<float> *trackerVertexX = 0;
    vector<float> *trackerVertexY = 0;
    vector<float> *trackerVertexZ = 0;
    vector<float> *trackerVertexChisq = 0;
    vector<int> *trackerVertexNdof = 0;
    vector<int> *trackerVertexNTracks = 0;
    vector<short> *trackerVertexCrossing = 0;
    vector<vector<int> > *trackerVertexTrackIDs = 0;

    // tree_in -> SetBranchAddress("gl1BunchCrossing", &gl1BunchCrossing);

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
    
    tree_in -> SetBranchAddress("trackerVertexId", &trackerVertexId);
    tree_in -> SetBranchAddress("trackerVertexX", &trackerVertexX);
    tree_in -> SetBranchAddress("trackerVertexY", &trackerVertexY);
    tree_in -> SetBranchAddress("trackerVertexZ", &trackerVertexZ);
    tree_in -> SetBranchAddress("trackerVertexChisq", &trackerVertexChisq);
    tree_in -> SetBranchAddress("trackerVertexNdof", &trackerVertexNdof);
    tree_in -> SetBranchAddress("trackerVertexNTracks", &trackerVertexNTracks);
    tree_in -> SetBranchAddress("trackerVertexCrossing", &trackerVertexCrossing);
    tree_in -> SetBranchAddress("trackerVertexTrackIDs", &trackerVertexTrackIDs);

    TFile * file_out = new TFile("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82405/82405_converter.root", "RECREATE");
    TTree * tree_out = new TTree("EventTree", "Tree for Run 82405");
    // note : MBD & centrality relevant
    float MBD_z_vtx = 0;
    bool is_min_bias = 1;
    float MBD_centrality = 1;
    float MBD_south_charge_sum = 100;
    float MBD_north_charge_sum = 100;
    float MBD_charge_sum = 200;
    float MBD_charge_asymm = 1;
    int InttBcoFullDiff_next = 200;

    // note : trigger tag
    int MBDNSg2 = 1;
    int MBDNSg2_vtxZ10cm = 1;
    int MBDNSg2_vtxZ30cm = 1;
    int MBDNSg2_vtxZ60cm = 1;
    long long BunchNumber;
    int nTrack_vtx;

    long long NClus;
    std::vector<float> ClusX; ClusX.clear();
    std::vector<float> ClusY; ClusY.clear();
    std::vector<float> ClusZ; ClusZ.clear();
    std::vector<int> ClusLayer; ClusLayer.clear();
    std::vector<unsigned char> ClusLadderZId; ClusLadderZId.clear();
    std::vector<unsigned char> ClusLadderPhiId; ClusLadderPhiId.clear();
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
    tree_out -> Branch("NClus", &NClus);
    tree_out -> Branch("ClusX", &ClusX);
    tree_out -> Branch("ClusY", &ClusY);
    tree_out -> Branch("ClusZ", &ClusZ);
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

    TH1D * h1D_nVTXperCrossing = new TH1D("h1D_nVTXperCrossing", "Number of Vertices per Crossing;Number of Vertices (in one crossing);Counts", 30,0,30);


    std::map<int,int> bunchnumber_vertex_map; bunchnumber_vertex_map.clear();
    std::map<int, vector<double>> bunchnumber_tracker_vtxZ; bunchnumber_tracker_vtxZ.clear();
    std::map<int, vector<int>> bunchnumber_nTrack_vtx; bunchnumber_nTrack_vtx.clear();

    std::map<int, vector<float>> bunchnumber_ClusX_map; bunchnumber_ClusX_map.clear();
    std::map<int, vector<float>> bunchnumber_ClusY_map; bunchnumber_ClusY_map.clear();
    std::map<int, vector<float>> bunchnumber_ClusZ_map; bunchnumber_ClusZ_map.clear();
    std::map<int, vector<int>> bunchnumber_ClusLayer_map; bunchnumber_ClusLayer_map.clear();
    std::map<int, vector<unsigned char>> bunchnumber_ClusLadderZId_map; bunchnumber_ClusLadderZId_map.clear();
    std::map<int, vector<unsigned char>> bunchnumber_ClusLadderPhiId_map; bunchnumber_ClusLadderPhiId_map.clear();
    std::map<int, vector<int>> bunchnumber_ClusAdc_map; bunchnumber_ClusAdc_map.clear();
    std::map<int, vector<float>> bunchnumber_ClusPhiSize_map; bunchnumber_ClusPhiSize_map.clear();
    // std::map<int,ClusEta_INTTz

    for (int i = 0; i < tree_in->GetEntries(); i++){
        // for (int i = 0; i < 100; i++){
        tree_in -> GetEntry(i);

        if (i%10 == 0){
            std::cout << "Processing entry " << i << " / " << tree_in->GetEntries() << std::endl;
        }

        bunchnumber_vertex_map.clear();
        bunchnumber_tracker_vtxZ.clear();
        bunchnumber_nTrack_vtx.clear();
        
        bunchnumber_ClusX_map.clear();
        bunchnumber_ClusY_map.clear();
        bunchnumber_ClusZ_map.clear();
        bunchnumber_ClusLayer_map.clear();
        bunchnumber_ClusLadderZId_map.clear();
        bunchnumber_ClusLadderPhiId_map.clear();
        bunchnumber_ClusAdc_map.clear();
        bunchnumber_ClusPhiSize_map.clear();

        for (int j = 0; j < trackerVertexCrossing -> size(); j++){
            int crossing = trackerVertexCrossing -> at(j);

            if (bunchnumber_vertex_map.find(crossing) == bunchnumber_vertex_map.end()){
                bunchnumber_vertex_map[crossing] = 1;

                bunchnumber_tracker_vtxZ[crossing] = vector<double>();
                bunchnumber_tracker_vtxZ[crossing].push_back(trackerVertexZ -> at(j));

                bunchnumber_nTrack_vtx[crossing] = vector<int>();
                bunchnumber_nTrack_vtx[crossing].push_back(trackerVertexNTracks -> at(j));
            }
            else{
                bunchnumber_vertex_map[crossing] += 1;
                bunchnumber_tracker_vtxZ[crossing].push_back(trackerVertexZ -> at(j));
                bunchnumber_nTrack_vtx[crossing].push_back(trackerVertexNTracks -> at(j));
            }
        }

        for (int j = 0; j < cluster_globalX -> size(); j++){
            int crossing = cluster_timeBucketID -> at(j);

            if (bunchnumber_ClusX_map.find(crossing) == bunchnumber_ClusX_map.end()){

                bunchnumber_ClusX_map[crossing] = vector<float>();
                bunchnumber_ClusY_map[crossing] = vector<float>();
                bunchnumber_ClusZ_map[crossing] = vector<float>();
                bunchnumber_ClusLayer_map[crossing] = vector<int>();
                bunchnumber_ClusLadderZId_map[crossing] = vector<unsigned char>();
                bunchnumber_ClusLadderPhiId_map[crossing] = vector<unsigned char>();
                bunchnumber_ClusAdc_map[crossing] = vector<int>();
                bunchnumber_ClusPhiSize_map[crossing] = vector<float>();

                bunchnumber_ClusX_map[crossing].push_back(cluster_globalX -> at(j));
                bunchnumber_ClusY_map[crossing].push_back(cluster_globalY -> at(j));
                bunchnumber_ClusZ_map[crossing].push_back(cluster_globalZ -> at(j));
                bunchnumber_ClusLayer_map[crossing].push_back(cluster_layer -> at(j));
                bunchnumber_ClusLadderZId_map[crossing].push_back(cluster_ladderZId -> at(j));
                bunchnumber_ClusLadderPhiId_map[crossing].push_back(cluster_ladderPhiId -> at(j));
                bunchnumber_ClusAdc_map[crossing].push_back(cluster_adc -> at(j));
                bunchnumber_ClusPhiSize_map[crossing].push_back(cluster_phiSize -> at(j));
            }
            else {
                bunchnumber_ClusX_map[crossing].push_back(cluster_globalX -> at(j));
                bunchnumber_ClusY_map[crossing].push_back(cluster_globalY -> at(j));
                bunchnumber_ClusZ_map[crossing].push_back(cluster_globalZ -> at(j));
                bunchnumber_ClusLayer_map[crossing].push_back(cluster_layer -> at(j));
                bunchnumber_ClusLadderZId_map[crossing].push_back(cluster_ladderZId -> at(j));
                bunchnumber_ClusLadderPhiId_map[crossing].push_back(cluster_ladderPhiId -> at(j));
                bunchnumber_ClusAdc_map[crossing].push_back(cluster_adc -> at(j));
                bunchnumber_ClusPhiSize_map[crossing].push_back(cluster_phiSize -> at(j));
            }
        }

        for (auto &pair : bunchnumber_vertex_map){
            int crossing = pair.first;
            int vertex_count = pair.second;

            h1D_nVTXperCrossing -> Fill(vertex_count);

            if (vertex_count != 1){continue;}

            BunchNumber = crossing; // note : here we store the crossing number in the BunchNumber variable for later use

            NClus =           bunchnumber_ClusX_map[crossing].size();
            ClusX =           bunchnumber_ClusX_map[crossing];
            ClusY =           bunchnumber_ClusY_map[crossing];
            ClusZ =           bunchnumber_ClusZ_map[crossing];
            ClusLayer =       bunchnumber_ClusLayer_map[crossing];
            ClusLadderZId =   bunchnumber_ClusLadderZId_map[crossing];
            ClusLadderPhiId = bunchnumber_ClusLadderPhiId_map[crossing];
            ClusAdc =         bunchnumber_ClusAdc_map[crossing];
            ClusPhiSize =     bunchnumber_ClusPhiSize_map[crossing];

            INTTvtxZ =        bunchnumber_tracker_vtxZ[crossing].at(0);

            nTrack_vtx =      bunchnumber_nTrack_vtx[crossing].at(0);

            tree_out -> Fill();
        }
    }

    file_out -> cd();
    tree_out -> Write();
    h1D_nVTXperCrossing -> Write();
    file_out -> Close();

    return 888;
}