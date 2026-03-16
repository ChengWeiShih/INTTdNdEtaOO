#include "../GetFinalResult.h"

R__LOAD_LIBRARY(../libGetFinalResult.so)

int Run_82391_test(int condor_index)
{   
    std::vector<int> run_centrality_array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 70}; // note : 16 jobs in total

    int SelectedMbin = run_centrality_array[condor_index];

    int runnumber = 82391;
    // int SelectedMbin = 3; // note : 0, 1, ---- 10, 70, 100 
    std::pair<double,double> vtxZ_range = {-15,15}; // note : cm
    bool isTypeA = false;
    bool ApplyGeoAccCorr = false;
    std::pair<bool, std::pair<double,double>> cut_EtaRange = {true, {-1.5, 1.5}}; // note : not used

    std::string output_file_name_suffix = "";

    std::string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/FinalResult_15cm_Pol2BkgFit_DeltaPhi0p15";

    GetFinalResult * GFR = new GetFinalResult(
        runnumber,
        SelectedMbin,
        vtxZ_range,
        isTypeA,
        ApplyGeoAccCorr,
        cut_EtaRange,

        output_file_name_suffix,

        output_directory
    );

    GFR -> SetBaseLineDeltaPhiCut({-0.15,0.15}); // todo : change here

    std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly";
    std::string MC_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest";

    std::string SetResultRangeFolderName = GFR -> GetSetResultRangeFolderName();

    system(Form("if [ -d %s/completed/%s ]; then rm -r %s/completed/%s; fi;", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str(), SetResultRangeFolderName.c_str()));

    std::string baseline_subfolder = "TrackHist/baseline/completed";
    std::string baseline_subfolder_MC_001 = "TrackHist/baseline/completed"; // note : for the first half of data
    // std::string baseline_subfolder_MC_002 = "TrackHist_002/baseline/completed"; // note : for the second half of data 
    // note : here are for preparing the reco. dNdEta
    GFR -> PrepareBaseLine(
        data_input_directory + "/" + baseline_subfolder,
        "Data_TrackHist_BcoFullDiffCut_ClusQAAdc30PhiSize40_ColMulMask_00082391_merged.root",
        
        MC_input_directory + "/" + baseline_subfolder_MC_001,
        "MC_TrackHist_vtxZReweight_ClusQAAdc15PhiSize40_ColMulMask_merged_merged_001.root", // note : derive the alpha correction
        "MC_TrackHist_vtxZReweight_ClusQAAdc15PhiSize40_ColMulMask_merged_merged_002.root"  // note : closure test
    );


    system(Form("mv %s/%s %s/completed", output_directory.c_str(), SetResultRangeFolderName.c_str(), output_directory.c_str()));

    return 777;
}