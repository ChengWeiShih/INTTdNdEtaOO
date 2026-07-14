#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/EvtVtxZTracklet/EvtVtxZLeftRight.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/EvtVtxZTracklet/libEvtVtxZLeftRight.so)

void Run_EvtVtxZLeftRight_template(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "",
  string input_filename = "",
  string output_directory = ""
)
{
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_EvtVtxZLeftRight_template.C" << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  -- per-job arguments --" << std::endl;
  std::cout << "  process_id       : " << process_id << std::endl;
  std::cout << "  run_num          : " << run_num << std::endl;
  std::cout << "  nevents          : " << nevents << std::endl;
  std::cout << "  input_directory  : " << input_directory << std::endl;
  std::cout << "  input_filename   : " << input_filename << std::endl;
  std::cout << "  output_directory : " << output_directory << std::endl;
  std::cout << "  -- analysis parameters --" << std::endl;
  std::cout << "  output_file_name_suffix : " << PassParams::output_file_name_suffix << std::endl;
  std::cout << "  vertexXYIncm            : (" << PassParams::vertexXYIncm.first << ", " << PassParams::vertexXYIncm.second << ")" << std::endl;
  std::cout << "  IsFieldOn               : " << PassParams::IsFieldOn << std::endl;
  std::cout << "  IsDCACutApplied         : " << PassParams::IsDCACutApplied << std::endl;
  std::cout << "  DeltaPhiCutInDegree     : ((" << PassParams::DeltaPhiCutInDegree.first.first << ", " << PassParams::DeltaPhiCutInDegree.first.second << "), (" << PassParams::DeltaPhiCutInDegree.second.first << ", " << PassParams::DeltaPhiCutInDegree.second.second << "))" << std::endl;
  std::cout << "  DCAcutIncm              : ((" << PassParams::DCAcutIncm.first.first << ", " << PassParams::DCAcutIncm.first.second << "), (" << PassParams::DCAcutIncm.second.first << ", " << PassParams::DCAcutIncm.second.second << "))" << std::endl;
  std::cout << "  ClusAdcCut              : " << PassParams::ClusAdcCut << std::endl;
  std::cout << "  ClusPhiSizeCut          : " << PassParams::ClusPhiSizeCut << std::endl;
  std::cout << "  PrintRecoDetails        : " << PassParams::PrintRecoDetails << std::endl;
  std::cout << "  DrawEvtVtxZ             : " << PassParams::DrawEvtVtxZ << std::endl;
  std::cout << "  RunInttBcoFullDiff      : " << PassParams::RunInttBcoFullDiff << std::endl;
  std::cout << "  RunVtxZReco             : " << PassParams::RunVtxZReco << std::endl;
  std::cout << "  RunTrackletPair         : " << PassParams::RunTrackletPair << std::endl;
  std::cout << "  RunTrackletPairRotate   : " << PassParams::RunTrackletPairRotate << std::endl;
  std::cout << "  HaveGeoOffsetTag        : " << PassParams::HaveGeoOffsetTag << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  EvtVtxZLeftRight * evzpt_left = new EvtVtxZLeftRight(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,
    PassParams::output_file_name_suffix,

    PassParams::vertexXYIncm,
    PassParams::IsFieldOn,
    PassParams::IsDCACutApplied,
    PassParams::DeltaPhiCutInDegree,
    PassParams::DCAcutIncm,
    PassParams::ClusAdcCut,
    PassParams::ClusPhiSizeCut,

    PassParams::PrintRecoDetails,
    PassParams::DrawEvtVtxZ,

    PassParams::RunInttBcoFullDiff,
    PassParams::RunVtxZReco,
    PassParams::RunTrackletPair,
    PassParams::RunTrackletPairRotate,
    true, // isLeftOnly

    PassParams::HaveGeoOffsetTag
  );

  string final_output_file_name_left = evzpt_left->GetOutputFileName();
  string plot_final_output_file_name_left = "Plot_" + final_output_file_name_left;
  cout << "final_output_file_name_left: " << final_output_file_name_left << endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), final_output_file_name_left.c_str(),
    output_directory.c_str(), final_output_file_name_left.c_str()));
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), plot_final_output_file_name_left.c_str(),
    output_directory.c_str(), plot_final_output_file_name_left.c_str()));

  evzpt_left -> MainProcess();
  evzpt_left -> EndRun();

  system(Form("mv %s/%s %s/completed",
    output_directory.c_str(), final_output_file_name_left.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed",
    output_directory.c_str(), plot_final_output_file_name_left.c_str(), output_directory.c_str()));

  string input_directory_for_right = output_directory + "/completed";
  string input_filename_for_right = final_output_file_name_left;

  EvtVtxZLeftRight * evzpt_right = new EvtVtxZLeftRight(
    process_id,
    run_num,
    nevents,
    input_directory_for_right,
    input_filename_for_right,
    output_directory,
    PassParams::output_file_name_suffix,

    PassParams::vertexXYIncm,
    PassParams::IsFieldOn,
    PassParams::IsDCACutApplied,
    PassParams::DeltaPhiCutInDegree,
    PassParams::DCAcutIncm,
    PassParams::ClusAdcCut,
    PassParams::ClusPhiSizeCut,

    PassParams::PrintRecoDetails,
    PassParams::DrawEvtVtxZ,

    PassParams::RunInttBcoFullDiff,
    PassParams::RunVtxZReco,
    PassParams::RunTrackletPair,
    PassParams::RunTrackletPairRotate,
    false, // isLeftOnly

    PassParams::HaveGeoOffsetTag
  );

  string final_output_file_name_right = evzpt_right->GetOutputFileName();
  string plot_final_output_file_name_right = "Plot_" + final_output_file_name_right;
  cout << "final_output_file_name_right: " << final_output_file_name_right << endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), final_output_file_name_right.c_str(),
    output_directory.c_str(), final_output_file_name_right.c_str()));
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;",
    output_directory.c_str(), plot_final_output_file_name_right.c_str(),
    output_directory.c_str(), plot_final_output_file_name_right.c_str()));

  evzpt_right -> MainProcess();
  evzpt_right -> EndRun();

  system(Form("mv %s/%s %s/completed",
    output_directory.c_str(), final_output_file_name_right.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed",
    output_directory.c_str(), plot_final_output_file_name_right.c_str(), output_directory.c_str()));

  return;
}
