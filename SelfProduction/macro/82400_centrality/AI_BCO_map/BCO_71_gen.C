#include <TFile.h>
#include <TTree.h>
#include <iostream>

void BCO_71_gen() {
    // Create the new ROOT file
    TFile *f = new TFile("bcomap_cdb_updated_71.root", "RECREATE");

    // --- Part 1: Create the "Single" Tree ---
    TTree *t1 = new TTree("Single", "Single");
    Double_t DStdDev;
    Int_t Ievents, Irunmode, Isize;

    t1->Branch("DStdDev", &DStdDev, "DStdDev/D");
    t1->Branch("Ievents", &Ievents, "Ievents/I");
    t1->Branch("Irunmode", &Irunmode, "Irunmode/I");
    t1->Branch("Isize", &Isize, "Isize/I");

    // Set values based on your Single->Show(0) output
    DStdDev = 0.0;
    Ievents = 10000;
    Irunmode = 0;
    Isize = 112;
    t1->Fill();

    // --- Part 2: Create the "Multiple" Tree ---
    TTree *t2 = new TTree("Multiple", "Multiple");
    Int_t IID, Ibco_diff, Ifelix_channel, Ifelix_server;

    t2->Branch("IID", &IID, "IID/I");
    t2->Branch("Ibco_diff", &Ibco_diff, "Ibco_diff/I");
    t2->Branch("Ifelix_channel", &Ifelix_channel, "Ifelix_channel/I");
    t2->Branch("Ifelix_server", &Ifelix_server, "Ifelix_server/I");

    // Loop to fill 112 entries (8 servers * 14 channels)
    for (int i = 0; i < 112; i++) {
        IID = i;
        Ibco_diff = 71; // Updated value as requested
        
        // Logic derived from your Scan: 
        // Felix server changes every 14 entries, channel cycles 0-13
        Ifelix_server = i / 14; 
        Ifelix_channel = i % 14;

        t2->Fill();
    }

    // Write and close
    f->Write();
    std::cout << "File 'bcomap_cdb_updated_71.root' has been created successfully." << std::endl;
    f->Close();
}