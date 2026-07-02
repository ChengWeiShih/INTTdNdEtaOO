#include <mbd/MbdReco.h>

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

#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>

#include <zdcinfo/ZdcReco.h>

// #include <tpccalib/PHTpcResiduals.h>

// #include <trackingqa/SiliconSeedsQA.h>
// #include <trackingqa/TpcSeedsQA.h>
// #include <trackingqa/TpcSiliconQA.h>
// #include <trackingqa/MvtxClusterQA.h>
// #include <trackingqa/InttClusterQA.h>
// #include <trackingdiagnostics/TrackResiduals.h>
// #include <trackingdiagnostics/TrkrNtuplizer.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <calotrigger/TriggerRunInfoReco.h>

// R__LOAD_LIBRARY(libcaloreco.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
//R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libzdcinfo.so)
//R__LOAD_LIBRARY(libtpc.so)
//R__LOAD_LIBRARY(libmicromegas.so)
// R__LOAD_LIBRARY(libTrackingDiagnostics.so)
// R__LOAD_LIBRARY(libcalo_reco.so)
// R__LOAD_LIBRARY(libtrackingqa.so)
//R__LOAD_LIBRARY(libmvtxrawhitqa.so)
//R__LOAD_LIBRARY(libinttrawhitqa.so)

R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libmbd.so)
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

int F4A(
    int process_id = 0,
    int run_num = 28,
    int nevents = -1,
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/PYTHIA_MC/run0028_detroit",
    string input_directory = "/sphenix/lustre01/sphnxpro/mdc2/js_pp200_signal/g4hits/run0028/detroit",
    string input_filename = "I_am_cool"
){
    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();


    std::string job_index = std::to_string( process_id );
    int job_index_len = 6;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    string output_ntuple_name = Form("MC_PYTHIA_Ntuple_000%d_%s.root", run_num, job_index.c_str());

    string input_truth   = Form("%s/G4Hits_pythia8_Detroit-0000000028-%s.root", input_directory.c_str(), job_index.c_str());
    std::cout<<"input_truth: "<<input_truth<<std::endl;

    Fun4AllInputManager *in_truth = new Fun4AllDstInputManager("DST_truth");
    in_truth->Verbosity(2);
    in_truth->AddFile( input_truth ); 
    se->registerInputManager(in_truth);


    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
    //  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", run_num);


    GlobalVertexReco *gvr = new GlobalVertexReco("GlobalVertexReco");
    se->registerSubsystem(gvr);

    // Division: -----Ntuplizer-------------------------------------------------------------------
    dNdEtaINTT * mydNdEtaINTT = new dNdEtaINTT(
        Form("dNdEtaINTT"),
        output_directory + "/" + output_ntuple_name,
        0 // note : isData
    );

    mydNdEtaINTT -> GetHEPMC(0);
    mydNdEtaINTT -> GetRecoCluster(0);
    mydNdEtaINTT -> GetINTTdata(1);
    mydNdEtaINTT -> GetCentrality(0);
    mydNdEtaINTT -> GetPMTInfo(0);
    mydNdEtaINTT -> GetInttRawHit(0);
    mydNdEtaINTT -> GetTrkrHit(0);
    mydNdEtaINTT -> GetPHG4(true);
    mydNdEtaINTT -> GetTrigger(0);
    mydNdEtaINTT -> GetTriggerFire(0);

    se->registerSubsystem(mydNdEtaINTT);
    
    system (Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_ntuple_name.c_str(), output_directory.c_str(), output_ntuple_name.c_str()));

    // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", output_directory + "/" + output_DST_name);
    // se->registerOutputManager(out);


    // se->skip(nevents);
    se->run(nevents);
    se->End();

    watch->Stop();
    cout << "----------------------------------" << endl;
    cout << "Real time: " << watch->RealTime() << endl;
    cout << "CPU time:  " << watch->CpuTime() << endl;  

    // system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));
    // system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_DST_name.c_str(), output_directory.c_str()));
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_ntuple_name.c_str(), output_directory.c_str()));

    std::cout<<"Done!"<<std::endl;

    delete se;

    return 888;
}