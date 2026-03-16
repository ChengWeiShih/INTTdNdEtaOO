#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
#include <QA.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C> // note : here fore the trackreco/*.h
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

#include <intt/InttOdbcQuery.h>
#include <intt/InttCombinedRawDataDecoder.h>

#include <globalvertex/GlobalVertexReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>

// #include <tpccalib/PHTpcResiduals.h>

// #include <trackingqa/SiliconSeedsQA.h>
// #include <trackingqa/TpcSeedsQA.h>
// #include <trackingqa/TpcSiliconQA.h>
// #include <trackingqa/MvtxClusterQA.h>
// #include <trackingqa/InttClusterQA.h>
// #include <trackingdiagnostics/TrackResiduals.h>
// #include <trackingdiagnostics/TrkrNtuplizer.h>
#include <calotrigger/TriggerRunInfoReco.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
//R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libintt.so)
//R__LOAD_LIBRARY(libtpc.so)
//R__LOAD_LIBRARY(libmicromegas.so)
// R__LOAD_LIBRARY(libTrackingDiagnostics.so)
// R__LOAD_LIBRARY(libcalo_reco.so)
// R__LOAD_LIBRARY(libtrackingqa.so)
//R__LOAD_LIBRARY(libmvtxrawhitqa.so)
//R__LOAD_LIBRARY(libinttrawhitqa.so)

//R__LOAD_LIBRARY(libmbd.so)
// R__LOAD_LIBRARY(libglobalvertex.so)
//R__LOAD_LIBRARY(libSeedChecker.so)
//R__LOAD_LIBRARY(libffamodules.so)
//R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libtrack_reco.so)

#include <checkinttrawhit/CheckInttRawHit.h>
R__LOAD_LIBRARY(libCheckInttRawHit.so)
// R__LOAD_LIBRARY(libtrack_reco.so)

#include <dndetaintt/dNdEtaINTT.h>
R__LOAD_LIBRARY(libdNdEtaINTT.so)

int GetTriggeredPeak(string bco_map_file){
    TFile * file_in = TFile::Open(bco_map_file.c_str(), "READ");
    TTree * tree_in = (TTree *) file_in -> Get("Multiple");

    int bco_diff;
    tree_in -> SetBranchAddress("Ibco_diff", &bco_diff);
    std::vector<int> unique_bco_diff; unique_bco_diff.clear();

    // for (int i = 0; i < tree_in -> GetEntries(); i++)
    // {
    //     tree_in -> GetEntry(i);

    //     if ( std::find(unique_bco_diff.begin(), unique_bco_diff.end(), bco_diff) == unique_bco_diff.end() )
    //     {
    //         unique_bco_diff.push_back(bco_diff);
    //     }
    // }

    // if ( unique_bco_diff.size() != 1 )
    // {
    //     std::cout<<"Error! More than one unique BCO diff found!, size: "<<unique_bco_diff.size()<<std::endl;
    //     for (int i = 0; i < unique_bco_diff.size(); i++)
    //     {
    //         std::cout<<"  unique_bco_diff["<<i<<"]: "<<unique_bco_diff[i]<<std::endl;
    //     }

    //     gSystem->Exit(1);
    //     exit(1);
    // }

    // return unique_bco_diff[0];

    tree_in -> GetEntry(0);

    if (bco_diff == -1){
        std::cout<<"Error! BCO diff is -1!"<<std::endl;
        gSystem->Exit(1);
        exit(1);
    }

    return bco_diff;
}



int INTTRawHit_check(
    int process_id = 0,
    int run_num = 54679,
    int nevents = 10,
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_HV/54679",
  
    // note : for macro
    string input_directory = "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/jaein_decode_calib/completed_map_1M",
    string input_filename = "test_DST_beam_intt-00054679_special.root",

    string output_file_name_suffix_in = "",

    bool apply_hotchannel_in = true,
    bool clone_hit_remove_BCO_in = true,
    bool hitQA_in = true 
){

    // string output_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run24AuAu_HV/%d", run_num);
    // string input_filename = Form("test_DST_beam_intt-000%d_special.root", run_num);

    // string hotchannel_full_directory_in = input_directory + "/" + Form("cdb_000%dhot.root", run_num);
    string hotchannel_full_directory_in = Form("/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/jaein_decode_calib/completed_map_1M/cdb_00054685hot.root"); // note : this is for the DAC Scan runs
    string bco_full_directory_in        = input_directory + "/" + Form("cdb_000%dbco.root", run_num);
    int triggered_BcoDiff_in = GetTriggeredPeak(bco_full_directory_in);

    std::cout<<"triggered_BcoDiff_in: "<<triggered_BcoDiff_in<<std::endl;

    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();

    std::map<int, std::string> runnumber_dacmap = {
        {54692, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_8_12_16_20_24_28_32_36.root"},
        {54694, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_28_32_36_40_44_48_52_56.root"},
        {54695, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_48_52_56_60_64_68_72_76.root"},
        {54696, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_68_72_76_80_84_88_92_96.root"},
        {54698, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_88_92_96_100_104_108_112_116.root"},
        {54699, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_108_112_116_120_124_128_132_136.root"},
        {54700, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_128_132_136_140_144_148_152_156.root"},
        {54708, "/sphenix/user/ChengWei/SelfINTTProduction/2025_Oct_20/Analysis/ProductionPreparation/cdb_intt_dac_148_152_156_160_164_168_172_176.root"}
        
    };

    std::cout<<"run: "<<run_num<<"used dac map: "<<runnumber_dacmap[run_num]<<std::endl;

    // std::vector<Fun4AllInputManager *> input_managers; input_managers.clear();

    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    string output_DST_name = Form("DST_test_000%d_standalone_%s.root", run_num, job_index.c_str());
    string output_ntuple_name = Form("Ntuple_000%d_standalone_%s.root", run_num, job_index.c_str());

    // for (int i = 0; i < 8; i++)
    // {
    //     std::string intt_folder_direcotry = Form("DST_STREAMING_EVENT_INTT%d/run_00053800_00053900/dst", i);
    //     std::string intt_file_name = Form("DST_STREAMING_EVENT_INTT%d_run2pp_ana478_nocdbtag_v002-000%d-%s.root", i, run_num, job_index.c_str());

    //     input_managers.push_back(new Fun4AllDstInputManager(Form("DST_track_%d",i)));
    //     input_managers.back() -> Verbosity(2);
    //     input_managers.back() -> AddFile( input_directory + "/" + intt_folder_direcotry + "/" + intt_file_name);
    //     se->registerInputManager(input_managers.back());

    //     // Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
    //     // in->Verbosity(2);
    //     // in->AddFile( input_directory + "/" + input_filename); 
    //     // // in->AddListFile( input_directory + "/" + input_filename );
    //     // se->registerInputManager(in);
    // }


    Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
    in->Verbosity(2);
    in->fileopen( input_directory + "/" + input_filename);
    // in->AddFile( input_directory + "/" + input_filename); 
    // in->AddListFile( input_directory + "/" + input_filename );
    se->registerInputManager(in);



    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    //  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", run_num);

    // std::string BadChannelMap = CDBInterface::instance()->getUrl("INTT_HotMap");
    // std::cout<<"BadChannelMap: "<<BadChannelMap<<std::endl;

    // Division: ----------tracking geometry----------------
    Enable::MVTX_APPLYMISALIGNMENT = true;
    ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

    std::string geofile = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/Tracking_Geometry/93/42/9342254d8393560d92364f4c048fcf03_TrackingGeometry-ana500.root";

    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(geofile);
    se->registerInputManager(ingeo);


    // Division: -----InttCombinedRawDataDecoder-------------------------------------------------------------------
    InttCombinedRawDataDecoder *myDecoder = new InttCombinedRawDataDecoder("myUnpacker");
    myDecoder->runInttStandalone(true);
    myDecoder->writeInttEventHeader(true);
    // myDecoder->Verbosity(10000);

    // string cdbttree_name = kIntt_cdb_dir + "cdb_" + run_num_str + ".root";
    // string cdbttree_name = kIntt_cdb_dir + "cdb_49737_special.root";
    // cout << "Hot channel CDB is forced to be " << cdbttree_name << endl;
    myDecoder->LoadHotChannelMapLocal(hotchannel_full_directory_in);

    //string cdbttree_name_bco = cdb_output_path + "cdb_bco_" + to_string(run_num) + ".root";
    // string cdbttree_name_bco = kIntt_cdb_dir + "cdb_bco_" + run_num_str + ".root";
    myDecoder->SetCalibBCO(bco_full_directory_in, InttCombinedRawDataDecoder::FILE);

    // string cdbtree_name_dac = "/sphenix/tg/tg01/commissioning/INTT/data/CDB_files/2024/dac_map/cdb_intt_dac_35_45_60_90_120_150_180_210_streaming.root";
    // myDecoder->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE);
    myDecoder->SetCalibDAC( runnumber_dacmap[run_num], InttCombinedRawDataDecoder::FILE);
    
    myDecoder->set_triggeredMode(true);
    myDecoder->set_bcoFilter(true);
    // myDecoder->set_outputBcoDiff(true);

    se->registerSubsystem(myDecoder);
  
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_DST_name.c_str(), output_directory.c_str(), output_DST_name.c_str()));  


    // Division: -----INTT clustering-------------------------------------------------------------------
    G4MAGNET::magfield_rescale = 1;

    TrackingInit();
    
    Intt_Clustering();


    // Division: -----Quick check-------------------------------------------------------------------
    CheckInttRawHit * IBR = new CheckInttRawHit(
        "CheckInttRawHit",
        process_id,
        run_num,
        output_directory,
        
        hotchannel_full_directory_in,
        triggered_BcoDiff_in,
        apply_hotchannel_in,
        clone_hit_remove_BCO_in,
        hitQA_in
    );
    string final_output_file_name = IBR->GetOutputFileName();
    cout<<"final_output_file_name: "<<final_output_file_name<<endl;

    
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
    se->registerSubsystem(IBR);


    // Division: -----Ntuplizer-------------------------------------------------------------------
    dNdEtaINTT * mydNdEtaINTT = new dNdEtaINTT(
        "dNdEtaINTT",
        output_directory + "/" + output_ntuple_name,
        true // note : isData
    );

    mydNdEtaINTT -> GetHEPMC(0);
    mydNdEtaINTT -> GetRecoCluster(true);
    mydNdEtaINTT -> GetINTTdata(true);
    mydNdEtaINTT -> GetCentrality(0);
    mydNdEtaINTT -> GetPMTInfo(0);
    mydNdEtaINTT -> GetInttRawHit(true);
    mydNdEtaINTT -> GetTrkrHit(0);
    mydNdEtaINTT -> GetPHG4(0);
    mydNdEtaINTT -> GetTrigger(0);
    
    system (Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_ntuple_name.c_str(), output_directory.c_str(), output_ntuple_name.c_str()));
    se->registerSubsystem(mydNdEtaINTT);


    Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", output_directory + "/" + output_DST_name);
    se->registerOutputManager(out);


    se->skip(process_id * nevents);
    se->run(nevents);
    se->End();

    watch->Stop();
    cout << "----------------------------------" << endl;
    cout << "Real time: " << watch->RealTime() << endl;
    cout << "CPU time:  " << watch->CpuTime() << endl;  

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_DST_name.c_str(), output_directory.c_str()));
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_ntuple_name.c_str(), output_directory.c_str()));

    std::cout<<"Done!"<<std::endl;

    delete se;

    return 888;
}