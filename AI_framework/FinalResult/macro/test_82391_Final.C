#include "../DoOnedNdEta.cpp"
#include "../EachVariation.cpp"
#include "../GetFinaldNdEta.cpp"
#include "../../../Constants.cpp"

#include <glob.h>
#include <iostream>
#include <string>

std::string GetSingleMatchedFile(const std::string& file_pattern)
{
    glob_t glob_result{};

    const int status = glob(file_pattern.c_str(), 0, nullptr, &glob_result);

    if (status != 0)
    {
        std::cerr << "Error: no file matches pattern: "
                  << file_pattern << std::endl;

        globfree(&glob_result);
        return "";
    }

    if (glob_result.gl_pathc != 1)
    {
        std::cerr << "Error: expected exactly one file matching: "
                  << file_pattern
                  << ", but found " << glob_result.gl_pathc
                  << std::endl;

        globfree(&glob_result);
        return "";
    }

    const std::string matched_file = glob_result.gl_pathv[0];

    globfree(&glob_result);

    return matched_file;
}

int test_82391_Final(int Mbin = 0){
    
    std::string mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO";

    std::string input_data         = mother_directory + "/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/InttDoublets";
    std::string input_data_AlingP  = mother_directory + "/test_82391_LoacalPos_PrivateCentrality/InttDoublets";

    std::string MC_sub_dir = "MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex";
    std::string input_MC           = mother_directory + "/" + MC_sub_dir + "/InttDoublets";
    std::string input_MC_AlingP    = mother_directory + "/" + MC_sub_dir + "/InttDoublets_WithAlignP";

    // std::string MC_EPOS_sub_dir = "";
    // std::string input_MC_EPOS   = mother_directory + "/" + MC_sub_dir + "/InttDoublets";
    // std::string MC_AMPT_sub_dir = "";
    // std::string input_MC_AMPT   = mother_directory + "/" + MC_sub_dir + "/InttDoublets";

    std::string output_dir = input_data + "/FinaldNdEta";

    std::pair<int,int> centrality_range = {Constants::centrality_edges[Mbin]-1, Constants::centrality_edges[Mbin+1]-1};
    std::pair<int,int> zvtx_range = {-10, 10};
    std::string sPHENIX_label = "Internal";
    std::string Collision_system = "O+O #sqrt{s_{NN}} = 200 GeV";

    std::pair<double,double> each_dNdEta_eta_range = {-1.1, 1.1};
    std::pair<double,double> each_variation_x_range = {-1.5, 1.5};

    // Division: ---------------------------------------------------------------------------------
    std::string sub_folder_str = "baseline";
    DoOnedNdEta * baseline = new DoOnedNdEta(
        Form("%s_%d", sub_folder_str.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_correction",
        "h1D_Truth_ChargedHadron_dNdEta"
    );
        
    // Division: ---------------------------------------------------------------------------------
    // note : NoClusPhiCut
    sub_folder_str = "NoClusPhiCut";
    DoOnedNdEta * NoClusPhiCut = new DoOnedNdEta(
        Form("%s_%d", sub_folder_str.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : DeltaPhiLoose
    sub_folder_str = "DeltaPhiLoose";
    DoOnedNdEta * DeltaPhiLoose = new DoOnedNdEta(
        Form("%s_%d", sub_folder_str.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : DeltaPhiTight
    sub_folder_str = "DeltaPhiTight";
    DoOnedNdEta * DeltaPhiTight = new DoOnedNdEta(
        Form("%s_%d", sub_folder_str.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : ClusADC, data 0 MC 0
    std::string sub_folder_str_data = "ClusADCLoose";
    std::string sub_folder_str_MC   = "ClusADCLoose";
    std::string case_name = "ClusADC_Loose_Loose";
    DoOnedNdEta * ClusADC_Loose_Loose = new DoOnedNdEta(
        Form("%s_%d", case_name.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str_data.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str_MC.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : ClusADC, data 35 MC 35
    sub_folder_str_data = "ClusADCTight";
    sub_folder_str_MC   = "ClusADCTight";
    case_name = "ClusADC_Tight_Tight";
    DoOnedNdEta * ClusADC_Tight_Tight = new DoOnedNdEta(
        Form("%s_%d", case_name.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str_data.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str_MC.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : ClusADC, data 35 MC 0
    sub_folder_str_data = "baseline";
    sub_folder_str_MC   = "ClusADCLoose";
    case_name = "ClusADC_baseline_Loose";
    DoOnedNdEta * ClusADC_baseline_Loose = new DoOnedNdEta(
        Form("%s_%d", case_name.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data.c_str(), sub_folder_str_data.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC.c_str(), sub_folder_str_MC.c_str(), Mbin)),
        "h1D_correction"
    );

    // Division: ---------------------------------------------------------------------------------
    // note : with Alignment parameter
    sub_folder_str_data = "baseline";
    sub_folder_str_MC   = "baseline";
    case_name = "baseline_AlignP";
    DoOnedNdEta * baseline_AlignP = new DoOnedNdEta(
        Form("%s_%d", case_name.c_str(), Mbin),
        each_dNdEta_eta_range,
        
        // note : data
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_data_AlingP.c_str(), sub_folder_str_data.c_str(), Mbin)),
        "h1D_GoodProtoTracklet_Eta_Subtracted_normalized",

        // ntoe : MC
        GetSingleMatchedFile(Form("%s/%s_%d/completed/Output*.root", input_MC_AlingP.c_str(), sub_folder_str_MC.c_str(), Mbin)),
        "h1D_correction"
    );
    



    // Division: ---------------------------------------------------------------------------------
    EachVariation * var_ClusPhiSize = new EachVariation(
        Mbin,
        centrality_range,
        zvtx_range,
        sPHENIX_label,
        Collision_system,
        output_dir,

        "var_ClusPhiSize",
        "Cluster #phi-size variation",

        {
            {"Baseline (Cluster #phi-size < 40)",   baseline->GetdNdEtaHist(), baseline->GetdNdEtagrE()},
            {"w/o Cluster #phi-size cut",   NoClusPhiCut->GetdNdEtaHist(), NoClusPhiCut->GetdNdEtagrE()}
        },

        each_variation_x_range
    );


    // Division: ---------------------------------------------------------------------------------
    EachVariation * var_DeltaPhi = new EachVariation(
        Mbin,
        centrality_range,
        zvtx_range,
        sPHENIX_label,
        Collision_system,
        output_dir,

        "var_DeltaPhi",
        "#Delta#phi variation",

        {
            {"Baseline (|#Delta#phi| < 0.15)",   baseline->GetdNdEtaHist(), baseline->GetdNdEtagrE()},
            {"|#Delta#phi| < 0.2",   DeltaPhiLoose->GetdNdEtaHist(), DeltaPhiLoose->GetdNdEtagrE()},
            {"|#Delta#phi| < 0.1",   DeltaPhiTight->GetdNdEtaHist(), DeltaPhiTight->GetdNdEtagrE()}
        },

        each_variation_x_range
    );


    // Division: ---------------------------------------------------------------------------------
    EachVariation * var_ClusADC = new EachVariation(
        Mbin,
        centrality_range,
        zvtx_range,
        sPHENIX_label,
        Collision_system,
        output_dir,

        "var_ClusADC",
        "Cluster ADC variation",

        {
            {"Baseline (Data: Cluster ADC > 30 & MC: Cluster ADC > 15)",   baseline->GetdNdEtaHist(), baseline->GetdNdEtagrE()},
            {"Data & MC: w/o cluster ADC cut",   ClusADC_Loose_Loose->GetdNdEtaHist(), ClusADC_Loose_Loose->GetdNdEtagrE()},
            {"Data & MC: Cluster ADC > 35",      ClusADC_Tight_Tight->GetdNdEtaHist(), ClusADC_Tight_Tight->GetdNdEtagrE()},
            {"Data: Cluster ADC > 30  & MC: No ClusADC cut", ClusADC_baseline_Loose->GetdNdEtaHist(), ClusADC_baseline_Loose->GetdNdEtagrE()}
        },

        each_variation_x_range
    );


    // Division: ---------------------------------------------------------------------------------
    EachVariation * var_AlignP = new EachVariation(
        Mbin,
        centrality_range,
        zvtx_range,
        sPHENIX_label,
        Collision_system,
        output_dir,

        "var_AlignP",
        "Offline geometry variation",

        {
            {"Baseline (Survey geometry only)",   baseline->GetdNdEtaHist(), baseline->GetdNdEtagrE()},
            {"Survey geo. + alignment parameters",   baseline_AlignP->GetdNdEtaHist(), baseline_AlignP->GetdNdEtagrE()}
        },

        each_variation_x_range
    );

    // Division: ---------------------------------------------------------------------------------
    GetFinaldNdEta * GFD = new GetFinaldNdEta(
        Mbin,
        centrality_range,
        zvtx_range,
        sPHENIX_label,
        Collision_system,
        output_dir
    );

    GFD->SetUpBaselinedNdEta(
        "Data (INTT-only, combinatoric approach)",
        baseline->GetdNdEtagrE(),
        baseline->GetdNdEtaHist()
    );

    GFD->SetUpMCdNdEta(
        "HIJING_dNdEta",
        "HIJING (generator)",
        baseline->GetMCTruthHist()
    );

    GFD->SetUpOneVariation(
        "var_ClusPhiSize",
        "ClusPhiSize variation",
        var_ClusPhiSize->GetMaxRelativeDiff()  
    );

    GFD->SetUpOneVariation(
        "var_DeltaPhi",
        "#Delta#phi variation",
        var_DeltaPhi->GetMaxRelativeDiff()
    );

    GFD->SetUpOneVariation(
        "var_ClusADC",
        "Cluster ADC variation",
        var_ClusADC->GetMaxRelativeDiff()
    );

    GFD->SetUpOneVariation(
        "var_AlignP",
        "Offline geometry variation",
        var_AlignP->GetMaxRelativeDiff()
    );

    GFD->PrepareFinalSystematicUnc();
    GFD->PrepareFinaldNdEta(-999,true);
    GFD->PrepareOutputRootFile();

    return 888;
}
