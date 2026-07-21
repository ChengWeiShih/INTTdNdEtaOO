#include "ClusBased.h"

ClusBased::ClusBased(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,
    std::pair<double, double> vertexXYIncm_in,

    std::pair<bool, TH1D*> vtxZReweight_in,
    bool BcoFullDiffCut_in,
    bool INTT_vtxZ_QA_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in,
    bool HaveGeoOffsetTag_in,
    std::pair<bool, int> SetRandomHits_in,
    bool RandInttZ_in,
    bool ColMulMask_in,
    int c_type_in
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    vertexXYIncm(vertexXYIncm_in),
    vtxZReweight(vtxZReweight_in),
    BcoFullDiffCut(BcoFullDiffCut_in),
    INTT_vtxZ_QA(INTT_vtxZ_QA_in),
    isClusQA(isClusQA_in),
    HaveGeoOffsetTag(HaveGeoOffsetTag_in),
    SetRandomHits(SetRandomHits_in),
    RandInttZ(RandInttZ_in),
    ColMulMask(ColMulMask_in),
    c_type(c_type_in)
{
    PrepareInputRootFile();

    h1D_INTT_vtxZ_reweighting = (vtxZReweight.first) ? (TH1D*)vtxZReweight.second->Clone() : nullptr;

    nCentrality_bin = centrality_edges.size() - 1;

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;

    if (HaveGeoOffsetTag == false){
        for (int layer_i = B0L0_index; layer_i <= B1L1_index; layer_i++){
            double N_layer_ladder = (layer_i == B0L0_index || layer_i == B0L1_index) ? nLadder_inner : nLadder_outer;
            for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++){
                geo_offset_map[Form("%i_%i", layer_i, phi_i)] = {0, 0, 0};
            }
        }
    }

    if (RandInttZ && INTT_vtxZ_QA){
        std::cout << "RandInttZ and INTT_vtxZ_QA cannot be true at the same time" << std::endl;
        exit(1);
    }

    if (SetRandomHits.first || RandInttZ){
        rand3 = new TRandom3(0);
    }
}

void ClusBased::SetGeoOffset(std::map<std::string, std::vector<double>> input_geo_offset_map)
{
    geo_offset_map.clear();
    geo_offset_map = input_geo_offset_map;

    for (auto &pair : geo_offset_map)
    {
        std::cout<<pair.first<<" : {"<<pair.second[0]<<", "<<pair.second[1]<<", "<<pair.second[2]<<"}"<<std::endl;
    }
}

std::map<std::string, int> ClusBased::GetInputTreeBranchesMap(TTree * m_tree_in)
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

void ClusBased::PrepareInputRootFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    if (!file_in || file_in -> IsZombie() || file_in == nullptr) {
        std::cout << "Error: cannot open file: " << input_file_name << std::endl;
        exit(1);
    }

    tree_in = (TTree*)file_in -> Get("EventTree");

    std::map<std::string, int> branch_map = GetInputTreeBranchesMap(tree_in);
    if(branch_map.find("event") != branch_map.end()){tree_in -> SetBranchStatus("event",0);}

    if(branch_map.find("ClusEta_MBDz") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_MBDz",0);}
    if(branch_map.find("ClusPhi_AvgPV") != branch_map.end()){tree_in -> SetBranchStatus("ClusPhi_AvgPV",0);}
    if(branch_map.find("ClusEta_TrueXYZ") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_TrueXYZ",0);}
    if(branch_map.find("ClusPhi_TrueXY") != branch_map.end()){tree_in -> SetBranchStatus("ClusPhi_TrueXY",0);}

    if(branch_map.find("INTT_BCO") != branch_map.end()){tree_in -> SetBranchStatus("INTT_BCO",0);}
    if(branch_map.find("ClusTrkrHitSetKey") != branch_map.end()){tree_in -> SetBranchStatus("ClusTrkrHitSetKey",0);}
    if(branch_map.find("ClusTimeBucketId") != branch_map.end()){tree_in -> SetBranchStatus("ClusTimeBucketId",0);}

    if(branch_map.find("GL1Packet_BCO") != branch_map.end()){tree_in -> SetBranchStatus("GL1Packet_BCO",0);}
    if(branch_map.find("ncoll") != branch_map.end()){tree_in -> SetBranchStatus("ncoll",0);}
    if(branch_map.find("npart") != branch_map.end()){tree_in -> SetBranchStatus("npart",0);}
    if(branch_map.find("centrality_bimp") != branch_map.end()){tree_in -> SetBranchStatus("centrality_bimp",0);}
    if(branch_map.find("centrality_impactparam") != branch_map.end()){tree_in -> SetBranchStatus("centrality_impactparam",0);}
    if(branch_map.find("clk") != branch_map.end()){tree_in -> SetBranchStatus("clk",0);}
    if(branch_map.find("femclk") != branch_map.end()){tree_in -> SetBranchStatus("femclk",0);}
    if(branch_map.find("is_min_bias_wozdc") != branch_map.end()){tree_in -> SetBranchStatus("is_min_bias_wozdc",0);}
    if(branch_map.find("MBD_south_npmt") != branch_map.end()){tree_in -> SetBranchStatus("MBD_south_npmt",0);}
    if(branch_map.find("MBD_north_npmt") != branch_map.end()){tree_in -> SetBranchStatus("MBD_north_npmt",0);}
    if(branch_map.find("MBD_nhitsoverths_south") != branch_map.end()){tree_in -> SetBranchStatus("MBD_nhitsoverths_south",0);}
    if(branch_map.find("MBD_nhitsoverths_north") != branch_map.end()){tree_in -> SetBranchStatus("MBD_nhitsoverths_north",0);}

    if(branch_map.find("TrackletPair") != branch_map.end()) {tree_in -> SetBranchStatus("TrackletPair", 0);}
    if(branch_map.find("TrackletPairRotate") != branch_map.end()) {tree_in -> SetBranchStatus("TrackletPairRotate", 0);}

    ClusX = 0;
    ClusY = 0;
    ClusZ = 0;
    ClusLocalX = 0;
    ClusLocalY = 0;
    ClusLayer = 0;
    ClusLadderZId = 0;
    ClusLadderPhiId = 0;
    ClusAdc = 0;
    ClusPhiSize = 0;
    ClusEta_INTTz = 0;
    firedTriggers = 0;

    PrimaryG4P_Pt = 0;
    PrimaryG4P_Eta = 0;
    PrimaryG4P_Phi = 0;
    PrimaryG4P_E = 0;
    PrimaryG4P_PID = 0;
    PrimaryG4P_isChargeHadron = 0;

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress((Constants::is_min_bias_name).c_str(), &is_min_bias);
    tree_in -> SetBranchAddress((Constants::MBD_centrality_name).c_str(), &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);
    if (runnumber != -1) {
        if(branch_map.find("InttBcoFullDiff_next") != branch_map.end()){
            tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);
        }
    }

    if (runnumber != -1){
        if(branch_map.find("MBDNSg1") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg1", &MBDNSg1);}
        if(branch_map.find("MBDNSg1_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg1_vtxZ10cm", &MBDNSg1_vtxZ10cm);}
        if(branch_map.find("MBDNSg1_vtxZ13cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg1_vtxZ13cm", &MBDNSg1_vtxZ13cm);}
        if(branch_map.find("MBDNSg1_vtxZ150cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg1_vtxZ150cm", &MBDNSg1_vtxZ150cm);}
        if(branch_map.find("BunchNumber") != branch_map.end()) {tree_in -> SetBranchAddress("BunchNumber", &BunchNumber);}
    }

    tree_in -> SetBranchAddress("ClusX", &ClusX);
    tree_in -> SetBranchAddress("ClusY", &ClusY);
    tree_in -> SetBranchAddress("ClusZ", &ClusZ);
    tree_in -> SetBranchAddress("ClusLocalX", &ClusLocalX);
    tree_in -> SetBranchAddress("ClusLocalY", &ClusLocalY);
    tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    tree_in -> SetBranchAddress("ClusLadderZId", &ClusLadderZId);
    tree_in -> SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);
    tree_in -> SetBranchAddress("ClusAdc", &ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    if(branch_map.find("ClusEta_INTTz") != branch_map.end()) {tree_in -> SetBranchAddress("ClusEta_INTTz", &ClusEta_INTTz);}

    if(branch_map.find("Crossing") != branch_map.end()) {tree_in -> SetBranchAddress("Crossing", &crossing);}
    if(branch_map.find("FPHXBCO") != branch_map.end()) {tree_in -> SetBranchAddress("FPHXBCO", &INTTClus_FPHXBCO);}
    if (branch_map.find("firedTriggers") != branch_map.end()) {tree_in ->SetBranchAddress("firedTriggers", &firedTriggers);}

    if(branch_map.find("INTTvtxZ") != branch_map.end()) {tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);}
    if(branch_map.find("INTTvtxZError") != branch_map.end()) {tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);}
    if(branch_map.find("NgroupTrapezoidal") != branch_map.end()) {tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);}
    if(branch_map.find("NgroupCoarse") != branch_map.end()) {tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);}
    if(branch_map.find("TrapezoidalFitWidth") != branch_map.end()) {tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);}
    if(branch_map.find("TrapezoidalFWHM") != branch_map.end()) {tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);}

    if (runnumber == -1){
        tree_in -> SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        tree_in -> SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);
        tree_in -> SetBranchAddress("NPrimaryG4P", &NPrimaryG4P);
        tree_in -> SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
        tree_in -> SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
        tree_in -> SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
        tree_in -> SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);
        tree_in -> SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);
        tree_in -> SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron);
    }
}

void ClusBased::PrepareOutPutFileName()
{
}

void ClusBased::PrepareOutPutRootFile()
{
}

void ClusBased::PrepareHistograms()
{
}

void ClusBased::EvtCleanUp()
{
    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();
}

void ClusBased::PrepareClusterVec()
{
    if (geo_offset_map.size() == 0){
        std::cout<<"the set Geo Offset is set but no input"<<std::endl;
        exit(1);
    }

    for (int clu_i = 0; clu_i < ClusX -> size(); clu_i++)
    {
        ClusBased::clu_info this_clu;

        this_clu.adc = ClusAdc -> at(clu_i);
        this_clu.phi_size = ClusPhiSize -> at(clu_i);
        this_clu.sensorZID = ClusLadderZId -> at(clu_i);
        this_clu.ladderPhiID = ClusLadderPhiId -> at(clu_i);
        this_clu.layerID = ClusLayer -> at(clu_i);

        this_clu.index = clu_i;

        this_clu.x = ClusX -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][0];
        this_clu.y = ClusY -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][1];
        this_clu.z = ClusZ -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][2];
        this_clu.eta_INTTz = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {this_clu.x, this_clu.y, this_clu.z});

        this_clu.local_x = ClusLocalX -> at(clu_i);
        this_clu.local_y = ClusLocalY -> at(clu_i);
        this_clu.columnZID = GetGlobalZIndex(ClusLadderZId -> at(clu_i), ClusLocalY -> at(clu_i));

        if (ColMulMask){
            int GoodColMap_ZId = h1D_GoodColMap_ZId -> Fill(this_clu.columnZID);
            if (GoodColMap_ZId == -1) {continue;}

            int GoodColMap_XId = (this_clu.layerID - 3) * 20 + this_clu.ladderPhiID + 1;

            if (
                h2D_GoodColMap != nullptr &&
                h2D_GoodColMap -> GetBinContent(GoodColMap_XId, GoodColMap_ZId) == 0
            )
            {
                continue;
            }
        }

        if (isClusQA.first && this_clu.adc <= isClusQA.second.first) {continue;}
        if (isClusQA.first && this_clu.phi_size > isClusQA.second.second) {continue;}

        std::vector<ClusBased::clu_info>* p_evt_sPH_nocolumn_vec =
        (this_clu.layerID == 3 || this_clu.layerID == 4) ? (&evt_sPH_inner_nocolumn_vec) : (&evt_sPH_outer_nocolumn_vec);
        p_evt_sPH_nocolumn_vec -> push_back(this_clu);
    }

    if (SetRandomHits.first){
        if (inner_UniqueClusXYZ_map.size() == 0 || outer_UniqueClusXYZ_map.size() == 0){
            std::cout<<"the Unique Clus XYZ is not set for generating the random hits"<<std::endl;
            exit(1);
        }

        for (int clu_i = 0; clu_i < SetRandomHits.second; clu_i++)
        {
            ClusBased::clu_info this_clu;

            int inner_or_outer = (rand() % 2 == 0) ? 0 : 1;

            std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>>* p_UniqueClusXYZ_map = (inner_or_outer == 0) ? (&inner_UniqueClusXYZ_map) : (&outer_UniqueClusXYZ_map);
            std::vector<std::string>* p_UniqueClusXYZ_vec = (inner_or_outer == 0) ? (&inner_UniqueClusXYZ_vec) : (&outer_UniqueClusXYZ_vec);
            int Rand_index = int(rand3 -> Uniform(0, p_UniqueClusXYZ_vec -> size()));

            double selected_x = std::get<0>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            double selected_y = std::get<1>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            double selected_z = std::get<2>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_sensorZID = std::get<3>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_layerID = std::get<4>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_adc = std::get<5>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_phi_size = std::get<6>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_ladderPhiID = std::get<7>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));

            this_clu.x = selected_x;
            this_clu.y = selected_y;
            this_clu.z = selected_z;
            this_clu.eta_INTTz = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {selected_x, selected_y, selected_z});
            this_clu.sensorZID = selected_sensorZID;
            this_clu.layerID = selected_layerID;
            this_clu.adc = selected_adc;
            this_clu.phi_size = selected_phi_size;
            this_clu.ladderPhiID = selected_ladderPhiID;

            this_clu.index = ClusX -> size() + clu_i;

            std::vector<ClusBased::clu_info>* p_evt_sPH_nocolumn_vec =
            (inner_or_outer == 0) ? (&evt_sPH_inner_nocolumn_vec) : (&evt_sPH_outer_nocolumn_vec);
            p_evt_sPH_nocolumn_vec -> push_back(this_clu);
        }
    }
}

void ClusBased::PrepareUniqueClusXYZ()
{
    std::cout<<"In preparaing the unique cluster XYZ map"<<std::endl;

    inner_UniqueClusXYZ_map.clear();
    outer_UniqueClusXYZ_map.clear();
    inner_UniqueClusXYZ_vec.clear();
    outer_UniqueClusXYZ_vec.clear();

    h2D_RandClusXY_ref = new TH2D("h2D_RandClusXY_ref","h2D_RandClusXY_ref;ClusX [cm];ClusY [cm]",200,-15,15,200,-15,15);
    h1D_RandClusZ_ref = new TH1D("h1D_RandClusZ_ref","h1D_RandClusZ_ref;ClusZ [cm];Entries",100,-30,30);

    if (geo_offset_map.size() == 0){
        std::cout<<"the set Geo Offset is set but no input"<<std::endl;
        exit(1);
    }

    for (int i = 0; i < 500; i++)
    {
        tree_in -> GetEntry(i);

        for (int clu_i = 0; clu_i < ClusZ -> size(); clu_i++){
            if (ClusPhiSize -> at(clu_i) > 1) {continue;}

            double clu_x = ClusX -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][0];
            double clu_y = ClusY -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][1];
            double clu_z = ClusZ -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][2];

            std::string clu_key = Form("%.2f_%.2f_%d", clu_x, clu_y, h1D_RandClusZ_ref->FindBin(clu_z));

            std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>>* p_UniqueClusXYZ_map = (ClusLayer -> at(clu_i) == 3 || ClusLayer -> at(clu_i) == 4) ? (&inner_UniqueClusXYZ_map) : (&outer_UniqueClusXYZ_map);
            std::vector<std::string>* p_UniqueClusXYZ_vec = (ClusLayer -> at(clu_i) == 3 || ClusLayer -> at(clu_i) == 4) ? (&inner_UniqueClusXYZ_vec) : (&outer_UniqueClusXYZ_vec);

            if (ColMulMask){
                int GoodColMap_ZId = h1D_GoodColMap_ZId -> Fill(GetGlobalZIndex(ClusLadderZId -> at(clu_i), ClusLocalY -> at(clu_i)));
                if (GoodColMap_ZId == -1) {continue;}

                int GoodColMap_XId = (ClusLayer->at(clu_i) - 3) * 20 + ClusLadderPhiId->at(clu_i) + 1;

                if (
                    h2D_GoodColMap != nullptr &&
                    h2D_GoodColMap -> GetBinContent(GoodColMap_XId, GoodColMap_ZId) == 0
                )
                {
                    continue;
                }
            }

            if (p_UniqueClusXYZ_map -> find(clu_key) == p_UniqueClusXYZ_map -> end()){
                p_UniqueClusXYZ_map -> insert(
                    std::make_pair(
                        clu_key,
                        std::make_tuple(
                            clu_x, clu_y, clu_z,
                            ClusLadderZId->at(clu_i),
                            ClusLayer -> at(clu_i),
                            ClusAdc -> at(clu_i),
                            ClusPhiSize -> at(clu_i),
                            ClusLadderPhiId -> at(clu_i)
                        )
                    )
                );

                p_UniqueClusXYZ_vec -> push_back(clu_key);

                h2D_RandClusXY_ref -> Fill(clu_x, clu_y);
                h1D_RandClusZ_ref -> Fill(clu_z);
            }
        }
    }
}

double ClusBased::CheckGeoOffsetMap()
{
    double sum = 0;
    for (auto &pair : geo_offset_map)
    {
        sum += fabs(pair.second[0]) + fabs(pair.second[1]) + fabs(pair.second[2]);
    }
    return sum;
}

void ClusBased::MainProcess()
{
}

double ClusBased::get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos)
{
    double correct_x = clu_pos[0] - vertex[0];
    double correct_y = clu_pos[1] - vertex[1];
    double correct_z = clu_pos[2] - vertex[2];
    double clu_r = sqrt(pow(correct_x,2) + pow(correct_y,2));

    return -0.5 * TMath::Log((sqrt(pow(correct_z,2)+pow(clu_r,2))-(correct_z)) / (sqrt(pow(correct_z,2)+pow(clu_r,2))+(correct_z)));
}

void ClusBased::EndRun()
{
}

int ClusBased::GetGlobalZIndex(int ClusLadderZId_in, float ClusLocalZ_in) {
    const float eps = 0.01;
    int local_idx = -1;
    int global_offset = 0;

    if (ClusLadderZId_in == 1 || ClusLadderZId_in == 3) {
        std::vector<float> bins = {-4.0, -2.0, 0.0, 2.0, 4.0};
        for (int i = 0; i < (int)bins.size(); ++i) {
            if (std::fabs(ClusLocalZ_in - bins[i]) < eps) {
                local_idx = i;
                break;
            }
        }
        global_offset = (ClusLadderZId_in == 1) ? 0 : 21;
    }
    else if (ClusLadderZId_in == 0 || ClusLadderZId_in == 2) {
        std::vector<float> bins = {-5.6, -4.0, -2.4, -0.8, 0.8, 2.4, 4.0, 5.6};
        for (int i = 0; i < (int)bins.size(); ++i) {
            if (std::fabs(ClusLocalZ_in - bins[i]) < eps) {
                local_idx = i;
                break;
            }
        }
        global_offset = (ClusLadderZId_in == 0) ? 5 : 13;
    }

    if (local_idx == -1) return -1;
    return global_offset + local_idx;
}

std::string ClusBased::DoubleToPString(double val, int precision) {
    bool isNegative = (val < 0);
    double absVal = std::abs(val);

    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << absVal;
    std::string s = out.str();

    std::replace(s.begin(), s.end(), '.', 'p');

    if (isNegative) {
        s = "N" + s;
    }

    return s;
}
