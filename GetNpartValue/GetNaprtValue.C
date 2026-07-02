#include "../Constants.cpp"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <THStack.h>
#include <TString.h>

#include <iostream>
#include <string>
#include <vector>

void GetNaprtValue(
    int         process_id_in = 0,
    std::string MCName_in = "HIJING",
    int         run_nEvents_in = -1,
    std::string input_directory_in = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/per5k",
    std::string input_file_name_in = "ntuple_per5k_00000.root",
    std::string output_directory_in = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/NpartValue",
    std::string output_file_name_suffix_in = "test"
)
{
    double eta_cut = 0.3;
    int number_of_file = 800;

    // -------------------------------------------------------------------------
    // 1. Open input TFile
    // -------------------------------------------------------------------------
    // std::string input_file_path = input_directory_in + "/" + input_file_name_in;
    // TFile * input_file = TFile::Open(input_file_path.c_str(), "READ");
    // if (!input_file || input_file->IsZombie()) {
    //     std::cerr << "[GetNaprtValue] ERROR: Cannot open input file: " << input_file_path << std::endl;
    //     return;
    // }
    // std::cout << "[GetNaprtValue] Opened input file: " << input_file_path << std::endl;

    // -------------------------------------------------------------------------
    // 2. Get EventTree
    // -------------------------------------------------------------------------
    // TTree * EventTree = (TTree *) input_file->Get("EventTree");
    // if (!EventTree) {
    //     std::cerr << "[GetNaprtValue] ERROR: Cannot find TTree 'EventTree' in " << input_file_path << std::endl;
    //     input_file->Close();
    //     return;
    // }

    TChain * EventTree = new TChain("EventTree");
    for (int i = 0; i < number_of_file; i++){
        std::string file_name = Form("ntuple_per5k_%05d.root", i);
        std::string file_path = input_directory_in + "/" + file_name;
        std::cout << "[GetNaprtValue] Adding file: " << file_path << std::endl;
        EventTree->Add(file_path.c_str());
    }
    
    std::cout << "[GetNaprtValue] Opened input file: " << input_directory_in << std::endl;
    std::cout << "[GetNaprtValue] Total entries in tree: " << EventTree->GetEntries() << std::endl;

    // -------------------------------------------------------------------------
    // 3. Set all branch status = 0
    // -------------------------------------------------------------------------
    EventTree->SetBranchStatus("*", 0);

    // -------------------------------------------------------------------------
    // 4. Activate only the needed branches
    // -------------------------------------------------------------------------
    // Branch variables
    int            event                   = 0;
    int            ncoll                   = 0;
    int            npart                   = 0;
    float          centrality_bimp         = 0.f;
    float          centrality_impactparam  = 0.f;
    unsigned short clk                     = 0;
    unsigned short femclk                  = 0;
    bool           is_min_bias             = false;
    float          MBD_centrality          = 0.f;
    float          MBD_z_vtx               = 0.f;
    short          MBD_south_npmt          = 0;
    short          MBD_north_npmt          = 0;
    float          MBD_south_charge_sum    = 0.f;
    float          MBD_north_charge_sum    = 0.f;
    float          MBD_charge_sum          = 0.f;
    float          MBD_charge_asymm        = 0.f;
    int            MBD_nhitsoverths_south  = 0;
    int            MBD_nhitsoverths_north  = 0;

    int NPrimaryG4P;
    vector<float>* PrimaryG4P_Pt   = 0;
    vector<float>* PrimaryG4P_Eta  = 0;
    vector<float>* PrimaryG4P_Phi  = 0;
    vector<float>* PrimaryG4P_E    = 0;
    vector<int>* PrimaryG4P_PID  = 0;
    vector<int>* PrimaryG4P_trackID = 0;
    vector<bool>* PrimaryG4P_isChargeHadron = 0;

    // Activate branches and set addresses
    EventTree->SetBranchStatus("is_min_bias_private_MinDeposit",            1); EventTree->SetBranchAddress("is_min_bias_private_MinDeposit",            &is_min_bias);
    EventTree->SetBranchStatus("MBD_centrality_private",         1); EventTree->SetBranchAddress("MBD_centrality_private",         &MBD_centrality);

    EventTree->SetBranchStatus("event",                  1); EventTree->SetBranchAddress("event",                  &event);
    EventTree->SetBranchStatus("ncoll",                  1); EventTree->SetBranchAddress("ncoll",                  &ncoll);
    EventTree->SetBranchStatus("npart",                  1); EventTree->SetBranchAddress("npart",                  &npart);
    EventTree->SetBranchStatus("NPrimaryG4P",      1); EventTree->SetBranchAddress("NPrimaryG4P",      &NPrimaryG4P);
    EventTree->SetBranchStatus("PrimaryG4P_Pt",    1); EventTree->SetBranchAddress("PrimaryG4P_Pt",    &PrimaryG4P_Pt);
    EventTree->SetBranchStatus("PrimaryG4P_Eta",   1); EventTree->SetBranchAddress("PrimaryG4P_Eta",   &PrimaryG4P_Eta);
    EventTree->SetBranchStatus("PrimaryG4P_Phi",   1); EventTree->SetBranchAddress("PrimaryG4P_Phi",   &PrimaryG4P_Phi);
    EventTree->SetBranchStatus("PrimaryG4P_E",     1); EventTree->SetBranchAddress("PrimaryG4P_E",     &PrimaryG4P_E);
    EventTree->SetBranchStatus("PrimaryG4P_PID",   1); EventTree->SetBranchAddress("PrimaryG4P_PID",   &PrimaryG4P_PID);
    EventTree->SetBranchStatus("PrimaryG4P_trackID",   1); EventTree->SetBranchAddress("PrimaryG4P_trackID",   &PrimaryG4P_trackID);
    EventTree->SetBranchStatus("PrimaryG4P_isChargeHadron",   1); EventTree->SetBranchAddress("PrimaryG4P_isChargeHadron",   &PrimaryG4P_isChargeHadron);

    // EventTree->SetBranchStatus("centrality_bimp",        1); EventTree->SetBranchAddress("centrality_bimp",        &centrality_bimp);
    // EventTree->SetBranchStatus("centrality_impactparam", 1); EventTree->SetBranchAddress("centrality_impactparam", &centrality_impactparam);
    // EventTree->SetBranchStatus("clk",                    1); EventTree->SetBranchAddress("clk",                    &clk);
    // EventTree->SetBranchStatus("femclk",                 1); EventTree->SetBranchAddress("femclk",                 &femclk);
    // EventTree->SetBranchStatus("MBD_z_vtx",              1); EventTree->SetBranchAddress("MBD_z_vtx",              &MBD_z_vtx);
    // EventTree->SetBranchStatus("MBD_south_npmt",         1); EventTree->SetBranchAddress("MBD_south_npmt",         &MBD_south_npmt);
    // EventTree->SetBranchStatus("MBD_north_npmt",         1); EventTree->SetBranchAddress("MBD_north_npmt",         &MBD_north_npmt);
    // EventTree->SetBranchStatus("MBD_south_charge_sum",   1); EventTree->SetBranchAddress("MBD_south_charge_sum",   &MBD_south_charge_sum);
    // EventTree->SetBranchStatus("MBD_north_charge_sum",   1); EventTree->SetBranchAddress("MBD_north_charge_sum",   &MBD_north_charge_sum);
    // EventTree->SetBranchStatus("MBD_charge_sum",         1); EventTree->SetBranchAddress("MBD_charge_sum",         &MBD_charge_sum);
    // EventTree->SetBranchStatus("MBD_charge_asymm",       1); EventTree->SetBranchAddress("MBD_charge_asymm",       &MBD_charge_asymm);
    // EventTree->SetBranchStatus("MBD_nhitsoverths_south", 1); EventTree->SetBranchAddress("MBD_nhitsoverths_south", &MBD_nhitsoverths_south);
    // EventTree->SetBranchStatus("MBD_nhitsoverths_north", 1); EventTree->SetBranchAddress("MBD_nhitsoverths_north", &MBD_nhitsoverths_north);

    // -------------------------------------------------------------------------
    // 5. Prepare output ROOT file
    //    Name format: MC_Npart_<MCName_in>_<process_id_in (5 digits)><suffix>.root
    // -------------------------------------------------------------------------
    std::string output_file_name = Form("MC_Npart_%s_%05d_%s.root",
                                        MCName_in.c_str(),
                                        process_id_in,
                                        output_file_name_suffix_in.c_str());
    std::string output_file_path = output_directory_in + "/" + output_file_name;

    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory_in.c_str(), output_file_name.c_str(), output_directory_in.c_str(), output_file_name.c_str()));

    TFile * output_file = TFile::Open(output_file_path.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "[GetNaprtValue] ERROR: Cannot create output file: " << output_file_path << std::endl;
        // input_file->Close();
        return;
    }
    std::cout << "[GetNaprtValue] Output file: " << output_file_path << std::endl;

    // -------------------------------------------------------------------------
    // 6. Prepare per-centrality TH1D vector
    //    Number of bins = Constants::centrality_edges.size() - 1
    // -------------------------------------------------------------------------
    int nCentBins = (int) Constants::centrality_edges.size() - 1;
    std::vector<TH1D *> h1D_Npart_vec;
    h1D_Npart_vec.reserve(nCentBins);

    // Color palette for per-centrality histograms (cycles through nCentBins colors)
    std::vector<int> cent_colors = {
        kRed,       kBlue,      kGreen+2,   kMagenta+1,
        kCyan+1,    kOrange+7,  kViolet+1,  kTeal+2,
        kPink+6,    kAzure+1,   kSpring+9,  kYellow+2,
        kRed+2,     kBlue+2,    kGreen+4,   kMagenta-4,
        kCyan+3,    kOrange-3,  kViolet+5,  kTeal-4
    };

    for (int ic = 0; ic < nCentBins; ic++) {
        std::string hname  = Form("h1D_%s_Npart_Cent%02d", MCName_in.c_str(), ic);
        std::string htitle = Form("%s, Npart, centrality bin %d (%.0f-%.0f%%);Npart;Counts",
                                  MCName_in.c_str(),
                                  ic,
                                  Constants::centrality_edges[ic],
                                  Constants::centrality_edges[ic + 1]-1);
        TH1D * h = new TH1D(hname.c_str(), htitle.c_str(), 52, -0.5, 51.5);
        h->SetLineColor( cent_colors[ic % (int) cent_colors.size()] );
        h->SetLineWidth(2);
        h1D_Npart_vec.push_back(h);
    }

    // 6.1 Inclusive Npart histogram
    TH1D * h1D_Inclusive_Npart = new TH1D(Form("h1D_%s_Inclusive_Npart", MCName_in.c_str()),
                                           Form("%s, Npart (Inclusive min-bias);Npart;Counts", MCName_in.c_str()),
                                           52, -0.5, 51.5);

    TH2D * h2D_Npart_NorMiddNdEta = new TH2D(
        Form("h2D_%s_Npart_NorMiddNdEta", MCName_in.c_str()),
        Form("%s, Npart, (dN_{ch}/d#eta)/(0.5N_{part})|_{|#eta|#leq%.1f};N_{part};(dN_{ch}/d#eta)/(0.5N_{part})|_{|#eta|#leq%.1f}", MCName_in.c_str(),eta_cut, eta_cut),
        52,-0.5,51.5,
        600,0,30
    );

    TH2D * h2D_Centrality_MiddNdEta = new TH2D(
        Form("h2D_%s_Centrality_MiddNdEta", MCName_in.c_str()),
        Form("%s, centrality, dN_{ch}/d#eta|_{|#eta|#leq%.1f};centrality;dN_{ch}/d#eta|_{|#eta|#leq%.1f}", MCName_in.c_str(),eta_cut, eta_cut),
        101,-0.5,100.5,
        500,0,500
    );

    // -------------------------------------------------------------------------
    // 7. Event loop
    // -------------------------------------------------------------------------
    long long nEntries = EventTree->GetEntries();
    long long nLoop    = (run_nEvents_in == -1) ? nEntries
                                                : std::min((long long) run_nEvents_in, nEntries);

    std::cout << "[GetNaprtValue] Total entries in tree: " << nEntries
              << "  |  Will process: " << nLoop << std::endl;

    for (long long i = 0; i < nLoop; i++) {
        EventTree->GetEntry(i);

        int Mid_charged_hadron_count = 0;
        for (int ip = 0; ip < NPrimaryG4P; ip++){
            
            if (PrimaryG4P_isChargeHadron->at(ip) == 0){continue;}

            if (fabs(PrimaryG4P_Eta->at(ip)) > eta_cut){continue;}

            Mid_charged_hadron_count++;
        }
        double normalized_mid_dNdEta = ((double) Mid_charged_hadron_count / (npart / 2.0)) / (eta_cut*2.);
        double mid_dNdEta            = ((double) Mid_charged_hadron_count) / (eta_cut*2.);

        h2D_Npart_NorMiddNdEta -> Fill(
            npart,
            normalized_mid_dNdEta
        );

        // --- event selection ---
        if (is_min_bias != 1)                             continue;  // not min-bias
        if (MBD_centrality < 0 || MBD_centrality >= 100) continue;  // out-of-range centrality

        h2D_Centrality_MiddNdEta -> Fill(
            MBD_centrality,
            mid_dNdEta
        );

        // -------------------------------------------------------------------
        // 8. Fill inclusive histogram
        // -------------------------------------------------------------------
        h1D_Inclusive_Npart->Fill(npart);

        // -------------------------------------------------------------------
        // 9. Find centrality bin index and fill per-bin histogram
        //    Convention: [ centrality_edges[ic], centrality_edges[ic+1] )
        //    e.g.  edges = {1,6,11,...}
        //    MBD_centrality >= 1 && < 6  --> bin 0
        //    MBD_centrality >= 6 && < 11 --> bin 1, etc.
        // -------------------------------------------------------------------
        int cent_index = -1;
        for (int ic = 0; ic < nCentBins; ic++) {
            if (MBD_centrality >= Constants::centrality_edges[ic] &&
                MBD_centrality <  Constants::centrality_edges[ic + 1])
            {
                cent_index = ic;
                break;
            }
        }

        if (cent_index < 0) continue;  // outside all defined edges

        h1D_Npart_vec[cent_index]->Fill(npart);
    }

    std::cout << "[GetNaprtValue] Event loop finished." << std::endl;

    // -------------------------------------------------------------------------
    // Draw canvas: overlay inclusive + per-centrality Npart distributions
    // -------------------------------------------------------------------------
    TCanvas * c1 = new TCanvas("c1_Npart", "Npart distributions", 1200, 800);
    c1->SetLogy();

    // Inclusive histogram: black, thicker
    h1D_Inclusive_Npart->SetLineColor(kBlack);
    h1D_Inclusive_Npart->SetLineWidth(3);
    h1D_Inclusive_Npart->Draw("hist");

    // Per-centrality histograms
    for (int ic = 0; ic < nCentBins; ic++) {
        h1D_Npart_vec[ic]->Draw("hist same");
    }

    // Legend
    TLegend * leg = new TLegend(0.60, 0.35, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(h1D_Inclusive_Npart, "Inclusive min-bias", "l");
    for (int ic = 0; ic < nCentBins; ic++) {
        std::string leg_label = Form("Cent %.0f-%.0f%%",
                                     Constants::centrality_edges[ic],
                                     Constants::centrality_edges[ic + 1] - 1);
        leg->AddEntry(h1D_Npart_vec[ic], leg_label.c_str(), "l");
    }
    leg->Draw();
    c1->Update();

    // -------------------------------------------------------------------------
    // Canvas 2: h1D_Inclusive_Npart (solid black) + THStack with transparent fills
    // -------------------------------------------------------------------------
    TCanvas * c2 = new TCanvas("c2_Npart_Stack", "Npart distributions (stack)", 1200, 800);
    c2->SetLogy();

    // Draw inclusive first (no fill, just the line)
    h1D_Inclusive_Npart->SetFillStyle(0);
    h1D_Inclusive_Npart->Draw("hist");

    // Build THStack: add clones with semi-transparent fills
    THStack * hs = new THStack("hs_Npart", "");
    for (int ic = 0; ic < nCentBins; ic++) {
        TH1D * hclone = (TH1D *) h1D_Npart_vec[ic]->Clone(Form("hclone_Cent%02d", ic));
        int col = cent_colors[ic % (int) cent_colors.size()];
        hclone->SetLineColor(col);
        hclone->SetLineWidth(2);
        hclone->SetFillColorAlpha(col, 0.5);
        hs->Add(hclone);
    }
    hs->Draw("hist same");

    // Legend for c2
    TLegend * leg2 = new TLegend(0.50, 0.55, 0.88, 0.88);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->AddEntry(h1D_Inclusive_Npart, "Inclusive min-bias", "l");
    TList * hs_list = hs->GetHists();
    for (int ic = 0; ic < nCentBins; ic++) {
        std::string leg_label2 = Form("Cent %.0f-%.0f%%",
                                      Constants::centrality_edges[ic],
                                      Constants::centrality_edges[ic + 1] - 1);
        leg2->AddEntry((TH1D *) hs_list->At(ic), leg_label2.c_str(), "f");
    }
    leg2->Draw();
    c2->Update();

    // -------------------------------------------------------------------------
    // TProfile and TGraph from h2D_Npart_NorMiddNdEta
    // -------------------------------------------------------------------------
    // Project: average Y (dNch/deta / 0.5Npart) for each X (Npart) slice
    TProfile * prof_Npart_NorMiddNdEta = h2D_Npart_NorMiddNdEta->ProfileX(
        Form("prof_%s_Npart_NorMiddNdEta", MCName_in.c_str())
    );
    prof_Npart_NorMiddNdEta->SetLineColor(kRed);
    prof_Npart_NorMiddNdEta->SetLineWidth(2);
    prof_Npart_NorMiddNdEta->SetMarkerColor(kRed);
    prof_Npart_NorMiddNdEta->SetMarkerStyle(20);
    prof_Npart_NorMiddNdEta->SetMarkerSize(0.8);

    // Convert profile points to a TGraphErrors (skip empty bins)
    std::vector<double> gr_x, gr_y, gr_ex, gr_ey;
    for (int ib = 1; ib <= prof_Npart_NorMiddNdEta->GetNbinsX(); ib++) {
        if (prof_Npart_NorMiddNdEta->GetBinEntries(ib) == 0) continue;
        gr_x.push_back(  prof_Npart_NorMiddNdEta->GetBinCenter(ib)  );
        gr_y.push_back(  prof_Npart_NorMiddNdEta->GetBinContent(ib) );
        gr_ex.push_back( prof_Npart_NorMiddNdEta->GetBinWidth(ib) / 2.0 );
        gr_ey.push_back( prof_Npart_NorMiddNdEta->GetBinError(ib)  );
    }
    TGraphErrors * gr_Npart_NorMiddNdEta = new TGraphErrors(
        (int) gr_x.size(),
        gr_x.data(), gr_y.data(),
        gr_ex.data(), gr_ey.data()
    );
    gr_Npart_NorMiddNdEta->SetName(  Form("gr_%s_Npart_NorMiddNdEta", MCName_in.c_str()) );
    gr_Npart_NorMiddNdEta->SetTitle( Form("%s, #LTdN_{ch}/d#eta#GT/(0.5N_{part})|_{|#eta|#leq%.1f} vs N_{part};N_{part};#LT(dN_{ch}/d#eta)/(0.5N_{part})#GT", MCName_in.c_str(), eta_cut) );
    gr_Npart_NorMiddNdEta->SetLineColor(kBlue);
    gr_Npart_NorMiddNdEta->SetLineWidth(2);
    gr_Npart_NorMiddNdEta->SetMarkerColor(kBlue);
    gr_Npart_NorMiddNdEta->SetMarkerStyle(21);
    gr_Npart_NorMiddNdEta->SetMarkerSize(0.8);

    // -------------------------------------------------------------------------
    // Canvas 3: h2D + TProfile + TGraph
    // -------------------------------------------------------------------------
    TCanvas * c3 = new TCanvas("c3_Npart_NorMiddNdEta", "Npart vs NorMid dNch/deta", 1200, 800);
    h2D_Npart_NorMiddNdEta->Draw("colz");
    prof_Npart_NorMiddNdEta->Draw("same");
    gr_Npart_NorMiddNdEta->Draw("P same");

    TLegend * leg3 = new TLegend(0.55, 0.72, 0.88, 0.88);
    leg3->SetBorderSize(0);
    leg3->SetFillStyle(0);
    leg3->AddEntry(prof_Npart_NorMiddNdEta, "TProfile (mean per N_{part} bin)", "lp");
    leg3->AddEntry(gr_Npart_NorMiddNdEta,   "TGraphErrors (non-empty bins)",    "lp");
    leg3->Draw();
    c3->Update();

    // -------------------------------------------------------------------------
    // Rebinned (every 2 X-slices) TProfile, TGraph, TCanvas
    // -------------------------------------------------------------------------
    // Clone and rebin: merge every 2 consecutive Npart bins on the X axis
    TH2D * h2D_Npart_NorMiddNdEta_rebin2 = (TH2D *) h2D_Npart_NorMiddNdEta->Clone(
        Form("h2D_%s_Npart_NorMiddNdEta_rebin2", MCName_in.c_str())
    );
    h2D_Npart_NorMiddNdEta_rebin2->RebinX(2);
    h2D_Npart_NorMiddNdEta_rebin2->SetTitle(
        Form("%s, Npart (rebin×2), (dN_{ch}/d#eta)/(0.5N_{part})|_{|#eta|#leq%.1f};N_{part};(dN_{ch}/d#eta)/(0.5N_{part})", MCName_in.c_str(), eta_cut)
    );

    TProfile * prof2_Npart_NorMiddNdEta = h2D_Npart_NorMiddNdEta_rebin2->ProfileX(
        Form("prof2_%s_Npart_NorMiddNdEta", MCName_in.c_str())
    );
    prof2_Npart_NorMiddNdEta->SetLineColor(kOrange+7);
    prof2_Npart_NorMiddNdEta->SetLineWidth(2);
    prof2_Npart_NorMiddNdEta->SetMarkerColor(kOrange+7);
    prof2_Npart_NorMiddNdEta->SetMarkerStyle(20);
    prof2_Npart_NorMiddNdEta->SetMarkerSize(0.8);

    // Convert rebinned profile to TGraphErrors (skip empty bins)
    std::vector<double> gr2_x, gr2_y, gr2_ex, gr2_ey;
    for (int ib = 1; ib <= prof2_Npart_NorMiddNdEta->GetNbinsX(); ib++) {
        if (prof2_Npart_NorMiddNdEta->GetBinEntries(ib) == 0) continue;
        gr2_x.push_back(  prof2_Npart_NorMiddNdEta->GetBinCenter(ib)  );
        gr2_y.push_back(  prof2_Npart_NorMiddNdEta->GetBinContent(ib) );
        gr2_ex.push_back( prof2_Npart_NorMiddNdEta->GetBinWidth(ib) / 2.0 );
        gr2_ey.push_back( prof2_Npart_NorMiddNdEta->GetBinError(ib)  );
    }
    TGraphErrors * gr2_Npart_NorMiddNdEta = new TGraphErrors(
        (int) gr2_x.size(),
        gr2_x.data(), gr2_y.data(),
        gr2_ex.data(), gr2_ey.data()
    );
    gr2_Npart_NorMiddNdEta->SetName(  Form("gr2_%s_Npart_NorMiddNdEta", MCName_in.c_str()) );
    gr2_Npart_NorMiddNdEta->SetTitle( Form("%s, #LT(dN_{ch}/d#eta)/(0.5N_{part})#GT vs N_{part} (2-bin avg);N_{part};#LT(dN_{ch}/d#eta)/(0.5N_{part})#GT", MCName_in.c_str()) );
    gr2_Npart_NorMiddNdEta->SetLineColor(kRed);
    gr2_Npart_NorMiddNdEta->SetLineWidth(2);
    gr2_Npart_NorMiddNdEta->SetMarkerColor(kRed);
    gr2_Npart_NorMiddNdEta->SetMarkerStyle(21);
    gr2_Npart_NorMiddNdEta->SetMarkerSize(0.8);

    // Canvas 4: original h2D (colz) + rebinned profile + rebinned TGraph
    TCanvas * c4 = new TCanvas("c4_Npart_NorMiddNdEta_rebin2", "Npart vs NorMid dNch/deta (2-bin avg)", 1200, 800);
    h2D_Npart_NorMiddNdEta->Draw("colz");
    prof2_Npart_NorMiddNdEta->Draw("same");
    gr2_Npart_NorMiddNdEta->Draw("P same");

    TLegend * leg4 = new TLegend(0.55, 0.72, 0.88, 0.88);
    leg4->SetBorderSize(0);
    leg4->SetFillStyle(0);
    leg4->AddEntry(prof2_Npart_NorMiddNdEta, "TProfile (mean per 2 N_{part} bins)", "lp");
    leg4->AddEntry(gr2_Npart_NorMiddNdEta,   "TGraphErrors (2-bin avg)",            "lp");
    leg4->Draw();
    c4->Update();

    // -------------------------------------------------------------------------
    // Centrality-based TProfile, TGraph, TCanvas (1 X-bin per slice)
    // -------------------------------------------------------------------------
    TProfile * prof_Centrality_MiddNdEta = h2D_Centrality_MiddNdEta->ProfileX(
        Form("prof_%s_Centrality_MiddNdEta", MCName_in.c_str())
    );
    prof_Centrality_MiddNdEta->SetLineColor(kRed);
    prof_Centrality_MiddNdEta->SetLineWidth(2);
    prof_Centrality_MiddNdEta->SetMarkerColor(kRed);
    prof_Centrality_MiddNdEta->SetMarkerStyle(20);
    prof_Centrality_MiddNdEta->SetMarkerSize(0.8);

    // Convert profile to TGraphErrors (skip empty bins)
    std::vector<double> grC_x, grC_y, grC_ex, grC_ey;
    for (int ib = 1; ib <= prof_Centrality_MiddNdEta->GetNbinsX(); ib++) {
        if (prof_Centrality_MiddNdEta->GetBinEntries(ib) == 0) continue;
        grC_x.push_back(  prof_Centrality_MiddNdEta->GetBinCenter(ib)  );
        grC_y.push_back(  prof_Centrality_MiddNdEta->GetBinContent(ib) );
        grC_ex.push_back( prof_Centrality_MiddNdEta->GetBinWidth(ib) / 2.0 );
        grC_ey.push_back( prof_Centrality_MiddNdEta->GetBinError(ib)  );
    }
    TGraphErrors * gr_Centrality_MiddNdEta = new TGraphErrors(
        (int) grC_x.size(),
        grC_x.data(), grC_y.data(),
        grC_ex.data(), grC_ey.data()
    );
    gr_Centrality_MiddNdEta->SetName(  Form("gr_%s_Centrality_MiddNdEta", MCName_in.c_str()) );
    gr_Centrality_MiddNdEta->SetTitle( Form("%s, #LT(dN_{ch}/d#eta)/(0.5N_{part})#GT vs centrality;centrality;#LT(dN_{ch}/d#eta)/(0.5N_{part})#GT", MCName_in.c_str()) );
    gr_Centrality_MiddNdEta->SetLineColor(kBlue);
    gr_Centrality_MiddNdEta->SetLineWidth(2);
    gr_Centrality_MiddNdEta->SetMarkerColor(kBlue);
    gr_Centrality_MiddNdEta->SetMarkerStyle(21);
    gr_Centrality_MiddNdEta->SetMarkerSize(0.8);

    // Canvas 5: h2D_Centrality (colz) + profile + TGraph
    TCanvas * c5 = new TCanvas("c5_Centrality_MiddNdEta", "Centrality vs NorMid dNch/deta", 1200, 800);
    h2D_Centrality_MiddNdEta->Draw("colz");
    prof_Centrality_MiddNdEta->Draw("same");
    gr_Centrality_MiddNdEta->Draw("P same");

    TLegend * leg5 = new TLegend(0.55, 0.72, 0.88, 0.88);
    leg5->SetBorderSize(0);
    leg5->SetFillStyle(0);
    leg5->AddEntry(prof_Centrality_MiddNdEta, "TProfile (mean per centrality bin)", "lp");
    leg5->AddEntry(gr_Centrality_MiddNdEta,   "TGraphErrors (non-empty bins)",      "lp");
    leg5->Draw();
    c5->Update();

    // -------------------------------------------------------------------------
    // Save all histograms and the canvases to the output file
    // -------------------------------------------------------------------------
    output_file->cd();
    h1D_Inclusive_Npart->Write();
    for (int ic = 0; ic < nCentBins; ic++) {
        h1D_Npart_vec[ic]->Write();
    }
    c1->Write();
    c2->Write();
    h2D_Npart_NorMiddNdEta->Write();
    prof_Npart_NorMiddNdEta->Write();
    gr_Npart_NorMiddNdEta->Write();
    c3->Write();
    h2D_Npart_NorMiddNdEta_rebin2->Write();
    prof2_Npart_NorMiddNdEta->Write();
    gr2_Npart_NorMiddNdEta->Write();
    c4->Write();
    h2D_Centrality_MiddNdEta->Write();
    prof_Centrality_MiddNdEta->Write();
    gr_Centrality_MiddNdEta->Write();
    c5->Write();

    output_file->Close();
    // input_file->Close();

    system(Form("mv %s/%s %s/completed", output_directory_in.c_str(), output_file_name.c_str(), output_directory_in.c_str()));

    std::cout << "[GetNaprtValue] Done. Output written to: " << output_file_path << std::endl;
}
