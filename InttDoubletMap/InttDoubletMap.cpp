#include "InttDoubletMap.h"

InttDoubletMap::InttDoubletMap(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,
    std::pair<double, double> vertexXYIncm_in,

    int data_type_in, // note : 0 pure_trigger, 1 streaming_trigger, 2 streaming_data
    bool isUsedMBDz_in,

    bool BcoFullDiffCut_in,
    int CentralityBin_in,
    bool isMinBiasCut_in,
    bool isTriggerSel_in,
    std::pair<bool, std::pair<int,int>> isMBDChargeCut_in,
    std::pair<bool, std::pair<int,int>> isBunchNumber_cut_in,

    std::pair<bool, TH1D*> vtxZReweight_in,
    bool INTT_vtxZ_QA_in,
    std::pair<double, double> VtxZRange_in,

    bool ColMulMask_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in,
    double DeltaPhiCut_in,

    bool HaveGeoOffsetTag_in
    
) : ClusHistogram(
    process_id_in,
    runnumber_in,
    run_nEvents_in,
    input_directory_in,
    input_file_name_in,
    output_directory_in,

    output_file_name_suffix_in,
    vertexXYIncm_in,

    vtxZReweight_in,
    BcoFullDiffCut_in,
    INTT_vtxZ_QA_in,
    isClusQA_in,
    HaveGeoOffsetTag_in,
    {false, 0}, // note : SetRandomHits_in is set to {false, 0} for InttDoubletMap
    false, // note : RandInttZ_in is set to false for InttDoubletMap
    ColMulMask_in,

    1 // note : constructor type
),
data_type(data_type_in),
CentralityBin(CentralityBin_in),
VtxZRange(VtxZRange_in),
DeltaPhiCut(DeltaPhiCut_in),
isBunchNumber_cut(isBunchNumber_cut_in),
isMinBiasCut(isMinBiasCut_in),
isUsedMBDz(isUsedMBDz_in),
isMBDChargeCut(isMBDChargeCut_in),
isTriggerSel(isTriggerSel_in)
{
    track_gr = new TGraphErrors();
    fit_rz = new TF1("fit_rz","pol1",-1000,1000);

    cut_GoodProtoTracklet_DeltaPhi.second = DeltaPhiCut;

    if (data_type == 0){isTrigger = true;}
    if (data_type == 1){isStreamTrig = true;}
    if (data_type == 2){isStreaming = true;}

    if (CentralityBin == 70) {CentralityRange = {1, 71};}
    else if (CentralityBin == 100) {CentralityRange = {1, 101};}
    else if (CentralityBin == -1 ) {CentralityRange = {-20, -20};}
    else {
        CentralityRange = {
            centrality_edges[CentralityBin],
            centrality_edges[CentralityBin + 1]
        };
    }
    std::cout << "process_id : "<<process_id<<std::endl;
    std::cout << "runnumber: "<< runnumber<<std::endl;
    std::cout << "run_nEvents: "<< run_nEvents<<std::endl;
    std::cout << "input_directory: "<< input_directory <<std::endl;
    std::cout << "input_file_name: "<< input_file_name <<std::endl;
    std::cout << "output_directory: "<< output_directory <<std::endl;
    std::cout << "output_file_name_suffix: "<< output_file_name_suffix <<std::endl;
    std::cout << "vertexXYIncm : "<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<std::endl;

    std::cout << "data_type: " << data_type << std::endl; 
    std::cout << "isUsedMBDz: " << isUsedMBDz << std::endl;

    std::cout << "BcoFullDiffCut: " << BcoFullDiffCut <<", cut value: "<<cut_InttBcoFullDIff_next<<std::endl;
    std::cout << "CentralityBin: "<<CentralityBin<<", range: "<<CentralityRange.first<<" - "<<CentralityRange.second<<std::endl;
    std::cout << "isMinBiasCut: " << isMinBiasCut << std::endl;
    std::cout << "isTriggerSel: " << isTriggerSel << std::endl;
    std::cout << "isMBDChargeCut : "<<isMBDChargeCut.first <<", range: "<< isMBDChargeCut.second.first<<", "<<isMBDChargeCut.second.second<<std::endl;
    std::cout << "isBunchNumber_cut: " << isBunchNumber_cut.first<<", range: "<<isBunchNumber_cut.second.first<<", "<<isBunchNumber_cut.second.second<<std::endl;

    std::cout << "vtxZReweight: " << vtxZReweight.first<<std::endl;
    std::cout << "INTT_vtxZ_QA: " << INTT_vtxZ_QA<<std::endl;
    std::cout << "VtxZRange: " << VtxZRange.first<<", "<<VtxZRange.second<<std::endl;

    std::cout << "ColMulMask: " << ColMulMask<<std::endl;
    std::cout << "isClusQA: " << isClusQA.first<<", ADC: "<< isClusQA.second.first<<", PhiSize: "<<isClusQA.second.second<<std::endl;
    std::cout << "DeltaPhiCut: " << DeltaPhiCut<<std::endl;

    std::cout << "HaveGeoOffsetTag: " << HaveGeoOffsetTag<<std::endl;
    std::cout << "RandInttZ: " << RandInttZ<<std::endl;
    std::cout << "ColMulMask: " << ColMulMask<<std::endl;

    PrepareOutPutFileName();
    PrepareOutPutRootFile();
    PrepareHistograms();

}

void InttDoubletMap::PrepareOutPutFileName()
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

    output_filename = "INTTDoublets";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;

    if (data_type == 0){output_filename += "_Trigger";}
    if (data_type == 1){output_filename += "_StreamTrig";}
    if (data_type == 2){output_filename += "_Streaming";}

    output_filename += ((isTrigger || isStreamTrig) && isUsedMBDz) ? "_UsedMBDz" : "";

    output_filename += (BcoFullDiffCut && runnumber != -1) ? "_BcoFullDiffCut" : "";
    output_filename += ((isTrigger || isStreamTrig) && CentralityBin != -1) ? Form("_CentralityRange%dto%d", CentralityRange.first, CentralityRange.second - 1) : "";
    output_filename += ((isTrigger || isStreamTrig) && isMinBiasCut) ? "_MinBiasEvt" : ""; 
    output_filename += ((isTrigger || isStreamTrig) && isTriggerSel) ? "_TriggerSel" : "";
    output_filename += ((isTrigger || isStreamTrig) && isMBDChargeCut.first) ? Form("_MBDChargeCut%dto%d",isMBDChargeCut.second.first, isMBDChargeCut.second.second) : ""; 
    output_filename += (runnumber != -1 && (isStreamTrig || isStreaming) && isBunchNumber_cut.first) ? Form("_BunchNumberRange%dto%d", isBunchNumber_cut.second.first, isBunchNumber_cut.second.second) : "";

    output_filename += (vtxZReweight.first) ? "_vtxZReweight" : "";
    output_filename += (INTT_vtxZ_QA) ? "_VtxZQA" : "";
    output_filename += Form("_VtxZRange%sto%s", DoubleToPString(VtxZRange.first,0).c_str(), DoubleToPString(VtxZRange.second,0).c_str());

    output_filename += (ColMulMask) ? "_ColMulMask" : "";
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";
    output_filename += Form("_DeltaPhiCut%s", DoubleToPString(DeltaPhiCut,3).c_str());

    output_filename += (HaveGeoOffsetTag) ? "_GeoOffset" : "";

    output_filename += (SetRandomHits.first) ? Form("_Rand%dHits",SetRandomHits.second) : "";
    output_filename += (RandInttZ) ? "_RandInttZ" : "";

    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

void InttDoubletMap::PrepareOutPutRootFile(){
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
}


void InttDoubletMap::PrepareHistograms()
{   
    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    for (int eta_bin = 0; eta_bin < 11; eta_bin++)
    {
        for (int vtxz_bin = 0; vtxz_bin < 1; vtxz_bin++)
        {
            for (int Phibin = 0; Phibin < 16; Phibin++)
            {
                h1D_map.insert( std::make_pair(
                        Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d", Phibin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d", Phibin, eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d;Pair #Delta#phi [radian];Entries", Phibin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second;

                h1D_map.insert( std::make_pair(
                        Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d_rotated", Phibin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d_rotated", Phibin, eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian];Entries", Phibin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second;
                
            } // note : end Phibin
        }
    }// note : end of Delta_Phi hist

    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
    {
        h1D_map.insert( std::make_pair(
                Form("h1D_DeltaPhi_Eta%d", eta_bin),
                new TH1D(Form("h1D_DeltaPhi_Eta%d", eta_bin), Form("h1D_DeltaPhi_Eta%d;Pair #Delta#phi [radian];Entries", eta_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
            )
        ).second;

        h1D_map.insert( std::make_pair(
                Form("h1D_DeltaPhi_Eta%d_rotated", eta_bin),
                new TH1D(Form("h1D_DeltaPhi_Eta%d_rotated", eta_bin), Form("h1D_DeltaPhi_Eta%d_rotated;Pair #Delta#phi [radian];Entries", eta_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
            )
        ).second;

    }// note : end of Delta_Phi hist

    h1D_map.insert( std::make_pair(
            Form("h1D_GoodProtoTracklet_Eta"),
            new TH1D(Form("h1D_GoodProtoTracklet_Eta"), Form("h1D_GoodProtoTracklet_Eta;#eta;Entries"), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

    h1D_map.insert( std::make_pair(
            Form("h1D_GoodProtoTracklet_Eta_rotated"),
            new TH1D(Form("h1D_GoodProtoTracklet_Eta_rotated"), Form("h1D_GoodProtoTracklet_Eta_rotated;#eta;Entries"), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

    
    
    h1D_eta_template_wide = new TH1D("h1D_eta_template_wide", "h1D_eta_template_wide", nEtaBin, EtaEdge_min, EtaEdge_max); // note : coarse
    h1D_eta_bin = new TH1D("h1D_eta_bin","h1D_eta_bin;Pair #eta;Entries",11,-1.1,1.1);
    h1D_phi_bin = new TH1D("h1D_phi_bin","h1D_phi_bin;Phi [radian];Entries",16,-3.2,3.2);
    h1D_nEvent = new TH1D("h1D_nEvent","h1D_nEvent;Event Count;Entries",1,0.5,1.5);
    h1D_BunchNumber = new TH1D("h1D_BunchNumber","h1D_BunchNumber;Selected BunchNumber;Entries",150,-20,130);
    h1D_MBDChargeSum = new TH1D("h1D_MBDChargeSum","h1D_MBDChargeSum;Selected MBD Charged Sum;Entries",250, 0, 500);
    h1D_ClusEtaInttZ = new TH1D("h1D_ClusEtaInttZ","h1D_ClusEtaInttZ;INTT ClusEta;Counts", nEtaBin, EtaEdge_min, EtaEdge_max);


    h1D_map.insert( std::make_pair(
            Form("h1D_Truth_ChargedHadron_Eta"),
            new TH1D(Form("h1D_Truth_ChargedHadron_Eta"), Form("h1D_Truth_ChargedHadron_Eta;Charged Hadron #eta;Entries"), 53,-5.3,5.3)
        )
    ).second;

    h1D_map.insert( std::make_pair(
            Form("h1D_Truth_ChargedHadron"),
            new TH1D(Form("h1D_Truth_ChargedHadron"), Form("h1D_Truth_ChargedHadron;Number of Charged Hadrons;Entries"), 200, 0, 1000)
        )
    ).second;

    h1D_map.insert( std::make_pair(
            Form("h1D_InttVtxZ"),
            new TH1D(Form("h1D_InttVtxZ"), Form("h1D_InttVtxZ;INTT z-vertex [cm];Entries"), 100, -50, 50)
        )
    ).second;


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // note : for 2D
    h2D_map.clear();

    h2D_map.insert( std::make_pair(
            Form("h2D_GoodProtoTracklet_EtaPhi"),
            new TH2D(Form("h2D_GoodProtoTracklet_EtaPhi"), Form("h2D_GoodProtoTracklet_EtaPhi;Pair #eta;Phi [radian]"), 11,-1.1,1.1, 16,-3.2,3.2) 
        )
    );

    h2D_map.insert( std::make_pair(
            Form("h2D_GoodProtoTracklet_EtaPhi_rotated"),
            new TH2D(Form("h2D_GoodProtoTracklet_EtaPhi_rotated"), Form("h2D_GoodProtoTracklet_EtaPhi_rotated;Pair #eta;Phi [radian]"), 11,-1.1,1.1, 16,-3.2,3.2) 
        )
    );

    h2D_map.insert( std::make_pair(
            Form("h2D_Truth_ChargedHadron_EtaPhi"),
            new TH2D(Form("h2D_Truth_ChargedHadron_EtaPhi"), Form("h2D_Truth_ChargedHadron_EtaPhi;Charged hadron #eta;Charged hadron Phi [radian]"), 11,-1.1,1.1, 16,-3.2,3.2) 
        )
    );

    h2D_map.insert( std::make_pair(
            Form("h2D_Clus_ColumnZID_LayerPhiID"),
            new TH2D(Form("h2D_Clus_ColumnZID_LayerPhiID"), Form("h2D_Clus_ColumnZID_LayerPhiID;Clus Column ZID;Layer x 20 + LadderPhiID"), 26,0,26, 90,0,90) 
        )
    );
}



void InttDoubletMap::EvtCleanUp()
{
    Used_Clus_index_vec.clear();
    Pair_DeltaPhi_vec.clear();

    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();

    evt_TrackletPair_vec.clear();
    evt_TrackletPairRotate_vec.clear();

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
}

void InttDoubletMap::GetTrackletPair(std::vector<pair_str> &input_TrackletPair_vec, bool isRotated)
{
    input_TrackletPair_vec.clear();

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);

    if (INTTvtxZ != INTTvtxZ || INTTvtxZError != INTTvtxZError) {return;}

    std::vector<ClusHistogram::clu_info> temp_evt_sPH_inner_nocolumn_vec = (isRotated) ? GetRotatedClusterVec(evt_sPH_inner_nocolumn_vec) : evt_sPH_inner_nocolumn_vec;

    for (int inner_i = 0; inner_i < int(temp_evt_sPH_inner_nocolumn_vec.size()); inner_i++) {
      double Clus_InnerPhi_Offset = (temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second < 0) ? atan2(temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_evt_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_evt_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi());
      inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_evt_sPH_inner_nocolumn_vec[inner_i]});
    }
    for (int outer_i = 0; outer_i < int(evt_sPH_outer_nocolumn_vec.size()); outer_i++) {
        double Clus_OuterPhi_Offset = (evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second < 0) ? atan2(evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi());
        outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,evt_sPH_outer_nocolumn_vec[outer_i]});
    }


    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
        {
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

            ClusHistogram::clu_info inner_clu = inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second;

            double Clus_InnerPhi_Offset_radian = atan2(inner_clu.y - vertexXYIncm.second, inner_clu.x - vertexXYIncm.first);
            double Clus_InnerPhi_Offset = (inner_clu.y - vertexXYIncm.second < 0) ? Clus_InnerPhi_Offset_radian * (180./TMath::Pi()) + 360 : Clus_InnerPhi_Offset_radian * (180./TMath::Pi());

            // todo: change the outer phi scan range
            // note : the outer phi index, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5
            // for (int scan_i = -5; scan_i < 6; scan_i++)
            for (int scan_i = -18; scan_i < 19; scan_i++)
            {
                int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

                // note : N clusters in that outer phi cell
                for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++)
                {
                    if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                    ClusHistogram::clu_info outer_clu = outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second;

                    double Clus_OuterPhi_Offset_radian = atan2(outer_clu.y - vertexXYIncm.second, outer_clu.x - vertexXYIncm.first);
                    double Clus_OuterPhi_Offset = (outer_clu.y - vertexXYIncm.second < 0) ? Clus_OuterPhi_Offset_radian * (180./TMath::Pi()) + 360 : Clus_OuterPhi_Offset_radian * (180./TMath::Pi());
                    double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                    // note : degree to radian 
                    double delta_phi_radian = delta_phi * (TMath::Pi() / 180.);
                    
                    // if (fabs(delta_phi) > 5.72) {continue;}
                    // if (fabs(delta_phi) > 3.5) {continue;}

                    double inner_clu_eta = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ},{inner_clu.x, inner_clu.y, inner_clu.z});
                    double outer_clu_eta = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ},{outer_clu.x, outer_clu.y, outer_clu.z});
                    double delta_eta = inner_clu_eta - outer_clu_eta;

                    std::pair<double,double> z_range_info = Get_possible_zvtx( 
                        0., // get_radius(vertexXYIncm.first,vertexXYIncm.second), 
                        {
                            get_radius(inner_clu.x - vertexXYIncm.first, inner_clu.y - vertexXYIncm.second), 
                            inner_clu.z,
                            double(inner_clu.sensorZID)
                        }, // note : unsign radius
                        
                        {
                            get_radius(outer_clu.x - vertexXYIncm.first, outer_clu.y - vertexXYIncm.second), 
                            outer_clu.z,
                            double(outer_clu.sensorZID)
                        }  // note : unsign radius
                    );

                    // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                    // if (z_range_info.first - z_range_info.second > INTTvtxZ + INTTvtxZError || z_range_info.first + z_range_info.second < INTTvtxZ - INTTvtxZError) {continue;}
                    if (z_range_info.first - z_range_info.second > INTTvtxZ || z_range_info.first + z_range_info.second < INTTvtxZ) {continue;} // todo: whether consider the INTTvtxZError or not

                    // note : do the fill here (find the best match outer cluster with the inner cluster )
                    // std::pair<double,double> Get_eta_pair = Get_eta(
                    //     {0., INTTvtxZ,INTTvtxZError},
                    //     {
                    //         get_radius(inner_clu.x - vertexXYIncm.first, inner_clu.y - vertexXYIncm.second), 
                    //         inner_clu.z,
                    //         double(inner_clu.sensorZID)
                    //     },
                    //     {
                    //         get_radius(outer_clu.x - vertexXYIncm.first, outer_clu.y - vertexXYIncm.second), 
                    //         outer_clu.z,
                    //         double(outer_clu.sensorZID)
                    //     }
                    // );  

                    // if (isRotated == false && fabs(delta_phi_radian) < 0.021 && INTTvtxZ > 30 && Get_eta_pair.second > 0){
                    //     std::cout<<"Check, vertex: {"<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<", "<<INTTvtxZ<<"}, INTTvtxZError: "<<INTTvtxZError<<std::endl;
                    //     std::cout<<"Inner cluster Pos: {"<<inner_clu.x<<", "<<inner_clu.y<<", "<<inner_clu.z<<"}, eta_INTTz: "<<inner_clu_eta<<", sensorID: "<<inner_clu.sensorZID<<std::endl;
                    //     std::cout<<"Outer cluster Pos: {"<<outer_clu.x<<", "<<outer_clu.y<<", "<<outer_clu.z<<"}, eta_INTTz: "<<outer_clu_eta<<", sensorID: "<<outer_clu.sensorZID<<std::endl;
                    //     std::cout<<"DeltaPhi: "<<delta_phi_radian<<", DeltaEta: "<<delta_eta<<", PairEta: "<<(inner_clu_eta + outer_clu_eta) / 2.<<", PairEtaFit: "<<Get_eta_pair.second<<std::endl;
                    //     std::cout<<std::endl;
                    // }

                    pair_str temp_pair_str;
                    temp_pair_str.delta_phi = delta_phi_radian;
                    temp_pair_str.delta_eta = delta_eta;
                    temp_pair_str.pair_eta_num = (inner_clu_eta + outer_clu_eta) / 2.;
                    temp_pair_str.pair_eta_fit = std::nan("");//Get_eta_pair.second; // todo: Cancel the eta fitting

                    // temp_pair_str.inner_phi_id = inner_clu.ladderPhiID;
                    // temp_pair_str.inner_layer_id = inner_clu.layerID;
                    temp_pair_str.inner_zid = inner_clu.sensorZID;
                    temp_pair_str.inner_phi_size = inner_clu.phi_size;
                    temp_pair_str.inner_adc = inner_clu.adc;
                    temp_pair_str.inner_x = inner_clu.x;
                    temp_pair_str.inner_y = inner_clu.y;
                    // temp_pair_str.inner_z = inner_clu.z;
                    // temp_pair_str.inner_phi = Clus_InnerPhi_Offset_radian;
                    // temp_pair_str.inner_eta = inner_clu_eta;
                    temp_pair_str.inner_index = inner_clu.index;
                    
                    // temp_pair_str.outer_phi_id = outer_clu.ladderPhiID;
                    // temp_pair_str.outer_layer_id = outer_clu.layerID;
                    temp_pair_str.outer_zid = outer_clu.sensorZID;
                    temp_pair_str.outer_phi_size = outer_clu.phi_size;
                    temp_pair_str.outer_adc = outer_clu.adc;
                    temp_pair_str.outer_x = outer_clu.x;
                    temp_pair_str.outer_y = outer_clu.y;
                    // temp_pair_str.outer_z = outer_clu.z;
                    // temp_pair_str.outer_phi = Clus_OuterPhi_Offset_radian;
                    // temp_pair_str.outer_eta = outer_clu_eta;
                    temp_pair_str.outer_index = outer_clu.index;

                    input_TrackletPair_vec.push_back(temp_pair_str);

                }
            }
        }
    }


}

void InttDoubletMap::FillPairs(std::vector<pair_str> input_TrackletPair_vec, bool isRotated, int Mbin_in, int vtxz_bin_in, double vtxZ_weight_in, int eID_in)
{
    Pair_DeltaPhi_vec.clear();
    Used_Clus_index_vec.clear();

    std::string rotated_text = (isRotated) ? "_rotated" : "";

    for (int pair_i = 0; pair_i < input_TrackletPair_vec.size(); pair_i++) // note : either non-rotated or rotated
    {
        pair_str this_pair = input_TrackletPair_vec[pair_i];

        double this_pair_inner_phi = atan2(this_pair.inner_y - vertexXYIncm.second, this_pair.inner_x - vertexXYIncm.first);
        double this_pair_outer_phi = atan2(this_pair.outer_y - vertexXYIncm.second, this_pair.outer_x - vertexXYIncm.first);
        double this_pair_phi = (this_pair_inner_phi + this_pair_outer_phi) / 2.;
        double used_phi = this_pair_phi; // todo : change here

        if (fabs(this_pair.delta_phi) < cut_GoodProtoTracklet_DeltaPhi.second){
            h2D_map[Form("h2D_GoodProtoTracklet_EtaPhi%s",rotated_text.c_str())] -> Fill(this_pair.pair_eta_num,used_phi);
        }

        int eta_bin = h1D_eta_bin -> Fill(this_pair.pair_eta_num);
        eta_bin = (eta_bin == -1) ? -1 : eta_bin - 1;
        if (eta_bin == -1) {continue;}

        int phi_bin = h1D_phi_bin -> Fill(used_phi);
        phi_bin = (phi_bin == -1) ? -1 : phi_bin - 1;
        if (phi_bin == -1) {continue;}

        // note : for both rotated and non-rotated
        h1D_map[Form("h1D_DeltaPhi_Phibin%d_Eta%d_VtxZ%d%s", phi_bin, eta_bin, 0, rotated_text.c_str())] -> Fill(this_pair.delta_phi);

    }

    for (int pair_i = 0; pair_i < input_TrackletPair_vec.size(); pair_i++) // note : either non-rotated or rotated
    {
        pair_str this_pair = input_TrackletPair_vec[pair_i];
        
        if (fabs(this_pair.delta_phi) < cut_GoodProtoTracklet_DeltaPhi.second){
            h1D_map[Form("h1D_GoodProtoTracklet_Eta%s", rotated_text.c_str())] -> Fill(this_pair.pair_eta_num);            
        }

        int eta_bin = h1D_eta_template_wide -> Fill(this_pair.pair_eta_num);
        eta_bin = (eta_bin == -1) ? -1 : eta_bin - 1;
        if (eta_bin == -1) {continue;}

        // note : for both rotated and non-rotated
        h1D_map[Form("h1D_DeltaPhi_Eta%d%s", eta_bin, rotated_text.c_str())] -> Fill(this_pair.delta_phi);
    }
    

    // h1D_PairDeltaEta_inclusive
    // h1D_PairDeltaPhi_inclusive

    // Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin)

    // Form("h1D_BestPair_DeltaEta_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_DeltaPhi_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_ClusPhiSize_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_ClusAdc_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_DeltaEta_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_ClusAdc_Mbin%d",Mbin_in)

    // Form("h2D_BestPairEtaVtxZ_Mbin%d",Mbin_in)
    // Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)
    // Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d",Mbin_in)
    // Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)

    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin_in)
    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin_in)
    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin_in)

    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin_in)
    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin_in)
    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin_in)

    
}

void InttDoubletMap::MainProcess()
{
    if (SetRandomHits.first){PrepareUniqueClusXYZ();}
    
    if (ColMulMask && h2D_GoodColMap == nullptr){
        std::cout<<"The GoodColMap is not set correctly"<<std::endl;
        exit(1);
    }

    if (HaveGeoOffsetTag && CheckGeoOffsetMap() <= 0){
        std::cout<<"The geo offset map is not set correctly"<<std::endl;
        exit(1);
    }

    if (HaveGeoOffsetTag == false && CheckGeoOffsetMap() > 0.0001) {
        std::cout<<"The HaveGeoOffsetTag is false, but the GeoOffsetMap has some non-zero numbers, please check the GeoOffsetMap"<<std::endl;
        exit(1);
    }

    if (
        ColMulMask &&
        (
            h1D_GoodColMap_ZId -> GetNbinsX() != h2D_GoodColMap -> GetNbinsY() ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin() - h2D_GoodColMap -> GetYaxis() -> GetXmin()) > 0.0000001 ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax() - h2D_GoodColMap -> GetYaxis() -> GetXmax()) > 0.0000001
        )

    ){
        std::cout<<"The setting of h1D_GoodColMap_ZId is different from h2D_GoodColMap"<<std::endl;
        std::cout<<"h1D_GoodColMap_ZId : "<<h1D_GoodColMap_ZId -> GetNbinsX()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax()<<std::endl;
        std::cout<<"h2D_GoodColMap : "<<h2D_GoodColMap -> GetNbinsY()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmin()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmax()<<std::endl;
        exit(1);
    }

    std::vector<int> test_count(40,0);

    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        EvtCleanUp();

        test_count[0] += 1;

        if (i % 10 == 0) {std::cout << "Processing event " << i<<", NClus : "<< ClusX -> size()<<", BunchNumber: " << BunchNumber << std::endl;}

        if (RandInttZ){
            // INTTvtxZ = rand3 -> Uniform(VtxZEdge_min,VtxZEdge_max);
            INTTvtxZ = rand3 -> Uniform(-10, 10);
            INTTvtxZError = 0.;
        }

        // =======================================================================================================================================================
        // note : hard cut

        if ((isTrigger || isStreamTrig) && isUsedMBDz){
            INTTvtxZ = MBD_z_vtx;
            INTTvtxZError = 0;
        }

        if ((isTrigger || isStreamTrig) && isTriggerSel){
            std::vector<int> this_trigger_vec = *(firedTriggers);
            if (
                std::find(this_trigger_vec.begin(), this_trigger_vec.end(), 12) == this_trigger_vec.end() &&
                std::find(this_trigger_vec.begin(), this_trigger_vec.end(), 13) == this_trigger_vec.end() &&
                std::find(this_trigger_vec.begin(), this_trigger_vec.end(), 14) == this_trigger_vec.end()
            ){
                continue;
            }
        }

        test_count[1] += 1;

        // note : for data
        if (runnumber != -1 && BcoFullDiffCut && InttBcoFullDiff_next <= cut_InttBcoFullDIff_next) {continue;}
        // if (runnumber != -1 && MBDNSg2 != 1) {continue;} // todo: assume MC no trigger

        // note : for MC
        // if (runnumber == -1 && NTruthVtx != 1) {continue;}
        
        test_count[2] += 1;

        // note : both data and MC
        if ( (isTrigger || isStreamTrig) && isMinBiasCut && is_min_bias != 1) {continue;}
        test_count[3] += 1;
        if ( (isTrigger || isStreamTrig) && CentralityBin != -1 && MBD_centrality != MBD_centrality) {continue;}
        test_count[4] += 1;
        if ( (isTrigger || isStreamTrig) && CentralityBin != -1 && (MBD_centrality < 0 || MBD_centrality > 100)) {continue;}
        test_count[5] += 1;
        if ( (isTrigger || isStreamTrig) && MBD_z_vtx != MBD_z_vtx) {continue;}
        test_count[6] += 1;
        if ( (isTrigger || isStreamTrig) && (MBD_z_vtx < cut_GlobalMBDvtxZ.first || MBD_z_vtx > cut_GlobalMBDvtxZ.second)) {continue;} // todo: the hard cut 60 cm 
        test_count[7] += 1;

        if (INTTvtxZ != INTTvtxZ) {continue;}
        test_count[8] += 1;
        // =======================================================================================================================================================

        if (runnumber == 82391){
            if (BunchNumber == BunchNumber && std::find(Constants::selected_bunch_id.begin(),Constants::selected_bunch_id.end(), BunchNumber) == Constants::selected_bunch_id.end()){
                // std::cout<<"Event with BunchNumber "<<BunchNumber<<" is rejected due to the bunch cut"<<std::endl;
                continue;
            }
        }
        test_count[9] += 1;

        if (runnumber != -1 && (isStreaming || isStreamTrig) && isBunchNumber_cut.first) {
            if (BunchNumber < isBunchNumber_cut.second.first || BunchNumber > isBunchNumber_cut.second.second){continue;}
        }
        test_count[10] += 1;

        // todo: it could be 1, or 0, be careful
        if (isStreamTrig && crossing != 0) {continue;} // note : bunchnumber -> the the crossing value, 0 is the trigger crossing
        test_count[11] += 1;
        if (isStreaming  && crossing == 0) {continue;}   // note : bunchnumber -> the the crossing value, 0 is the trigger crossing
        test_count[12] += 1;
        if (isStreaming  && crossing == 511) {continue;}   // note : the crossing value on the edge //todo: review this cut later
        test_count[13] += 1;
        if (runnumber != -1 && crossing > 5000){continue;}
        test_count[14] += 1;
        // =======================================================================================================================================================
        // note : optional cut
        if (INTT_vtxZ_QA && (MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {continue;}
        test_count[15] += 1;
        if (INTT_vtxZ_QA && (TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){continue;}
        test_count[16] += 1;
        if (INTT_vtxZ_QA && (TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){continue;}
        test_count[17] += 1;
        if (INTT_vtxZ_QA && (INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){continue;}
        test_count[18] += 1;
        // =======================================================================================================================================================

        if (INTTvtxZ < VtxZRange.first || INTTvtxZ > VtxZRange.second){continue;}
        test_count[19] += 1;

        if ((isTrigger || isStreamTrig) && CentralityBin != -1 && (MBD_centrality < CentralityRange.first || MBD_centrality >= CentralityRange.second) ){continue;}
        test_count[20] += 1;

        if ((isTrigger || isStreamTrig) && isMBDChargeCut.first && (MBD_charge_sum < isMBDChargeCut.second.first || MBD_charge_sum > isMBDChargeCut.second.second)) {continue;}
        test_count[21] += 1;

        if (vtxZReweight.first && runnumber != -1){
            std::cout<<"Should not have the vtxZ weighting from the data"<<std::endl;
            exit(1);
        }

        double INTTvtxZWeighting;
        if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting != nullptr){
            INTTvtxZWeighting = h1D_INTT_vtxZ_reweighting -> GetBinContent(h1D_INTT_vtxZ_reweighting -> FindBin(INTTvtxZ));
        }
        else if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting == nullptr){
            std::cout << "ApplyVtxZReWeighting is true, but h1D_INTT_vtxZ_reweighting is nullptr" << std::endl;
            exit(1);
        }
        else {
            INTTvtxZWeighting = 1.0;
        }
        test_count[22] += 1;


        if (runnumber == -1){
            int NHadrons = 0;

            for (int true_i = 0; true_i < NPrimaryG4P; true_i++){

                if (PrimaryG4P_isChargeHadron->at(true_i) != 1) { continue; }
                NHadrons += 1;
                h2D_map["h2D_Truth_ChargedHadron_EtaPhi"] -> Fill(PrimaryG4P_Eta->at(true_i), PrimaryG4P_Phi->at(true_i));
                h1D_map["h1D_Truth_ChargedHadron_Eta"] -> Fill(PrimaryG4P_Eta->at(true_i));
            }

            h1D_map["h1D_Truth_ChargedHadron"] -> Fill(NHadrons);
        }

        if (runnumber != -1) {h1D_BunchNumber -> Fill(BunchNumber);}

        if ((isTrigger || isStreamTrig)) {h1D_MBDChargeSum -> Fill(MBD_charge_sum);}

        test_count[23] += 1;
        
        PrepareClusterVec();

        h1D_map["h1D_InttVtxZ"] -> Fill(INTTvtxZ, INTTvtxZWeighting);

        for (ClusHistogram::clu_info this_clu : evt_sPH_inner_nocolumn_vec){
            h2D_map["h2D_Clus_ColumnZID_LayerPhiID"] -> Fill(this_clu.columnZID, (this_clu.layerID - 3) * 20 + this_clu.ladderPhiID, INTTvtxZWeighting);
            h1D_ClusEtaInttZ -> Fill(this_clu.eta_INTTz);
        }

        for (ClusHistogram::clu_info this_clu : evt_sPH_outer_nocolumn_vec){
            h2D_map["h2D_Clus_ColumnZID_LayerPhiID"] -> Fill(this_clu.columnZID, (this_clu.layerID - 3) * 20 + this_clu.ladderPhiID, INTTvtxZWeighting);
            h1D_ClusEtaInttZ -> Fill(this_clu.eta_INTTz);
        } 
        
        test_count[24] += 1;

        GetTrackletPair(evt_TrackletPair_vec, false);
        FillPairs(evt_TrackletPair_vec, false, -999, -999, INTTvtxZWeighting, i);

        test_count[25] += 1;

        GetTrackletPair(evt_TrackletPairRotate_vec, true);  
        FillPairs(evt_TrackletPairRotate_vec, true, -999, -999, INTTvtxZWeighting, i);

        test_count[26] += 1;

        h1D_nEvent -> Fill(1);
    }

    for (int ele = 0; ele < test_count.size(); ele++){
        std::cout<<ele<<", "<<test_count[ele]<<std::endl;
    }
}

void InttDoubletMap::EndRun()
{
    file_out -> cd();

    h1D_eta_bin -> Write();
    h1D_phi_bin -> Write();
    h1D_nEvent -> Write();
    h1D_BunchNumber -> Write();
    h1D_MBDChargeSum -> Write();
    h1D_ClusEtaInttZ -> Write();

    for (auto &pair : h2D_map){

        if (runnumber != -1 && pair.first.find("Truth") != std::string::npos) {continue;} // note : if it's data, skip the truth histograms

        pair.second -> Write();
    }

    for (auto &pair : h1D_map){

        if (runnumber != -1 && pair.first.find("Truth") != std::string::npos) {continue;} // note : if it's data, skip the truth histograms

        pair.second -> Write();
    }

    file_out -> Close();
}

std::vector<ClusHistogram::clu_info> InttDoubletMap::GetRotatedClusterVec(std::vector<ClusHistogram::clu_info> input_cluster_vec)
{
    std::vector<ClusHistogram::clu_info> output_cluster_vec; output_cluster_vec.clear();

    for (ClusHistogram::clu_info this_clu : input_cluster_vec)
    {
        std::pair<double,double> rotated_xy = rotatePoint(this_clu.x, this_clu.y);
     
        ClusHistogram::clu_info rotated_clu = this_clu;
        rotated_clu.x = rotated_xy.first;
        rotated_clu.y = rotated_xy.second;

        rotated_clu.eta_INTTz = ClusHistogram::get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {rotated_clu.x, rotated_clu.y, rotated_clu.z});

        output_cluster_vec.push_back(rotated_clu);
    }

    return output_cluster_vec;
}

std::pair<double,double> InttDoubletMap::rotatePoint(double x, double y)
{
    // Convert the rotation angle from degrees to radians
    double rotation = rotate_phi_angle; // todo rotation is here
    double angleRad = rotation * M_PI / 180.0;

    // Perform the rotation
    double xOut = x * cos(angleRad) - y * sin(angleRad);
    double yOut = x * sin(angleRad) + y * cos(angleRad);

    xOut = (fabs(xOut) < 0.0000001) ? 0 : xOut;
    yOut = (fabs(yOut) < 0.0000001) ? 0 : yOut;

    // cout<<"Post rotation: "<<xOut<<" "<<yOut<<endl;

    return {xOut,yOut};
}

std::pair<double,double> InttDoubletMap::Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1) // note : inner p0, outer p1, vector {r,z, zid}, -> {y,x}
{
    std::vector<double> p0_z_edge = { ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] - typeA_sensor_half_length_incm : p0[1] - typeB_sensor_half_length_incm, ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] + typeA_sensor_half_length_incm : p0[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}
    std::vector<double> p1_z_edge = { ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] - typeA_sensor_half_length_incm : p1[1] - typeB_sensor_half_length_incm, ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] + typeA_sensor_half_length_incm : p1[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}

    double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
    double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

    double mid_point = (edge_first + edge_second) / 2.;
    double possible_width = fabs(edge_first - edge_second) / 2.;

    return {mid_point, possible_width}; // note : first : mid point, second : width

}

// note : angle_1 = inner clu phi
// note: angle_2 = outer clu phi
double InttDoubletMap::get_delta_phi(double angle_1, double angle_2)
{
    std::vector<double> vec_abs = {std::fabs(angle_1 - angle_2), std::fabs(angle_1 - angle_2 + 360), std::fabs(angle_1 - angle_2 - 360)};
    std::vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double InttDoubletMap::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

// note : pair<reduced chi2, eta of the track>
// note : vector : {r,z}
// note : p0 : vertex point {r,z,zerror}
// note : p1 : inner layer
// note : p2 : outer layer
std::pair<double,double> InttDoubletMap::Get_eta(std::vector<double>p0, std::vector<double>p1, std::vector<double>p2)
{
    // if (track_gr -> GetN() != 0) {cout<<"In InttDoubletMap class, track_gr is not empty, track_gr size : "<<track_gr -> GetN()<<endl; exit(0);}
    
    if (track_gr -> GetN() != 0) {track_gr -> Set(0);}
    
    std::vector<double> r_vec  = {p0[0], p1[0], p2[0]}; 
    std::vector<double> z_vec  = {p0[1], p1[1], p2[1]}; 
    std::vector<double> re_vec = {0, 0, 0}; 
    std::vector<double> ze_vec = {
        p0[2],
        (p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm,
        (p2[2] == typeA_sensorZID[0] || p2[2] == typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm
    };

    // std::vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < sensor_type_segment ) ? sensor_length_typeA : sensor_length_typeB, ( fabs( p2[1] ) < sensor_type_segment ) ? sensor_length_typeA : sensor_length_typeB}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    for (int i = 0; i < 3; i++)
    {
        track_gr -> SetPoint(i,r_vec[i],z_vec[i]);
        track_gr -> SetPointError(i,re_vec[i],ze_vec[i]);

        // cout<<"In InttDoubletMap class, point : "<<i<<" r : "<<r_vec[i]<<" z : "<<z_vec[i]<<" re : "<<re_vec[i]<<" ze : "<<ze_vec[i]<<endl;
    }    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        std::pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};
    }

}

double InttDoubletMap::grEY_stddev(TGraphErrors * input_grr)
{
    std::vector<double> input_vector; input_vector.clear();
    for (int i = 0; i < input_grr -> GetN(); i++) {input_vector.push_back( input_grr -> GetPointY(i) );}

    double average = std::accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());

    double sum_subt = 0;
	for (int ele : input_vector) {sum_subt+=pow((ele-average),2);}
	
	return sqrt(sum_subt/(input_vector.size()-1));
}	

std::pair<double, double> InttDoubletMap::mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

double InttDoubletMap::Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
{
    if ( fabs(p0x - p1x) < 0.000001 ){ // note : the line is vertical (if z is along the x axis)
        return p0x;
    }
    else {
        double slope = (p1y - p0y) / (p1x - p0x);
        double yIntercept = p0y - slope * p0x;
        double xCoordinate = (given_y - yIntercept) / slope;
        return xCoordinate;
    }
}