#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/InttDoubletMap/InttDoubletMap.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/InttDoubletMap/libInttDoubletMap.so)

TH2D * GetGoodColMap(std::string ColMulMask_map_dir_in, std::string ColMulMask_map_file_in, std::string map_name_in)
{
  TFile * f = TFile::Open(Form("%s/%s", ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
  TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
  return h;
}

TH1D * GetTH1D(std::string h1D_input_directory, std::string h1D_filename, std::string h1D_name)
{
  TFile * f1 = TFile::Open(Form("%s/%s", h1D_input_directory.c_str(), h1D_filename.c_str()));
  TH1D * h1 = (TH1D*)f1->Get(h1D_name.c_str());
  return h1;
}

int Run_PrepareHist_template(
  int process_id = 8,
  int run_num = -1,
  int nevents = -1,
  string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/EvtVtxZ/completed",
  string input_filename = "MC_EvtVtxZProtoTracklet_FieldOn_VtxZReco_00008.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/DoubletCount_test/NewMethod"
)
{
  
  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  

  std::string ColMulMask_map_mother_dir = PassParams::ColMulMask_map_dir;
  std::string ColMulMask_map_file = PassParams::ColMulMask_map_file;

  std::string zvtx_weight_dir  = PassParams::zvtx_weight_dir;
  std::string zvtx_weight_file = PassParams::zvtx_weight_file;
  std::string zvtx_weight_hist = PassParams::zvtx_weight_hist;

  std::string TrigEffiWeight_dir = PassParams::TrigEffiWeight_dir;
  std::string TrigEffiWeight_file = PassParams::TrigEffiWeight_file;
  std::string TrigEffiWeight_hist = PassParams::TrigEffiWeight_hist;

  // todo : modify here
  std::string output_file_name_suffix = PassParams::output_file_name_suffix;
  std::pair<double, double> vertexXYIncm = PassParams::vertexXYIncm;

  int data_type_in = PassParams::data_type; // note : 0 pure_trigger, 1 streaming_trigger, 2 streaming_data
  bool isUsedMBDz_in = PassParams::isUsedMBDz;

  bool BcoFullDiffCut_in = PassParams::BcoFullDiffCut;
  int CentralityBin_in = PassParams::CentralityBin;
  bool isMinBiasCut_in = PassParams::isMinBiasCut;
  bool isTriggerSel_in = PassParams::isTriggerSel;
  std::pair<bool, std::pair<int,int>> isMBDChargeCut_in = PassParams::isMBDChargeCut;
  std::pair<bool, std::pair<int,int>> isBunchNumber_cut_in = PassParams::isBunchNumber_cut;

  // std::pair<bool, TH1D*> vtxZReweight_in = {false, nullptr};
  bool IsTrigEffiWeight_in = PassParams::IsTrigEffiWeight;
  bool vtxZReweight_in = PassParams::vtxZReweight;
  bool INTT_vtxZ_QA_in = PassParams::INTT_vtxZ_QA;
  std::pair<double, double> VtxZRange_in = PassParams::VtxZRange;

  bool ColMulMask_in = PassParams::ColMulMask;
  std::pair<bool, std::pair<double, double>> isClusQA_in = PassParams::isClusQA; // note : {adc, phi size}
  double DeltaPhiCut_in = PassParams::DeltaPhiCut;

  bool HaveGeoOffsetTag_in = PassParams::HaveGeoOffsetTag;

  // Division: ── PassParams confirmation print ──────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_PrepareHist_template.C" << std::endl;
  std::cout << "============================================================" << std::endl;

  std::cout << "  -- per-job arguments --" << std::endl;
  std::cout << "  process_id       : " << process_id       << std::endl;
  std::cout << "  run_num          : " << run_num          << std::endl;
  std::cout << "  nevents          : " << nevents          << std::endl;
  std::cout << "  input_directory  : " << input_directory  << std::endl;
  std::cout << "  input_filename   : " << input_filename   << std::endl;
  std::cout << "  output_directory : " << output_directory << std::endl;

  std::cout << "  -- trigger correction reweight --" << std::endl;
  std::cout << "IsTrigEffiWeight_in: "<< IsTrigEffiWeight_in <<std::endl;
  std::cout << "TrigEffiWeight_dir: "<< TrigEffiWeight_dir <<std::endl;
  std::cout << "TrigEffiWeight_file: "<< TrigEffiWeight_file <<std::endl;
  std::cout << "TrigEffiWeight_hist: "<< TrigEffiWeight_hist <<std::endl;

  std::cout << "  -- vtxZ reweight --" << std::endl;
  std::cout << "  vtxZReweight     : " << vtxZReweight_in  << std::endl;
  std::cout << "  zvtx_weight_dir  : " << zvtx_weight_dir  << std::endl;
  std::cout << "  zvtx_weight_file : " << zvtx_weight_file << std::endl;
  std::cout << "  zvtx_weight_hist : " << zvtx_weight_hist << std::endl;

  std::cout << "  -- ColMulMask map --" << std::endl;
  std::cout << "  ColMulMask_map_dir  : " << ColMulMask_map_mother_dir << std::endl;
  std::cout << "  ColMulMask_map_file : " << ColMulMask_map_file       << std::endl;

  std::cout << "  -- InttDoubletMap constructor parameters --" << std::endl;
  std::cout << "  output_file_name_suffix : " << output_file_name_suffix          << std::endl;
  std::cout << "  vertexXYIncm     : (" << vertexXYIncm.first << ", " << vertexXYIncm.second << ")" << std::endl;
  std::cout << "  data_type        : " << data_type_in    << "  (0=pure_trig, 1=stream_trig, 2=stream_data)" << std::endl;
  std::cout << "  isUsedMBDz       : " << isUsedMBDz_in   << std::endl;
  std::cout << "  BcoFullDiffCut   : " << BcoFullDiffCut_in  << std::endl;
  std::cout << "  CentralityBin    : " << CentralityBin_in   << std::endl;
  std::cout << "  isMinBiasCut     : " << isMinBiasCut_in    << std::endl;
  std::cout << "  isTriggerSel     : " << isTriggerSel_in    << std::endl;
  std::cout << "  isMBDChargeCut   : (" << isMBDChargeCut_in.first
            << ", (" << isMBDChargeCut_in.second.first << ", " << isMBDChargeCut_in.second.second << "))" << std::endl;
  std::cout << "  isBunchNumber_cut: (" << isBunchNumber_cut_in.first
            << ", (" << isBunchNumber_cut_in.second.first << ", " << isBunchNumber_cut_in.second.second << "))" << std::endl;
  std::cout << "  INTT_vtxZ_QA     : " << INTT_vtxZ_QA_in << std::endl;
  std::cout << "  VtxZRange        : (" << VtxZRange_in.first << ", " << VtxZRange_in.second << ")" << std::endl;
  std::cout << "  ColMulMask       : " << ColMulMask_in   << std::endl;
  std::cout << "  isClusQA         : (" << isClusQA_in.first
            << ", (" << isClusQA_in.second.first << ", " << isClusQA_in.second.second << "))"
            << "  (adc, phi_size)" << std::endl;
  std::cout << "  DeltaPhiCut      : " << DeltaPhiCut_in  << std::endl;
  std::cout << "  HaveGeoOffsetTag : " << HaveGeoOffsetTag_in << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  // Division: ── vtxZReweight construction ───────────────────────────────────
  std::pair<bool, TH1D*> vtxZReweight_final = {vtxZReweight_in, nullptr};
  std::pair<bool, TH1D*> TrigCorrWeight_final = {IsTrigEffiWeight_in, nullptr};

  if (vtxZReweight_final.first){
    vtxZReweight_final.second = GetTH1D(
      zvtx_weight_dir, zvtx_weight_file, zvtx_weight_hist
    );
  }

  if (vtxZReweight_final.first && vtxZReweight_final.second == nullptr){
    std::cout<<"no vtxZReweight map, kill the job"<<std::endl;
    return 666;
  }

  if (TrigCorrWeight_final.first){
    TrigCorrWeight_final.second = GetTH1D(
      TrigEffiWeight_dir, TrigEffiWeight_file, TrigEffiWeight_hist
    );
  }

  if (TrigCorrWeight_final.first && TrigCorrWeight_final.second == nullptr){
    std::cout<<"no TrigCorrWeight map, kill the job"<<std::endl;
    return 666;
  }
  

  std::string final_output_directory = output_directory;
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));


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
    isMinBiasCut_in,
    isTriggerSel_in,
    isMBDChargeCut_in,
    isBunchNumber_cut_in,

    TrigCorrWeight_final,
    vtxZReweight_final,
    INTT_vtxZ_QA_in,
    VtxZRange_in,

    ColMulMask_in,
    isClusQA_in,
    DeltaPhiCut_in,

    HaveGeoOffsetTag_in
  );

  if (ColMulMask_in){
    TLHN -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_mother_dir, ColMulMask_map_file, TLHN->GetGoodColMapName())
    );
  }

  string final_output_file_name = TLHN->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str(), final_output_file_name.c_str()));  

  TLHN -> MainProcess();
  TLHN -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str()));

  return 888;
}


// PassConfig.py -> my_new_flag: bool = False

// generate_params_header() in run_pass.py -> f'  const bool        my_new_flag             = {_cpp_bool(cfg.my_new_flag)};',

// Run_PrepareHist_template.C -> bool my_new_flag_in = PassParams::my_new_flag;
// Run_PrepareHist_template.C -> the cout

// pass_configs/*.py