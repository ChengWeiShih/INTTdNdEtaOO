#include "header_to_be_replace"
#include "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/ColumnCheck/ColumnCheck.h" 

R__LOAD_LIBRARY(/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/ColumnCheck/libColumnCheck.so)

void Run_ColumnCheck_template(
  int process_id       = PassParams::default_process_id,
  int run_num          = PassParams::default_run_num,
  int nevents          = PassParams::default_nevents,
  string input_directory  = PassParams::default_input_directory,
  string input_filename   = PassParams::default_input_filename,
  string output_directory = PassParams::default_output_directory,
  string macro_name       = "Run_ColumnCheck_template"
)
{
  bool BcoFullDiffCut = (run_num == -1) ? false : true;

  // ── PassParams confirmation print ──────────────────────────────────────────
  std::cout << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << "  PassParams confirmation in Run_ColumnCheck_template.C" << std::endl;
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
  std::cout << "  SetMbinFloat       : " << PassParams::SetMbinFloat << std::endl;
  std::cout << "  VtxZRange          : (" << PassParams::VtxZRange.first << ", " << PassParams::VtxZRange.second << ")" << std::endl;
  std::cout << "  IsZClustering      : " << PassParams::IsZClustering << std::endl;
  std::cout << "  vertexXYIncm       : (" << PassParams::vertexXYIncm.first << ", " << PassParams::vertexXYIncm.second << ")" << std::endl;
  std::cout << "  isClusQA           : {" << PassParams::isClusQA.first << ", {" << PassParams::isClusQA.second.first << ", " << PassParams::isClusQA.second.second << "}}" << std::endl;
  std::cout << "  ColMulMask         : " << PassParams::ColMulMask << std::endl;
  std::cout << "  BcoFullDiffCut     : " << BcoFullDiffCut << std::endl;
  std::cout << "============================================================" << std::endl;
  std::cout << std::endl;

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : baseline
  std::string final_output_directory = output_directory + "/baseline";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  ColumnCheck * CC = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

    PassParams::output_file_name_suffix,
    PassParams::vertexXYIncm,
    PassParams::SetMbinFloat,

    PassParams::VtxZRange,
    PassParams::IsZClustering,
    BcoFullDiffCut,
    PassParams::isClusQA,

    PassParams::ColMulMask
  );

  string final_output_file_name = CC->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str(), final_output_file_name.c_str()));  

  CC -> MainProcess();
  CC -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str()));

  return;
}
