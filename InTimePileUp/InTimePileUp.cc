//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in InTimePileUp.h.
//
// InTimePileUp(const std::string &name = "InTimePileUp")
// everything is keyed to InTimePileUp, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// InTimePileUp::~InTimePileUp()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int InTimePileUp::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int InTimePileUp::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int InTimePileUp::process_event(PHCompositeNode *topNode)
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
// int InTimePileUp::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int InTimePileUp::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int InTimePileUp::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int InTimePileUp::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void InTimePileUp::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "InTimePileUp.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
InTimePileUp::InTimePileUp(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in)
{
  std::cout << "InTimePileUp::InTimePileUp(const std::string &name) Calling ctor" << std::endl;


  output_file_name = "InTimePileUp_"+std::to_string(runnumber);

  std::string job_index = std::to_string( process_id );
  int total_index_len = 5;
  job_index.insert(0, total_index_len - job_index.size(), '0');
  output_file_name += "_"+job_index;

  output_file_name += ".root";

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_file_name.c_str()),"recreate");
  tree_out = new TTree("tree","tree");

  tree_out->Branch("eID", &eID_count);
  tree_out->Branch("packet_BCO", &packet_BCO);
  tree_out->Branch("raw_clock_count", &raw_clock_count);
  tree_out->Branch("raw_MBDNS1_count", &raw_MBDNS1_count);

  eID_count = 0;
}

//____________________________________________________________________________..
InTimePileUp::~InTimePileUp()
{
  std::cout << "InTimePileUp::~InTimePileUp() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int InTimePileUp::Init(PHCompositeNode *topNode)
{
  std::cout << "InTimePileUp::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::InitRun(PHCompositeNode *topNode)
{
  std::cout << "InTimePileUp::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::process_event(PHCompositeNode *topNode)
{
  // std::cout << "InTimePileUp::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  gl1packet = findNode::getClass<Gl1Packetv3>(topNode, "14001"/*"GL1RAWHIT"*/ /*"GL1Packet"*/); // note : for the selfgen DST, it may be the "GL1RAWHIT"

  if (!gl1packet)
  {
    std::cout << "InTimePileUp::process_event(PHCompositeNode *topNode) No GL1Packetv3 found on node tree, skipping event" << std::endl;
    eID_count += 1;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  packet_BCO = gl1packet->getBCO();
  raw_clock_count = gl1packet -> lValue(clock_trigger_index,0);
  raw_MBDNS1_count = gl1packet -> lValue(MBDNS1_trigger_index,0);

  if (eID_count % 500 == 0){

    std::cout<<"====================================================================================="<<std::endl;
    gl1packet->dump();

    std::cout<<"eID: "<<eID_count<<", packet_BCO: "<<packet_BCO<<", raw_clock_count: "<<raw_clock_count<<", raw_MBDNS1_count: "<<raw_MBDNS1_count<<std::endl;
  }

  tree_out->Fill();

  eID_count += 1;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "InTimePileUp::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::EndRun(const int runnumber)
{
  std::cout << "InTimePileUp::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;


  file_out->cd();
  tree_out->Write();
  file_out->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::End(PHCompositeNode *topNode)
{
  std::cout << "InTimePileUp::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InTimePileUp::Reset(PHCompositeNode *topNode)
{
 std::cout << "InTimePileUp::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void InTimePileUp::Print(const std::string &what) const
{
  std::cout << "InTimePileUp::Print(const std::string &what) const Printing info for " << what << std::endl;
}
