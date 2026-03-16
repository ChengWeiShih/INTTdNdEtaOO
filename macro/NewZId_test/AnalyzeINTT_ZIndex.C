#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <vector>
#include <cmath>
#include <iostream>

// Helper function to calculate the 0-25 Global Z Index
int GetGlobalZIndex(int ClusLadderZId, float ClusLocalZ) {
    const float eps = 0.01; // Tolerance for float comparisons
    int local_idx = -1;
    int global_offset = 0;

    if (ClusLadderZId == 1 || ClusLadderZId == 3) {
        // These IDs (Outer segments) have 5 chips/variations
        std::vector<float> bins = {-4.0, -2.0, 0.0, 2.0, 4.0};
        for (int i = 0; i < (int)bins.size(); ++i) {
            if (std::fabs(ClusLocalZ - bins[i]) < eps) {
                local_idx = i;
                break;
            }
        }
        // Offset mapping: ID 1 is the most South (Start), ID 3 is the most North (End)
        global_offset = (ClusLadderZId == 1) ? 0 : 21; 
    } 
    else if (ClusLadderZId == 0 || ClusLadderZId == 2) {
        // These IDs (Inner segments) have 8 chips/variations
        std::vector<float> bins = {-5.6, -4.0, -2.4, -0.8, 0.8, 2.4, 4.0, 5.6};
        for (int i = 0; i < (int)bins.size(); ++i) {
            if (std::fabs(ClusLocalZ - bins[i]) < eps) {
                local_idx = i;
                break;
            }
        }
        // Offset mapping: ID 0 follows ID 1, ID 2 follows ID 0
        global_offset = (ClusLadderZId == 0) ? 5 : 13;
    }

    if (local_idx == -1) return -1; 
    return global_offset + local_idx;
}

void AnalyzeINTT_ZIndex() {
    // 1. Input configuration
    std::string input_path = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260215/HIJING_INTTSurveyOnly_CentralityScaleTest/completed/MC_HIJING_Ntuple_00082400_test_surveyonly_centralitytest_000000.root";
    
    TFile *f_in = TFile::Open(input_path.c_str());
    if (!f_in || f_in->IsZombie()) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return;
    }

    TTree *tree = (TTree*)f_in->Get("EventTree");

    // 2. Setup Branch Addresses
    // Note: We use pointers for vector branches
    std::vector<float> *ClusZ = 0;
    std::vector<float> *ClusLocalY = 0; // User requested to read LocalY as LocalZ
    std::vector<unsigned char> *ClusLadderZId = 0;

    tree->SetBranchAddress("ClusZ", &ClusZ);
    tree->SetBranchAddress("ClusLocalY", &ClusLocalY);
    tree->SetBranchAddress("ClusLadderZId", &ClusLadderZId);

    // 3. Prepare Output
    TFile *f_out = new TFile("INTT_ZCorrelation_Output.root", "RECREATE");
    TH2D *h2D_corr = new TH2D("h2D_ClusZ_vs_GlobalIdx", 
                              "Correlation: ClusZ vs. Global Z Index;ClusZ [cm];Global Z Index (0-25)", 
                              500, -25, 25, 26, -0.5, 25.5);

    // 4. Event Loop
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Processing " << nEntries << " entries..." << std::endl;

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // Loop over clusters in this event
        for (size_t c = 0; c < ClusZ->size(); ++c) {
            
            // Map the indices
            int zid = (int)ClusLadderZId->at(c);
            float lz = ClusLocalY->at(c); // Reading LocalY as LocalZ
            float gz = ClusZ->at(c);

            int global_index = GetGlobalZIndex(zid, lz);

            if (global_index != -1) {
                h2D_corr->Fill(gz, global_index);
            }
        }

        if (i % 100 == 0) std::cout << "Event " << i << " processed." << std::endl;
    }

    // 5. Save and Close
    h2D_corr->Write();
    f_out->Close();
    f_in->Close();

    std::cout << "Analysis complete. Output saved to INTT_ZCorrelation_Output.root" << std::endl;
}