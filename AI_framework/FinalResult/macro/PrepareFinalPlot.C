#include "../../../Constants.cpp"
#include "../sPhenixStyle.C"


double AuAu_2A = 394.;
double OO_2A   = 32.;

std::pair<std::vector<double>, std::vector<TGraph*>> PrepareMC(
    std::string MC_name,
    std::string system_name,
    double the_2A_value,
    std::vector<int> draw_parameter,
    std::string input_dir,
    std::string input_filename
){
    TFile * file = TFile::Open(Form("%s/%s",input_dir.c_str(),input_filename.c_str()));

    std::vector<double> x_vec_centrality; x_vec_centrality.clear();
    std::vector<double> x_vec_Npart; x_vec_Npart.clear();
    std::vector<double> x_vec_Npart2A; x_vec_Npart2A.clear();
    std::vector<double> y_vec_absdNdEta; y_vec_absdNdEta.clear();
    std::vector<double> y_vec_nordNdEta; y_vec_nordNdEta.clear();

    // TGraphErrors * gr_Npart_NorMiddNdEta  = (TGraphErrors *) file->Get(Form("gr_%s_Npart_NorMiddNdEta",MC_name.c_str())); // note : bin width == 1
    TGraphErrors * gr_Npart_NorMiddNdEta   = (TGraphErrors *) file->Get(Form("gr2_%s_Npart_NorMiddNdEta",MC_name.c_str())); // note: has rebin
    TGraphErrors * gr_Centrality_MiddNdEta = (TGraphErrors *) file->Get(Form("gr_%s_Centrality_MiddNdEta",MC_name.c_str()));

    if (!gr_Npart_NorMiddNdEta || !gr_Centrality_MiddNdEta){
        std::cout<<Form("gr2_%s_Npart_NorMiddNdEta: ",MC_name.c_str())<<gr_Npart_NorMiddNdEta<<Form(", gr_%s_Centrality_MiddNdEta: ",MC_name.c_str())<<gr_Centrality_MiddNdEta<<", quit"<<std::endl;
        exit(1);
    }
    
    for (int i = 0; i < gr_Npart_NorMiddNdEta->GetN(); i++){
        x_vec_Npart.push_back(    gr_Npart_NorMiddNdEta->GetPointX(i));
        x_vec_Npart2A.push_back(  gr_Npart_NorMiddNdEta->GetPointX(i)/the_2A_value);

        y_vec_nordNdEta.push_back(gr_Npart_NorMiddNdEta->GetPointY(i));
    }

    for (int i = 0; i < gr_Centrality_MiddNdEta->GetN(); i++){
        x_vec_centrality.push_back( gr_Centrality_MiddNdEta->GetPointX(i));
        y_vec_absdNdEta.push_back(  gr_Centrality_MiddNdEta->GetPointY(i));
    }
    

    std::vector<double> Npart_vec; Npart_vec.clear();
    for (int i = 0; i < 20; i++){
        TH1D * h1D = (TH1D*) file -> Get(Form("h1D_%s_Npart_Cent%.2d",MC_name.c_str(), i));
        Npart_vec.push_back(h1D->GetMean());
        // std::cout<<"MC: "<<MC_name<<", Mbin: "<<i<<", Mean of Npart: "<<Npart_vec.back()<<std::endl;
        std::cout<<"Cent.: "<<Constants::centrality_edges[i]-1<<"-"<<Constants::centrality_edges[i+1]-1<<"%, Npart mean: "<<Npart_vec.back()<<std::endl;
    }

    // Build HIJING theory TGraph objects
    TGraph* gr_theory_Centrality_absdNdEta = new TGraph(
        (int) x_vec_centrality.size(),
        x_vec_centrality.data(),
        y_vec_absdNdEta.data()
    );
    gr_theory_Centrality_absdNdEta->SetLineColor(draw_parameter[0]);
    gr_theory_Centrality_absdNdEta->SetLineWidth(draw_parameter[1]);
    gr_theory_Centrality_absdNdEta->SetLineStyle(draw_parameter[2]); // dashed

    TGraph* gr_theory_Npart_nordNdEta = new TGraph(
        (int) x_vec_Npart.size(),
        x_vec_Npart.data(),
        y_vec_nordNdEta.data()
    );
    gr_theory_Npart_nordNdEta->SetLineColor(draw_parameter[0]);
    gr_theory_Npart_nordNdEta->SetLineWidth(draw_parameter[1]);
    gr_theory_Npart_nordNdEta->SetLineStyle(draw_parameter[2]);

    TGraph* gr_theory_Npart2A_nordNdEta = new TGraph(
        (int) x_vec_Npart2A.size(),
        x_vec_Npart2A.data(),
        y_vec_nordNdEta.data()
    );
    gr_theory_Npart2A_nordNdEta->SetLineColor(draw_parameter[0]);
    gr_theory_Npart2A_nordNdEta->SetLineWidth(draw_parameter[1]);
    gr_theory_Npart2A_nordNdEta->SetLineStyle(draw_parameter[2]);

    gr_theory_Centrality_absdNdEta -> SetName(Form("%s_%s_gr_theory_Centrality_absdNdEta",MC_name.c_str(),system_name.c_str()));
    gr_theory_Npart_nordNdEta -> SetName(Form("%s_%s_gr_theory_Npart_nordNdEta",MC_name.c_str(),system_name.c_str()));
    gr_theory_Npart2A_nordNdEta -> SetName(Form("%s_%s_gr_theory_Npart2A_nordNdEta",MC_name.c_str(),system_name.c_str()));
    
    // note: Npart_vec
    // note: x_vec_centrality
    // note: x_vec_Npart
    // note: x_vec_Npart2A
    // note: y_vec_absdNdEta
    // note: y_vec_nordNdEta 

    std::vector<TGraph*> out_grE_vec = {
        gr_theory_Centrality_absdNdEta
        , gr_theory_Npart_nordNdEta
        , gr_theory_Npart2A_nordNdEta   
    };

    return {
        Npart_vec, out_grE_vec
    };

}

std::vector<TGraphErrors*> PreparesPHENIXAuAu(
    std::string input_dir,
    std::string input_filename
){
    // std::vector<double> x_Npart = {
    //     359.3, 331.2, 297.0, 257.3, 219.0,
    //     185.7, 156.0, 130.0, 107.1, 87.1,
    //     69.5, 54.2, 41.4, 30.7, 22.1
    // };

    // std::vector<double> x_Npart_err = {
    //     2.1, 2.9, 3.2, 3.8, 4.3,
    //     4.6, 5.0, 5.2, 5.2, 5.1,
    //     5.0, 4.7, 4.4, 3.9, 3.3
    // };

    // std::vector<double> y_dNdEta_per_participant_pair = {
    //     4.0, 3.9, 3.8, 3.6, 3.5,
    //     3.4, 3.3, 3.2, 3.1, 3.0,
    //     2.9, 2.8, 2.7, 2.6, 2.4
    // };

    // std::vector<double> y_dNdEta_per_participant_pair_err = {
    //     0.3, 0.2, 0.2, 0.2, 0.2,
    //     0.2, 0.2, 0.2, 0.2, 0.3,
    //     0.3, 0.3, 0.3, 0.4, 0.4
    // };

    std::vector<double> y_dNdEta_per_participant_pair_err = {0.25317,0.24493,0.23070,0.22773,0.22099,0.21754,0.22162,0.22940,0.24416,0.25508,0.28211,0.31091,0.34346,0.37755,0.40531};
    std::vector<double> y_dNdEta_per_participant_pair = {4.02696,3.90665,3.75840,3.64920,3.52802,3.41855,3.31865,3.22070,3.12592,3.01732,2.91648,2.83522,2.69184,2.56302,2.42309};

    std::vector<double> x_Npart = {359.30, 331.20, 297.00, 257.30, 219.00, 185.70, 156.00, 130.00, 107.10, 87.10, 69.50, 54.20, 41.40, 30.70, 22.10};
    std::vector<double> x_Npart_err = {2.10,2.90,3.20,3.80,4.30,4.60,5.00,5.20,5.20,5.10,5.00,4.70,4.40,3.90,3.30};

    std::vector<double> x_Npart2A; x_Npart2A.clear();
    std::vector<double> x_Npart2A_err; x_Npart2A_err.clear();

    for (int i = 0; i < (int) x_Npart.size(); i++){
        x_Npart2A.push_back(x_Npart.at(i) / AuAu_2A);
        x_Npart2A_err.push_back(x_Npart_err.at(i) / AuAu_2A);
    }


    TGraphErrors* grE_sPHAuAU_Npart_NorMiddNdEta = new TGraphErrors(
        x_Npart.size(),
        x_Npart.data(),
        y_dNdEta_per_participant_pair.data(),
        x_Npart_err.data(),
        y_dNdEta_per_participant_pair_err.data()
    );
    grE_sPHAuAU_Npart_NorMiddNdEta->SetName("grE_sPHAuAU_Npart_NorMiddNdEta");

    grE_sPHAuAU_Npart_NorMiddNdEta->SetMarkerStyle(28);
    grE_sPHAuAU_Npart_NorMiddNdEta->SetMarkerSize(1.3);
    grE_sPHAuAU_Npart_NorMiddNdEta->SetMarkerColor(4);
    grE_sPHAuAU_Npart_NorMiddNdEta->SetFillColorAlpha(4, 0.5);
    grE_sPHAuAU_Npart_NorMiddNdEta->SetLineColorAlpha(1, 0);
    grE_sPHAuAU_Npart_NorMiddNdEta->SetLineWidth(0);


    TGraphErrors* grE_sPHAuAU_Npart2A_NorMiddNdEta = new TGraphErrors(
        x_Npart2A.size(),
        x_Npart2A.data(),
        y_dNdEta_per_participant_pair.data(),
        x_Npart2A_err.data(),
        y_dNdEta_per_participant_pair_err.data()
    );
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetName("grE_sPHAuAU_Npart2A_NorMiddNdEta");

    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetMarkerStyle(28);
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetMarkerSize(1.3);
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetMarkerColor(4);
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetFillColorAlpha(4, 0.5);
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetLineColorAlpha(1, 0);
    grE_sPHAuAU_Npart2A_NorMiddNdEta->SetLineWidth(0);

    return {
        grE_sPHAuAU_Npart_NorMiddNdEta,
        grE_sPHAuAU_Npart2A_NorMiddNdEta
    };
}

// note: x_vec_centrality
// note: x_vec_Npart
// note: x_vec_Npart2A
// note: y_vec_absdNdEta
// note: y_vec_nordNdEta
std::vector<TGraphErrors*> PrepareOOsPHENIX(
    std::pair<double,double> eta_range,
    int N_cent_class,
    std::string input_dir,
    std::string input_file_suffix,
    std::string grE_prefix,
    std::vector<double> Npart_vec
){
    std::vector<double> x_vec_centrality; x_vec_centrality.clear();
    std::vector<double> x_vec_Npart; x_vec_Npart.clear();
    std::vector<double> x_vec_Npart2A; x_vec_Npart2A.clear();
    std::vector<double> xZeroError_vec;


    std::vector<double> y_vec_absdNdEta; y_vec_absdNdEta.clear();
    std::vector<double> y_vec_absdNdEtaErr; y_vec_absdNdEtaErr.clear();

    std::vector<double> y_vec_nordNdEta; y_vec_nordNdEta.clear();
    std::vector<double> y_vec_nordNdEtaErr; y_vec_nordNdEtaErr.clear();

    for (int i = 0; i < N_cent_class; i++){

        double sum_mean = 0;
        double sum_error = 0;
        int    N_point = 0;

        TFile * file = TFile::Open(Form("%s/Mbin%d_%s",input_dir.c_str(),i,input_file_suffix.c_str()));
        TGraphErrors * grE = (TGraphErrors*) file->Get(Form("%s%d",grE_prefix.c_str(),i));
        for (int point_i = 0; point_i < grE->GetN(); point_i++){
            double x_pos = grE->GetPointX(point_i);
            double y_pos = grE->GetPointY(point_i);
            double y_err = grE->GetErrorY(point_i);

            if (x_pos <= eta_range.first || x_pos >= eta_range.second){continue;}
            
            std::cout<<"----- N_point: "<<N_point<<", x_pos: "<<Form("%.3f",x_pos)<<", y_pos: "<<y_pos<<", y_err: "<<y_err<<std::endl;

            N_point += 1;
            sum_mean += y_pos;
            sum_error += y_err;
        }
        double avg_y        = sum_mean / (double)N_point;
        double avg_yerr     = sum_error / (double)N_point;
        
        double nor_avg_y    = avg_y / (Npart_vec[i] / 2.);
        double nor_avg_yerr = avg_yerr / (Npart_vec[i] / 2.);

        double mean_centrality = (Constants::centrality_edges[i] + Constants::centrality_edges[i+1]) / 2. - 1.; 

        std::cout<<"Mbin: "<<i<<", selected Centrality number: "<<Constants::centrality_edges[i]<<" to "<<Constants::centrality_edges[i+1]-1<<", mean: "<<mean_centrality<<std::endl;
        std::cout<<"Mbin: "<<i<<", Npart: "<<Npart_vec[i]<<", 2A: "<<OO_2A<<", Npart/2A: "<<Npart_vec[i]/OO_2A<<std::endl;
        std::cout<<"Mbin: "<<i<<", abs_dNdEta: "<<avg_y<<" #pm"<<avg_yerr<<", nor_dNdEta: "<<nor_avg_y<<" #pm"<<nor_avg_yerr<<std::endl;

        std::cout<<std::endl;


        x_vec_centrality.push_back(mean_centrality);
        x_vec_Npart.push_back(Npart_vec[i]);
        x_vec_Npart2A.push_back((double)Npart_vec[i]/OO_2A);

        y_vec_absdNdEta.push_back(avg_y);
        y_vec_absdNdEtaErr.push_back(avg_yerr);

        y_vec_nordNdEta.push_back(nor_avg_y);
        y_vec_nordNdEtaErr.push_back(nor_avg_yerr);
    }

    xZeroError_vec = std::vector<double>((int)x_vec_Npart.size(), 0);


    TGraphErrors* grE_sPHOO_Npart_NorMiddNdEta = new TGraphErrors(
        x_vec_Npart.size(),
        x_vec_Npart.data(),
        y_vec_nordNdEta.data(),
        xZeroError_vec.data(),
        y_vec_nordNdEtaErr.data()
    );

    TGraphErrors* grE_sPHOO_Npart2A_NorMiddNdEta = new TGraphErrors(
        x_vec_Npart2A.size(),
        x_vec_Npart2A.data(),
        y_vec_nordNdEta.data(),
        xZeroError_vec.data(),
        y_vec_nordNdEtaErr.data()
    );

    TGraphErrors* grE_sPHOO_Centrality_MiddNdEta = new TGraphErrors(
        x_vec_centrality.size(),
        x_vec_centrality.data(),
        y_vec_absdNdEta.data(),
        xZeroError_vec.data(),
        y_vec_absdNdEtaErr.data()
    );


    grE_sPHOO_Npart_NorMiddNdEta->SetMarkerStyle(20);
    grE_sPHOO_Npart_NorMiddNdEta->SetMarkerSize(1);
    grE_sPHOO_Npart_NorMiddNdEta->SetMarkerColor(1);
    grE_sPHOO_Npart_NorMiddNdEta->SetFillColorAlpha(1, 0.5);
    grE_sPHOO_Npart_NorMiddNdEta->SetLineColorAlpha(1, 0);
    grE_sPHOO_Npart_NorMiddNdEta->SetLineWidth(0);

    grE_sPHOO_Npart2A_NorMiddNdEta->SetMarkerStyle(20);
    grE_sPHOO_Npart2A_NorMiddNdEta->SetMarkerSize(1);
    grE_sPHOO_Npart2A_NorMiddNdEta->SetMarkerColor(1);
    grE_sPHOO_Npart2A_NorMiddNdEta->SetFillColorAlpha(1, 0.5);
    grE_sPHOO_Npart2A_NorMiddNdEta->SetLineColorAlpha(1, 0);
    grE_sPHOO_Npart2A_NorMiddNdEta->SetLineWidth(0);

    grE_sPHOO_Centrality_MiddNdEta->SetMarkerStyle(20);
    grE_sPHOO_Centrality_MiddNdEta->SetMarkerSize(1);
    grE_sPHOO_Centrality_MiddNdEta->SetMarkerColor(1);
    grE_sPHOO_Centrality_MiddNdEta->SetFillColorAlpha(1, 0.5);
    grE_sPHOO_Centrality_MiddNdEta->SetLineColorAlpha(1, 0);
    grE_sPHOO_Centrality_MiddNdEta->SetLineWidth(0);

    return {
        grE_sPHOO_Npart_NorMiddNdEta
        , grE_sPHOO_Npart2A_NorMiddNdEta
        , grE_sPHOO_Centrality_MiddNdEta
    };
}


int PrepareFinalPlot(){
    std::pair<int,int> zvtx_range = {-10, 10};
    std::string sPHENIX_label = "Internal";
    std::string Collision_system = "O+O #sqrt{s_{NN}} = 200 GeV";
    std::string Collision_energy = "AA collisions at #sqrt{s_{NN}} = 200 GeV";

    std::string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/InttDoublets/FinaldNdEta/plots";
    
    std::string sPH_OO_data_name = "sPHENIX (INTT-only combinatoric method)";
    std::pair<double, double> eta_range = {-0.3,0.3};
    int N_centrality_class = 11;
    int Centrality_high_end = 55; 
    std::string input_dNdEta_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/InttDoublets/FinaldNdEta"; 
    std::string input_dNdEta_file_suffix = "FinaldNdEta.root";
    std::string grE_prefix = "grE_baseline_Syst_Mbin";

    // std::string input_sPHENIX_AuAu_directory
    // std::string input_sPHENIX_AuAu_filename

    std::string HIJING_name = "HIJING";

    std::string input_HIJING_OO_name = "O+O";
    std::string input_HIJING_OO_npart_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260608/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/NpartValue/completed";
    std::string input_HIJING_OO_npart_filename = "MC_Npart_HIJING_00000_test.root";

    std::string input_HIJING_AuAu_name = "Au+Au";
    std::string input_HIJING_AuAu_npart_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/NpartValue/completed";
    std::string input_HIJING_AuAu_npart_filename = "MC_Npart_HIJING_00000_test.root";


    std::vector<TGraphErrors*> grE_sPHAuAu_vec = PreparesPHENIXAuAu("","");

    std::pair<std::vector<double>, std::vector<TGraph*>> HIJING_AuAu_vec = PrepareMC(
        HIJING_name,
        input_HIJING_AuAu_name,
        AuAu_2A,
        {kBlue+1, 2, 2},
        input_HIJING_AuAu_npart_directory,
        input_HIJING_AuAu_npart_filename
    );

    std::pair<std::vector<double>, std::vector<TGraph*>> HIJING_OO_vec = PrepareMC(
        HIJING_name,
        input_HIJING_OO_name,
        OO_2A,
        {kOrange+2, 2, 4},
        input_HIJING_OO_npart_directory,
        input_HIJING_OO_npart_filename
    );
    
    std::vector<TGraphErrors*> grE_sPHOO_vec = PrepareOOsPHENIX(
        eta_range,
        N_centrality_class,
        input_dNdEta_directory,
        input_dNdEta_file_suffix, 
        grE_prefix,
        HIJING_OO_vec.first // note: Npart_vec
    );

    SetsPhenixStyle();

    // Unpack data TGraphErrors
    TGraphErrors* grE_sPHAuAU_Npart_NorMiddNdEta  = grE_sPHAuAu_vec[0];
    TGraphErrors* grE_sPHAuAU_Npart2A_NorMiddNdEta = grE_sPHAuAu_vec[1];
    TGraphErrors* grE_sPHOO_Npart_NorMiddNdEta     = grE_sPHOO_vec[0];
    TGraphErrors* grE_sPHOO_Npart2A_NorMiddNdEta   = grE_sPHOO_vec[1];
    TGraphErrors* grE_sPHOO_Centrality_MiddNdEta   = grE_sPHOO_vec[2];

    // -----------------------------------------------------------------------
    // Canvas 1: Centrality (x) vs absolute dNdEta (y)
    // Division -------------------------------------------------------------------------------------------------------------------------
    TCanvas * c_Centrality_absdNdEta = new TCanvas("c_Centrality_absdNdEta", "", 950, 800);
    // c_Centrality_absdNdEta->SetLeftMargin(0.15);
    // c_Centrality_absdNdEta->SetBottomMargin(0.13);

    // Draw theory line first to set axes
    // gr_theory_Centrality_absdNdEta->SetTitle(
    //     Form(";Centrality [%%];#LTdN_{ch}/d#eta#GT|_{|#eta|#leq%.1f}", Form("%.1f<|#eta|<%.1f", -eta_range.second, eta_range.second))
    // );
    HIJING_OO_vec.second.at(0)->GetXaxis()->SetTitle("Centrality [%]");
    HIJING_OO_vec.second.at(0)->GetXaxis()->SetRangeUser(0, 100);
    HIJING_OO_vec.second.at(0)->GetYaxis()->SetTitle(Form("#LTdN_{ch}/d#eta#GT#cbar_{|#eta|<%.1f}", eta_range.second));
    HIJING_OO_vec.second.at(0)->GetYaxis()->SetRangeUser(0,80);
    HIJING_OO_vec.second.at(0)->Draw("AL");

    TLatex latex_final;
    latex_final.SetNDC();
    latex_final.SetTextFont(62);
    latex_final.SetTextSize(0.042);
    latex_final.DrawLatex(0.17, 0.955, Form("#it{sPHENIX} #bf{%s}", sPHENIX_label.c_str()));

    latex_final.SetTextFont(42);
    latex_final.SetTextSize(0.038);
    latex_final.DrawLatex(0.2, 0.89, Collision_system.c_str());
    latex_final.DrawLatex(0.2, 0.84, Form("z-vertex: [%d, %d] cm, Cent: 0-%.d%%",zvtx_range.first, zvtx_range.second, Centrality_high_end));

    grE_sPHOO_Centrality_MiddNdEta->Draw("pE3 same");

    TLegend * leg1 = new TLegend(0.25, 0.65, 0.88, 0.8);
    leg1->SetBorderSize(0);
    leg1->SetFillStyle(0);
    leg1->SetTextSize(0.03);
    leg1->SetMargin(0.1);
    leg1->AddEntry(HIJING_OO_vec.second.at(0),  HIJING_name.c_str(),     "l");
    leg1->AddEntry(grE_sPHOO_Centrality_MiddNdEta,  sPH_OO_data_name.c_str(),            "fp");
    leg1->Draw();

    c_Centrality_absdNdEta->SaveAs(Form("%s/Centrality_absdNdEta.pdf", output_directory.c_str()));

    // -----------------------------------------------------------------------
    // Canvas 2: Npart (x) vs normalized dNdEta (y)
    // Division -------------------------------------------------------------------------------------------------------------------------
    TCanvas * c_Npart_nordNdEta = new TCanvas("c_Npart_nordNdEta", "", 950, 800);
    // c_Npart_nordNdEta->SetLeftMargin(0.15);
    // c_Npart_nordNdEta->SetBottomMargin(0.13);

    // gr_theory_Npart_nordNdEta->SetTitle(";#LTN_{part}#GT;(dN_{ch}/d#eta)/(0.5 N_{part})");
    HIJING_OO_vec.second.at(1)->GetXaxis()->SetTitle("#LTN_{part}#GT");
    HIJING_OO_vec.second.at(1)->GetXaxis()->SetLimits(0, 40);
    HIJING_OO_vec.second.at(1)->GetYaxis()->SetTitle(Form("(2/#LTN_{part}#GT)#LTdN_{ch}/d#eta#GT#cbar_{|#eta|<%.1f}", eta_range.second));
    HIJING_OO_vec.second.at(1)->GetYaxis()->SetRangeUser(0,7);
    HIJING_OO_vec.second.at(1)->Draw("AL");

    latex_final.SetNDC();
    latex_final.SetTextFont(62);
    latex_final.SetTextSize(0.042);
    latex_final.DrawLatex(0.17, 0.955, Form("#it{sPHENIX} #bf{%s}", sPHENIX_label.c_str()));

    latex_final.SetTextFont(42);
    latex_final.SetTextSize(0.038);
    latex_final.DrawLatex(0.2, 0.89, Collision_system.c_str());
    latex_final.DrawLatex(0.2, 0.84, Form("z-vertex: [%d, %d] cm, Cent: 0-%.d%%",zvtx_range.first, zvtx_range.second, Centrality_high_end));

    // grE_sPHAuAU_Npart_NorMiddNdEta->Draw("PE3 same");
    grE_sPHOO_Npart_NorMiddNdEta->Draw("PE3 same");

    TLegend * leg2 = new TLegend(0.25, 0.65, 0.88, 0.8);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetTextSize(0.035);
    leg2->SetMargin(0.1);
    leg2->AddEntry(HIJING_OO_vec.second.at(1),       HIJING_name.c_str(),     "l");
    leg2->AddEntry(grE_sPHOO_Npart_NorMiddNdEta,    sPH_OO_data_name.c_str(),     "fp");
    leg2->Draw();

    c_Npart_nordNdEta->SaveAs(Form("%s/Npart_nordNdEta.pdf", output_directory.c_str()));

    // -----------------------------------------------------------------------
    // Canvas 3: Npart/2A (x) vs normalized dNdEta (y)
    // Division -------------------------------------------------------------------------------------------------------------------------
    TCanvas * c_Npart2A_nordNdEta = new TCanvas("c_Npart2A_nordNdEta", "", 950, 800);
    // c_Npart2A_nordNdEta->SetLeftMargin(0.15);
    // c_Npart2A_nordNdEta->SetBottomMargin(0.13);

    HIJING_OO_vec.second.at(2)->SetTitle(";N_{part}/2A;(dN_{ch}/d#eta)/(0.5 N_{part})");
    HIJING_OO_vec.second.at(2)->GetXaxis()->SetTitle("#LTN_{part}#GT/2A");
    HIJING_OO_vec.second.at(2)->GetXaxis()->SetLimits(0, 1);
    HIJING_OO_vec.second.at(2)->GetYaxis()->SetTitle(Form("(2/#LTN_{part}#GT)#LTdN_{ch}/d#eta#GT#cbar_{|#eta|<%.1f}", eta_range.second));
    HIJING_OO_vec.second.at(2)->GetYaxis()->SetRangeUser(0,7);
    HIJING_OO_vec.second.at(2)->Draw("AL");

    HIJING_AuAu_vec.second.at(2)->Draw("L same");

    latex_final.SetNDC();
    latex_final.SetTextFont(62);
    latex_final.SetTextSize(0.042);
    latex_final.DrawLatex(0.17, 0.955, Form("#it{sPHENIX} #bf{%s}", sPHENIX_label.c_str()));

    latex_final.SetTextFont(42);
    latex_final.SetTextSize(0.038);
    latex_final.DrawLatex(0.2, 0.89, Collision_energy.c_str());
    latex_final.DrawLatex(0.2, 0.84, Form("z-vertex: [%d, %d] cm",zvtx_range.first, zvtx_range.second));

    grE_sPHAuAU_Npart2A_NorMiddNdEta->Draw("p2 same");
    grE_sPHOO_Npart2A_NorMiddNdEta->Draw("PE3 same");

    TLegend * leg3 = new TLegend(0.25, 0.63, 0.88, 0.83);
    leg3->SetBorderSize(0);
    leg3->SetFillStyle(0);
    leg3->SetTextSize(0.035);
    leg3->SetMargin(0.1);
    leg3->AddEntry(HIJING_OO_vec.second.at(2),      "HIJING (O+O)",     "l");
    leg3->AddEntry(HIJING_AuAu_vec.second.at(2),      "HIJING (Au+Au)",     "l");
    leg3->AddEntry(grE_sPHAuAU_Npart2A_NorMiddNdEta, "sPHENIX Au+Au (0-70%)",          "fP");
    leg3->AddEntry(grE_sPHOO_Npart2A_NorMiddNdEta,   Form("sPHENIX O+O (0-%d%%)", Centrality_high_end),            "fP");
    leg3->Draw();

    c_Npart2A_nordNdEta->SaveAs(Form("%s/Npart2A_nordNdEta.pdf", output_directory.c_str()));

    return 888;
}