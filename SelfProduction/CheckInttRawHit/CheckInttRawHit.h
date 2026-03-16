// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CHECKINTTRAWHIT_H
#define CHECKINTTRAWHIT_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>   // for PHIODataNode
#include <phool/PHNodeIterator.h> // for PHNodeIterator
#include <phool/PHObject.h>       // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/InttDefs.h>

#include <ffarawobjects/Gl1RawHit.h>
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

// #include <intt/InttMap.h>
// #include <intt/InttFeeMap.h>

#include <TMath.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <THStack.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <numeric>

class PHCompositeNode;
class Gl1Packetv2;
class InttRawHitContainer;

class TH1;
class TTree;
class TCanvas;

class TrkrClusterContainer;
class TrkrCluster;

class CheckInttRawHit : public SubsysReco
{
 public:

  CheckInttRawHit(
        const std::string &name = "INTTRawHitSanityCheck",
        const int process_id_in = 0,
        const int runnumber_in = 0,
        const std::string output_directory_in = "no_directory",
        
        const std::string hotchannel_full_directory_in = "no_directory", // note : bad channel map 
        const int triggered_BcoDiff_in = 67, // note : -1 for streaming (maybe)
        const bool apply_hotchannel_in = true,
        const bool clone_hit_remove_BCO_in = true,
        const bool hitQA_in = true,

        const std::vector<int> adc_conversion_vec_in = {30, 35, 45, 80, 115, 150, 185, 215}
  );

  ~CheckInttRawHit() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  std::string GetOutputFileName() { return output_file_name; }

 private:
    
    struct inttHitstr{
        int hit_server;
        int hit_felix_ch;
        int hit_chip;
        int hit_channel;
        int hit_adc;
        int hit_bco;
    };

    int process_id;
    int runnumber;
    std::string output_directory;
    std::string hotchannel_full_directory;
    int triggered_BcoDiff;
    bool apply_hotchannel_tag;
    bool clone_hit_remove_BCO_tag;
    bool hitQA_tag;
    std::vector<int> adc_conversion_vec;

    long long eID_count;
    
    std::map<std::string, inttHitstr> evt_inttHits_map;
    TH1D * h1D_nInttHit;

    int PrepareINTT(PHCompositeNode *topNode);
    int PrepareINTTCluster(PHCompositeNode *topNode);


    std::string m_clusterContainerName = "TRKR_CLUSTER";
    std::string m_actsgeometryName = "ActsGeometry";
    TrkrClusterContainer*clustermap;
    ActsGeometry*geometry;

    TH1D * h1D_NClus;
    TH2D * h2D_ClusXY;
    TH1D * h1D_ClusZ;
    TH2D * h2D_NInnerClus_NOuterClus;
    TH1D * h1D_ClusPhiSize;
    TH1D * h1D_ClusZSize;
    TH1D * h1D_ClusADC;
    TH1D * h1D_ClusADC_narrow;
    TH1D * h1D_ClusTimeBucket;
    TH2D * h2D_InttClusZ_ZID;


    std::string output_file_name;

    std::string m_InttRawNodeName = "INTTRAWHIT";
    InttRawHitContainer* inttcont;

    std::vector<TH1D *> all_felix_BcoDiffHist_vec;

    TFile * file_out;

    // note : -------------------------------- Hot channel branch ------------------------------
    int PrepareHotChannel();
    TFile * hot_file_in;
    TTree * hot_tree_in;
    std::string hot_tree_name = "Multiple";
    int IID;             
    int Ichannel;        
    int Ichip;           
    int Ifelix_channel;  
    int Ifelix_server;   
    int Iflag;           

    std::map<std::string,std::string> hot_channel_map;
};

#endif // CHECKINTTRAWHIT_H
