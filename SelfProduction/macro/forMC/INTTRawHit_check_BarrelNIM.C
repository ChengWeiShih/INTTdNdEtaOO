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



int INTTRawHit_check_BarrelNIM(
    int process_id = 0,
    int run_num = 82400,
    int nevents = -1,
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING",
  
    // note : for macro
    string input_directory = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/nopileup",
    string input_filename = "I_am_cool",

    string output_file_name_suffix_in = "test"
){
    TStopwatch* watch = new TStopwatch();
    watch->Start();
  
    Fun4AllServer* se = Fun4AllServer::instance();


    std::string job_index = std::to_string( process_id );
    int job_index_len = 6;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    string output_ntuple_name = Form("MC_HIJING_Ntuple_000%d_%s_%s.root", run_num, output_file_name_suffix_in.c_str(), job_index.c_str());
    string input_MBDEPD  = Form("%s/mbdepd/run0034/DST_MBD_EPD_sHijing_OO_0_15fm-0000000034-%s.root"          , input_directory.c_str(), job_index.c_str());
    string input_track   = Form("%s/trkrcluster/run0034/DST_TRKR_CLUSTER_sHijing_OO_0_15fm-0000000034-%s.root", input_directory.c_str(), job_index.c_str());
    string input_truth   = Form("%s/trkrhit/run0034/DST_TRUTH_sHijing_OO_0_15fm-0000000034-%s.root"           , input_directory.c_str(), job_index.c_str());

    std::cout<<"input_MBDEPD: "<<input_MBDEPD<<std::endl;
    std::cout<<"input_track: "<<input_track<<std::endl;
    std::cout<<"input_truth: "<<input_truth<<std::endl;


    Fun4AllInputManager *in_calo = new Fun4AllDstInputManager("DST_calo");
    in_calo->Verbosity(2);
    in_calo->AddFile( input_MBDEPD );
    in_calo->Print("ALL");
    se->registerInputManager(in_calo);


    Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
    in->Verbosity(2);
    in->AddFile( input_track ); 
    se->registerInputManager(in);

    Fun4AllInputManager *in_truth = new Fun4AllDstInputManager("DST_truth");
    in_truth->Verbosity(2);
    in_truth->AddFile( input_truth ); 
    se->registerInputManager(in_truth);


    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", run_num);
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    //  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", run_num);

    // std::string BadChannelMap = CDBInterface::instance()->getUrl("INTT_HotMap");
    std::cout<<"INTT_HotMap: "<<CDBInterface::instance()->getUrl("INTT_HotMap")<<std::endl;
    std::cout<<"INTT_BCOMAP: "<<CDBInterface::instance()->getUrl("INTT_BCOMAP")<<std::endl;
    std::cout<<"INTT_DACMAP: "<<CDBInterface::instance()->getUrl("INTT_DACMAP")<<std::endl;
    std::cout<<"Tracking_Geometry: "<<CDBInterface::instance()->getUrl("Tracking_Geometry")<<std::endl;
    std::cout<<"CentralityScale: "<<CDBInterface::instance()->getUrl("CentralityScale")<<std::endl;
    std::cout<<"CentralityVertexScale: "<<CDBInterface::instance()->getUrl("CentralityVertexScale")<<std::endl;
    std::cout<<"the follwoing is the test: "<<std::endl;
    CDBInterface::instance()->Print();



    Enable::MVTX_APPLYMISALIGNMENT = true;
    ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(CDBInterface::instance()->getUrl("Tracking_Geometry"));
    se->registerInputManager(ingeo);


    G4MAGNET::magfield_rescale = 1;

    TrackingInit();
    
    // Intt_Clustering();




    // Division: -----Trigger-------------------------------------------------------------------
    // note : in MC, assume no trigger
    // TriggerRunInfoReco * triggerinfo = new TriggerRunInfoReco("TriggerInfo");
    // se->registerSubsystem(triggerinfo);

    // Division: -----MbdReco-------------------------------------------------------------------
    // MbdReco *mbdreco = new MbdReco();
    // se->registerSubsystem(mbdreco);

    // std::cout << "ZDCReco " << std::endl;
    // CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
    // caZDC->set_detector_type(CaloTowerDefs::ZDC);
    // caZDC->set_builder_type(buildertype);
    // caZDC->set_processing_type(CaloWaveformProcessing::FAST);
    // caZDC->set_nsamples(16);
    // caZDC->set_offlineflag();
    // se->registerSubsystem(caZDC);

    //ZDC Reconstruction--Calib Info    
    // note : no ZDC information in MC                                                                                                                                                                          
    // ZdcReco *zdcreco = new ZdcReco();
    // zdcreco->set_zdc1_cut(0.0);
    // zdcreco->set_zdc2_cut(0.0);
    // se->registerSubsystem(zdcreco);

    GlobalVertexReco *gvr = new GlobalVertexReco("GlobalVertexReco");
    se->registerSubsystem(gvr);

    // MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    // // mb->Verbosity(100000);
    // mb->setSpecies(MinimumBiasInfo::SPECIES::OO);
    // se->registerSubsystem(mb);

    // CentralityReco *cr = new CentralityReco();
    // cr->Verbosity(0);
    // se->registerSubsystem(cr);


    MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    mb->Verbosity(111111);
    mb->setSpecies(MinimumBiasInfo::SPECIES::OO);
    mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations26_full/scales/cdb_centrality_scale_82391.root");
    se->registerSubsystem(mb);

    CentralityReco *cr = new CentralityReco();
    cr->Verbosity(111111);
    cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations26_full/scales/cdb_centrality_scale_82391.root");
    se->registerSubsystem(cr);

    // Division: -----Ntuplizer-------------------------------------------------------------------
    dNdEtaINTT * mydNdEtaINTT = new dNdEtaINTT(
        Form("dNdEtaINTT"),
        output_directory + "/" + output_ntuple_name,
        0 // note : isData
    );

    mydNdEtaINTT -> GetHEPMC(0);
    mydNdEtaINTT -> GetRecoCluster(true);
    mydNdEtaINTT -> GetINTTdata(true);
    mydNdEtaINTT -> GetCentrality(true);
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