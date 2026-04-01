#include "EvtVtxZProtoTracklet_MultiVtx.h"

EvtVtxZProtoTracklet_MultiVtx::EvtVtxZProtoTracklet_MultiVtx(
    int process_id_in, int runnumber_in, int run_nEvents_in,
    std::string input_directory_in, std::string input_file_name_in,
    std::string output_directory_in, std::string output_file_name_suffix_in,
    std::pair<double, double> vertexXYIncm_in,
    bool IsFieldOn_in, bool IsDCACutApplied_in,
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree_in,
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm_in,
    int ClusAdcCut_in, int ClusPhiSizeCut_in,
    bool PrintRecoDetails_in, bool DrawEvtVtxZ_in,
    bool RunInttBcoFullDiff_in, bool RunVtxZReco_in,
    bool RunTrackletPair_in, bool RunTrackletPairRotate_in,
    bool HaveGeoOffsetTag_in
):
    process_id(process_id_in), runnumber(runnumber_in), run_nEvents(run_nEvents_in),
    input_directory(input_directory_in), input_file_name(input_file_name_in),
    output_directory(output_directory_in), output_file_name_suffix(output_file_name_suffix_in),
    vertexXYIncm(vertexXYIncm_in), IsFieldOn(IsFieldOn_in), IsDCACutApplied(IsDCACutApplied_in),
    DeltaPhiCutInDegree(DeltaPhiCutInDegree_in), DCAcutIncm(DCAcutIncm_in),
    ClusAdcCut(ClusAdcCut_in), ClusPhiSizeCut(ClusPhiSizeCut_in),
    PrintRecoDetails(PrintRecoDetails_in), DrawEvtVtxZ(DrawEvtVtxZ_in),
    RunInttBcoFullDiff(RunInttBcoFullDiff_in), RunVtxZReco(RunVtxZReco_in),
    RunTrackletPair(RunTrackletPair_in), RunTrackletPairRotate(RunTrackletPairRotate_in),
    HaveGeoOffsetTag(HaveGeoOffsetTag_in)
{
    PrepareOutPutFileName();

    for (int layer_i = B0L0_index; layer_i <= B1L1_index; layer_i++){
        double N_layer_ladder = (layer_i == B0L0_index || layer_i == B0L1_index) ? nLadder_inner : nLadder_outer;
        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++){
            geo_offset_map[Form("%i_%i", layer_i, phi_i)] = {0, 0, 0};
        }
    }

    PrepareRootFile();
    PrepareINTTvtxZ();

    track_gr = new TGraphErrors();
    fit_rz = new TF1("fit_rz","pol1",-1000,1000);

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;

    out_eID_count = 0;

    if (DrawEvtVtxZ){ PrepareEvtCanvas(); }
}

void EvtVtxZProtoTracklet_MultiVtx::PrepareOutPutFileName()
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

    output_filename = "EvtVtxZProtoTracklet_MultiVtx";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (IsFieldOn) ? "_FieldOn" : "_FieldOff";
    output_filename += (RunInttBcoFullDiff && runnumber != -1) ? "_BcoFullDiff" : "";
    output_filename += (RunVtxZReco) ? "_VtxZReco" : "";
    output_filename += (RunTrackletPair) ? "_Tracklet" : "";
    output_filename += (RunTrackletPairRotate) ? "_TrackletRotate" : "";
    output_filename += (HaveGeoOffsetTag) ? "_GeoOffset" : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

void EvtVtxZProtoTracklet_MultiVtx::SetGeoOffset(std::map<std::string, std::vector<double>> input_geo_offset_map)
{
    std::cout<<"In EvtVtxZProtoTracklet_MultiVtx, reading geo offset map:"<<std::endl;
    geo_offset_map.clear();
    geo_offset_map = input_geo_offset_map;
    for (auto &pair : geo_offset_map){
        std::cout<<pair.first<<" : {"<<pair.second[0]<<", "<<pair.second[1]<<", "<<pair.second[2]<<"}"<<std::endl;
    }
}

double EvtVtxZProtoTracklet_MultiVtx::CheckGeoOffsetMap()
{
    double sum = 0;
    for (auto &pair : geo_offset_map){ sum += fabs(pair.second[0]) + fabs(pair.second[1]) + fabs(pair.second[2]); }
    return sum;
}

std::map<std::string, int> EvtVtxZProtoTracklet_MultiVtx::GetInputTreeBranches(TTree * m_tree_in)
{
    std::map<std::string, int> branch_map;
    TObjArray * branch_list = m_tree_in -> GetListOfBranches();
    for (int i = 0; i < branch_list -> GetEntries(); i++){
        TBranch * branch = dynamic_cast<TBranch*>(branch_list->At(i));
        branch_map[branch -> GetName()] = 1;
    }
    return branch_map;
}

void EvtVtxZProtoTracklet_MultiVtx::PrepareRootFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    if (!file_in || file_in -> IsZombie() || file_in == nullptr) {
        std::cout << "Error: cannot open file: " << input_file_name << std::endl;
        exit(1);
    }

    tree_in = (TTree*)file_in -> Get("EventTree");
    std::map<std::string, int> branch_map = GetInputTreeBranches(tree_in);

    if(branch_map.find("TrkrHitRow") != branch_map.end())          {tree_in -> SetBranchStatus("TrkrHitRow", 0);}
    if(branch_map.find("TrkrHitColumn") != branch_map.end())       {tree_in -> SetBranchStatus("TrkrHitColumn", 0);}
    if(branch_map.find("TrkrHitLadderZId") != branch_map.end())    {tree_in -> SetBranchStatus("TrkrHitLadderZId", 0);}
    if(branch_map.find("TrkrHitLadderPhiId") != branch_map.end())  {tree_in -> SetBranchStatus("TrkrHitLadderPhiId", 0);}
    if(branch_map.find("TrkrHitLayer") != branch_map.end())        {tree_in -> SetBranchStatus("TrkrHitLayer", 0);}
    if(branch_map.find("TrkrHitADC") != branch_map.end())          {tree_in -> SetBranchStatus("TrkrHitADC", 0);}
    if(branch_map.find("TrkrHitX") != branch_map.end())            {tree_in -> SetBranchStatus("TrkrHitX", 0);}
    if(branch_map.find("TrkrHitY") != branch_map.end())            {tree_in -> SetBranchStatus("TrkrHitY", 0);}
    if(branch_map.find("TrkrHitZ") != branch_map.end())            {tree_in -> SetBranchStatus("TrkrHitZ", 0);}
    if(branch_map.find("TrkrHit_truthHit_x0") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_x0", 0);}
    if(branch_map.find("TrkrHit_truthHit_y0") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_y0", 0);}
    if(branch_map.find("TrkrHit_truthHit_z0") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_z0", 0);}
    if(branch_map.find("TrkrHit_truthHit_x1") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_x1", 0);}
    if(branch_map.find("TrkrHit_truthHit_y1") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_y1", 0);}
    if(branch_map.find("TrkrHit_truthHit_z1") != branch_map.end()) {tree_in -> SetBranchStatus("TrkrHit_truthHit_z1", 0);}
    if(branch_map.find("MBD_PMT_charge") != branch_map.end())      {tree_in -> SetBranchStatus("MBD_PMT_charge", 0);}
    if(branch_map.find("triggervec") != branch_map.end())                {tree_in -> SetBranchStatus("triggervec", 0);}
    if(branch_map.find("firedTriggers_name") != branch_map.end())        {tree_in -> SetBranchStatus("firedTriggers_name", 0);}
    if(branch_map.find("firedTriggers_checkraw") != branch_map.end())    {tree_in -> SetBranchStatus("firedTriggers_checkraw", 0);}
    if(branch_map.find("firedTriggers_prescale") != branch_map.end())    {tree_in -> SetBranchStatus("firedTriggers_prescale", 0);}
    if(branch_map.find("firedTriggers_scalers") != branch_map.end())     {tree_in -> SetBranchStatus("firedTriggers_scalers", 0);}
    if(branch_map.find("firedTriggers_livescalers") != branch_map.end()) {tree_in -> SetBranchStatus("firedTriggers_livescalers", 0);}
    if(branch_map.find("firedTriggers_rawscalers") != branch_map.end())  {tree_in -> SetBranchStatus("firedTriggers_rawscalers", 0);}

    if (runnumber == -1){
        if(branch_map.find("PHG4Hit_x0") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_x0", 0);}
        if(branch_map.find("PHG4Hit_y0") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_y0", 0);}
        if(branch_map.find("PHG4Hit_z0") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_z0", 0);}
        if(branch_map.find("PHG4Hit_x1") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_x1", 0);}
        if(branch_map.find("PHG4Hit_y1") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_y1", 0);}
        if(branch_map.find("PHG4Hit_z1") != branch_map.end())     {tree_in -> SetBranchStatus("PHG4Hit_z1", 0);}
        if(branch_map.find("PHG4Hit_edep") != branch_map.end())   {tree_in -> SetBranchStatus("PHG4Hit_edep", 0);}
        if(branch_map.find("HepMCFSPrtl_Pt") != branch_map.end())    {tree_in -> SetBranchStatus("HepMCFSPrtl_Pt", 0);}
        if(branch_map.find("HepMCFSPrtl_Eta") != branch_map.end())   {tree_in -> SetBranchStatus("HepMCFSPrtl_Eta", 0);}
        if(branch_map.find("HepMCFSPrtl_Phi") != branch_map.end())   {tree_in -> SetBranchStatus("HepMCFSPrtl_Phi", 0);}
        if(branch_map.find("HepMCFSPrtl_E") != branch_map.end())     {tree_in -> SetBranchStatus("HepMCFSPrtl_E", 0);}
        if(branch_map.find("HepMCFSPrtl_PID") != branch_map.end())   {tree_in -> SetBranchStatus("HepMCFSPrtl_PID", 0);}
        if(branch_map.find("HepMCFSPrtl_prodx") != branch_map.end()) {tree_in -> SetBranchStatus("HepMCFSPrtl_prodx", 0);}
        if(branch_map.find("HepMCFSPrtl_prody") != branch_map.end()) {tree_in -> SetBranchStatus("HepMCFSPrtl_prody", 0);}
        if(branch_map.find("HepMCFSPrtl_prodz") != branch_map.end()) {tree_in -> SetBranchStatus("HepMCFSPrtl_prodz", 0);}
    }

    ClusX = 0; ClusY = 0; ClusZ = 0; ClusLayer = 0;
    ClusLadderZId = 0; ClusLadderPhiId = 0; ClusAdc = 0; ClusPhiSize = 0; firedTriggers = 0;

    if(branch_map.find("firedTriggers") != branch_map.end()) {
        tree_in -> SetBranchAddress("firedTriggers", &firedTriggers);
        m_withTrig = true;
    }

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("INTT_BCO", &INTT_BCO);
    tree_in -> SetBranchAddress("ClusX", &ClusX);
    tree_in -> SetBranchAddress("ClusY", &ClusY);
    tree_in -> SetBranchAddress("ClusZ", &ClusZ);
    tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    tree_in -> SetBranchAddress("ClusLadderZId", &ClusLadderZId);
    tree_in -> SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);
    tree_in -> SetBranchAddress("ClusAdc", &ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    if (!RunVtxZReco){
        tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
        tree_in -> SetBranchAddress("INTTvtxZError",&INTTvtxZError);
    }

    if (runnumber == -1) {
        tree_in -> SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        tree_in -> SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);
    }

    file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"recreate");
    tree_out = tree_in->CloneTree(0);

    if (RunInttBcoFullDiff && runnumber != -1) {b_InttBcoFullDiff_next = tree_out -> Branch("InttBcoFullDiff_next", &out_InttBcoFullDiff_next);}

    if (RunVtxZReco){
        b_INTTvtxZ             = tree_out -> Branch("INTTvtxZ",           &out_INTTvtxZ);
        b_INTTvtxZError        = tree_out -> Branch("INTTvtxZError",      &out_INTTvtxZError);
        b_NgroupTrapezoidal    = tree_out -> Branch("NgroupTrapezoidal",   &out_NgroupTrapezoidal);
        b_NgroupCoarse         = tree_out -> Branch("NgroupCoarse",        &out_NgroupCoarse);
        b_TrapezoidalFitWidth  = tree_out -> Branch("TrapezoidalFitWidth", &out_TrapezoidalFitWidth);
        b_TrapezoidalFWHM      = tree_out -> Branch("TrapezoidalFWHM",    &out_TrapezoidalFWHM);
        b_FitHeight            = tree_out -> Branch("FitHeight",              &out_FitHeight);

        b_ClusEta_INTTz        = tree_out -> Branch("ClusEta_INTTz",  &out_ClusEta_INTTz);
        b_ClusEta_MBDz         = tree_out -> Branch("ClusEta_MBDz",   &out_ClusEta_MBDz);
        b_ClusPhi_AvgPV        = tree_out -> Branch("ClusPhi_AvgPV",  &out_ClusPhi_AvgPV);

        if (runnumber == -1){
            b_ClusEta_TrueXYZ  = tree_out -> Branch("ClusEta_TrueXYZ", &out_ClusEta_TrueXYZ);
            b_ClusPhi_TrueXY   = tree_out -> Branch("ClusPhi_TrueXY",  &out_ClusPhi_TrueXY);
        }

        if (m_withTrig){
            b_MBDNSg2          = tree_out -> Branch("MBDNSg2",          &out_MBDNSg2);
            b_MBDNSg2_vtxZ10cm = tree_out -> Branch("MBDNSg2_vtxZ10cm",&out_MBDNSg2_vtxZ10cm);
            b_MBDNSg2_vtxZ30cm = tree_out -> Branch("MBDNSg2_vtxZ30cm",&out_MBDNSg2_vtxZ30cm);
            b_MBDNSg2_vtxZ60cm = tree_out -> Branch("MBDNSg2_vtxZ60cm",&out_MBDNSg2_vtxZ60cm);
        }

        b_eID_count            = tree_out -> Branch("eID_count", &out_eID_count);

        // note : ---- Multi-vertex vector branches ----
        b_vec_INTTvtxZ            = tree_out -> Branch("vec_INTTvtxZ",           &out_vec_INTTvtxZ);
        b_vec_INTTvtxZError       = tree_out -> Branch("vec_INTTvtxZError",      &out_vec_INTTvtxZError);
        b_vec_NgroupTrapezoidal   = tree_out -> Branch("vec_NgroupTrapezoidal",  &out_vec_NgroupTrapezoidal);
        b_vec_NgroupCoarse        = tree_out -> Branch("vec_NgroupCoarse",       &out_vec_NgroupCoarse);
        b_vec_TrapezoidalFitWidth = tree_out -> Branch("vec_TrapezoidalFitWidth",&out_vec_TrapezoidalFitWidth);
        b_vec_TrapezoidalFWHM    = tree_out -> Branch("vec_TrapezoidalFWHM",    &out_vec_TrapezoidalFWHM);
        b_vec_FitHeight           = tree_out -> Branch("vec_FitHeight",          &out_vec_FitHeight);
        b_N_INTT_VtxZ             = tree_out -> Branch("N_INTT_VtxZ",           &out_N_INTT_VtxZ);
    }

    if (RunTrackletPair)       {b_evt_TrackletPair_vec       = tree_out -> Branch("TrackletPair",       &out_evt_TrackletPair_vec);}
    if (RunTrackletPairRotate) {b_evt_TrackletPairRotate_vec = tree_out -> Branch("TrackletPairRotate", &out_evt_TrackletPairRotate_vec);}
}

void EvtVtxZProtoTracklet_MultiVtx::GetTriggerInfo()
{
    std::vector<int> firedTriggers_vec = *(firedTriggers);
    std::map<int, int> firedTriggers_map; firedTriggers_map.clear();
    for (int trig : firedTriggers_vec){ firedTriggers_map[trig] = 1; }
    out_MBDNSg2          = (firedTriggers_map.find(index_MBDNSg2)          != firedTriggers_map.end()) ? 1 : 0;
    out_MBDNSg2_vtxZ10cm = (firedTriggers_map.find(index_MBDNSg2_vtxZ10cm) != firedTriggers_map.end()) ? 1 : 0;
    out_MBDNSg2_vtxZ30cm = (firedTriggers_map.find(index_MBDNSg2_vtxZ30cm) != firedTriggers_map.end()) ? 1 : 0;
    out_MBDNSg2_vtxZ60cm = (firedTriggers_map.find(index_MBDNSg2_vtxZ60cm) != firedTriggers_map.end()) ? 1 : 0;
}

void EvtVtxZProtoTracklet_MultiVtx::PrepareINTTvtxZ()
{
    evt_possible_z = new TH1D("","evt_possible_z",50, evt_possible_z_range.first, evt_possible_z_range.second);
    evt_possible_z -> SetLineWidth(1);
    evt_possible_z -> GetXaxis() -> SetTitle("Z [cm]");
    evt_possible_z -> GetYaxis() -> SetTitle("Entries");

    line_breakdown_hist = new TH1D("line_breakdown_hist","line_breakdown_hist",
        2*zvtx_hist_Nbins+1,
        -1*(fine_bin_width*zvtx_hist_Nbins + fine_bin_width/2.),
         fine_bin_width*zvtx_hist_Nbins + fine_bin_width/2.);
    line_breakdown_hist -> SetLineWidth(1);
    line_breakdown_hist -> GetXaxis() -> SetTitle("Z [cm]");
    line_breakdown_hist -> GetYaxis() -> SetTitle("Entries [A.U.]");
    line_breakdown_hist -> GetXaxis() -> SetNdivisions(705);

    combination_zvtx_range_shape = new TH1D("","",
        line_breakdown_hist -> GetNbinsX(),
        line_breakdown_hist -> GetXaxis() -> GetXmin(),
        line_breakdown_hist -> GetXaxis() -> GetXmax());

    gaus_fit = new TF1("gaus_fit",gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetLineWidth(1);
    gaus_fit -> SetNpx(1000);

    gaus_fit_vec.clear();
    for (int i = 0; i < number_of_gaus; i++){
        gaus_fit_vec.push_back(new TF1(Form("gaus_fit_vec_%i",i),gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4));
        gaus_fit_vec[i] -> SetLineColor(ROOT_color_code[i]);
        gaus_fit_vec[i] -> SetLineWidth(1);
        gaus_fit_vec[i] -> SetNpx(1000);
    }

    reco_INTTvtxZ = new TH1D("reco_INTTvtxZ","reco_INTTvtxZ;INTT vtxZ [cm];Entries",nVtxZ_bin,vtxZ_range.first,vtxZ_range.second);
}

void EvtVtxZProtoTracklet_MultiVtx::PrepareClusterVec()
{
    for (int clu_i = 0; clu_i < (int)ClusX -> size(); clu_i++){
        EvtVtxZProtoTracklet_MultiVtx::clu_info this_clu;
        this_clu.adc       = ClusAdc     -> at(clu_i);
        this_clu.phi_size  = ClusPhiSize -> at(clu_i);
        this_clu.sensorZID = ClusLadderZId  -> at(clu_i);
        this_clu.ladderPhiID = ClusLadderPhiId -> at(clu_i);
        this_clu.layerID   = ClusLayer   -> at(clu_i);
        this_clu.index     = clu_i;
        this_clu.x = ClusX -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][0];
        this_clu.y = ClusY -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][1];
        this_clu.z = ClusZ -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][2];

        std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info>* p_evt_sPH_nocolumn_vec =
            (this_clu.layerID == 3 || this_clu.layerID == 4) ? (&evt_sPH_inner_nocolumn_vec) : (&evt_sPH_outer_nocolumn_vec);
        p_evt_sPH_nocolumn_vec -> push_back(this_clu);

        if (this_clu.adc <= ClusAdcCut || this_clu.phi_size > ClusPhiSizeCut) {continue;}

        std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info>* p_evt_sPH_nocolumn_vec_PostCut =
            (this_clu.layerID == 3 || this_clu.layerID == 4) ? (&evt_sPH_inner_nocolumn_vec_PostCut) : (&evt_sPH_outer_nocolumn_vec_PostCut);
        p_evt_sPH_nocolumn_vec_PostCut -> push_back(this_clu);
    }
}

void EvtVtxZProtoTracklet_MultiVtx::GetINTTvtxZ()
{
    inner_clu_phi_map_PostCut.clear();
    outer_clu_phi_map_PostCut.clear();
    inner_clu_phi_map_PostCut = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);
    outer_clu_phi_map_PostCut = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);

    for (int inner_i = 0; inner_i < (int)evt_sPH_inner_nocolumn_vec_PostCut.size(); inner_i++) {
        double phi = (evt_sPH_inner_nocolumn_vec_PostCut[inner_i].y - vertexXYIncm.second < 0)
            ? atan2(evt_sPH_inner_nocolumn_vec_PostCut[inner_i].y - vertexXYIncm.second, evt_sPH_inner_nocolumn_vec_PostCut[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360
            : atan2(evt_sPH_inner_nocolumn_vec_PostCut[inner_i].y - vertexXYIncm.second, evt_sPH_inner_nocolumn_vec_PostCut[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi());
        inner_clu_phi_map_PostCut[ int(phi) ].push_back({false,evt_sPH_inner_nocolumn_vec_PostCut[inner_i]});
    }
    for (int outer_i = 0; outer_i < (int)evt_sPH_outer_nocolumn_vec_PostCut.size(); outer_i++) {
        double phi = (evt_sPH_outer_nocolumn_vec_PostCut[outer_i].y - vertexXYIncm.second < 0)
            ? atan2(evt_sPH_outer_nocolumn_vec_PostCut[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec_PostCut[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360
            : atan2(evt_sPH_outer_nocolumn_vec_PostCut[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec_PostCut[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi());
        outer_clu_phi_map_PostCut[ int(phi) ].push_back({false,evt_sPH_outer_nocolumn_vec_PostCut[outer_i]});
    }

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++){
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < (int)inner_clu_phi_map_PostCut[inner_phi_i].size(); inner_phi_clu_i++){
            if (inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

            double Clus_InnerPhi = (inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second < 0)
                ? atan2(inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi()) + 360
                : atan2(inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi());

            for (int scan_i = -5; scan_i < 6; scan_i++){
                int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

                for (int outer_phi_clu_i = 0; outer_phi_clu_i < (int)outer_clu_phi_map_PostCut[true_scan_i].size(); outer_phi_clu_i++){
                    if (outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                    double Clus_OuterPhi = (outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second < 0)
                        ? atan2(outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi()) + 360
                        : atan2(outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi());
                    double delta_phi = get_delta_phi(Clus_InnerPhi, Clus_OuterPhi);

                    if (!IsFieldOn){ if (delta_phi <= DeltaPhiCutInDegree.first.first || delta_phi >= DeltaPhiCutInDegree.first.second) {continue;} }
                    else           { if (delta_phi <= DeltaPhiCutInDegree.first.first || delta_phi >= DeltaPhiCutInDegree.first.second) {continue;} }

                    double DCA_sign = calculateAngleBetweenVectors(
                        outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y,
                        inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y,
                        vertexXYIncm.first, vertexXYIncm.second);

                    if (IsDCACutApplied){
                        if (!IsFieldOn){ if (DCA_sign <= DCAcutIncm.first.first || DCA_sign >= DCAcutIncm.first.second) {continue;} }
                        else           { if (DCA_sign <= DCAcutIncm.first.first || DCA_sign >= DCAcutIncm.first.second) {continue;} }
                    }

                    std::pair<double,double> z_range_info = Get_possible_zvtx(
                        0.,
                        { get_radius(inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second),
                          inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.z,
                          double(inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.sensorZID) },
                        { get_radius(outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second),
                          outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.z,
                          double(outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.sensorZID) });

                    if (evt_possible_z_range.first < z_range_info.first && z_range_info.first < evt_possible_z_range.second) {
                        evt_possible_z -> Fill(z_range_info.first);
                        trapezoidal_line_breakdown(line_breakdown_hist,
                            get_radius(inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second),
                            inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.z, inner_clu_phi_map_PostCut[inner_phi_i][inner_phi_clu_i].second.sensorZID,
                            get_radius(outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second),
                            outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.z, outer_clu_phi_map_PostCut[true_scan_i][outer_phi_clu_i].second.sensorZID);
                    }
                }
            }
        }
    }

    for (int bin_i = 0; bin_i < line_breakdown_hist->GetNbinsX(); bin_i++){
        if (line_breakdown_hist -> GetBinCenter(bin_i+1) < edge_rejection.first || line_breakdown_hist -> GetBinCenter(bin_i+1) > edge_rejection.second){
            line_breakdown_hist -> SetBinContent(bin_i+1,0);
        }
    }

    if (line_breakdown_hist->Integral() == 0) { return; }

    std::vector<double> N_group_info        = find_Ngroup(evt_possible_z);
    std::vector<double> N_group_info_detail = find_Ngroup(line_breakdown_hist);
    double lbd_max_content   = line_breakdown_hist -> GetBinContent(line_breakdown_hist -> GetMaximumBin());
    double lbd_max_center    = line_breakdown_hist -> GetBinCenter (line_breakdown_hist -> GetMaximumBin());
    double temp_fit_region_half = 5.0;

    gaus_fit -> SetParameters(lbd_max_content, lbd_max_center, temp_fit_region_half, 0);
    gaus_fit -> SetParLimits(0,0,100000); gaus_fit -> SetParLimits(2,0.5,1000); gaus_fit -> SetParLimits(3,-200,10000);
    line_breakdown_hist -> Fit(gaus_fit, "NQ", "", lbd_max_center - temp_fit_region_half*0.65, lbd_max_center + temp_fit_region_half*0.65);

    for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++){
        gaus_fit_vec[fit_i] -> SetParameters(lbd_max_content, lbd_max_center, temp_fit_region_half, 0);
        gaus_fit_vec[fit_i] -> SetParLimits(0,0,100000); gaus_fit_vec[fit_i] -> SetParLimits(2,0.5,1000); gaus_fit_vec[fit_i] -> SetParLimits(3,-200,10000);
        double N_width_ratio = 0.2 + 0.15 * fit_i;
        double mean_l = lbd_max_center - temp_fit_region_half * N_width_ratio;
        double mean_r = lbd_max_center + temp_fit_region_half * N_width_ratio;
        line_breakdown_hist -> Fit(gaus_fit_vec[fit_i], "NQ", "", mean_l, mean_r);
        gaus_fit_vec[fit_i] -> SetRange(mean_l, mean_r);
        fit_mean_mean_vec.push_back(gaus_fit_vec[fit_i] -> GetParameter(1));
        fit_mean_reducedChi2_vec.push_back(gaus_fit_vec[fit_i] -> GetChisquare() / double(gaus_fit_vec[fit_i] -> GetNDF()));
        fit_mean_width_vec.push_back(fabs(gaus_fit_vec[fit_i] -> GetParameter(2)));
    }

    out_INTTvtxZ           = vector_average(fit_mean_mean_vec);
    out_INTTvtxZError      = vector_stddev(fit_mean_mean_vec);
    out_NgroupTrapezoidal  = N_group_info_detail[0];
    out_NgroupCoarse       = N_group_info[0];
    out_TrapezoidalFitWidth = gaus_fit -> GetParameter(2);
    out_TrapezoidalFWHM   = fabs(N_group_info_detail[3] - N_group_info_detail[2]) / 2.;
    out_FitHeight         = gaus_fit -> GetParameter(0) + gaus_fit -> GetParameter(3);

    if (DrawEvtVtxZ)  // &&
        // (out_eID_count % 40 == 0 ||
        //  (out_INTTvtxZ - MBD_z_vtx) < -4.5 || (out_INTTvtxZ - MBD_z_vtx) > 3.0 ||
        //  (out_TrapezoidalFitWidth < 0.9 && out_TrapezoidalFWHM < 6) ||
        //  (out_TrapezoidalFitWidth > 2.4 && out_TrapezoidalFitWidth < 2.55) ||
        //  (out_TrapezoidalFitWidth > 1.65 && out_TrapezoidalFitWidth < 1.8)))
    {
        line_breakdown_hist_zoomin = (TH1D*)line_breakdown_hist -> Clone("line_breakdown_hist_zoomin");
        line_breakdown_hist_zoomin -> GetXaxis() -> SetRangeUser(gaus_fit_vec.front()->GetParameter(1)-temp_fit_region_half, gaus_fit_vec.front()->GetParameter(1)+temp_fit_region_half);
        line_breakdown_hist_zoomin -> SetMinimum(gaus_fit_vec.front()->GetParameter(0)*0.5);
        line_breakdown_hist_zoomin -> SetMaximum(gaus_fit_vec.front()->GetParameter(0)*1.5);

        c1->cd(); pad_EvtZDist->Draw(); pad_EvtZDist->cd();
        line_breakdown_hist->SetMinimum(0);
        line_breakdown_hist->SetMaximum(lbd_max_content*1.65);
        line_breakdown_hist->Draw("hist");
        for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++) gaus_fit_vec[gaus_fit_vec.size()-fit_i-1]->Draw("l same");
        draw_text->DrawLatex(0.2,0.9,Form("Event ID: %i", out_eID_count));
        draw_text->DrawLatex(0.2,0.86,Form("NClusGood: %i",(int)(evt_sPH_inner_nocolumn_vec_PostCut.size()+evt_sPH_outer_nocolumn_vec_PostCut.size())));
        draw_text->DrawLatex(0.2,0.82,Form("Reco. vtx Z: %.3f cm, StdDev: %.3f cm", out_INTTvtxZ, out_INTTvtxZError));
        draw_text->DrawLatex(0.2,0.78,Form("MBD_z_vtx: %.3f cm", MBD_z_vtx));
        if (runnumber == -1){ draw_text->DrawLatex(0.2,0.74,Form("True vtx Z: %.3f cm",TruthPV_trig_z)); }
        INTTvtxZ_EvtDisplay_file_out->cd();
        c1->Write(Form("eID_%d_VtxN_0_EvtZDist", out_eID_count));

        c1->cd(); pad_ZoomIn_EvtZDist->Draw(); pad_ZoomIn_EvtZDist->cd();
        line_breakdown_hist_zoomin->Draw("hist");
        for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++) gaus_fit_vec[gaus_fit_vec.size()-fit_i-1]->Draw("l same");
        INTTvtxZ_EvtDisplay_file_out->cd();
        c1->Write(Form("eID_%d_VtxN_0_EvtZDist_ZoomIn", out_eID_count));
        pad_ZoomIn_EvtZDist->Clear(); pad_EvtZDist->Clear();
    }

    if (PrintRecoDetails && out_eID_count % 1 == 0){
        std::cout<<"eID: "<<out_eID_count<<" INTTvtxZ: "<<out_INTTvtxZ<<" INTTvtxZError: "<<out_INTTvtxZError
                 <<" NgroupTrapezoidal: "<<out_NgroupTrapezoidal<<" TrapezoidalFitWidth: "<<out_TrapezoidalFitWidth
                 <<" MBD_z: "<<MBD_z_vtx<<std::endl;
    }
}

std::pair<double,double> EvtVtxZProtoTracklet_MultiVtx::rotatePoint(double x, double y)
{
    double angleRad = rotate_phi_angle * M_PI / 180.0;
    double xOut = x*cos(angleRad) - y*sin(angleRad);
    double yOut = x*sin(angleRad) + y*cos(angleRad);
    xOut = (fabs(xOut) < 0.0000001) ? 0 : xOut;
    yOut = (fabs(yOut) < 0.0000001) ? 0 : yOut;
    return {xOut,yOut};
}

std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info> EvtVtxZProtoTracklet_MultiVtx::GetRoatedClusterVec(std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info> input_cluster_vec)
{
    std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info> output_cluster_vec;
    for (EvtVtxZProtoTracklet_MultiVtx::clu_info this_clu : input_cluster_vec){
        std::pair<double,double> rotated_xy = rotatePoint(this_clu.x, this_clu.y);
        EvtVtxZProtoTracklet_MultiVtx::clu_info rotated_clu = this_clu;
        rotated_clu.x = rotated_xy.first; rotated_clu.y = rotated_xy.second;
        output_cluster_vec.push_back(rotated_clu);
    }
    return output_cluster_vec;
}

void EvtVtxZProtoTracklet_MultiVtx::GetClusUpdated()
{
    for (int clu_i = 0; clu_i < (int)ClusX->size(); clu_i++){
        if (temp_INTTvtxZ == temp_INTTvtxZ && temp_INTTvtxZError == temp_INTTvtxZError){
            out_ClusEta_INTTz.push_back(get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, temp_INTTvtxZ}, {ClusX->at(clu_i), ClusY->at(clu_i), ClusZ->at(clu_i)}));
        }
        if (runnumber == -1){
            out_ClusEta_TrueXYZ.push_back(get_clu_eta({TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z}, {ClusX->at(clu_i), ClusY->at(clu_i), ClusZ->at(clu_i)}));
            out_ClusPhi_TrueXY.push_back(atan2(ClusY->at(clu_i)-TruthPV_trig_y, ClusX->at(clu_i)-TruthPV_trig_x));
        }
        out_ClusPhi_AvgPV.push_back(atan2(ClusY->at(clu_i)-vertexXYIncm.second, ClusX->at(clu_i)-vertexXYIncm.first));
    }
}

void EvtVtxZProtoTracklet_MultiVtx::GetTrackletPair(std::vector<pair_str> &input_TrackletPair_vec, bool isRotated)
{
    inner_clu_phi_map.clear(); outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);

    if (temp_INTTvtxZ != temp_INTTvtxZ || temp_INTTvtxZError != temp_INTTvtxZError) {return;}

    std::vector<EvtVtxZProtoTracklet_MultiVtx::clu_info> temp_inner = (isRotated) ? GetRoatedClusterVec(evt_sPH_inner_nocolumn_vec) : evt_sPH_inner_nocolumn_vec;

    for (int inner_i = 0; inner_i < (int)temp_inner.size(); inner_i++) {
        double phi = (temp_inner[inner_i].y-vertexXYIncm.second < 0)
            ? atan2(temp_inner[inner_i].y-vertexXYIncm.second, temp_inner[inner_i].x-vertexXYIncm.first)*(180./TMath::Pi())+360
            : atan2(temp_inner[inner_i].y-vertexXYIncm.second, temp_inner[inner_i].x-vertexXYIncm.first)*(180./TMath::Pi());
        inner_clu_phi_map[int(phi)].push_back({false,temp_inner[inner_i]});
    }
    for (int outer_i = 0; outer_i < (int)evt_sPH_outer_nocolumn_vec.size(); outer_i++) {
        double phi = (evt_sPH_outer_nocolumn_vec[outer_i].y-vertexXYIncm.second < 0)
            ? atan2(evt_sPH_outer_nocolumn_vec[outer_i].y-vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x-vertexXYIncm.first)*(180./TMath::Pi())+360
            : atan2(evt_sPH_outer_nocolumn_vec[outer_i].y-vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x-vertexXYIncm.first)*(180./TMath::Pi());
        outer_clu_phi_map[int(phi)].push_back({false,evt_sPH_outer_nocolumn_vec[outer_i]});
    }

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++){
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < (int)inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++){
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}
            const clu_info& ic = inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second;
            double Clus_InnerPhi_r = atan2(ic.y-vertexXYIncm.second, ic.x-vertexXYIncm.first);
            double Clus_InnerPhi   = (ic.y-vertexXYIncm.second < 0) ? Clus_InnerPhi_r*(180./TMath::Pi())+360 : Clus_InnerPhi_r*(180./TMath::Pi());

            for (int scan_i = -5; scan_i < 6; scan_i++){
                int true_scan_i = ((inner_phi_i+scan_i)<0) ? 360+(inner_phi_i+scan_i) : ((inner_phi_i+scan_i)>359) ? (inner_phi_i+scan_i)-360 : inner_phi_i+scan_i;
                for (int outer_phi_clu_i = 0; outer_phi_clu_i < (int)outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++){
                    if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}
                    const clu_info& oc = outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second;
                    double Clus_OuterPhi_r = atan2(oc.y-vertexXYIncm.second, oc.x-vertexXYIncm.first);
                    double Clus_OuterPhi   = (oc.y-vertexXYIncm.second < 0) ? Clus_OuterPhi_r*(180./TMath::Pi())+360 : Clus_OuterPhi_r*(180./TMath::Pi());
                    double delta_phi = get_delta_phi(Clus_InnerPhi, Clus_OuterPhi);
                    double delta_phi_radian = delta_phi * (TMath::Pi() / 180.);

                    double inner_clu_eta = get_clu_eta({vertexXYIncm.first,vertexXYIncm.second,temp_INTTvtxZ},{ic.x,ic.y,ic.z});
                    double outer_clu_eta = get_clu_eta({vertexXYIncm.first,vertexXYIncm.second,temp_INTTvtxZ},{oc.x,oc.y,oc.z});

                    std::pair<double,double> z_range_info = Get_possible_zvtx(0.,
                        {get_radius(ic.x-vertexXYIncm.first,ic.y-vertexXYIncm.second),ic.z,double(ic.sensorZID)},
                        {get_radius(oc.x-vertexXYIncm.first,oc.y-vertexXYIncm.second),oc.z,double(oc.sensorZID)});

                    if (z_range_info.first-z_range_info.second > temp_INTTvtxZ+temp_INTTvtxZError || z_range_info.first+z_range_info.second < temp_INTTvtxZ-temp_INTTvtxZError) {continue;}

                    std::pair<double,double> Get_eta_pair = Get_eta(
                        {0.,temp_INTTvtxZ,temp_INTTvtxZError},
                        {get_radius(ic.x-vertexXYIncm.first,ic.y-vertexXYIncm.second),ic.z,double(ic.sensorZID)},
                        {get_radius(oc.x-vertexXYIncm.first,oc.y-vertexXYIncm.second),oc.z,double(oc.sensorZID)});

                    pair_str temp_pair_str;
                    temp_pair_str.delta_phi    = delta_phi_radian;
                    temp_pair_str.delta_eta    = inner_clu_eta - outer_clu_eta;
                    temp_pair_str.pair_eta_num = (inner_clu_eta + outer_clu_eta) / 2.;
                    temp_pair_str.pair_eta_fit = Get_eta_pair.second;
                    temp_pair_str.inner_zid    = ic.sensorZID; temp_pair_str.inner_phi_size = ic.phi_size;
                    temp_pair_str.inner_adc    = ic.adc;       temp_pair_str.inner_x = ic.x; temp_pair_str.inner_y = ic.y;
                    temp_pair_str.inner_index  = ic.index;
                    temp_pair_str.outer_zid    = oc.sensorZID; temp_pair_str.outer_phi_size = oc.phi_size;
                    temp_pair_str.outer_adc    = oc.adc;       temp_pair_str.outer_x = oc.x; temp_pair_str.outer_y = oc.y;
                    temp_pair_str.outer_index  = oc.index;
                    input_TrackletPair_vec.push_back(temp_pair_str);
                }
            }
        }
    }
}

void EvtVtxZProtoTracklet_MultiVtx::FillRecoINTTVtxZH1D(int event_index)
{
    if (event_index % 20 == 0){
        std::cout<<"In Filling RecoZ: "<<event_index<<", "<<MBD_z_vtx<<", "<<temp_INTTvtxZ<<", diff: "<<MBD_z_vtx-temp_INTTvtxZ<<std::endl;
    }
    if (runnumber != -1 && out_InttBcoFullDiff_next <= cut_InttBcoFullDIff_next) {return;}
    if (runnumber != -1 && out_MBDNSg2 != 1) {return;}
    if (temp_INTTvtxZ != temp_INTTvtxZ) {return;}
    if ((out_TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || out_TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){return;}
    if ((out_TrapezoidalFWHM    < cut_TrapezoidalFWHM.first    || out_TrapezoidalFWHM    > cut_TrapezoidalFWHM.second))   {return;}
    reco_INTTvtxZ -> Fill(temp_INTTvtxZ);
}

void EvtVtxZProtoTracklet_MultiVtx::MainProcess()
{
    if (HaveGeoOffsetTag == true && CheckGeoOffsetMap() <= 0) {
        std::cout<<"The HaveGeoOffsetTag is true, but the GeoOffsetMap is empty"<<std::endl; exit(1);
    }
    if (HaveGeoOffsetTag == false && CheckGeoOffsetMap() > 0.0001) {
        std::cout<<"The HaveGeoOffsetTag is false, but the GeoOffsetMap has non-zero numbers"<<std::endl; exit(1);
    }

    for (int i = 0; i < run_nEvents; i++){
        tree_in -> GetEntry(i);
        if (i % 1 == 0){ std::cout<<"In EvtVtxZProtoTracklet_MultiVtx, Processing event: "<<i<<std::endl; }

        EvtCleanUp();
        PrepareClusterVec();

        if (RunVtxZReco){
            GetAllINTTvtxZ();    // note : replaces single GetINTTvtxZ()
            temp_INTTvtxZ      = out_INTTvtxZ;
            temp_INTTvtxZError = out_INTTvtxZError;
            GetClusUpdated();
            if (m_withTrig) {GetTriggerInfo();}
        }
        else {
            temp_INTTvtxZ      = INTTvtxZ;
            temp_INTTvtxZError = INTTvtxZError;
        }

        if (RunTrackletPair)       {GetTrackletPair(out_evt_TrackletPair_vec,       false);}
        if (RunTrackletPairRotate) {GetTrackletPair(out_evt_TrackletPairRotate_vec, true);}

        if (RunInttBcoFullDiff && runnumber != -1){
            if (i != run_nEvents-1){
                ULong_t this_bco = INTT_BCO;
                tree_in -> GetEntry(i+1);
                ULong_t next_bco = INTT_BCO;
                out_InttBcoFullDiff_next = next_bco - this_bco;
                tree_in -> GetEntry(i);
            }
            else { out_InttBcoFullDiff_next = -1; }
        }

        FillRecoINTTVtxZH1D(i);
        tree_out -> Fill();
        out_eID_count++;
    }
}

void EvtVtxZProtoTracklet_MultiVtx::EvtCleanUp()
{
    evt_sPH_inner_nocolumn_vec_PostCut.clear();
    evt_sPH_outer_nocolumn_vec_PostCut.clear();
    inner_clu_phi_map_PostCut.clear();
    outer_clu_phi_map_PostCut.clear();
    inner_clu_phi_map_PostCut = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);
    outer_clu_phi_map_PostCut = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);

    evt_possible_z     -> Reset("ICESM");
    line_breakdown_hist -> Reset("ICESM");

    out_INTTvtxZ           = std::nan("");
    out_INTTvtxZError      = std::nan("");
    out_NgroupTrapezoidal  = std::nan("");
    out_NgroupCoarse       = std::nan("");
    out_TrapezoidalFitWidth = std::nan("");
    out_TrapezoidalFWHM   = std::nan("");
    out_FitHeight         = std::nan("");

    temp_INTTvtxZ      = std::nan("");
    temp_INTTvtxZError = std::nan("");

    fit_mean_mean_vec.clear();
    fit_mean_reducedChi2_vec.clear();
    fit_mean_width_vec.clear();

    out_ClusEta_INTTz.clear(); out_ClusEta_MBDz.clear(); out_ClusEta_TrueXYZ.clear();
    out_ClusPhi_AvgPV.clear(); out_ClusPhi_TrueXY.clear();

    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();
    inner_clu_phi_map.clear(); outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,EvtVtxZProtoTracklet_MultiVtx::clu_info>>>(360);

    out_evt_TrackletPair_vec.clear();
    out_evt_TrackletPairRotate_vec.clear();
    out_InttBcoFullDiff_next = -1;

    // note : multi-vertex state
    out_vec_INTTvtxZ.clear(); out_vec_INTTvtxZError.clear();
    out_vec_NgroupTrapezoidal.clear(); out_vec_NgroupCoarse.clear();
    out_vec_TrapezoidalFitWidth.clear(); out_vec_TrapezoidalFWHM.clear();
    out_vec_FitHeight.clear();
    out_N_INTT_VtxZ = 0;
    used_inner_indices.clear(); used_outer_indices.clear();
}

void EvtVtxZProtoTracklet_MultiVtx::EndRun(int close_file_in)
{
    file_out -> cd();
    tree_out -> Write();
    reco_INTTvtxZ -> Write();
    file_out -> Close();

    if (DrawEvtVtxZ && INTTvtxZ_EvtDisplay_file_out != nullptr){
        INTTvtxZ_EvtDisplay_file_out -> cd();
        INTTvtxZ_EvtDisplay_file_out -> Close();
    }
    if (close_file_in == 1){ file_in -> cd(); file_in -> Close(); }
}

// ---- Math helpers (identical to EvtVtxZProtoTracklet) ----

double EvtVtxZProtoTracklet_MultiVtx::get_radius(double x, double y)
{ return sqrt(pow(x,2)+pow(y,2)); }

double EvtVtxZProtoTracklet_MultiVtx::get_delta_phi(double angle_1, double angle_2)
{
    std::vector<double> vec_abs = {std::fabs(angle_1-angle_2), std::fabs(angle_1-angle_2+360), std::fabs(angle_1-angle_2-360)};
    std::vector<double> vec     = {(angle_1-angle_2),          (angle_1-angle_2+360),          (angle_1-angle_2-360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double EvtVtxZProtoTracklet_MultiVtx::calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY)
{
    double vector1X = x2-x1, vector1Y = y2-y1;
    double vector2X = targetX-x1, vector2Y = targetY-y1;
    double crossProduct = vector1X*vector2Y - vector1Y*vector2X;
    double magnitude1 = std::sqrt(vector1X*vector1X + vector1Y*vector1Y);
    double magnitude2 = std::sqrt(vector2X*vector2X + vector2Y*vector2Y);
    double angleInRadians_new = std::asin(crossProduct/(magnitude1*magnitude2));
    return sin(angleInRadians_new) * magnitude2;
}

double EvtVtxZProtoTracklet_MultiVtx::Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y)
{
    if (fabs(p0x-p1x) < 0.000001){ return p0x; }
    double slope = (p1y-p0y)/(p1x-p0x);
    return (given_y - (p0y - slope*p0x)) / slope;
}

std::pair<double,double> EvtVtxZProtoTracklet_MultiVtx::Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1)
{
    std::vector<double> p0_z_edge = {
        (p0[2]==typeA_sensorZID[0]||p0[2]==typeA_sensorZID[1]) ? p0[1]-typeA_sensor_half_length_incm : p0[1]-typeB_sensor_half_length_incm,
        (p0[2]==typeA_sensorZID[0]||p0[2]==typeA_sensorZID[1]) ? p0[1]+typeA_sensor_half_length_incm : p0[1]+typeB_sensor_half_length_incm};
    std::vector<double> p1_z_edge = {
        (p1[2]==typeA_sensorZID[0]||p1[2]==typeA_sensorZID[1]) ? p1[1]-typeA_sensor_half_length_incm : p1[1]-typeB_sensor_half_length_incm,
        (p1[2]==typeA_sensorZID[0]||p1[2]==typeA_sensorZID[1]) ? p1[1]+typeA_sensor_half_length_incm : p1[1]+typeB_sensor_half_length_incm};
    double edge_first  = Get_extrapolation(rvtx, p0_z_edge[0], p0[0], p1_z_edge[1], p1[0]);
    double edge_second = Get_extrapolation(rvtx, p0_z_edge[1], p0[0], p1_z_edge[0], p1[0]);
    return {(edge_first+edge_second)/2., fabs(edge_first-edge_second)/2.};
}

void EvtVtxZProtoTracklet_MultiVtx::line_breakdown(TH1D* hist_in, std::pair<double,double> line_range)
{
    int first_bin = int((line_range.first  - hist_in->GetXaxis()->GetXmin()) / hist_in->GetBinWidth(1)) + 1;
    int last_bin  = int((line_range.second - hist_in->GetXaxis()->GetXmin()) / hist_in->GetBinWidth(1)) + 1;
    if (first_bin < 1) first_bin = 0; else if (first_bin > hist_in->GetNbinsX()) first_bin = hist_in->GetNbinsX()+1;
    if (last_bin  < 1) last_bin  = 0; else if (last_bin  > hist_in->GetNbinsX()) last_bin  = hist_in->GetNbinsX()+1;
    for (int i = 0; i < (last_bin-first_bin)+1; i++){
        hist_in -> SetBinContent(first_bin+i, hist_in->GetBinContent(first_bin+i) + 1.);
    }
}

void EvtVtxZProtoTracklet_MultiVtx::trapezoidal_line_breakdown(TH1D* hist_in, double inner_r, double inner_z, int inner_zid, double outer_r, double outer_z, int outer_zid)
{
    combination_zvtx_range_shape -> Reset("ICESM");
    std::vector<double> inner_edge = {
        (inner_zid==typeA_sensorZID[0]||inner_zid==typeA_sensorZID[1]) ? inner_z-typeA_sensor_half_length_incm : inner_z-typeB_sensor_half_length_incm,
        (inner_zid==typeA_sensorZID[0]||inner_zid==typeA_sensorZID[1]) ? inner_z+typeA_sensor_half_length_incm : inner_z+typeB_sensor_half_length_incm};
    std::vector<double> outer_edge = {
        (outer_zid==typeA_sensorZID[0]||outer_zid==typeA_sensorZID[1]) ? outer_z-typeA_sensor_half_length_incm : outer_z-typeB_sensor_half_length_incm,
        (outer_zid==typeA_sensorZID[0]||outer_zid==typeA_sensorZID[1]) ? outer_z+typeA_sensor_half_length_incm : outer_z+typeB_sensor_half_length_incm};

    for (int possible_i = 0; possible_i < 2001; possible_i++){
        double random_inner_z = inner_edge[0] + ((inner_edge[1]-inner_edge[0])/2000.)*possible_i;
        double edge_first  = Get_extrapolation(0, random_inner_z, inner_r, outer_edge[1], outer_r);
        double edge_second = Get_extrapolation(0, random_inner_z, inner_r, outer_edge[0], outer_r);
        double mid_point = (edge_first+edge_second)/2.;
        double possible_width = fabs(edge_first-edge_second)/2.;
        line_breakdown(combination_zvtx_range_shape, {mid_point-possible_width, mid_point+possible_width});
    }
    combination_zvtx_range_shape -> Scale(1./combination_zvtx_range_shape->Integral(-1,-1));
    for (int bin_i = 1; bin_i <= combination_zvtx_range_shape->GetNbinsX(); bin_i++){
        hist_in -> SetBinContent(bin_i, hist_in->GetBinContent(bin_i) + combination_zvtx_range_shape->GetBinContent(bin_i));
    }
}

std::vector<double> EvtVtxZProtoTracklet_MultiVtx::find_Ngroup(TH1D* hist_in)
{
    double Highest_bin_Content = hist_in->GetBinContent(hist_in->GetMaximumBin());
    double Highest_bin_Center  = hist_in->GetBinCenter (hist_in->GetMaximumBin());
    int group_Nbin = 0; int peak_group_ID = -9999; double group_entry = 0;
    std::vector<int>    group_Nbin_vec;    group_Nbin_vec.clear();
    std::vector<double> group_entry_vec;   group_entry_vec.clear();
    std::vector<double> group_widthL_vec;  group_widthL_vec.clear();
    std::vector<double> group_widthR_vec;  group_widthR_vec.clear();

    for (int i = 0; i < hist_in->GetNbinsX(); i++){
        double bin_content = (hist_in->GetBinContent(i+1) <= Highest_bin_Content*0.8) ? 0. : (hist_in->GetBinContent(i+1) - Highest_bin_Content*0.8);
        if (bin_content != 0){
            if (group_Nbin == 0) group_widthL_vec.push_back(hist_in->GetBinCenter(i+1) - (hist_in->GetBinWidth(i+1)/2.));
            group_Nbin += 1; group_entry += bin_content;
        }
        else if (bin_content == 0 && group_Nbin != 0){
            group_widthR_vec.push_back(hist_in->GetBinCenter(i+1) - (hist_in->GetBinWidth(i+1)/2.));
            group_Nbin_vec.push_back(group_Nbin); group_entry_vec.push_back(group_entry);
            group_Nbin = 0; group_entry = 0;
        }
    }
    if (group_Nbin != 0){
        group_Nbin_vec.push_back(group_Nbin); group_entry_vec.push_back(group_entry);
        group_widthR_vec.push_back(hist_in->GetXaxis()->GetXmax());
    }
    for (int i = 0; i < (int)group_Nbin_vec.size(); i++){
        if (group_widthL_vec[i] < Highest_bin_Center && Highest_bin_Center < group_widthR_vec[i]){ peak_group_ID = i; break; }
    }
    double peak_group_ratio = -999;
    double peak_group_left  = (group_Nbin_vec.size() == 0) ? -999 : group_widthL_vec[peak_group_ID];
    double peak_group_right = (group_Nbin_vec.size() == 0) ?  999 : group_widthR_vec[peak_group_ID];
    return {double(group_Nbin_vec.size()), peak_group_ratio, peak_group_left, peak_group_right};
}

double EvtVtxZProtoTracklet_MultiVtx::vector_average(std::vector<double> input_vector)
{ return accumulate(input_vector.begin(),input_vector.end(),0.0) / double(input_vector.size()); }

double EvtVtxZProtoTracklet_MultiVtx::vector_stddev(std::vector<double> input_vector)
{
    double sum_subt = 0;
    double average  = accumulate(input_vector.begin(),input_vector.end(),0.0) / double(input_vector.size());
    for (int i = 0; i < (int)input_vector.size(); i++){ sum_subt += pow((input_vector[i]-average),2); }
    return sqrt(sum_subt / double(input_vector.size()-1));
}

double EvtVtxZProtoTracklet_MultiVtx::get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos)
{
    double correct_x = clu_pos[0]-vertex[0], correct_y = clu_pos[1]-vertex[1], correct_z = clu_pos[2]-vertex[2];
    double clu_r = sqrt(pow(correct_x,2)+pow(correct_y,2));
    return -0.5*TMath::Log((sqrt(pow(correct_z,2)+pow(clu_r,2))-(correct_z))/(sqrt(pow(correct_z,2)+pow(clu_r,2))+(correct_z)));
}

std::pair<double,double> EvtVtxZProtoTracklet_MultiVtx::Get_eta(std::vector<double>p0, std::vector<double>p1, std::vector<double>p2)
{
    if (track_gr->GetN() != 0) {track_gr->Set(0);}
    std::vector<double> r_vec  = {p0[0],p1[0],p2[0]};
    std::vector<double> z_vec  = {p0[1],p1[1],p2[1]};
    std::vector<double> re_vec = {0,0,0};
    std::vector<double> ze_vec = {p0[2],
        (p1[2]==typeA_sensorZID[0]||p1[2]==typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm,
        (p2[2]==typeA_sensorZID[0]||p2[2]==typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm};
    for (int i = 0; i < 3; i++){
        track_gr->SetPoint(i,r_vec[i],z_vec[i]);
        track_gr->SetPointError(i,re_vec[i],ze_vec[i]);
    }
    double vertical_line = (fabs(grEY_stddev(track_gr)) < 0.00001) ? 1 : 0;
    if (vertical_line) {return {0,0};}
    fit_rz->SetParameters(0,0);
    track_gr->Fit(fit_rz,"NQ");
    std::pair<double,double> ax_b = mirrorPolynomial(fit_rz->GetParameter(1),fit_rz->GetParameter(0));
    return {(fit_rz->GetChisquare()/double(fit_rz->GetNDF())), -1*TMath::Log(fabs(tan(atan2(ax_b.first,(ax_b.first>0)?1.:-1.)/2)))};
}

double EvtVtxZProtoTracklet_MultiVtx::grEY_stddev(TGraphErrors* input_grr)
{
    std::vector<double> input_vector;
    for (int i = 0; i < input_grr->GetN(); i++) input_vector.push_back(input_grr->GetPointY(i));
    double average = std::accumulate(input_vector.begin(),input_vector.end(),0.0)/double(input_vector.size());
    double sum_subt = 0;
    for (int ele : input_vector) sum_subt += pow((ele-average),2);
    return sqrt(sum_subt/(input_vector.size()-1));
}

std::pair<double,double> EvtVtxZProtoTracklet_MultiVtx::mirrorPolynomial(double a, double b)
{ return {1.0/a, -b/a}; }

void EvtVtxZProtoTracklet_MultiVtx::Characterize_Pad(TPad *pad, float left, float right, float top, float bottom, bool set_logY, int setgrid_bool)
{
    if (setgrid_bool) pad->SetGrid(1,1);
    pad->SetLeftMargin(left); pad->SetRightMargin(right); pad->SetTopMargin(top); pad->SetBottomMargin(bottom);
    pad->SetTicks(1,1);
    if (set_logY) pad->SetLogy(1);
}

void EvtVtxZProtoTracklet_MultiVtx::PrepareEvtCanvas()
{
    std::string PlotOut_filename = "Plot_" + output_filename;
    INTTvtxZ_EvtDisplay_file_out = new TFile(Form("%s/%s",output_directory.c_str(),PlotOut_filename.c_str()),"RECREATE");
    c1 = new TCanvas("","",950,800);
    c1->cd();
    pad_EvtZDist = new TPad("pad_EvtZDist","pad_EvtZDist",0.0,0.0,1.0,1.0);
    pad_ZoomIn_EvtZDist = new TPad("pad_ZoomIn_EvtZDist","pad_ZoomIn_EvtZDist",0.52,0.25,0.82,0.60);
    Characterize_Pad(pad_ZoomIn_EvtZDist,0.15,0.1,0.1,0.2,0,0);
    pad_ZoomIn_EvtZDist->SetFillColor(0);
    draw_text = new TLatex();
    draw_text->SetName("draw_text");
    draw_text->SetNDC();
    draw_text->SetTextSize(0.03);
}

// ============================================================================
// New multi-vertex methods
// ============================================================================

void EvtVtxZProtoTracklet_MultiVtx::MarkAssociatedClusters(double vtxZ, double /*vtxZError*/)
{
    // note : build phi-maps from currently un-used PostCut clusters
    std::vector<std::vector<std::pair<bool,clu_info>>> local_inner_phi(360);
    std::vector<std::vector<std::pair<bool,clu_info>>> local_outer_phi(360);

    for (const auto& c : evt_sPH_inner_nocolumn_vec_PostCut) {
        if (used_inner_indices.count(c.index)) continue;
        double phi = (c.y-vertexXYIncm.second < 0)
            ? atan2(c.y-vertexXYIncm.second, c.x-vertexXYIncm.first)*(180./TMath::Pi())+360.
            : atan2(c.y-vertexXYIncm.second, c.x-vertexXYIncm.first)*(180./TMath::Pi());
        local_inner_phi[int(phi)].push_back({false,c});
    }
    for (const auto& c : evt_sPH_outer_nocolumn_vec_PostCut) {
        if (used_outer_indices.count(c.index)) continue;
        double phi = (c.y-vertexXYIncm.second < 0)
            ? atan2(c.y-vertexXYIncm.second, c.x-vertexXYIncm.first)*(180./TMath::Pi())+360.
            : atan2(c.y-vertexXYIncm.second, c.x-vertexXYIncm.first)*(180./TMath::Pi());
        local_outer_phi[int(phi)].push_back({false,c});
    }

    double loose_dphi_lo = assoc_cut_factor * DeltaPhiCutInDegree.first.first;
    double loose_dphi_hi = assoc_cut_factor * DeltaPhiCutInDegree.first.second;
    double loose_dca_lo  = assoc_cut_factor * DCAcutIncm.first.first;
    double loose_dca_hi  = assoc_cut_factor * DCAcutIncm.first.second;

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++){
        for (auto& inner_entry : local_inner_phi[inner_phi_i]){
            const clu_info& ic = inner_entry.second;
            double InnerPhi = (ic.y-vertexXYIncm.second < 0)
                ? atan2(ic.y-vertexXYIncm.second, ic.x-vertexXYIncm.first)*(180./TMath::Pi())+360.
                : atan2(ic.y-vertexXYIncm.second, ic.x-vertexXYIncm.first)*(180./TMath::Pi());

            for (int scan_i = -5; scan_i < 6; scan_i++){
                int true_scan_i = ((inner_phi_i+scan_i)<0) ? 360+(inner_phi_i+scan_i) : ((inner_phi_i+scan_i)>359) ? (inner_phi_i+scan_i)-360 : inner_phi_i+scan_i;
                for (auto& outer_entry : local_outer_phi[true_scan_i]){
                    const clu_info& oc = outer_entry.second;
                    double OuterPhi = (oc.y-vertexXYIncm.second < 0)
                        ? atan2(oc.y-vertexXYIncm.second, oc.x-vertexXYIncm.first)*(180./TMath::Pi())+360.
                        : atan2(oc.y-vertexXYIncm.second, oc.x-vertexXYIncm.first)*(180./TMath::Pi());
                    double delta_phi = get_delta_phi(InnerPhi, OuterPhi);

                    if (delta_phi <= loose_dphi_lo || delta_phi >= loose_dphi_hi) continue;

                    if (IsDCACutApplied){
                        double DCA_sign = calculateAngleBetweenVectors(oc.x,oc.y,ic.x,ic.y,vertexXYIncm.first,vertexXYIncm.second);
                        if (DCA_sign <= loose_dca_lo || DCA_sign >= loose_dca_hi) continue;
                    }

                    // note : z-range overlap check (InttDoubletMap.cpp line 337 pattern)
                    std::pair<double,double> z_range = Get_possible_zvtx(0.,
                        {get_radius(ic.x-vertexXYIncm.first,ic.y-vertexXYIncm.second),ic.z,double(ic.sensorZID)},
                        {get_radius(oc.x-vertexXYIncm.first,oc.y-vertexXYIncm.second),oc.z,double(oc.sensorZID)});
                    if (z_range.first - z_range.second > vtxZ) continue;
                    if (z_range.first + z_range.second < vtxZ) continue;

                    used_inner_indices.insert(ic.index);
                    used_outer_indices.insert(oc.index);
                }
            }
        }
    }
}

EvtVtxZProtoTracklet_MultiVtx::VtxFitResult
EvtVtxZProtoTracklet_MultiVtx::RunSingleVtxReco(const std::vector<clu_info>& inner_vec, const std::vector<clu_info>& outer_vec, int vtx_index, int eID)
{
    VtxFitResult result{std::nan(""),std::nan(""),std::nan(""),std::nan(""),std::nan(""),std::nan(""),false};
    if (inner_vec.empty() || outer_vec.empty()) return result;

    TH1D* local_lbd  = new TH1D(Form("local_lbd_e%d_v%d",eID,vtx_index),"",
        2*zvtx_hist_Nbins+1, -(fine_bin_width*zvtx_hist_Nbins+fine_bin_width/2.), fine_bin_width*zvtx_hist_Nbins+fine_bin_width/2.);
    TH1D* local_evtz = new TH1D(Form("local_evtz_e%d_v%d",eID,vtx_index),"",
        50, evt_possible_z_range.first, evt_possible_z_range.second);

    std::vector<std::vector<std::pair<bool,clu_info>>> local_inner_phi(360), local_outer_phi(360);
    for (const auto& c : inner_vec){
        double phi = (c.y-vertexXYIncm.second<0) ? atan2(c.y-vertexXYIncm.second,c.x-vertexXYIncm.first)*(180./TMath::Pi())+360. : atan2(c.y-vertexXYIncm.second,c.x-vertexXYIncm.first)*(180./TMath::Pi());
        local_inner_phi[int(phi)].push_back({false,c});
    }
    for (const auto& c : outer_vec){
        double phi = (c.y-vertexXYIncm.second<0) ? atan2(c.y-vertexXYIncm.second,c.x-vertexXYIncm.first)*(180./TMath::Pi())+360. : atan2(c.y-vertexXYIncm.second,c.x-vertexXYIncm.first)*(180./TMath::Pi());
        local_outer_phi[int(phi)].push_back({false,c});
    }

    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++){
        for (size_t ii = 0; ii < local_inner_phi[inner_phi_i].size(); ii++){
            if (local_inner_phi[inner_phi_i][ii].first) continue;
            const clu_info& ic = local_inner_phi[inner_phi_i][ii].second;
            double InnerPhi = (ic.y-vertexXYIncm.second<0) ? atan2(ic.y-vertexXYIncm.second,ic.x-vertexXYIncm.first)*(180./TMath::Pi())+360. : atan2(ic.y-vertexXYIncm.second,ic.x-vertexXYIncm.first)*(180./TMath::Pi());

            for (int scan_i = -5; scan_i < 6; scan_i++){
                int ts = ((inner_phi_i+scan_i)<0) ? 360+(inner_phi_i+scan_i) : ((inner_phi_i+scan_i)>359) ? (inner_phi_i+scan_i)-360 : inner_phi_i+scan_i;
                for (size_t oi = 0; oi < local_outer_phi[ts].size(); oi++){
                    if (local_outer_phi[ts][oi].first) continue;
                    const clu_info& oc = local_outer_phi[ts][oi].second;
                    double OuterPhi = (oc.y-vertexXYIncm.second<0) ? atan2(oc.y-vertexXYIncm.second,oc.x-vertexXYIncm.first)*(180./TMath::Pi())+360. : atan2(oc.y-vertexXYIncm.second,oc.x-vertexXYIncm.first)*(180./TMath::Pi());
                    double delta_phi = get_delta_phi(InnerPhi, OuterPhi);

                    if (delta_phi <= DeltaPhiCutInDegree.first.first || delta_phi >= DeltaPhiCutInDegree.first.second) continue;

                    if (IsDCACutApplied){
                        double DCA_sign = calculateAngleBetweenVectors(oc.x,oc.y,ic.x,ic.y,vertexXYIncm.first,vertexXYIncm.second);
                        if (DCA_sign <= DCAcutIncm.first.first || DCA_sign >= DCAcutIncm.first.second) continue;
                    }

                    std::pair<double,double> z_range = Get_possible_zvtx(0.,
                        {get_radius(ic.x-vertexXYIncm.first,ic.y-vertexXYIncm.second),ic.z,double(ic.sensorZID)},
                        {get_radius(oc.x-vertexXYIncm.first,oc.y-vertexXYIncm.second),oc.z,double(oc.sensorZID)});

                    if (evt_possible_z_range.first < z_range.first && z_range.first < evt_possible_z_range.second){
                        local_evtz -> Fill(z_range.first);
                        trapezoidal_line_breakdown(local_lbd,
                            get_radius(ic.x-vertexXYIncm.first,ic.y-vertexXYIncm.second), ic.z, ic.sensorZID,
                            get_radius(oc.x-vertexXYIncm.first,oc.y-vertexXYIncm.second), oc.z, oc.sensorZID);
                    }
                }
            }
        }
    }

    for (int bin_i = 0; bin_i < local_lbd->GetNbinsX(); bin_i++){
        double center = local_lbd->GetBinCenter(bin_i+1);
        if (center < edge_rejection.first || center > edge_rejection.second) local_lbd->SetBinContent(bin_i+1,0);
    }

    if (local_lbd->GetBinContent(local_lbd->GetMaximumBin()) <= min_peak_content || local_lbd->Integral() == 0){
        delete local_lbd; delete local_evtz; return result;
    }

    std::vector<double> N_group_info        = find_Ngroup(local_evtz);
    std::vector<double> N_group_info_detail = find_Ngroup(local_lbd);
    double lbd_max_content = local_lbd->GetBinContent(local_lbd->GetMaximumBin());
    double lbd_max_center  = local_lbd->GetBinCenter (local_lbd->GetMaximumBin());
    double temp_fit_region_half = 5.0;

    gaus_fit->SetParameters(lbd_max_content,lbd_max_center,temp_fit_region_half,0);
    gaus_fit->SetParLimits(0,0,100000); gaus_fit->SetParLimits(2,0.5,1000); gaus_fit->SetParLimits(3,-200,10000);
    local_lbd->Fit(gaus_fit,"NQ","",lbd_max_center-temp_fit_region_half*0.65,lbd_max_center+temp_fit_region_half*0.65);

    fit_mean_mean_vec.clear(); fit_mean_reducedChi2_vec.clear(); fit_mean_width_vec.clear();
    for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++){
        gaus_fit_vec[fit_i]->SetParameters(lbd_max_content,lbd_max_center,temp_fit_region_half,0);
        gaus_fit_vec[fit_i]->SetParLimits(0,0,100000); gaus_fit_vec[fit_i]->SetParLimits(2,0.5,1000); gaus_fit_vec[fit_i]->SetParLimits(3,-200,10000);
        double N_width_ratio = 0.2+0.15*fit_i;
        double mean_l = lbd_max_center-temp_fit_region_half*N_width_ratio;
        double mean_r = lbd_max_center+temp_fit_region_half*N_width_ratio;
        local_lbd->Fit(gaus_fit_vec[fit_i],"NQ","",mean_l,mean_r);
        gaus_fit_vec[fit_i]->SetRange(mean_l,mean_r);
        fit_mean_mean_vec.push_back(gaus_fit_vec[fit_i]->GetParameter(1));
        fit_mean_reducedChi2_vec.push_back(gaus_fit_vec[fit_i]->GetChisquare()/double(gaus_fit_vec[fit_i]->GetNDF()));
        fit_mean_width_vec.push_back(fabs(gaus_fit_vec[fit_i]->GetParameter(2)));
    }

    result.vtxZ              = vector_average(fit_mean_mean_vec);
    result.vtxZError         = vector_stddev(fit_mean_mean_vec);
    result.NgroupTrapezoidal = N_group_info_detail[0];
    result.NgroupCoarse      = N_group_info[0];
    result.TrapezoidalFitWidth = gaus_fit->GetParameter(2);
    result.TrapezoidalFWHM   = fabs(N_group_info_detail[3]-N_group_info_detail[2])/2.;
    result.FitHeight         = gaus_fit->GetParameter(0) + gaus_fit->GetParameter(3);
    result.valid             = true;


    std::cout<<" More than 1 vertex found!,  eID: "<<eID<<"  vtx_index: "<<vtx_index<<", vtxZ: "<<result.vtxZ<<"  vtxZError: "<<result.vtxZError<<"  NgroupTrapezoidal: "<<result.NgroupTrapezoidal<<"  NgroupCoarse: "<<result.NgroupCoarse<<"  TrapezoidalFitWidth: "<<result.TrapezoidalFitWidth<<"  TrapezoidalFWHM: "<<result.TrapezoidalFWHM<<std::endl;

    if (DrawEvtVtxZ && INTTvtxZ_EvtDisplay_file_out != nullptr){
        TH1D* lbd_zoomin = (TH1D*)local_lbd->Clone(Form("local_lbd_zoomin_e%d_v%d",eID,vtx_index));
        lbd_zoomin->GetXaxis()->SetRangeUser(gaus_fit_vec.front()->GetParameter(1)-temp_fit_region_half, gaus_fit_vec.front()->GetParameter(1)+temp_fit_region_half);
        lbd_zoomin->SetMinimum(gaus_fit_vec.front()->GetParameter(0)*0.5);
        lbd_zoomin->SetMaximum(gaus_fit_vec.front()->GetParameter(0)*1.5);

        c1->cd(); pad_EvtZDist->Draw(); pad_EvtZDist->cd();
        local_lbd->SetMinimum(0); local_lbd->SetMaximum(lbd_max_content*1.65);
        local_lbd->Draw("hist");
        for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++) gaus_fit_vec[gaus_fit_vec.size()-fit_i-1]->Draw("l same");
        draw_text->DrawLatex(0.2,0.90,Form("Event ID: %i  |  Vertex #%i",eID,vtx_index));
        draw_text->DrawLatex(0.2,0.86,Form("Remaining clusters: inner=%i outer=%i",(int)inner_vec.size(),(int)outer_vec.size()));
        draw_text->DrawLatex(0.2,0.82,Form("Reco. vtxZ: %.3f cm  StdDev: %.3f cm",result.vtxZ,result.vtxZError));
        INTTvtxZ_EvtDisplay_file_out->cd();
        c1->Write(Form("eID_%d_VtxN_%d_EvtZDist",eID,vtx_index));

        c1->cd(); pad_ZoomIn_EvtZDist->Draw(); pad_ZoomIn_EvtZDist->cd();
        lbd_zoomin->Draw("hist");
        for (int fit_i = 0; fit_i < (int)gaus_fit_vec.size(); fit_i++) gaus_fit_vec[gaus_fit_vec.size()-fit_i-1]->Draw("l same");
        INTTvtxZ_EvtDisplay_file_out->cd();
        c1->Write(Form("eID_%d_VtxN_%d_EvtZDist_ZoomIn",eID,vtx_index));
        pad_ZoomIn_EvtZDist->Clear(); pad_EvtZDist->Clear();
        delete lbd_zoomin;
    }
    delete local_lbd; delete local_evtz;
    return result;
}

void EvtVtxZProtoTracklet_MultiVtx::GetAllINTTvtxZ()
{
    // note : Step 1: first vertex using the existing single-vertex reco
    GetINTTvtxZ();
    if (out_INTTvtxZ != out_INTTvtxZ) { out_N_INTT_VtxZ = 0; return; }

    out_vec_INTTvtxZ.push_back(out_INTTvtxZ);
    out_vec_INTTvtxZError.push_back(out_INTTvtxZError);
    out_vec_NgroupTrapezoidal.push_back(out_NgroupTrapezoidal);
    out_vec_NgroupCoarse.push_back(out_NgroupCoarse);
    out_vec_TrapezoidalFitWidth.push_back(out_TrapezoidalFitWidth);
    out_vec_TrapezoidalFWHM.push_back(out_TrapezoidalFWHM);
    out_vec_FitHeight.push_back(out_FitHeight);

    double current_vtxZ      = out_INTTvtxZ;
    double current_vtxZError = out_INTTvtxZError;
    int    vtx_index         = 1;

    while (true) {
        // note : mark clusters linked to current vertex (accumulate into used sets)
        MarkAssociatedClusters(current_vtxZ, current_vtxZError);

        // note : build remaining cluster subsets
        std::vector<clu_info> rem_inner, rem_outer;
        for (const auto& c : evt_sPH_inner_nocolumn_vec_PostCut)
            if (!used_inner_indices.count(c.index)) rem_inner.push_back(c);
        for (const auto& c : evt_sPH_outer_nocolumn_vec_PostCut)
            if (!used_outer_indices.count(c.index)) rem_outer.push_back(c);

        if (rem_inner.empty() || rem_outer.empty()) break;

        VtxFitResult res = RunSingleVtxReco(rem_inner, rem_outer, vtx_index, out_eID_count);
        if (!res.valid) break;

        out_vec_INTTvtxZ.push_back(res.vtxZ);
        out_vec_INTTvtxZError.push_back(res.vtxZError);
        out_vec_NgroupTrapezoidal.push_back(res.NgroupTrapezoidal);
        out_vec_NgroupCoarse.push_back(res.NgroupCoarse);
        out_vec_TrapezoidalFitWidth.push_back(res.TrapezoidalFitWidth);
        out_vec_TrapezoidalFWHM.push_back(res.TrapezoidalFWHM);
        out_vec_FitHeight.push_back(res.FitHeight);

        current_vtxZ      = res.vtxZ;
        current_vtxZError = res.vtxZError;
        vtx_index++;
    }

    out_N_INTT_VtxZ = int(out_vec_INTTvtxZ.size());
    // note : scalar branches (out_INTTvtxZ etc.) already set by GetINTTvtxZ() = first vertex
}


