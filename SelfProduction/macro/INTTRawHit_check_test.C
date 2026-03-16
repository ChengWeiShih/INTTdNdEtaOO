
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

#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>
#include <calotrigger/TriggerRunInfoReco.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libtrack_reco.so)



int INTTRawHit_check_test(
    int process_id = 0,
    int run_num = 54280,
    int nevents = 10
){
    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();
    se->Verbosity(100000);

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", run_num);

    Enable::MVTX_APPLYMISALIGNMENT = true;
    ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

    std::string geofile = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/Tracking_Geometry/14/ef/14efbebe6cf8da7ef8dcc1cdde86d48c_TrackingGeometry_run24-ana517.root";

    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(geofile);
    se->registerInputManager(ingeo);

    G4MAGNET::magfield_rescale = 1;    
    TrackingInit();

    se->run(nevents);
    se->End();

    watch->Stop();
    cout << "----------------------------------" << endl;
    cout << "Real time: " << watch->RealTime() << endl;
    cout << "CPU time:  " << watch->CpuTime() << endl;  

    delete se;

    return 888;
}