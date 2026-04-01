#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TPad.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>

#include <cmath>
#include <iostream>

void DrawCentralityBias()
{
  const TString input_dir =
      "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/"
      "Run6_EvtZFitWidthChange/EvtInterval/completed";
  const TString input_pattern =
      "Data_EvtVtxZProtoTracklet_FieldOff_BcoFullDiff_00054280_00%03d.root";
  const TString output_pdf = input_dir + "/CentralityBias_Run54280.pdf";
  const TString output_pdf_2d = input_dir + "/MBDzvtx_vs_MBDcentrality_Run54280.pdf";
  const TString output_pdf_min_bias = input_dir + "/h_min_bias_Run54280.pdf";

  TH1D *h_min_bias = new TH1D(
      "h_min_bias",
      ";MBD centrality [%];Entries (A.U.)",
      101,
      -0.5,
      100.5);
  TH1D *h_min_bias_bco_cut = new TH1D(
      "h_min_bias_bco_cut",
      ";MBD centrality [%];Entries (A.U.)",
      101,
      -0.5,
      100.5);
  TH2D *h2_mbd_z_vtx_vs_centrality = new TH2D(
      "h2_mbd_z_vtx_vs_centrality",
      ";MBD_z_vtx [cm];Centrality [%]",
      120,
      -30,
      30,
      101,
      -0.5,
      100.5);

  TH1D *h1D_event_interval = new TH1D(
      "h1D_event_interval",
      "h1D_event_interval;Time interval between two triggered events [BCO];Entries",
      200,
      0,
      200);

  h_min_bias->SetDirectory(nullptr);
  h_min_bias_bco_cut->SetDirectory(nullptr);
  h2_mbd_z_vtx_vs_centrality->SetDirectory(nullptr);
  h1D_event_interval->SetDirectory(nullptr);

  bool is_min_bias = false;
  float MBD_centrality = 0.;
  int InttBcoFullDiff_next = 0;
  float MBD_z_vtx = 0.;
  int NClus = 0;

  Long64_t total_entries = 0;

  for (int ifile = 0; ifile < 300; ++ifile)
  {
    const TString input_file = Form("%s/%s", input_dir.Data(), Form(input_pattern.Data(), ifile));

    std::cout << "running the file: " << ifile << std::endl;

    if (gSystem->AccessPathName(input_file))
    {
      std::cerr << "Skip missing file: " << input_file << std::endl;
      continue;
    }

    TFile *file = TFile::Open(input_file, "READ");
    if (!file || file->IsZombie())
    {
      std::cerr << "Failed to open file: " << input_file << std::endl;
      if (file)
      {
        file->Close();
        delete file;
      }
      continue;
    }

    TTree *tree = dynamic_cast<TTree *>(file->Get("EventTree"));
    if (!tree)
    {
      std::cerr << "Missing EventTree in file: " << input_file << std::endl;
      file->Close();
      delete file;
      continue;
    }

    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("is_min_bias", 1);
    tree->SetBranchStatus("MBD_centrality", 1);
    tree->SetBranchStatus("InttBcoFullDiff_next", 1);
    tree->SetBranchStatus("NClus", 1);
    tree->SetBranchStatus("MBD_z_vtx", 1);

    tree->SetBranchAddress("is_min_bias", &is_min_bias);
    tree->SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree->SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);
    tree->SetBranchAddress("NClus", &NClus);
    tree->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);

    const Long64_t nentries = tree->GetEntries();
    total_entries += nentries;

    for (Long64_t ientry = 0; ientry < nentries; ++ientry)
    {
      tree->GetEntry(ientry);

      if (!is_min_bias)
      {
        continue;
      }

      h1D_event_interval->Fill(InttBcoFullDiff_next);
      h2_mbd_z_vtx_vs_centrality->Fill(MBD_z_vtx, MBD_centrality);

      // if (std::fabs(MBD_z_vtx) >= 20)
      // {
      //   continue;
      // }

      h_min_bias->Fill(MBD_centrality);

      if (InttBcoFullDiff_next >= 61)
      {
        h_min_bias_bco_cut->Fill(MBD_centrality);
      }
    }

    file->Close();
    delete file;
  }

  h_min_bias->Sumw2();
  h_min_bias_bco_cut->Sumw2();

  if (h_min_bias->Integral() > 0)
  {
    h_min_bias->Scale(1. / h_min_bias->Integral());
  }
  if (h_min_bias_bco_cut->Integral() > 0)
  {
    h_min_bias_bco_cut->Scale(1. / h_min_bias_bco_cut->Integral());
  }

  TH1D *h_ratio = static_cast<TH1D *>(h_min_bias_bco_cut->Clone("h_ratio"));
  h_ratio->SetDirectory(nullptr);
  h_ratio->SetTitle("");
  h_ratio->Divide(h_min_bias);

  h_min_bias->SetStats(0);
  h_min_bias_bco_cut->SetStats(0);
  h_ratio->SetStats(0);
  h2_mbd_z_vtx_vs_centrality->SetStats(0);

  h_min_bias->SetLineColor(kBlack);
  h_min_bias->SetLineWidth(2);

  h_min_bias_bco_cut->SetLineColor(kRed + 1);
  h_min_bias_bco_cut->SetLineWidth(2);

  h_ratio->SetLineColor(kBlue + 1);
  h_ratio->SetMarkerColor(kBlue + 1);
  h_ratio->SetMarkerStyle(20);
  h_ratio->SetMarkerSize(0.8);
  h_ratio->GetYaxis()->SetTitle("With Cut / No cut");
  h_ratio->GetYaxis()->CenterTitle();
  h_ratio->GetYaxis()->SetNdivisions(505);
  h_ratio->GetYaxis()->SetTitleSize(0.09);
  h_ratio->GetYaxis()->SetTitleOffset(0.5);
  h_ratio->GetYaxis()->SetLabelSize(0.08);
  h_ratio->GetXaxis()->SetTitle("MBD centrality [%]");
  h_ratio->GetXaxis()->SetTitleSize(0.10);
  h_ratio->GetXaxis()->SetTitleOffset(1.0);
  h_ratio->GetXaxis()->SetLabelSize(0.08);
  h_ratio->SetMinimum(0.9);
  h_ratio->SetMaximum(1.1);

  TCanvas *canvas = new TCanvas("canvas", "Centrality Bias", 900, 900);

  TPad *pad_top = new TPad("pad_top", "pad_top", 0.0, 0.30, 1.0, 1.0);
  TPad *pad_bottom = new TPad("pad_bottom", "pad_bottom", 0.0, 0.0, 1.0, 0.30);

  pad_top->SetBottomMargin(0.02);
  pad_top->SetLeftMargin(0.12);
  pad_top->SetRightMargin(0.05);

  pad_bottom->SetTopMargin(0.03);
  pad_bottom->SetBottomMargin(0.32);
  pad_bottom->SetLeftMargin(0.12);
  pad_bottom->SetRightMargin(0.05);
  pad_bottom->SetGridy();

  pad_top->Draw();
  pad_bottom->Draw();

  pad_top->cd();
  h_min_bias->GetXaxis()->SetLabelSize(0.0);
  h_min_bias->GetXaxis()->SetTitleSize(0.0);
  h_min_bias->GetYaxis()->SetTitleOffset(1.2);
  h_min_bias->SetMaximum(h_min_bias->GetBinContent(h_min_bias->GetMaximumBin()) * 1.4);
  h_min_bias->Draw("hist");
  h_min_bias_bco_cut->Draw("hist same");

  TLegend *legend = new TLegend(0.25, 0.73, 0.88, 0.88);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  legend->AddEntry(h_min_bias, "is minimum bias event", "l");
  legend->AddEntry(h_min_bias_bco_cut, "is minimum bias event && event interval >= 61", "l");
  legend->Draw();

  pad_bottom->cd();
  h_ratio->Draw("ep");

  canvas->SaveAs(output_pdf);

  canvas->Clear();
  canvas->SetLeftMargin(0.12);
  canvas->SetRightMargin(0.16);
  canvas->SetBottomMargin(0.12);
  canvas->SetTopMargin(0.08);
  h1D_event_interval->Draw("hist");
  canvas->Print(Form("%s/Event_interval_200.pdf", input_dir.Data()));

  canvas->Clear();
  canvas->SetLeftMargin(0.12);
  canvas->SetRightMargin(0.18);
  canvas->SetBottomMargin(0.12);
  canvas->SetTopMargin(0.08);
  h2_mbd_z_vtx_vs_centrality->Draw("colz");
  canvas->Print(output_pdf_2d);

  canvas->Clear();
  canvas->SetLeftMargin(0.16);
  canvas->SetRightMargin(0.05);
  canvas->SetBottomMargin(0.12);
  canvas->SetTopMargin(0.08);
  h_min_bias->GetXaxis()->SetLabelSize(0.04);
  h_min_bias->GetXaxis()->SetTitleSize(0.045);
  h_min_bias->GetYaxis()->SetTitleOffset(1.8);
  h_min_bias->Draw("hist");

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.045);
  latex.DrawLatex(0.18, 0.92, "#bf{#it{sPHENIX} Internal}");

  canvas->Print(output_pdf_min_bias);

  std::cout << "Processed entries: " << total_entries << std::endl;
  std::cout << "Saved plot to: " << output_pdf << std::endl;
  std::cout << "Saved plot to: " << output_pdf_2d << std::endl;
  std::cout << "Saved plot to: " << output_pdf_min_bias << std::endl;
}
