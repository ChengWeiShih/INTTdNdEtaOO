#include "../EvtVtxZLeftRight.h"

R__LOAD_LIBRARY(../libEvtVtxZLeftRight.so)

void Run_EvtVtxZLeftRight(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  
  bool IsFieldOn = true,
  bool IsDCACutApplied = 1,
  std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-1.146, 1.146},{-1000.,1000.}}, // note : in degree
  std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm = {{-0.7, 0.7},{-1000.,1000.}}, // note : in cm
  int ClusAdcCut = 15, // Division: 15 for MC and 30 for Data
  int ClusPhiSizeCut = 40,
  
  bool PrintRecoDetails = 1,
  bool DrawEvtVtxZ = 1,

  bool RunInttBcoFullDiff = true,
  bool RunVtxZReco = 1,
  bool RunTrackletPair = false,
  bool RunTrackletPairRotate = false,
  
  bool HaveGeoOffsetTag = false
)
{

  // todo :
  // std::pair<double, double> vertexXYIncm_MC = {-0.0218978, 0.223183}; // note : in cm // note : MC HIJING
  // std::pair<double, double> vertexXYIncm_MC = {-0.0216964, 0.223331}; // note : in cm // note : MC HIJING + strangeness
  std::pair<double, double> vertexXYIncm_MC = {-0.0059375, -0.0025}; // note : in cm // note : MC HIJING, {MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest}
  // std::pair<double, double> vertexXYIncm_MC = {-0.021907, 0.223293}; // note : in cm // note : MC EPOS

  std::pair<double, double> vertexXYIncm_data = {-0.0537813, 0.137281}; // note : in cm // note : data {test_82391_INTTsurveyOnly/AvgVtxXY}

  std::pair<double, double> vertexXYIncm = (run_num == -1) ? vertexXYIncm_MC : vertexXYIncm_data;

  std::cout<<"RunNumber: "<<run_num<<std::endl;
  std::cout<<"ProcessID: "<<process_id<<std::endl;
  std::cout<<"InputVtxXY: "<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<std::endl;

  EvtVtxZLeftRight * evzpt_left = new EvtVtxZLeftRight(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,
    output_file_name_suffix,

    vertexXYIncm,
    IsFieldOn,
    IsDCACutApplied,
    DeltaPhiCutInDegree,
    DCAcutIncm,
    ClusAdcCut,
    ClusPhiSizeCut,
      
    PrintRecoDetails,
    DrawEvtVtxZ,

    RunInttBcoFullDiff,
    RunVtxZReco,
    RunTrackletPair,
    RunTrackletPairRotate,
    true, // note : isLeftOnly
      
    HaveGeoOffsetTag
  );

  string final_output_file_name_left = evzpt_left->GetOutputFileName();
  string plot_final_output_file_name_left = "Plot_"+final_output_file_name_left;
  cout<<"final_output_file_name_left: "<<final_output_file_name_left<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name_left.c_str(), output_directory.c_str(), final_output_file_name_left.c_str()));  
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), plot_final_output_file_name_left.c_str(), output_directory.c_str(), plot_final_output_file_name_left.c_str()));  

  evzpt_left -> MainProcess();
  evzpt_left -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name_left.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), plot_final_output_file_name_left.c_str(), output_directory.c_str()));


  string input_directory_for_right = output_directory + "/completed";
  string input_filename_for_right = final_output_file_name_left;

  EvtVtxZLeftRight * evzpt_right = new EvtVtxZLeftRight(
    process_id,
    run_num,
    nevents,
    input_directory_for_right,
    input_filename_for_right,
    output_directory,
    output_file_name_suffix,

    vertexXYIncm,
    IsFieldOn,
    IsDCACutApplied,
    DeltaPhiCutInDegree,
    DCAcutIncm,
    ClusAdcCut,
    ClusPhiSizeCut,
      
    PrintRecoDetails,
    DrawEvtVtxZ,

    RunInttBcoFullDiff,
    RunVtxZReco,
    RunTrackletPair,
    RunTrackletPairRotate,
    false, // note : isLeftOnly
      
    HaveGeoOffsetTag
  );

  string final_output_file_name_right = evzpt_right->GetOutputFileName();
  string plot_final_output_file_name_right = "Plot_"+final_output_file_name_right;
  cout<<"final_output_file_name_right: "<<final_output_file_name_right<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name_right.c_str(), output_directory.c_str(), final_output_file_name_right.c_str()));  
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), plot_final_output_file_name_right.c_str(), output_directory.c_str(), plot_final_output_file_name_right.c_str()));  

  evzpt_right -> MainProcess();
  evzpt_right -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name_right.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), plot_final_output_file_name_right.c_str(), output_directory.c_str()));

  return;
}
