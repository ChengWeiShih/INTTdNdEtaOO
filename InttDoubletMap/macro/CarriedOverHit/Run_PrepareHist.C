#include "../../InttDoubletMap.h"

R__LOAD_LIBRARY(../../libInttDoubletMap.so)

TH2D * GetGoodColMap (std::string ColMulMask_map_dir_in, std::string ColMulMask_map_file_in, std::string map_name_in)
{
  TFile * f = TFile::Open(Form("%s/%s", ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
  TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
  return h;
}

std::map<int, std::pair<int,int>> BunchNumber_range_map = {
    {0,  {0,   1}},
    {1,  {2,   3}},
    {2,  {4,   5}},
    {3,  {6,   7}},
    {4,  {8,   9}},
    {5,  {10, 11}},
    {6,  {12, 13}},
    {7,  {14, 15}},
    {8,  {16, 17}},
    {9,  {18, 19}},
    {10, {20, 21}},
    {11, {22, 23}},
    {12, {24, 25}},
    {13, {26, 27}},
    {14, {28, 29}},
    {15, {30, 31}},
    {16, {32, 33}},
    {17, {34, 35}},
    {18, {36, 37}},
    {19, {38, 39}},
    {20, {40, 41}},
    {21, {42, 43}},
    {22, {44, 45}},
    {23, {46, 47}},
    {24, {48, 49}},
    {25, {50, 51}},
    {26, {52, 53}},
    {27, {54, 55}},
    {28, {56, 57}},
    {29, {58, 59}},
    {30, {60, 61}},
    {31, {62, 63}},
    {32, {64, 65}},
    {33, {66, 67}},
    {34, {68, 69}},
    {35, {70, 71}},
    {36, {72, 73}},
    {37, {74, 75}},
    {38, {76, 77}},
    {39, {78, 79}},
    {40, {80, 81}},
    {41, {82, 83}},
    {42, {84, 85}},
    {43, {86, 87}},
    {44, {88, 89}},
    {45, {90, 91}},
    {46, {92, 93}},
    {47, {94, 95}},
    {48, {96, 97}},
    {49, {98, 99}},
    {50, {100, 101}},
    {51, {102, 103}},
    {52, {104, 105}},
    {53, {106, 107}},
    {54, {108, 109}},
    {55, {110, 111}}
};


int Run_PrepareHist(
  int process_id = 0,
  int run_num = 82420,
  int nevents = -1,
  string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/CarryOverHit/82420/completed",
  string input_filename = "Ntuple_convertTrigMbdZ_00082420_standalone_ana538_Acts_00000.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/CarryOverHit/82420/InttDoublets",
  
  int BunchNumber_range_id = 0,
  int MBDChargeSum_range_id = 0,

  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.03662,0.1482},

  int data_type_in = 1, // note : pure_trigger 0 //// streaming_trigger 1 //// streaming_data 2
  bool isUsedMBDz_in = true,

  bool BcoFullDiffCut_in = false,
  int CentralityBin_in = -1, 
  bool is_min_bias_in = false,
  bool isTriggerSel_in = false,
  // std::pair<bool, std::pair<int,int>> isBunchNumber_cut_in
  // std::pair<bool, std::pair<int,int>> isMBDChargeCut_in = {true, {101,150}},

  std::pair<bool, TH1D*> vtxZReweight_in = {false, nullptr},
  bool INTT_vtxZ_QA_in = false,
  std::pair<double, double> VtxZRange_in = {-25, 25},

  bool ColMulMask_in = false,
  std::pair<bool, std::pair<double, double>> isClusQA_in = {true, {0, 128}},
  double DeltaPhiCut_in = 0.15,

  bool HaveGeoOffsetTag_in = false,

  std::string ColMulMask_map_mother_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/ColumnCheck"

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_ClusQAAdc35PhiSize500_00054280.root"

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck_NoClusQA/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_00054280.root"
  
  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/ColumnCheck_noPhiCut/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize350_00054280.root"
)
{
  // std::map<int, std::pair<int,int>> BunchNumber_range_map = {
  //   {0,  {0  , 9}},
  //   {1,  {10, 19}},
  //   {2,  {20, 29}},
  //   {3,  {30, 39}},
  //   {4,  {40, 49}},
  //   {5,  {50, 59}},
  //   {6,  {60, 69}},
  //   {7,  {70, 79}},
  //   {8,  {80, 89}},
  //   {9,  {90, 99}},
  //   {10, {100, 110}}
  // };

  // std::map<int, std::pair<int,int>> BunchNumber_range_map = {
  //     {0,  {0,   4}},
  //     {1,  {5,   9}},
  //     {2,  {10, 14}},
  //     {3,  {15, 19}},
  //     {4,  {20, 24}},
  //     {5,  {25, 29}},
  //     {6,  {30, 34}},
  //     {7,  {35, 39}},
  //     {8,  {40, 44}},
  //     {9,  {45, 49}},
  //     {10, {50, 54}},
  //     {11, {55, 59}},
  //     {12, {60, 64}},
  //     {13, {65, 69}},
  //     {14, {70, 74}},
  //     {15, {75, 79}},
  //     {16, {80, 84}},
  //     {17, {85, 89}},
  //     {18, {90, 94}},
  //     {19, {95, 99}},
  //     {20, {100, 104}},
  //     {21, {105, 110}}
  // };

  std::map<int, std::pair<bool, std::pair<int,int>>> isMBDChargeCut_map = {
    {0, {false, {101, 150}}},
    {1, {true,  {10,   49}}},
    {2, {true,  {50,  99}}},
    {3, {true,  {100, 149}}},
    {4, {true,  {150, 300}}}
  };

  std::pair<bool, std::pair<int,int>> isMBDChargeCut_in = isMBDChargeCut_map[MBDChargeSum_range_id];


  std::pair<bool, std::pair<int,int>> isBunchNumber_cut_in = {true, BunchNumber_range_map[BunchNumber_range_id]};

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : baseline
  std::string final_output_directory = output_directory + Form("/BunchNumber_range_%d",BunchNumber_range_id);
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  std::pair<bool, std::pair<double, double>> isClusQA = {true, {0,128}}; // note : {adc, phi size}

  std::string ColMulMask_map_dir = ColMulMask_map_mother_dir + "/baseline/completed/MulMap/completed";
  std::string ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00082391.root", isClusQA.second.first, isClusQA.second.second);

  InttDoubletMap * TLHN = new InttDoubletMap(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    data_type_in,
    isUsedMBDz_in,

    BcoFullDiffCut_in,
    CentralityBin_in,
    is_min_bias_in,
    isTriggerSel_in,
    isMBDChargeCut_in,
    isBunchNumber_cut_in,

    vtxZReweight_in,
    INTT_vtxZ_QA_in,
    VtxZRange_in,

    ColMulMask_in,
    isClusQA_in,
    DeltaPhiCut_in,

    HaveGeoOffsetTag_in
  );

  if (ColMulMask_in){
    TLHN -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN->GetGoodColMapName())
    );
  }

  string final_output_file_name = TLHN->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str(), final_output_file_name.c_str()));  

  TLHN -> MainProcess();
  TLHN -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str()));

  // // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // // note : noAdcCut
  // final_output_directory = output_directory + "/noAdcCut";
  // system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  // isClusQA = {true, {0,40}}; // note : {adc, phi size}

  // ColMulMask_map_dir = ColMulMask_map_mother_dir + "/noAdcCut/completed/MulMap/completed";
  // ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  // TrackletHistogramNew * TLHN1 = new TrackletHistogramNew(
  //   process_id,
  //   run_num,
  //   nevents,
  //   input_directory,
  //   input_filename,
  //   final_output_directory,

  //   output_file_name_suffix,
  //   vertexXYIncm,

  //   vtxZReweight,
  //   BcoFullDiffCut,
  //   INTT_vtxZ_QA,
  //   isClusQA, // note : {adc, phi size}
  //   HaveGeoOffsetTag,
  //   SetRandomHits,
  //   RandInttZ,
  //   ColMulMask
  // );

  // if (ColMulMask){
  //   TLHN1 -> SetGoodColMap(
  //     GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN1->GetGoodColMapName())
  //   );
  // }

  // string final_output_file_name1 = TLHN1->GetOutputFileName();
  // cout<<"final_output_file_name1: "<<final_output_file_name1<<endl;
  // system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str(), final_output_file_name1.c_str()));  

  // TLHN1 -> MainProcess();
  // TLHN1 -> EndRun();

  // system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str()));

  // // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // // note : 50AdcCut
  // final_output_directory = output_directory + "/50AdcCut";
  // system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  // isClusQA = {true, {50,40}}; // note : {adc, phi size}

  // ColMulMask_map_dir = ColMulMask_map_mother_dir + "/50AdcCut/completed/MulMap/completed";
  // ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  // TrackletHistogramNew * TLHN2 = new TrackletHistogramNew(
  //   process_id,
  //   run_num,
  //   nevents,
  //   input_directory,
  //   input_filename,
  //   final_output_directory,

  //   output_file_name_suffix,
  //   vertexXYIncm,

  //   vtxZReweight,
  //   BcoFullDiffCut,
  //   INTT_vtxZ_QA,
  //   isClusQA, // note : {adc, phi size}
  //   HaveGeoOffsetTag,
  //   SetRandomHits,
  //   RandInttZ,
  //   ColMulMask
  // );

  // if (ColMulMask){
  //   TLHN2 -> SetGoodColMap(
  //     GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN2->GetGoodColMapName())
  //   );
  // }

  // string final_output_file_name2 = TLHN2->GetOutputFileName();
  // cout<<"final_output_file_name2: "<<final_output_file_name2<<endl;
  // system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str(), final_output_file_name2.c_str()));  

  // TLHN2 -> MainProcess();
  // TLHN2 -> EndRun();

  // system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str()));

  // // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // // note : noPhiCut
  // final_output_directory = output_directory + "/noPhiCut";
  // system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  // isClusQA = {true, {35,350}}; // note : {adc, phi size}

  // ColMulMask_map_dir = ColMulMask_map_mother_dir + "/noPhiCut/completed/MulMap/completed";
  // ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  // TrackletHistogramNew * TLHN3 = new TrackletHistogramNew(
  //   process_id,
  //   run_num,
  //   nevents,
  //   input_directory,
  //   input_filename,
  //   final_output_directory,

  //   output_file_name_suffix,
  //   vertexXYIncm,

  //   vtxZReweight,
  //   BcoFullDiffCut,
  //   INTT_vtxZ_QA,
  //   isClusQA, // note : {adc, phi size}
  //   HaveGeoOffsetTag,
  //   SetRandomHits,
  //   RandInttZ,
  //   ColMulMask
  // );

  // if (ColMulMask){
  //   TLHN3 -> SetGoodColMap(
  //     GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN3->GetGoodColMapName())
  //   );
  // }

  // string final_output_file_name3 = TLHN3->GetOutputFileName();
  // cout<<"final_output_file_name3: "<<final_output_file_name3<<endl;
  // system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str(), final_output_file_name3.c_str()));  

  // TLHN3 -> MainProcess();
  // TLHN3 -> EndRun();

  // system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str()));

  return 888;
}
