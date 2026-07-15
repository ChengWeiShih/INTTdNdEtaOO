#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/PrepareAllDist/RestDist/RestDist.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/PrepareAllDist/RestDist/libRestDist.so)

TH1D * GetReweighting_hist(string input_map_directory, string map_name)
{
  TFile * file_in = TFile::Open(Form("%s", input_map_directory.c_str()));
  TH1D * h1D_INTT_vtxZ_reweighting = (TH1D*)file_in->Get(map_name.c_str()); // todo : the map of the vtxZ reweighting
  return h1D_INTT_vtxZ_reweighting;
}

void Run_RestDist_template(
  int process_id       = PassParams::default_process_id,
  int run_num          = PassParams::default_run_num,
  int nevents          = PassParams::default_nevents,
  string input_directory  = PassParams::default_input_directory,
  string input_filename   = PassParams::default_input_filename,
  string output_directory = PassParams::default_output_directory,
  string macro_name       = "Run_RestDist_template"
)
{
  // ── PassParams confirmation print ──────────────────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_RestDist_template.C" << std::endl;
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
  std::cout << "  vertexXYIncm          : (" << PassParams::vertexXYIncm.first << ", " << PassParams::vertexXYIncm.second << ")" << std::endl;
  std::cout << "  Apply_cut             : " << PassParams::Apply_cut << std::endl;
  std::cout << "  ApplyVtxZReWeighting  : " << PassParams::ApplyVtxZReWeighting << std::endl;
  std::cout << "  ApplyEvtBcoFullDiffCut: {" << PassParams::ApplyEvtBcoFullDiffCut.first << ", " << PassParams::ApplyEvtBcoFullDiffCut.second << "}" << std::endl;
  std::cout << "  RequireVtxZRange      : {" << PassParams::RequireVtxZRange.first << ", {" << PassParams::RequireVtxZRange.second.first << ", " << PassParams::RequireVtxZRange.second.second << "}}" << std::endl;
  std::cout << "  isClusQA              : {" << PassParams::isClusQA.first << ", {" << PassParams::isClusQA.second.first << ", " << PassParams::isClusQA.second.second << "}}" << std::endl;
  std::cout << "  isRotated             : " << PassParams::isRotated << std::endl;
  std::cout << "  vtxZReWeight_dir      : " << PassParams::vtxZReWeighting_input_directory << std::endl;
  std::cout << "  map_name              : " << PassParams::map_name << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  string final_output_file_name = "";
  TH1D * h1D_INTT_vtxZ_reweighting = (run_num == -1 && PassParams::ApplyVtxZReWeighting) ? GetReweighting_hist(PassParams::vtxZReWeighting_input_directory, PassParams::map_name) : nullptr;

  system(Form("mkdir -p %s/completed", output_directory.c_str()));

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and no ClusQA, no rotated
  RestDist * RDs1 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    PassParams::output_file_name_suffix,
    PassParams::vertexXYIncm,

    PassParams::Apply_cut,
    PassParams::ApplyVtxZReWeighting,
    PassParams::ApplyEvtBcoFullDiffCut,
    
    PassParams::RequireVtxZRange,
    {false,{-10,20000}},

    false // note : isRotated
  );
  if (PassParams::ApplyVtxZReWeighting) {RDs1->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);}

  final_output_file_name = RDs1->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs1->PrepareEvent();
  RDs1->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and w/ ClusQA, no rotated
  RestDist * RDs2 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    PassParams::output_file_name_suffix,
    PassParams::vertexXYIncm,

    PassParams::Apply_cut,
    PassParams::ApplyVtxZReWeighting,
    PassParams::ApplyEvtBcoFullDiffCut,
    
    PassParams::RequireVtxZRange,
    PassParams::isClusQA,

    false // note : isRotated
  );
  if (PassParams::ApplyVtxZReWeighting) {RDs2->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);}

  final_output_file_name = RDs2->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs2->PrepareEvent();
  RDs2->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));


  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : No vtxZ cut, No ClusQA, no rotated
  RestDist * RDs3 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    PassParams::output_file_name_suffix,
    PassParams::vertexXYIncm,

    PassParams::Apply_cut,
    PassParams::ApplyVtxZReWeighting,
    PassParams::ApplyEvtBcoFullDiffCut,
    
    {false, {-1000,1000}},
    {false,{-10,20000}},

    false // note : isRotated
  );
  if (PassParams::ApplyVtxZReWeighting) {RDs3->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);}

  final_output_file_name = RDs3->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs3->PrepareEvent();
  RDs3->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));


  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : No vtxZQA, vtxZ cut, No ClusQA, no rotated
  RestDist * RDs4 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    PassParams::output_file_name_suffix,
    PassParams::vertexXYIncm,

    false, // note : vtxZQA
    PassParams::ApplyVtxZReWeighting,
    PassParams::ApplyEvtBcoFullDiffCut,
    
    PassParams::RequireVtxZRange,
    {false,{-10,20000}},

    false // note : isRotated
  );
  if (PassParams::ApplyVtxZReWeighting) {RDs4->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);}

  final_output_file_name = RDs4->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs4->PrepareEvent();
  RDs4->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
