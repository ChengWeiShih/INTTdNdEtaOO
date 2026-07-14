#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/AvgVtxXY_FieldOn/AvgVtxXY.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/AvgVtxXY_FieldOn/libavgvtxxy.so)

void Run_AvgVtxXY_FieldOn_template(
  int process_id       = 0,
  int number_of_file   = 10,
  int run_num          = 82391,
  int nevents          = -1,
  string input_directory  = "",
  string input_filename   = "",
  string output_directory = ""
)
{
  // ── PassParams confirmation print ──────────────────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_AvgVtxXY_FieldOn_template.C" << std::endl;
  std::cout << "============================================================" << std::endl;

  std::cout << "  -- per-job arguments --" << std::endl;
  std::cout << "  process_id         : " << process_id         << std::endl;
  std::cout << "  number_of_file     : " << number_of_file     << std::endl;
  std::cout << "  run_num            : " << run_num            << std::endl;
  std::cout << "  nevents            : " << nevents            << std::endl;
  std::cout << "  input_directory    : " << input_directory    << std::endl;
  std::cout << "  input_filename     : " << input_filename     << std::endl;
  std::cout << "  output_directory   : " << output_directory   << std::endl;

  std::cout << "  -- analysis parameters --" << std::endl;
  std::cout << "  output_file_name_suffix : " << PassParams::output_file_name_suffix << std::endl;
  std::cout << "  MBD_vtxZ_cut       : (" << PassParams::MBD_vtxZ_cut.first  << ", " << PassParams::MBD_vtxZ_cut.second  << ") cm" << std::endl;
  std::cout << "  INTTNClus_cut      : (" << PassParams::INTTNClus_cut.first << ", " << PassParams::INTTNClus_cut.second << ")" << std::endl;
  std::cout << "  ClusAdc_cut        : " << PassParams::ClusAdc_cut        << std::endl;
  std::cout << "  ClusPhiSize_cut    : " << PassParams::ClusPhiSize_cut    << std::endl;
  std::cout << "  HaveGeoOffsetTag   : " << PassParams::HaveGeoOffsetTag   << std::endl;
  std::cout << "  random_range_XYZ   : " << PassParams::random_range_XYZ   << " cm" << std::endl;
  std::cout << "  random_seed        : " << PassParams::random_seed         << std::endl;
  std::cout << "  input_offset_map   : " << PassParams::input_offset_map   << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  // ── Construct AvgVtxXY ─────────────────────────────────────────────────────
  AvgVtxXY * avgvtxxy = new AvgVtxXY(
    process_id,
    number_of_file,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,
    PassParams::output_file_name_suffix,

    PassParams::MBD_vtxZ_cut,
    PassParams::INTTNClus_cut,
    PassParams::ClusAdc_cut,
    PassParams::ClusPhiSize_cut,

    PassParams::HaveGeoOffsetTag,
    PassParams::random_range_XYZ,
    PassParams::random_seed,
    PassParams::input_offset_map
  );

  string final_output_file_name = avgvtxxy->GetOutputFileName();
  cout << "final_output_file_name: " << final_output_file_name << endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), final_output_file_name.c_str(),
    output_directory.c_str(), final_output_file_name.c_str()));

  avgvtxxy -> PreparePairs();

  avgvtxxy -> FindVertexQuadrant( 8, 0.4, {0,0} ); // note : unit : cm
  avgvtxxy -> FindVertexLineFill(avgvtxxy->GetVertexQuadrant(), 150, 2, 0.001); // note : unit : cm
  avgvtxxy -> EndRun();

  system(Form("mv %s/%s %s/completed",
    output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
