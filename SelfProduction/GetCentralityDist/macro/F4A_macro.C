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

#include <getcentralitydist/GetCentralityDist.h>
R__LOAD_LIBRARY(libGetCentralityDist.so)

int F4A_macro(
    int nevents = -1,
    string  job_index = "00000",
    int run_num = 82391
){

    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();

    Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
    in_calo->Verbosity(2);
    in_calo->AddFile( Form("/sphenix/lustre01/sphnxpro/production2/run3oo/physics/calofitting/new_newcdbtag_v008/run_00082300_00082400/DST_CALOFITTING_run3oo_new_newcdbtag_v008-00082391-%s.root", job_index.c_str()) );
    in_calo->Print("ALL");
    se->registerInputManager(in_calo);

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", run_num);

    std::cout<<"CentralityScale: "<<CDBInterface::instance()->getUrl("CentralityScale")<<std::endl;
    std::cout<<"CentralityVertexScale: "<<CDBInterface::instance()->getUrl("CentralityVertexScale")<<std::endl;
    std::cout<<"the follwoing is the test: "<<std::endl;
    CDBInterface::instance()->Print();

    G4MAGNET::magfield_rescale = 1;

    // Division: -----Trigger-------------------------------------------------------------------
    TriggerRunInfoReco * triggerinfo = new TriggerRunInfoReco("TriggerInfo");
    se->registerSubsystem(triggerinfo);

    // Division: -----MbdReco-------------------------------------------------------------------
    MbdReco *mbdreco = new MbdReco();
    se->registerSubsystem(mbdreco);

    //ZDC Reconstruction--Calib Info                                                                                                                                                                              
    ZdcReco *zdcreco = new ZdcReco();
    zdcreco->set_zdc1_cut(0.0);
    zdcreco->set_zdc2_cut(0.0);
    se->registerSubsystem(zdcreco);

    GlobalVertexReco *gvr = new GlobalVertexReco("GlobalVertexReco");
    se->registerSubsystem(gvr);

    MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    mb->Verbosity(11110);
    mb->setSpecies(MinimumBiasInfo::SPECIES::OO);
    mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations26_full/scales/cdb_centrality_scale_82391.root");
    se->registerSubsystem(mb);

    CentralityReco *cr = new CentralityReco();
    cr->Verbosity(11110);
    cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations26_full/scales/cdb_centrality_scale_82391.root");
    se->registerSubsystem(cr);

    GetCentralityDist * getcentralitydist = new GetCentralityDist("GetCentralityDist");
    se->registerSubsystem(getcentralitydist);

    // se->skip(nevents);
    se->run(nevents);
    se->End();

    watch->Stop();
    cout << "----------------------------------" << endl;
    cout << "Real time: " << watch->RealTime() << endl;
    cout << "CPU time:  " << watch->CpuTime() << endl;  

    std::cout<<"Done!"<<std::endl;

    delete se;

    return 888;
}