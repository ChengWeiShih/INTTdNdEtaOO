#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/EvtVtxZTracklet/EvtVtxZProtoTracklet.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/EvtVtxZTracklet/libEvtVtxZProtoTracklet.so)

void Run_EvtVtxZTracklet_template(
  int process_id       = PassParams::default_process_id,
  int run_num          = PassParams::default_run_num,
  int nevents          = PassParams::default_nevents,
  string input_directory  = PassParams::default_input_directory,
  string input_filename   = PassParams::default_input_filename,
  string output_directory = PassParams::default_output_directory,
  string macro_name       = "Run_EvtVtxZTracklet_template"
)
{
  // ── PassParams confirmation print ──────────────────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_EvtVtxZTracklet_template.C" << std::endl;
  std::cout << "============================================================" << std::endl;

  std::cout << "  -- per-job arguments --" << std::endl;
  std::cout << "  process_id         : " << process_id         << std::endl;
  std::cout << "  run_num            : " << run_num            << std::endl;
  std::cout << "  nevents            : " << nevents            << std::endl;
  std::cout << "  input_directory    : " << input_directory    << std::endl;
  std::cout << "  input_filename     : " << input_filename     << std::endl;
  std::cout << "  output_directory   : " << output_directory   << std::endl;
  std::cout << "  macro_name         : " << macro_name         << std::endl;

  std::cout << "  -- analysis parameters --" << std::endl;
  std::cout << "  output_file_name_suffix : " << PassParams::output_file_name_suffix << std::endl;
  std::cout << "  vertexXYIncm       : (" << PassParams::vertexXYIncm.first  << ", " << PassParams::vertexXYIncm.second  << ") cm" << std::endl;
  std::cout << "  IsFieldOn          : " << PassParams::IsFieldOn << std::endl;
  std::cout << "  IsDCACutApplied    : " << PassParams::IsDCACutApplied << std::endl;
  std::cout << "  DeltaPhiCutInDegree: ((" << PassParams::DeltaPhiCutInDegree.first.first << ", " << PassParams::DeltaPhiCutInDegree.first.second << "), (" << PassParams::DeltaPhiCutInDegree.second.first << ", " << PassParams::DeltaPhiCutInDegree.second.second << "))" << std::endl;
  std::cout << "  DCAcutIncm         : ((" << PassParams::DCAcutIncm.first.first << ", " << PassParams::DCAcutIncm.first.second << "), (" << PassParams::DCAcutIncm.second.first << ", " << PassParams::DCAcutIncm.second.second << "))" << std::endl;
  std::cout << "  ClusAdcCut         : " << PassParams::ClusAdcCut << std::endl;
  std::cout << "  ClusPhiSizeCut     : " << PassParams::ClusPhiSizeCut << std::endl;
  std::cout << "  PrintRecoDetails   : " << PassParams::PrintRecoDetails << std::endl;
  std::cout << "  DrawEvtVtxZ        : " << PassParams::DrawEvtVtxZ << std::endl;
  std::cout << "  RunInttBcoFullDiff : " << PassParams::RunInttBcoFullDiff << std::endl;
  std::cout << "  RunVtxZReco        : " << PassParams::RunVtxZReco << std::endl;
  std::cout << "  RunTrackletPair    : " << PassParams::RunTrackletPair << std::endl;
  std::cout << "  RunTrackletPairRotate: " << PassParams::RunTrackletPairRotate << std::endl;
  std::cout << "  HaveGeoOffsetTag   : " << PassParams::HaveGeoOffsetTag << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  EvtVtxZProtoTracklet * evzpt = new EvtVtxZProtoTracklet(
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
      
    PassParams::HaveGeoOffsetTag
  );

  string final_output_file_name = evzpt->GetOutputFileName();
  string plot_final_output_file_name = "Plot_"+final_output_file_name;
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str(), plot_final_output_file_name.c_str()));  

  evzpt -> MainProcess();
  evzpt -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));
  system(Form("if [ -f %s/%s ]; then mv %s/%s %s/completed; fi;", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
