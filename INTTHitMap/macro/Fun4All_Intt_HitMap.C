#include "Fun4All_Intt_HitMap.hh"

R__LOAD_LIBRARY(libINTTHitMap.so)
// R__LOAD_LIBRARY(libdNdEtaINTT.so)
// R__LOAD_LIBRARY(libmbd.so)
// R__LOAD_LIBRARY(libEvtIDReporter.so)

void Fun4All_Intt_HitMap(
  int process_id = 0,
  int run_num = 82391,
  int nevents = -1,
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/82391_HitMap_cdbFELIXbyFELIXHotMap",
  string input_directory = "/sphenix/lustre01/sphnxpro/production/run3oo/physics/ana537_nocdbtag_v001",
  string input_filename = "I_am_cool",
  
  // todo : modify here
  string hot_channel_full_file_directory = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/INTT_HotMap/36/37/3637b03579bd6161cafa35f5376e91cf_intt_hotmap_cdb_82391.root",
  int bco_diff_peak = 67,
  
  bool apply_hot_channel = true,
  bool apply_bco_diff = true,
  bool hitQA_check = true,
  bool clone_hit_remove_BCO = true,
  bool MBDNS_trigger_require_tag = true,
  int  trigger_MBDvtxZ_cm = 150 // note : cm

)
{

  //  gSystem->ListLibraries();
  TStopwatch* watch = new TStopwatch();
  watch->Start();
  
  Fun4AllServer* se = Fun4AllServer::instance();

  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  std::vector<Fun4AllInputManager *> input_managers; input_managers.clear();
  for (int i = 0; i < 8; i++)
  {
      std::string intt_folder_direcotry = Form("DST_STREAMING_EVENT_intt%d/run_00082300_00082400", i);
      std::string intt_file_name = Form("DST_STREAMING_EVENT_intt%d_run3oo_ana537_nocdbtag_v001-000%d-%s.root", i, run_num, job_index.c_str());

      input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track_%d",i)));
      input_managers.back() -> Verbosity(2);
      input_managers.back() -> AddFile( input_directory + "/" + intt_folder_direcotry + "/" + intt_file_name);
      se->registerInputManager(input_managers.back());

      // Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
      // in->Verbosity(2);
      // in->AddFile( input_directory + "/" + input_filename); 
      // // in->AddListFile( input_directory + "/" + input_filename );
      // se->registerInputManager(in);
  }

  INTTHitMap * IHM = new INTTHitMap (
    "INTTHitMap",
    process_id,
    run_num,
    output_directory,
    apply_hot_channel,
    hot_channel_full_file_directory,
    apply_bco_diff,
    bco_diff_peak,
    hitQA_check,
    clone_hit_remove_BCO,
    MBDNS_trigger_require_tag,
    trigger_MBDvtxZ_cm
  );
  
  string final_output_file_name = IHM->GetOutputFileName();

  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  se->registerSubsystem(IHM);

  se->run(nevents);
  se->End();

  watch->Stop();
  cout << "----------------------------------" << endl;
  cout << "Real time: " << watch->RealTime() << endl;
  cout << "CPU time:  " << watch->CpuTime() << endl;  

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  delete se;






  // ////////////////////////////////////////////////////////////////////////
  // string run_type = "physics";
  // string run_num_str = string( "000" ) + to_string( run_num );

  // string list_file = string( "list_run" ) + to_string( run_num ) + ".txt";
  // string list_file_calo = "list_run54280_calo.txt";
  
  // ////////////////////////////////////////////////////////////////////////
  // // Config for input/output files
  // ////////////////////////////////////////////////////////////////////////

  // // output_base format: {run_tpye}_intt_{run number}
  // // For example, cosmics_intt_01234567
  // string output_base = string("_") + run_type + "_intt-" + run_num_str;  
  // string output_dst = "DST" + output_base;

  // if (apply_hot_channel) output_dst += "_HotChannel";
  // if (apply_bco_cut)     output_dst += "_BCOCut";
  // output_dst += "_CaloCombined";
  // output_dst +=  ".root"; 

  // // string  cdb_hot_list = kIntt_cdb_dir + "cdb_49737_special.root";
  // // cdb_hot_list = kIntt_cdb_dir + "cdb_00050377_special.root";
  // // cout << "Hot channel CDB is forced to be " << cdb_hot_list  << endl;
  // string  cdb_hot_list = "/sphenix/user/jaein213/macros/inttcalib/hotmap_cdb/hotmap_run_000"+to_string(run_num)+".root";

  // // string cdb_bco = kIntt_cdb_dir + "bco_diff_map/"
  // //   + "cdb_bco_" + run_num_str + "_official" + ".root";
  
  // string cdb_bco = "/sphenix/user/jaein213/macros/inttcalib/bcomap_cdb/bcomap_run_000"+to_string(run_num)+".root"; 
  
  // string cdbtree_name_dac = kIntt_cdb_dir
  //   + "dac_map/"
  //   //  + "cdb_intt_dac_30_45_60_90_120_150_180_210.root";
  //   + "cdb_intt_dac_35_45_60_90_120_150_180_210_streaming.root";
  
  // Fun4AllServer* se = Fun4AllServer::instance();
  // //se->Verbosity(5);

  // // just if we set some flags somewhere in this macro
  // recoConsts *rc = recoConsts::instance();
  
  // Enable::CDB = true;
  // rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // // 64 bit timestamp
  // rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  // rc->set_IntFlag("RUNNUMBER", run_num );
  
  // cout<<"test test, CDB_GLOBALTAG: "<<CDB::global_tag<<endl;

  // //--input
  // Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
  // in->Verbosity(2);
  // in->AddListFile( list_file ); 
  // // in->AddListFile( list_file_calo );
  // se->registerInputManager(in);

  // Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
  // in_calo->Verbosity(2);
  // in_calo->AddListFile( list_file_calo );
  // se->registerInputManager(in_calo);
     
  // //Enable::BBC = true;
  // Enable::MVTX = true;
  // Enable::INTT = true;
  // Enable::TPC = true;
  // Enable::MICROMEGAS = true;
  // G4Init();
  // G4Setup();

  // TrackingInit();

  // //////////////////////////////////////
  // InttCombinedRawDataDecoder* inttdecode = new InttCombinedRawDataDecoder();
  // //  inttdecode->Verbosity(1);
  // inttdecode->runInttStandalone( true );
  // inttdecode->writeInttEventHeader( true );
  // inttdecode->set_triggeredMode( true );
  

  // if (apply_hot_channel) inttdecode->LoadHotChannelMapLocal( cdb_hot_list );

  // if( apply_bco_cut ) // BCO diff selection to get only hits from triggered collision
  //   inttdecode->SetCalibBCO( cdb_bco, InttCombinedRawDataDecoder::FILE); 

  // inttdecode->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE ); // not InttCombinedRawDataDecoder::CDB
  // //inttdecode->set_fphxBcoFilter( fphx_bco );
  // se->registerSubsystem( inttdecode );
  
  // //////////////////////////////////////
  // //Intt_Cells();
  // Intt_Clustering();


  // //////////////////MBD////////////////////
  // // MbdReco *mbdreco = new MbdReco();
  // // se->registerSubsystem(mbdreco);
  // //////////////////MBD////////////////////
  


  // // EvtIDReporter * evtridreporter = new EvtIDReporter("evtridreporter", 4000);
  // // se->registerSubsystem(evtridreporter);

  // dNdEtaINTT * dndetaintt = new dNdEtaINTT("dndetaintt", Form("%s/Trkr_%s",output_directory.c_str(), output_dst.c_str()), true);
  // se->registerSubsystem(dndetaintt);


  // //output
  // Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", output_directory + "/" + output_dst );
  // se->registerOutputManager(out);

  // se->run(nevents);
  // se->End();

  // cout << "CDB (hot channel) " << cdb_hot_list << endl;
  // cout << "CDB (BCO diff) " << cdb_bco << endl;
  // cout << "CDB (DAC map)    " << cdbtree_name_dac << endl;
  // cout << "FPHX BCO: " << fphx_bco << endl;
  // cout << "Output: " << output_dst << endl;

  // watch->Stop();
  // cout << "----------------------------------" << endl;
  // cout << "Real time: " << watch->RealTime() << endl;
  // cout << "CPU time:  " << watch->CpuTime() << endl;  
  // delete se;
}
