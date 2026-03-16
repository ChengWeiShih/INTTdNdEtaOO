#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>

#include <iostream>
#include <vector>

void MC_NormalizeddNdEta()
{
    // -------------------------------------------------------------------------
    // Input
    // -------------------------------------------------------------------------
    TChain *chain = new TChain("EventTree");
    chain->Add("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/"
               "HIJING_INTTSurveyOnly_CentralityScaleTest/completed/"
               "MC_HIJING_Ntuple_00082391_test_surveyonly_centralitytest_0*.root");

    Long64_t nEntries = chain->GetEntries();
    std::cout << "Total entries: " << nEntries << std::endl;

    // -------------------------------------------------------------------------
    // Branch setup
    // -------------------------------------------------------------------------
    int NPrimaryG4P = 0;
    std::vector<float> *PrimaryG4P_Pt  = nullptr;
    std::vector<float> *PrimaryG4P_Eta = nullptr;
    std::vector<float> *PrimaryG4P_Phi = nullptr;
    std::vector<float> *PrimaryG4P_E   = nullptr;
    std::vector<int>  *PrimaryG4P_PID = nullptr;
    std::vector<bool> *PrimaryG4P_isChargeHadron = nullptr;
    int npart = 0;

    chain->SetBranchAddress("NPrimaryG4P",              &NPrimaryG4P);
    chain->SetBranchAddress("PrimaryG4P_Pt",            &PrimaryG4P_Pt);
    chain->SetBranchAddress("PrimaryG4P_Eta",           &PrimaryG4P_Eta);
    chain->SetBranchAddress("PrimaryG4P_Phi",           &PrimaryG4P_Phi);
    chain->SetBranchAddress("PrimaryG4P_E",             &PrimaryG4P_E);
    chain->SetBranchAddress("PrimaryG4P_PID",           &PrimaryG4P_PID);
    chain->SetBranchAddress("PrimaryG4P_isChargeHadron",&PrimaryG4P_isChargeHadron);
    chain->SetBranchAddress("npart",                    &npart);


    // -------------------------------------------------------------------------
    // Histogram:  x = npart,  y = charged-hadron count in |eta| < 0.5
    // -------------------------------------------------------------------------
    TH2D *h2_npart_count = new TH2D(
        "h2_npart_count",
        "Charged hadrons |#eta|<0.5 vs N_{part};N_{part};Charged-hadron count / N_{part}",
        100, 0, 100,   // npart axis
        200, 0, 10     // count/npart axis
    );

    // Profile: mean of (count/npart) vs npart
    TProfile *prof_npart_count = new TProfile(
        "prof_npart_count",
        "Profile: Charged hadrons |#eta|<0.5 vs N_{part};N_{part};#LTCharged-hadron count / N_{part}#GT",
        100, 0, 100    // same x-axis as h2_npart_count
    );


    // -------------------------------------------------------------------------
    // Event loop
    // -------------------------------------------------------------------------
    for (Long64_t iEvt = 0; iEvt < nEntries; ++iEvt)
    {

        chain->GetEntry(iEvt);

        if (iEvt % 10000 == 0)
            std::cout << "Processing event " << iEvt << " / " << nEntries << std::endl;


        int count = 0;
        for (int iP = 0; iP < NPrimaryG4P; ++iP)
        {
            if (std::abs((*PrimaryG4P_Eta)[iP]) < 0.5 &&
                (*PrimaryG4P_isChargeHadron)[iP] == 1)
            {
                ++count;
            }
        }

        double ratio = (npart > 0) ? count / double(npart) : 0.0;
        h2_npart_count->Fill(npart, ratio);
        prof_npart_count->Fill(npart, ratio);
    }

    // -------------------------------------------------------------------------
    // Canvas: TH2D (COLZ) + TProfile overlay
    // -------------------------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "dNdEta vs Npart", 800, 600);
    c1->cd();

    h2_npart_count->SetStats(0);
    h2_npart_count->Draw("COLZ");

    prof_npart_count->SetLineColor(kRed);
    prof_npart_count->SetLineWidth(2);
    prof_npart_count->SetMarkerColor(kRed);
    prof_npart_count->SetMarkerStyle(20);
    prof_npart_count->SetMarkerSize(0.8);
    prof_npart_count->Draw("SAME");

    TLegend *leg = new TLegend(0.55, 0.70, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(h2_npart_count,  "Event distribution", "f");
    leg->AddEntry(prof_npart_count, "Profile (mean)",      "lp");
    leg->Draw();

    c1->Update();

    // -------------------------------------------------------------------------
    // Output
    // -------------------------------------------------------------------------
    TFile *fOut = new TFile("MC_NormalizeddNdEta_output.root", "RECREATE");
    h2_npart_count->Write();
    prof_npart_count->Write();
    c1->Write();
    fOut->Close();

    std::cout << "Done. Output saved to MC_NormalizeddNdEta_output.root" << std::endl;

    delete chain;
}
