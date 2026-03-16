// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTIMEPILEUP_H
#define INTIMEPILEUP_H

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
#include <ffarawobjects/Gl1Packetv3.h>
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

class InTimePileUp : public SubsysReco
{
 public:

  InTimePileUp(
    const std::string &name = "InTimePileUp",
    const int process_id_in = 0,
    const int runnumber_in = 0,
    const std::string output_directory_in = "no_directory"
  );

  std::string GetOutputFileName() { return output_file_name; }

  ~InTimePileUp() override;

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

 private:

    int process_id;
    int runnumber;
    std::string output_directory;

    Gl1Packet *gl1packet = nullptr;

    std::string output_file_name;
    TFile * file_out;
    TTree * tree_out;

    long long packet_BCO;
    long long raw_clock_count;
    long long raw_MBDNS1_count;

    int eID_count = 0;

    int clock_trigger_index = 0;
    int MBDNS1_trigger_index = 10;
};

#endif // INTIMEPILEUP_H
