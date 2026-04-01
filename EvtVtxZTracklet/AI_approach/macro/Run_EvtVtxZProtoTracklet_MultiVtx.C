#include "../EvtVtxZProtoTracklet_MultiVtx.h"

R__LOAD_LIBRARY(../libEvtVtxZProtoTracklet_MultiVtx.so)

void Run_EvtVtxZProtoTracklet_MultiVtx(
  int process_id = 0,
  int run_num    = 82391,
  int nevents    = 200,
  string input_directory  = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos/completed",
  string input_filename   = "Ntuple_00082391_standalone_ana538_00000.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos/AllEvtVtxZ",

  // todo : modify here
  std::string output_file_name_suffix = "",

  bool IsFieldOn       = true,
  bool IsDCACutApplied = true,
  std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-1.146, 1.146},{-1000.,1000.}}, // note : in degree
  std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm          = {{-0.7,   0.7  },{-1000.,1000.}}, // note : in cm
  int ClusAdcCut     = 30, // note : 15 for MC, 35 for Data
  int ClusPhiSizeCut = 40,

  bool PrintRecoDetails = true,
  bool DrawEvtVtxZ      = true,

  bool RunInttBcoFullDiff  = true,
  bool RunVtxZReco         = true,
  bool RunTrackletPair     = false,
  bool RunTrackletPairRotate = false,

  bool HaveGeoOffsetTag = false
)
{
  // todo : update beam XY for the run of interest
  // note : MC HIJING INTTSurveyOnly
  std::pair<double, double> vertexXYIncm_MC   = {-0.053125, 0.135938};  // note : in cm
  // note : data run 82391 {test_82391_INTTsurveyOnly/AvgVtxXY}
  std::pair<double, double> vertexXYIncm_data = {-0.0537813,  0.137281}; // note : in cm

  std::pair<double, double> vertexXYIncm = (run_num == -1) ? vertexXYIncm_MC : vertexXYIncm_data;

  std::cout << "RunNumber : " << run_num     << std::endl;
  std::cout << "ProcessID : " << process_id  << std::endl;
  std::cout << "InputVtxXY: " << vertexXYIncm.first << ", " << vertexXYIncm.second << std::endl;

  EvtVtxZProtoTracklet_MultiVtx * reco = new EvtVtxZProtoTracklet_MultiVtx(
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

  string final_output_filename      = reco -> GetOutputFileName();
  string plot_final_output_filename = "Plot_" + final_output_filename;
  cout << "output file      : " << final_output_filename      << endl;
  cout << "output plot file : " << plot_final_output_filename  << endl;

  // note : remove stale output files in completed/ before running
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), final_output_filename.c_str(),
    output_directory.c_str(), final_output_filename.c_str()));
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), plot_final_output_filename.c_str(),
    output_directory.c_str(), plot_final_output_filename.c_str()));

  reco -> MainProcess();
  reco -> EndRun();

  // note : move output to completed/
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_filename.c_str(),      output_directory.c_str()));
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), plot_final_output_filename.c_str(), output_directory.c_str()));

  return;
}
