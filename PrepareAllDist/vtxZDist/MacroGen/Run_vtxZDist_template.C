#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/PrepareAllDist/vtxZDist/vtxZDist.h"

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/PrepareAllDist/vtxZDist/libvtxZDist.so)

void Run_vtxZDist_template(
  int process_id       = PassParams::default_process_id,
  int run_num          = PassParams::default_run_num,
  int nevents          = PassParams::default_nevents,
  string input_directory  = PassParams::default_input_directory,
  string input_filename   = PassParams::default_input_filename,
  string output_directory = PassParams::default_output_directory,
  string macro_name       = "Run_vtxZDist_template"
)
{
  // ── PassParams confirmation print ──────────────────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_vtxZDist_template.C" << std::endl;
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
  std::cout << "  Apply_cut          : " << PassParams::Apply_cut << std::endl;
  std::cout << "  ApplyVtxZReWeighting : " << PassParams::ApplyVtxZReWeighting << std::endl;
  std::cout << "  ApplyEvtBcoFullDiffCut : {" << PassParams::ApplyEvtBcoFullDiffCut.first << ", " << PassParams::ApplyEvtBcoFullDiffCut.second << "}" << std::endl;
  std::cout << "  IsVtxZQACutKnown   : " << PassParams::IsVtxZQACutKnown << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  system(Form("mkdir -p %s/completed", output_directory.c_str()));

  vtxZDist * VZD1 = new vtxZDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    PassParams::output_file_name_suffix,

    PassParams::Apply_cut,
    PassParams::ApplyVtxZReWeighting,
    PassParams::ApplyEvtBcoFullDiffCut
  );

  string final_output_file_name1 = VZD1->GetOutputFileName();
  cout<<"final_output_file_name1: "<<final_output_file_name1<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name1.c_str(), output_directory.c_str(), final_output_file_name1.c_str()));  

  VZD1->PrepareEvent();
  VZD1->EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name1.c_str(), output_directory.c_str()));

  if (PassParams::IsVtxZQACutKnown && PassParams::Apply_cut == false)
  {
    vtxZDist * VZD2 = new vtxZDist(
      process_id,
      run_num,
      nevents,
      input_directory,
      input_filename,
      output_directory,

      PassParams::output_file_name_suffix,

      true, // note : vtxZ QA cut applied
      PassParams::ApplyVtxZReWeighting,
      PassParams::ApplyEvtBcoFullDiffCut
    );

    string final_output_file_name2 = VZD2->GetOutputFileName();
    cout<<"final_output_file_name2: "<<final_output_file_name2<<endl;

    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name2.c_str(), output_directory.c_str(), final_output_file_name2.c_str()));  

    VZD2->PrepareEvent();
    VZD2->EndRun();

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name2.c_str(), output_directory.c_str()));
  }

  return;
}
