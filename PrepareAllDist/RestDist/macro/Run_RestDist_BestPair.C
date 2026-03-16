#include "../RestDist.h"

R__LOAD_LIBRARY(../libRestDist.so)

void Run_RestDist_BestPair(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0220728, 0.222956},

  bool Apply_cut = true, // note : vtxZQA
  bool ApplyVtxZReWeighting = false,
  std::pair<bool, int> ApplyEvtBcoFullDiffCut = {true, 61},
  std::pair<bool, std::pair<double,double>> RequireVtxZRange = {true, {-10, 10}},
  std::pair<bool, std::pair<double,double>> isClusQA = {false, {35, 40}}, // note : adc, phi size
  bool isRotated = false,
  bool isCheckBestPair = true
)
{

  string final_output_file_name;
  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and no ClusQA, no inner rotated
  RestDist * RDs1 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut,
    
    RequireVtxZRange,
    isClusQA,
    isRotated, //note: isRotated
    isCheckBestPair
  );

  final_output_file_name = RDs1->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs1->PrepareEvent();
  RDs1->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
