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

R__LOAD_LIBRARY(libmbd.so)
// R__LOAD_LIBRARY(libglobalvertex.so)
//R__LOAD_LIBRARY(libSeedChecker.so)
//R__LOAD_LIBRARY(libffamodules.so)
//R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libtrack_reco.so)

#include <intimepileup/InTimePileUp.h>
R__LOAD_LIBRARY(libInTimePileUp.so)


int F4A_InTimePileUp(
    int process_id = 1000,
    int run_num = 82391,
    int nevents = -1,
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/InTimePileUp"
){

    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();

    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
    in_calo->Verbosity(2);
    in_calo->AddFile( Form("/sphenix/lustre01/sphnxpro/production2/run3oo/physics/calofitting/new_newcdbtag_v008/run_00082300_00082400/DST_CALOFITTING_run3oo_new_newcdbtag_v008-00082391-%s.root", job_index.c_str()) );
    in_calo->Print("ALL");
    se->registerInputManager(in_calo);


    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    //  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", run_num);


    InTimePileUp * IBR = new InTimePileUp(
        "InTimePileUp",
        process_id,
        run_num,
        output_directory
    );


    string final_output_file_name = IBR->GetOutputFileName();
    cout<<"final_output_file_name: "<<final_output_file_name<<endl;

    
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
    se->registerSubsystem(IBR);

    

    // se->skip(nevents);
    se->run(nevents);
    se->End();

    watch->Stop();
    cout << "----------------------------------" << endl;
    cout << "Real time: " << watch->RealTime() << endl;
    cout << "CPU time:  " << watch->CpuTime() << endl;  

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

    std::cout<<"Done!"<<std::endl;

    delete se;

    return 888;
}