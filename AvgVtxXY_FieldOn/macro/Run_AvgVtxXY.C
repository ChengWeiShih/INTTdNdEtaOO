#include "../AvgVtxXY.h"

R__LOAD_LIBRARY(../libavgvtxxy.so)

void Run_AvgVtxXY(
  int process_id = 0,
  int run_num = 54694,
  int nevents = -1,
  string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_DACScan/54694/completed",
  string input_filename = "Ntuple_00054694_standalone_00000.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_DACScan/54694/AvgVtxXY",
  
  // todo : modify here
  std::string output_file_name_suffix = "",

  std::pair<double,double> MBD_vtxZ_cut = {-20, 20}, // note : unit : cm
  std::pair<int,int> INTTNClus_cut = {20, 350},
  double ClusAdc_cut = 28,
  double ClusPhiSize_cut = 40,

  bool HaveGeoOffsetTag = false,
  double random_range_XYZ = 0.02, // note : unit : cm
  int random_seed = -999,
  std::string input_offset_map = "no_map" // note : full map
)
{

  AvgVtxXY * avgvtxxy = new AvgVtxXY(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,
    output_file_name_suffix,

    MBD_vtxZ_cut,
    INTTNClus_cut,
    ClusAdc_cut,
    ClusPhiSize_cut,

    HaveGeoOffsetTag,
    random_range_XYZ,
    random_seed,
    input_offset_map
  );

  string final_output_file_name = avgvtxxy->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  avgvtxxy -> PreparePairs();
  
  avgvtxxy -> FindVertexQuadrant( 1, 0.001, {0,0} ); // note : unit : cm
  // avgvtxxy -> FindVertexLineFill({-0.022,0.2229}, 100, 2, 0.0001); // note : unit : cm
  // avgvtxxy -> FindVertexLineFill({0,0}, 150, 2, 0.001); // note : unit : cm
  avgvtxxy -> EndRun();

  // note : -0.02901 +- 0.00001 [cm], 0.22689 +- 0.00001
  // avgvtxxy -> FindVertexQuadrant( 1, 0.001, {-0.02901-0.001, 0.22689-0.001} ); // note : unit : cm

  // avgvtxxy -> FindVertexQuadrant( 1, 0.001, {-0.0220728-0.001, 0.222956-0.001} ); // note : unit : cm
  // avgvtxxy -> FindVertexLineFill(avgvtxxy -> GetVertexQuadrant(), 100, 1, 0.0001); // note : unit : cm
  // avgvtxxy -> FindVertexLineFill({-0.0220728, 0.222956}, 100, 1.0, 0.0001); // note : unit : cm
  // avgvtxxy -> EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
