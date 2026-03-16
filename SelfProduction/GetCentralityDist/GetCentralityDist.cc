//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in GetCentralityDist.h.
//
// GetCentralityDist(const std::string &name = "GetCentralityDist")
// everything is keyed to GetCentralityDist, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// GetCentralityDist::~GetCentralityDist()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int GetCentralityDist::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int GetCentralityDist::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int GetCentralityDist::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int GetCentralityDist::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int GetCentralityDist::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int GetCentralityDist::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int GetCentralityDist::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void GetCentralityDist::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
// [[maybe_unused]] suppresses compiler warnings if topNode is not used in this method
//
//____________________________________________________________________________..

#include "GetCentralityDist.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
GetCentralityDist::GetCentralityDist(const std::string &name):
 SubsysReco(name)
{
  std::cout << "GetCentralityDist::GetCentralityDist(const std::string &name) Calling ctor" << std::endl;

  file_out = new TFile("ChengWei_CentralityDist_test.root", "RECREATE");
  h1D_centrality = new TH1D("h1D_centrality", "Centrality Distribution;Centrality;Counts", 101,-0.5,100.5);
}

//____________________________________________________________________________..
GetCentralityDist::~GetCentralityDist()
{
  std::cout << "GetCentralityDist::~GetCentralityDist() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int GetCentralityDist::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "GetCentralityDist::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::InitRun([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "GetCentralityDist::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::process_event([[maybe_unused]] PHCompositeNode *topNode)
{
  // std::cout << "GetCentralityDist::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!m_CentInfo)
  {
      std::cout << PHWHERE << "Error, can't find CentralityInfov1. No centrality info is filled" << std::endl;
      // exit(1);
  }

  _minimumbiasinfo = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!_minimumbiasinfo)
  {
      std::cout << "Error, can't find MinimumBiasInfo. No minimum bias info is filled" << std::endl;
      // exit(1);
  }


  bool is_min_bias = (_minimumbiasinfo) ? _minimumbiasinfo->isAuAuMinimumBias() : false;
  float centrality_mbd_;

  if (is_min_bias){
    if (m_CentInfo)
    {
      if (m_CentInfo->has_centrality_bin(CentralityInfo::PROP::mbd_NS))
      {
          centrality_mbd_ = m_CentInfo->get_centrality_bin(CentralityInfo::PROP::mbd_NS);
          h1D_centrality -> Fill(centrality_mbd_);
      }
      else
      {
          std::cout << "[WARNING/ERROR] No centrality information found in CentralityInfo. Setting centrality_mbd_ to NaN. Please check!" << std::endl;
          m_CentInfo->identify();
          centrality_mbd_ = std::numeric_limits<float>::quiet_NaN();
      }
    } 
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  // std::cout << "GetCentralityDist::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::EndRun(const int runnumber)
{
  std::cout << "GetCentralityDist::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

  file_out -> cd();
  h1D_centrality -> Write();
  file_out -> Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "GetCentralityDist::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int GetCentralityDist::Reset([[maybe_unused]] PHCompositeNode *topNode)
{
 std::cout << "GetCentralityDist::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void GetCentralityDist::Print(const std::string &what) const
{
  std::cout << "GetCentralityDist::Print(const std::string &what) const Printing info for " << what << std::endl;
}
