#include "../EvtVtxZProtoTracklet.h"

R__LOAD_LIBRARY(../libEvtVtxZProtoTracklet.so)

void Run_EvtVtxZTracklet(
  int process_id = 0,
  int run_num = 54679,
  int nevents = 20,
  string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_HV/54679/completed",
  string input_filename = "Ntuple_00054679_standalone_00000.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_HV/54679/EvtVtxZ",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  
  bool IsFieldOn = true,
  bool IsDCACutApplied = 1,
  std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-1.146, 1.146},{-1000.,1000.}}, // note : in degree
  std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm = {{-0.7, 0.7},{-1000.,1000.}}, // note : in cm
  // int ClusAdcCut = 30, // Division: 15 for MC, 30 from Data
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
  std::pair<double, double> vertexXYIncm_MC = {-0.053125, 0.13593800}; // note : in cm // note : MC HIJING, {MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest}
  // std::pair<double, double> vertexXYIncm_MC = {-0.021907, 0.223293}; // note : in cm // note : MC EPOS

  std::pair<double, double> vertexXYIncm_data = {-0.0537813, 0.137281}; // note : in cm // note : data {test_82391_INTTsurveyOnly/AvgVtxXY}

  std::pair<double, double> vertexXYIncm = (run_num == -1) ? vertexXYIncm_MC : vertexXYIncm_data;

  int ClusAdcCut = (run_num == -1) ? 15 : 30; // Division: 15 for MC, 30 from Data

  std::cout<<"RunNumber: "<<run_num<<std::endl;
  std::cout<<"ProcessID: "<<process_id<<std::endl;
  std::cout<<"InputVtxXY: "<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<std::endl;
  std::cout<<"ClusAdcCut: "<<ClusAdcCut<<std::endl;

  EvtVtxZProtoTracklet * evzpt = new EvtVtxZProtoTracklet(
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
      
    HaveGeoOffsetTag
  );

  string final_output_file_name = evzpt->GetOutputFileName();
  string plot_final_output_file_name = "Plot_"+final_output_file_name;
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str(), plot_final_output_file_name.c_str()));  

  evzpt -> MainProcess();
  evzpt -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
