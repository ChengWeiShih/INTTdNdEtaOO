// CheckMultiVtx.C
// Read up to 100 MC_EvtVtxZProtoTracklet_MultiVtx output files,
// fill two histograms, print events with N_INTT_VtxZ > 1,
// and save the corresponding event display canvases to a multi-page PDF.

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <iostream>
#include <vector>
#include <string>

void CheckMultiVtx()
{
    // ---- Input / output paths ----
    std::string input_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260318/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/AllEvtVtxZ/completed";
    std::string pattern   = input_dir + "/MC_EvtVtxZProtoTracklet_MultiVtx_FieldOn_VtxZReco_0*.root";

    std::string output_dir    = input_dir;
    std::string output_root   = output_dir + "/CheckMultiVtx_output.root";
    std::string output_pdf    = output_dir + "/MultiVtx_EventDisplay.pdf";

    // ---- Collect file list (up to 100 files) ----
    TString file_list_raw = gSystem->GetFromPipe(("ls " + pattern + " 2>/dev/null").c_str());
    std::vector<std::string> file_list;
    {
        TObjArray* tokens = file_list_raw.Tokenize("\n");
        for (int i = 0; i < tokens->GetEntries(); i++){
            std::string f = ((TObjString*)tokens->At(i))->GetString().Data();
            if (!f.empty()) file_list.push_back(f);
        }
        delete tokens;
    }
    const int max_files = 1000;
    if ((int)file_list.size() > max_files) file_list.resize(max_files);
    std::cout << "Found " << file_list.size() << " file(s) to process." << std::endl;

    // ---- Histograms ----
    TH1D* h_NVtxZ = new TH1D(
        "h_NVtxZ", "Number of INTT Z-Vertices per Event;N_{VtxZ};Entries",
        10, 0, 10);
    TH2D* h_NVtxZ_vs_NClus = new TH2D(
        "h_NVtxZ_vs_NClus", "N_{VtxZ} vs NClus;NClus;N_{VtxZ};Entries",
        200, 0, 800,
        10, 0, 10);

    TH2D* h_FitHeight_vs_NClus = new TH2D(
        "h_FitHeight_vs_NClus", "FitHeight vs NClus;NClus;FitHeight;Entries",
        200, 0, 800,
        200, 0, 2);

    // ---- Branch variables ----
    std::vector<double>* vec_INTTvtxZ           = nullptr;
    std::vector<double>* vec_INTTvtxZError       = nullptr;
    std::vector<double>* vec_NgroupTrapezoidal   = nullptr;
    std::vector<double>* vec_NgroupCoarse        = nullptr;
    std::vector<double>* vec_TrapezoidalFitWidth = nullptr;
    std::vector<double>* vec_TrapezoidalFWHM     = nullptr;
    std::vector<double>* vec_FitHeight           = nullptr;
    int N_INTT_VtxZ = 0;
    int NClus       = 0;
    int eID_count   = 0; // note : global event ID stored in the output tree

    int total_events        = 0;
    int total_multiVtx_evts = 0;

    // ---- Open PDF (multi-page) ----
    // note : using a dummy canvas to open/close the PDF stream
    TCanvas* pdf_canvas = new TCanvas("pdf_canvas", "pdf_canvas", 950, 800);
    pdf_canvas->Print((output_pdf + "[").c_str()); // open PDF

    // ---- Loop over files ----
    for (int file_i = 0; file_i < (int)file_list.size(); file_i++){
        std::string data_file = file_list[file_i];
        std::cout << Form("[%03d/%03d] %s", file_i+1, (int)file_list.size(), data_file.c_str()) << std::endl;

        // note : derive the matching plot file name
        // e.g. .../MC_EvtVtxZ..._00003.root  →  .../Plot_MC_EvtVtxZ..._00003.root
        std::string basename  = data_file.substr(data_file.rfind('/')+1); // just the filename
        std::string plot_file = input_dir + "/Plot_" + basename;

        TFile* fdata = TFile::Open(data_file.c_str(), "READ");
        if (!fdata || fdata->IsZombie()){
            std::cout << "  WARNING: cannot open data file, skipping." << std::endl;
            continue;
        }

        TTree* tree = (TTree*)fdata->Get("EventTree");
        if (!tree){
            std::cout << "  WARNING: EventTree not found, skipping." << std::endl;
            fdata->Close(); delete fdata; continue;
        }

        // note : mask all, enable only needed branches
        tree->SetBranchStatus("*", 0);
        tree->SetBranchStatus("vec_INTTvtxZ",           1);
        tree->SetBranchStatus("vec_INTTvtxZError",      1);
        tree->SetBranchStatus("vec_NgroupTrapezoidal",  1);
        tree->SetBranchStatus("vec_NgroupCoarse",       1);
        tree->SetBranchStatus("vec_TrapezoidalFitWidth",1);
        tree->SetBranchStatus("vec_TrapezoidalFWHM",    1);
        tree->SetBranchStatus("vec_FitHeight",          1);
        tree->SetBranchStatus("N_INTT_VtxZ",            1);
        tree->SetBranchStatus("NClus",                  1);
        tree->SetBranchStatus("eID_count",              1);

        vec_INTTvtxZ = vec_INTTvtxZError = vec_NgroupTrapezoidal =
            vec_NgroupCoarse = vec_TrapezoidalFitWidth = vec_TrapezoidalFWHM = vec_FitHeight = nullptr;
        N_INTT_VtxZ = NClus = eID_count = 0;

        tree->SetBranchAddress("vec_INTTvtxZ",           &vec_INTTvtxZ);
        tree->SetBranchAddress("vec_INTTvtxZError",      &vec_INTTvtxZError);
        tree->SetBranchAddress("vec_NgroupTrapezoidal",  &vec_NgroupTrapezoidal);
        tree->SetBranchAddress("vec_NgroupCoarse",       &vec_NgroupCoarse);
        tree->SetBranchAddress("vec_TrapezoidalFitWidth",&vec_TrapezoidalFitWidth);
        tree->SetBranchAddress("vec_TrapezoidalFWHM",   &vec_TrapezoidalFWHM);
        tree->SetBranchAddress("vec_FitHeight",          &vec_FitHeight);
        tree->SetBranchAddress("N_INTT_VtxZ",           &N_INTT_VtxZ);
        tree->SetBranchAddress("NClus",                 &NClus);
        tree->SetBranchAddress("eID_count",             &eID_count);

        Long64_t nEntries = tree->GetEntries();

        // note : open the matching plot file once per data file (lazy — only if needed)
        TFile* fplot = nullptr;

        for (Long64_t evt = 0; evt < nEntries; evt++){
            tree->GetEntry(evt);
            total_events++;

            h_NVtxZ         ->Fill(N_INTT_VtxZ);
            h_NVtxZ_vs_NClus->Fill(NClus, N_INTT_VtxZ);

            if (N_INTT_VtxZ == 0) {continue;}

            h_FitHeight_vs_NClus->Fill(NClus, (*vec_FitHeight)[0]);


            if (N_INTT_VtxZ <= 1) continue;

            // ---- Event with multiple vertices ----
            total_multiVtx_evts++;

            std::cout << Form("  [MultiVtx] file_index=%d  event=%lld  eID_count=%d  N_VtxZ=%d  NClus=%d",
                              file_i, evt, eID_count, N_INTT_VtxZ, NClus) << std::endl;

            for (int v = 0; v < N_INTT_VtxZ; v++){
                std::cout << Form(
                    "    vtx[%d]: Z=%8.4f cm  Zerr=%7.4f cm  NgroupTrap=%.0f"
                    "  NgroupCoarse=%.0f  FitWidth=%7.4f  FWHM=%7.4f",
                    v,
                    (vec_INTTvtxZ           && v < (int)vec_INTTvtxZ->size())           ? vec_INTTvtxZ->at(v)           : -999.,
                    (vec_INTTvtxZError      && v < (int)vec_INTTvtxZError->size())      ? vec_INTTvtxZError->at(v)      : -999.,
                    (vec_NgroupTrapezoidal  && v < (int)vec_NgroupTrapezoidal->size())  ? vec_NgroupTrapezoidal->at(v)  : -999.,
                    (vec_NgroupCoarse       && v < (int)vec_NgroupCoarse->size())       ? vec_NgroupCoarse->at(v)       : -999.,
                    (vec_TrapezoidalFitWidth && v < (int)vec_TrapezoidalFitWidth->size()) ? vec_TrapezoidalFitWidth->at(v) : -999.,
                    (vec_TrapezoidalFWHM    && v < (int)vec_TrapezoidalFWHM->size())   ? vec_TrapezoidalFWHM->at(v)    : -999.
                ) << std::endl;
            }

            // ---- Draw event display from plot ROOT file ----
            // note : open plot file if not already open
            if (!fplot){
                fplot = TFile::Open(plot_file.c_str(), "READ");
                if (!fplot || fplot->IsZombie()){
                    std::cout << "  WARNING: cannot open plot file: " << plot_file << std::endl;
                    fplot = nullptr;
                }
                else {
                    std::cout << "  Opened plot file: " << plot_file << std::endl;
                }
            }

            if (fplot){
                // note : for each vertex, retrieve the main event display canvas
                // Canvas names written by the class:
                //   eID_{eID_count}_VtxN_{vtx_index}_EvtZDist
                //   eID_{eID_count}_VtxN_{vtx_index}_EvtZDist_ZoomIn
                for (int v = 0; v < N_INTT_VtxZ; v++){
                    std::string cname = Form("eID_%d_VtxN_%d_EvtZDist", eID_count, v);
                    TCanvas* c = (TCanvas*)fplot->Get(cname.c_str());
                    if (c){
                        c->Draw();
                        c->Print(output_pdf.c_str(),
                                 Form("Title:file%d_eID%d_vtx%d", file_i, eID_count, v));
                    }
                    else {
                        std::cout << "  WARNING: canvas not found in plot file: " << cname << std::endl;
                    }
                }
            }

        } // end event loop

        // note : close plot file for this data file
        if (fplot){ fplot->Close(); delete fplot; fplot = nullptr; }

        fdata->Close();
        delete fdata;

    } // end file loop

    // ---- Close PDF ----
    pdf_canvas->Print((output_pdf + "]").c_str());
    delete pdf_canvas;

    // ---- Summary ----
    std::cout << "\n===== Summary =====" << std::endl;
    std::cout << "Files processed  : " << file_list.size()    << std::endl;
    std::cout << "Total events     : " << total_events         << std::endl;
    std::cout << "Events N_VtxZ>1  : " << total_multiVtx_evts << std::endl;
    std::cout << "PDF saved to     : " << output_pdf           << std::endl;

    // ---- Save histograms ----
    TFile* fout = new TFile(output_root.c_str(), "RECREATE");
    fout->cd();
    h_NVtxZ         ->Write();
    h_NVtxZ_vs_NClus->Write();
    h_FitHeight_vs_NClus->Write();
    fout->Close();
    delete fout;

    std::cout << "ROOT output      : " << output_root << std::endl;
}
