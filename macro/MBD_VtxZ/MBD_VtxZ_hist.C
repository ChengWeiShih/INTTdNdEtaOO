
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <string>
#include <glob.h>

void MBD_VtxZ_hist()
{
    // -------------------------------------------------------
    // Input / output paths
    // -------------------------------------------------------
    const TString inputDir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/"
        "dNdEtaOO/test_82391_INTTsurveyOnly/EvtVtxZ/completed/";

    const TString pattern =
        inputDir + "Data_EvtVtxZProtoTracklet_FieldOn_BcoFullDiff_"
                   "VtxZReco_00082391_*.root";

    const TString outputFile =
        inputDir + "MBD_VtxZ_hist_00082391.root";

    // -------------------------------------------------------
    // Collect file list (first 100 files)
    // -------------------------------------------------------
    glob_t globResult;
    glob(pattern.Data(), GLOB_TILDE, nullptr, &globResult);

    std::vector<std::string> fileList;
    const int maxFiles = 100;
    for (size_t i = 0; i < globResult.gl_pathc && (int)i < maxFiles; ++i)
        fileList.push_back(globResult.gl_pathv[i]);
    globfree(&globResult);

    std::cout << "Found " << fileList.size() << " file(s) to process." << std::endl;

    // -------------------------------------------------------
    // Create histograms
    // -------------------------------------------------------
    TH1D *h_MBD_z_vtx = new TH1D("h_MBD_z_vtx",
                                  "MBD vtx Z (min-bias);MBD z_{vtx} [cm];Events",
                                  200, -100, 100);
    h_MBD_z_vtx->Sumw2();

    TH2D *h2_VtxZ_ChargeSum = new TH2D("h2_VtxZ_ChargeSum",
                                        "MBD vtx Z vs Charge Sum;"
                                        "MBD z_{vtx} [cm];MBD charge sum [a.u.]",
                                        200, -100, 100,
                                        200,    0, 1000);
    h2_VtxZ_ChargeSum->Sumw2();

    // -------------------------------------------------------
    // Loop over files
    // -------------------------------------------------------
    long long globalEvt = 0;

    for (size_t iFile = 0; iFile < fileList.size(); ++iFile)
    {
        const std::string &fname = fileList[iFile];
        std::cout << "[File " << iFile + 1 << "/" << fileList.size() << "] "
                  << fname << std::endl;

        TFile *f = TFile::Open(fname.c_str(), "READ");
        if (!f || f->IsZombie())
        {
            std::cerr << "  WARNING: cannot open file, skipping." << std::endl;
            if (f) delete f;
            continue;
        }

        TTree *tree = dynamic_cast<TTree *>(f->Get("EventTree"));
        if (!tree)
        {
            std::cerr << "  WARNING: EventTree not found, skipping." << std::endl;
            f->Close();
            delete f;
            continue;
        }

        // ---- set branch addresses ----
        Float_t  MBD_z_vtx          = 0.f;
        Float_t  MBD_charge_sum      = 0.f;
        Bool_t   is_min_bias         = false;
        Int_t    MBDNS1_live_incl    = 0;

        tree->SetBranchStatus("*",                    0);  // disable all
        tree->SetBranchStatus("MBD_z_vtx",            1);
        tree->SetBranchStatus("MBD_charge_sum",        1);
        tree->SetBranchStatus("is_min_bias",           1);
        tree->SetBranchStatus("MBDNS1_live_inclusive", 1);

        tree->SetBranchAddress("MBD_z_vtx",            &MBD_z_vtx);
        tree->SetBranchAddress("MBD_charge_sum",        &MBD_charge_sum);
        tree->SetBranchAddress("is_min_bias",           &is_min_bias);
        tree->SetBranchAddress("MBDNS1_live_inclusive", &MBDNS1_live_incl);

        Long64_t nEntries = tree->GetEntries();

        for (Long64_t iEv = 0; iEv < nEntries; ++iEv)
        {
            tree->GetEntry(iEv);

            if (is_min_bias)
                h_MBD_z_vtx->Fill(MBD_z_vtx);

            // Fill TH2D whenever both values are finite (non-NaN, non-Inf)
            if (TMath::Finite(MBD_z_vtx) && TMath::Finite(MBD_charge_sum))
                h2_VtxZ_ChargeSum->Fill(MBD_z_vtx, MBD_charge_sum);

            ++globalEvt;
            if (globalEvt % 100 == 0)
                std::cout << "  Processed " << globalEvt << " events so far..." << std::endl;
        }

        f->Close();
        delete f;
    }

    std::cout << "\nTotal events processed: " << globalEvt << std::endl;
    std::cout << "Entries in h_MBD_z_vtx      : " << (long long)h_MBD_z_vtx->GetEntries()      << std::endl;
    std::cout << "Entries in h2_VtxZ_ChargeSum: " << (long long)h2_VtxZ_ChargeSum->GetEntries() << std::endl;

    // -------------------------------------------------------
    // Save histogram
    // -------------------------------------------------------
    TFile *fOut = TFile::Open(outputFile.Data(), "RECREATE");
    if (!fOut || fOut->IsZombie())
    {
        std::cerr << "ERROR: cannot create output file " << outputFile << std::endl;
        return;
    }
    h_MBD_z_vtx->Write();
    h2_VtxZ_ChargeSum->Write();
    fOut->Close();
    delete fOut;

    std::cout << "Histograms saved to: " << outputFile << std::endl;

    delete h_MBD_z_vtx;
    delete h2_VtxZ_ChargeSum;
}
