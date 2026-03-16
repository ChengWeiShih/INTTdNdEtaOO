//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CheckInttRawHit.h.
//
// CheckInttRawHit(const std::string &name = "CheckInttRawHit")
// everything is keyed to CheckInttRawHit, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CheckInttRawHit::~CheckInttRawHit()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CheckInttRawHit::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CheckInttRawHit::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CheckInttRawHit::process_event(PHCompositeNode *topNode)
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
// int CheckInttRawHit::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CheckInttRawHit::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CheckInttRawHit::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CheckInttRawHit::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CheckInttRawHit::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CheckInttRawHit.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
CheckInttRawHit::CheckInttRawHit(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const std::string hotchannel_full_directory_in,

  const int triggered_BcoDiff_in,
  const bool apply_hotchannel_in,
  const bool clone_hit_remove_BCO_in,
  const bool hitQA_in,

  const std::vector<int> adc_conversion_vec_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  hotchannel_full_directory(hotchannel_full_directory_in),
  triggered_BcoDiff(triggered_BcoDiff_in),
  apply_hotchannel_tag(apply_hotchannel_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_in),
  hitQA_tag(hitQA_in),
  adc_conversion_vec(adc_conversion_vec_in),
  inttcont(nullptr)
{
  std::cout << "CheckInttRawHit::CheckInttRawHit(const std::string &name) Calling ctor" << std::endl;


  output_file_name = "RawHitCheck_"+std::to_string(runnumber);
  output_file_name += (clone_hit_remove_BCO_tag) ? "_clonehitremoveBCO" : "";
  output_file_name += (apply_hotchannel_tag) ? "_hotchannelremove" : "";
  output_file_name += (hitQA_tag) ? "_hitQA" : "";

  std::string job_index = std::to_string( process_id );
  int total_index_len = 5;
  job_index.insert(0, total_index_len - job_index.size(), '0');
  output_file_name += "_"+job_index;

  output_file_name += ".root";

  eID_count = 0;

}

//____________________________________________________________________________..
CheckInttRawHit::~CheckInttRawHit()
{
  std::cout << "CheckInttRawHit::~CheckInttRawHit() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int CheckInttRawHit::Init(PHCompositeNode *topNode)
{
  std::cout << "CheckInttRawHit::Init(PHCompositeNode *topNode) Initializing" << std::endl;


  all_felix_BcoDiffHist_vec.clear();

  for (int i = 0; i < 8; i++)
  {
    all_felix_BcoDiffHist_vec.push_back(new TH1D(Form("all_felix_BcoDiffHist_%d",i),Form("all_felix_BcoDiffHist_%d;BCO_Diff;Entries (/1)",i),128,0,128));
  }

  h1D_nInttHit = new TH1D("h1D_NInttHit","h1D_NInttHit;N INTT Hit (triggered #pm 1);Entries",200,0,5000);

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_file_name.c_str()),"recreate");

  hot_channel_map.clear();

  // note : load hot channel map
  if (apply_hotchannel_tag){PrepareHotChannel();}



  h1D_NClus = new TH1D("h1D_NClus","h1D_NClus;Number of INTT clusters (trigger);Entries",400,0,2000);
  h2D_ClusXY = new TH2D("h2D_ClusXY","h2D_ClusXY;ClusX [cm];ClusY [cm]",300,-15,15,300,-15,15);
  h1D_ClusZ = new TH1D("h1D_ClusZ","h1D_ClusZ;ClusZ [cm];Entries",60,-30,30);
  h2D_NInnerClus_NOuterClus = new TH2D("h2D_NInnerClus_NOuterClus","h2D_NInnerClus_NOuterClus;N inner Clus;N outer Clus",200,0,1000,200,0,1000);
  h1D_ClusPhiSize = new TH1D("h1D_ClusPhiSize","h1D_ClusPhiSize;Clus Phi size;Entries",128,0,128);
  h1D_ClusZSize = new TH1D("h1D_ClusZSize","h1D_ClusZSize;Clus Z size;Entries",20,0,20);
  h1D_ClusADC = new TH1D("h1D_ClusADC","h1D_ClusADC;Clus ADC;Entries",200,0,5000);
  h1D_ClusADC_narrow = new TH1D("h1D_ClusADC_narrow","h1D_ClusADC_narrow;Clus ADC;Entries",150,0,1500);
  h1D_ClusTimeBucket = new TH1D("h1D_ClusTimeBucket","h1D_ClusTimeBucket;Clus Time Bucket;Entries",700,-200,500);
  h2D_InttClusZ_ZID = new TH2D("h2D_InttClusZ_ZID","h2D_InttClusZ_ZID;INTT Clus Z [cm];INTT ClusZ ID;Entries",60, -30, 30, 10, 0, 10);


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CheckInttRawHit::InitRun(PHCompositeNode *topNode)
{
  std::cout << "CheckInttRawHit::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int CheckInttRawHit::PrepareINTT(PHCompositeNode *topNode)
{

  evt_inttHits_map.clear();

  std::vector<long long> evt_Unique_bco_full_vec; evt_Unique_bco_full_vec.clear();

  for (int DST_felix_i = 0; DST_felix_i < 8; DST_felix_i++)// note : FELIX server
  // for (int DST_felix_i = 0; DST_felix_i < 1; DST_felix_i++)// note : FELIX server
  {
    inttcont = nullptr;
    inttcont = findNode::getClass<InttRawHitContainer>(topNode, Form("%s_%d",m_InttRawNodeName.c_str(),DST_felix_i));
    // inttcont = findNode::getClass<InttRawHitContainer>(topNode, Form("%s",m_InttRawNodeName.c_str()));
    if (!inttcont)
    {
      std::cout << PHWHERE << std::endl;
      std::cout << "InttCombinedRawDataDecoder::process_event(PHCompositeNode* topNode)" << std::endl;
      std::cout << PHWHERE << Form("Error, can't find INTTRAWHIT, %d",DST_felix_i) << std::endl;
      // std::cout << "Could not get \"" << m_InttRawNodeName << "\" from Node Tree" << std::endl;
      std::cout << "Exiting" << std::endl;

      gSystem->Exit(1);
      exit(1);
    }

    if (inttcont->get_nhits() == 0) {
      // out_INTT_bcofull = 
      // std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT, FELIX: "<< DST_felix_i <<" - no INTT hit found" << std::endl;
      std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT, FELIX: "<< DST_felix_i <<" - no INTT hit found" << std::endl;
      continue;
    }

    for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
    {
      InttRawHit* intthit = inttcont->get_hit(i);

      // if (i == 0) {out_INTT_bcofull = intthit->get_bco();}

      if (std::find(evt_Unique_bco_full_vec.begin(), evt_Unique_bco_full_vec.end(), intthit->get_bco()) == evt_Unique_bco_full_vec.end()){
        evt_Unique_bco_full_vec.push_back(intthit->get_bco());
      }

      // amp = intthit->get_amplitude();
  
      int server = intthit->get_packetid() - 3001; // note : the felix server ID
      int felix_ch = intthit->get_fee(); // note : the felix channel ID 0 - 13
      int chip = (intthit->get_chip_id() - 1) % 26; // note : chip ID 0 - 25
      int channel = intthit->get_channel_id();
      int adc = intthit->get_adc();
      int bco = intthit->get_FPHX_BCO();

      if (hitQA_tag && (server < 0 || server > 7)) // note : server ID 0 - 7
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - server out of range, this server is : "<< server << std::endl;
        continue;
      }

      if (hitQA_tag && (felix_ch < 0 || felix_ch > 13)) // note : felix channel ID 0 - 13
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - felix channel out of range, this channel is : "<< felix_ch << std::endl;
        continue;
      }

      if (hitQA_tag && (chip < 0 || chip > 25)) // note : chip ID 0 - 25
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - chip out of range, this chip is : "<< chip << std::endl;
        continue;
      }

      if (hitQA_tag && (channel < 0 || channel > 127)) // note : channel ID 0 - 127
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - channel out of range, this channel is : "<< channel << std::endl;
        continue;
      }

      if (hitQA_tag && (adc < 0 || adc > 7)) // note : adc ID 0 - 7
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - adc out of range, this adc is : "<< adc << std::endl;
        continue;
      }
      
      if (hitQA_tag && (bco < 0 || bco > 127)) // note : bco ID 0 - 127
      {
        std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - bco out of range, this bco is : "<< bco << std::endl;
        continue;
      }

      

      if (apply_hotchannel_tag) // note : if we want to remove the hot channels
      {
        if (hot_channel_map.find(Form("%d_%d_%d_%d",server,felix_ch,chip,channel)) != hot_channel_map.end())
        {
          continue;
        }
      }

      if (clone_hit_remove_BCO_tag)
      {
        if (evt_inttHits_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
        {
          evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco};
        }
      }
      else // note : if we don't want to remove the clone hits
      {
        evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco}; // note : only index i to make the key unique
      }



    } // note : end of INTT raw hit loop

  } // note: done with all the FELIX servers

  if (evt_Unique_bco_full_vec.size() != 1){
    std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - multiple bco_full found in one event, bco_full vec size is : "<< evt_Unique_bco_full_vec.size() << std::endl;
    for (auto &bco_full : evt_Unique_bco_full_vec){
      std::cout << "bco_full : "<< bco_full << std::endl;
    }

   return Fun4AllReturnCodes::EVENT_OK;
  }

  // std::cout<<"eID: "<<eID_count<<", all hits count: "<< evt_inttHits_map.size() << ", bco_full: "<< evt_Unique_bco_full_vec[0] << std::endl;

  int triggered_hit_count = 0;
  for (const auto& pair : evt_inttHits_map) // note : total hits post HitQA, BadChannelMask, CloneHitRemoval, if selected
  {
    int server = pair.second.hit_server;
    // int felix_ch = pair.second.hit_felix_ch;
    // int chip = pair.second.hit_chip;
    // int channel = pair.second.hit_channel;
    // int adc = pair.second.hit_adc;
    int bco = pair.second.hit_bco;

    // inttHitstr hit = pair.second; 

    int bco_diff = (bco - (evt_Unique_bco_full_vec[0] & 0x7fU) + 128) % 128;

    if (bco_diff < 0 || bco_diff > 127) // note : bco_diff ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" CheckInttRawHit::PrepareINTT - bco_diff out of range, this bco_diff is : "<< bco_diff<<" hitbco : "<<bco<<" bco_full : "<<evt_Unique_bco_full_vec[0]<< std::endl;
      
      gSystem->Exit(1);
      exit(1);
    }

    if (bco_diff == triggered_BcoDiff || bco_diff == (triggered_BcoDiff + 1) % 128 || bco_diff == (triggered_BcoDiff - 1 + 128) % 128)
    {
      triggered_hit_count++;
    }

    all_felix_BcoDiffHist_vec[server] -> Fill(bco_diff);

  }

  h1D_nInttHit -> Fill(triggered_hit_count);


  return Fun4AllReturnCodes::EVENT_OK;
}


int CheckInttRawHit::PrepareINTTCluster(PHCompositeNode *topNode)
{
  
  clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);
  geometry = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);

  if (!clustermap)
  {
    std::cout << PHWHERE << "Missing clustermap, can't continue" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }
  if (!geometry)
  {
    std::cout << PHWHERE << "Missing geometry, can't continue" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  int nINTT_InnerClus = 0;
  int nINTT_OuterClus = 0;
  int nINTT_Clus = 0;

  for (const auto& hitsetkey : clustermap->getHitSetKeys(TrkrDefs::inttId))
  {
    auto range = clustermap->getClusters(hitsetkey);
    for (auto iter = range.first; iter != range.second; ++iter)
    {
      TrkrDefs::cluskey ckey = iter->first;
      TrkrCluster *cluster = clustermap->findCluster(ckey);
      if (!cluster){continue;}

      unsigned int trkrId = TrkrDefs::getTrkrId(ckey);
      if (trkrId != TrkrDefs::inttId) {continue;} //note: we want only INTT clusters

      int layer = (TrkrDefs::getLayer(ckey) == 3 || TrkrDefs::getLayer(ckey) == 4) ? 0 : 1;
      if (layer == 0){nINTT_InnerClus += 1;}
      else {nINTT_OuterClus += 1;}
      nINTT_Clus += 1;

      auto globalpos = geometry->getGlobalPosition(ckey, cluster);

      int phisize = cluster->getPhiSize();
      if (phisize <= 0) {phisize += 256;}

      int zsize = cluster->getZSize();
      
      int InttZID = InttDefs::getLadderZId(ckey);

      unsigned int ClusAdc = cluster->getAdc();

      int time_bucket = InttDefs::getTimeBucketId(ckey);

      h2D_ClusXY->Fill(globalpos(0),globalpos(1));
      h1D_ClusZ->Fill(globalpos(2));
      h1D_ClusPhiSize->Fill(phisize);
      h1D_ClusADC->Fill(ClusAdc);
      h1D_ClusADC_narrow->Fill(ClusAdc);
      h1D_ClusZSize->Fill(zsize);
      h1D_ClusTimeBucket->Fill(time_bucket);
      h2D_InttClusZ_ZID->Fill(globalpos(2),InttZID);
    }
  }

  h1D_NClus->Fill(nINTT_Clus);
  h2D_NInnerClus_NOuterClus->Fill(nINTT_InnerClus,nINTT_OuterClus);
 
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int CheckInttRawHit::process_event(PHCompositeNode *topNode)
{
  // std::cout << "CheckInttRawHit::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  if (eID_count % 200 == 0) {std::cout << "eID: "<< eID_count << std::endl;}

  PrepareINTT(topNode);
  PrepareINTTCluster(topNode);

  eID_count += 1;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CheckInttRawHit::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "CheckInttRawHit::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CheckInttRawHit::EndRun(const int runnumber)
{
  std::cout << "CheckInttRawHit::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

  file_out->cd();
  for (auto &hist : all_felix_BcoDiffHist_vec)
  {
    hist->Write();
  }
  h1D_nInttHit->Write();


  h1D_NClus -> Write();
  h2D_ClusXY -> Write();
  h1D_ClusZ -> Write();
  h2D_NInnerClus_NOuterClus -> Write();
  h1D_ClusPhiSize -> Write();
  h1D_ClusZSize -> Write();
  h1D_ClusADC -> Write();
  h1D_ClusADC_narrow -> Write();
  h1D_ClusTimeBucket -> Write();
  h2D_InttClusZ_ZID -> Write();


  file_out->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CheckInttRawHit::End(PHCompositeNode *topNode)
{
  std::cout << "CheckInttRawHit::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CheckInttRawHit::Reset(PHCompositeNode *topNode)
{
 std::cout << "CheckInttRawHit::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void CheckInttRawHit::Print(const std::string &what) const
{
  std::cout << "CheckInttRawHit::Print(const std::string &what) const Printing info for " << what << std::endl;
}

int CheckInttRawHit::PrepareHotChannel()
{
  hot_file_in = TFile::Open(hotchannel_full_directory.c_str());

  if (!hot_file_in)
  {
    std::cout << "CheckInttRawHit::PrepareHotChannel - hot channel file not found" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  hot_tree_in = (TTree*)hot_file_in->Get(hot_tree_name.c_str());

  hot_tree_in->SetBranchAddress("IID",&IID);
  hot_tree_in->SetBranchAddress("Ichannel",&Ichannel);
  hot_tree_in->SetBranchAddress("Ichip",&Ichip);
  hot_tree_in->SetBranchAddress("Ifelix_channel",&Ifelix_channel);
  hot_tree_in->SetBranchAddress("Ifelix_server",&Ifelix_server);
  hot_tree_in->SetBranchAddress("Iflag",&Iflag);

  hot_channel_map.clear();

  for (int i=0; i<hot_tree_in->GetEntries(); i++)
  {
    hot_tree_in->GetEntry(i);
    hot_channel_map[Form("%d_%d_%d_%d",Ifelix_server,Ifelix_channel,Ichip,Ichannel)] = Form("%d",Iflag);
  }

  std::cout<<"CheckInttRawHit::PrepareHotChannel - total hot channels loaded: "<< hot_channel_map.size()<< ", the ratio: "<< hot_channel_map.size() / double(112 * 128 * 26) << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}