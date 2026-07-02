// std::string input_directory = "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/GlauberNBDfit_centrality_bounds_1pct.txt";
std::string input_directory = "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GetCentralityDist/macro/test_macro2/centrality_bounds_1pct_data.txt";

struct CentralityBin_str
{
    int percent_low;
    int percent_high;
    float x_cut_low;
    float x_cut_high;
};
std::vector<CentralityBin_str> centrality_bins_private;

bool ReadCentralityTable_private()
{
    // std::string& input_directory = PrivateCentrality.second;

    centrality_bins_private.clear();

    std::ifstream file(input_directory);

    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file: " << input_directory << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        // Skip empty lines
        if (line.empty()) continue;

        // Skip comment lines
        if (line[0] == '#') continue;

        std::stringstream ss(line);

        CentralityBin_str bin;

        ss >> bin.percent_low
           >> bin.percent_high
           >> bin.x_cut_low
           >> bin.x_cut_high;

        // Check if the line was successfully read
        if (ss.fail())
        {
            std::cerr << "Warning: failed to parse line: " << line << std::endl;
            continue;
        }

        centrality_bins_private.push_back(bin);
    }

    file.close();

    std::cout << "Loaded " << centrality_bins_private.size()
              << " centrality bins from " << input_directory << std::endl;

    
    std::cout << "\nCentrality table content:\n";
    std::cout << "# percent_low percent_high x_cut_low x_cut_high\n";

    for (const auto& bin : centrality_bins_private)
    {
        std::cout << bin.percent_low  << " "
                  << bin.percent_high << " "
                  << bin.x_cut_low    << " "
                  << bin.x_cut_high   << std::endl;
    }

    return true;
}

float GetCentralityBin_private(const float MBD_charge_sum_in)
{
    for (const auto& bin : centrality_bins_private)
    {
        if (MBD_charge_sum_in >= bin.x_cut_low &&
            MBD_charge_sum_in <  bin.x_cut_high)
        {
            return bin.percent_high;
        }
    }

    // Return -1 if the charge sum is outside all defined ranges
    return -1;
}

TH1* hglauber_data;

double NBD_getValue_data(int n, double mu, double k)
{
  double F;
  double f;

  if (n + k > 100.0)
  {
    F  = TMath::LnGamma(n + k) - TMath::LnGamma(n + 1.) - TMath::LnGamma(k);
    f  = n * TMath::Log(mu / k) - (n + k) * TMath::Log(1.0 + mu / k);
    F  = TMath::Exp(F + f);
  }
  else
  {
    F  = TMath::Gamma(n + k) / (TMath::Gamma(n + 1.) * TMath::Gamma(k));
    f  = TMath::Exp(n * TMath::Log(mu / k) - (n + k) * TMath::Log(1.0 + mu / k));
    F *= f;
  }

  return F;
}

double NBDGlauberConv_data(double *x, double *par)
{
  double ihit  = x[0];
  double mu    = par[0];
  double k     = par[1];
  double alpha = par[2];
  double result = 0;

  for (int ib = 2; ib <= hglauber_data->GetNbinsX(); ib++)
  {
    int npart = hglauber_data->GetBinCenter(ib);
    double weight = hglauber_data->GetBinContent(ib);
    if (weight <= 0) continue;

    double nbd = NBD_getValue_data(
        (int)ihit,
        mu * pow(npart, alpha),
        k  * pow(npart, alpha)
    );

    result += nbd * weight;
  }

  return par[3] * result;
}

double NBD(double n, double mu, double k)
{
    if (mu <= 0.0 || k <= 0.0) return 0.0;
    double r  = k / (mu + k);          // p in the standard form
    double lg = TMath::LnGamma(n + k)
              - TMath::LnGamma(k)
              - TMath::LnGamma(n + 1.0)
              + n * TMath::Log(1.0 - r)
              + k * TMath::Log(r);
    return (lg < -300.0) ? 0.0 : TMath::Exp(lg);
}

TH1D *g_h1D_OO_Npart = nullptr;

// ---------------------------------------------------------------
//  NBD-Glauber convolution evaluated at charge value x
//
//  The total MBD charge for a given Npart is the sum of Npart
//  independent NBD contributions.  For Npart identical NBD(mu,k)
//  random variables the resulting distribution is NBD(Npart*mu, Npart*k).
//
//  Full model:
//    f(x) = norm * Sum_{Npart} P(Npart) * NBD(x ; Npart*mu , Npart*k)
//
//  TF1 parameters:
//    [0]  norm  – overall normalization
//    [1]  mu    – mean charge per NN collision
//    [2]  k     – NBD shape parameter
// ---------------------------------------------------------------
double NBDGlauberConv(double *x_arr, double *par)
{
    double x    = x_arr[0];
    double norm = par[0];
    double mu   = par[1];
    double k    = par[2];

    if (!g_h1D_OO_Npart) return 0.0;
    if (mu <= 0.0 || k <= 0.0) return 0.0;

    double val = 0.0;
    int nbins  = g_h1D_OO_Npart->GetNbinsX();

    for (int ib = 1; ib <= nbins; ++ib)
    {
        double Npart    = g_h1D_OO_Npart->GetBinCenter(ib);
        double wNpart   = g_h1D_OO_Npart->GetBinContent(ib);
        if (wNpart <= 0.0 || Npart <= 0.0) continue;

        // For the sum of Npart NBD(mu,k) variates -> NBD(Npart*mu, Npart*k)
        double mu_eff = Npart * mu;
        double k_eff  = Npart * k;

        val += wNpart * NBD(x, mu_eff, k_eff);
    }

    return norm * val;
}

int macro(
    int process_id = 0,
    int runnumber = 37,
    string model = "HIJING"
){

    std::string job_index = std::to_string( process_id );
    int job_index_len = 6;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260318/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/quick_hist";
    string output_filename = Form("%s_HistOut_%d_%s.root",model.c_str(),runnumber,job_index.c_str());

    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));  

    TFile * file_in = TFile::Open(Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/20260318/HIJING_INTTSurveyOnly_CentralityScaleTest_customizedVertex/completed/MC_HIJING_Ntuple_00082400_ana538_CdbTagProdA_2024_%s.root",job_index.c_str()));
    TTree * tree = (TTree*) file_in -> Get("EventTree");

    bool is_min_bias;
    float MBD_south_charge_sum;
    float MBD_north_charge_sum;
    float MBD_charge_sum;
    float MBD_charge_asymm;
    float MBD_z_vtx;
    int NClus;

    short MBD_south_npmt;
    short MBD_north_npmt;

    // int MBDNS1_scaled_vtx10cm;

    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("is_min_bias", 1);
    tree->SetBranchStatus("MBD_south_charge_sum",1);
    tree->SetBranchStatus("MBD_north_charge_sum",1);
    tree->SetBranchStatus("MBD_charge_sum", 1);
    tree->SetBranchStatus("MBD_charge_asymm", 1);
    tree->SetBranchStatus("MBD_z_vtx", 1);
    tree->SetBranchStatus("NClus", 1);
    
    tree->SetBranchStatus("MBD_south_npmt", 1);
    tree->SetBranchStatus("MBD_north_npmt", 1);
    // tree->SetBranchStatus("MBDNS1_scaled_vtx10cm", 1);


    tree->SetBranchAddress("is_min_bias", &is_min_bias);
    tree->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree->SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);
    tree->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree->SetBranchAddress("NClus", &NClus);

    tree->SetBranchAddress("MBD_south_npmt", &MBD_south_npmt);
    tree->SetBranchAddress("MBD_north_npmt", &MBD_north_npmt);
    // tree->SetBranchAddress("MBDNS1_scaled_vtx10cm", &MBDNS1_scaled_vtx10cm);

    // const char *glauberFile =
    //     "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/Npart_output.root";

    // TFile *f_glauber = TFile::Open(glauberFile, "READ");
    // if (!f_glauber || f_glauber->IsZombie()) {
    //     std::cerr << "[ERROR] Cannot open: " << glauberFile << std::endl;
    //     return 777;
    // }

    // g_h1D_OO_Npart = dynamic_cast<TH1D*>(f_glauber->Get("h1D_OO_Npart"));
    // if (!g_h1D_OO_Npart) {
    //     std::cerr << "[ERROR] Cannot retrieve h1D_OO_Npart from Glauber file." << std::endl;
    //     return 777;
    // }

    // TFile *fglauber= TFile::Open("lemon_oo_hists.root");
    // hglauber_data= (TH1D*) fglauber->Get("hNpart");
    // if (!fglauber || fglauber->IsZombie() || !hglauber_data) {
    //     std::cerr << "Failed to open lemon_oo_hists.root or load hNpart" << std::endl;
    //     return 777;
    // }

    // double mu    = 4.87528;
    // double k     = 1.1651;
    // double alpha = 1.0;
    // double N     = 83.7634;

    // TF1 *fNBD = new TF1("nbd_data", NBDGlauberConv_data, 0, 270, 4);
    // fNBD->SetParameters(mu, k, alpha, N);
    // fNBD->SetParNames("mu","k","alpha","N");


    TH1D * h1D_MBD_charge_sum = new TH1D("h1D_MBD_charge_sum","h1D_MBD_charge_sum;MBD charge sum;Counts",400,0,400);
    TH2D * h2D_INTTNClus_MBDChargeSum = new TH2D("h2D_INTTNClus_MBDChargeSum","h2D_INTTNClus_MBDChargeSum;# of INTT clusters;MBD charge sum",400,0,2000,400,0,400);
    TH2D * h2D_MBDCharge_North_South = new TH2D("h2D_MBDCharge_North_South","h2D_MBDCharge_North_South;MBD charge (north);MBD charge (south)",200,0,200,200,0,200);
    TH1D * h1D_MBD_z_vtx = new TH1D("h1D_MBD_z_vtx","h1D_MBD_z_vtx;MBD z-vertex [cm];Counts",200,-100,100);
    TH2D * h2D_MBDz_MBDChargeAsym = new TH2D("h2D_MBDz_MBDChargeAsym","h2D_MBDz_MBDChargeAsym;MBD z-vertex [cm];MBD charge asymmetry",200,-100,100,200,-2,2);

    TH1D * h1D_centrality = new TH1D("h1D_centrality","h1D_centrality; Event Centrality;Counts",101,-0.5,100.5);
    TH1D * h1D_centrality_2 = new TH1D("h1D_centrality_2","h1D_centrality_2; Event Centrality;Counts",101,-0.5,100.5);
    TH1D * h1D_centrality_3 = new TH1D("h1D_centrality_3","h1D_centrality_3; Event Centrality;Counts",101,-0.5,100.5);

    // ReadCentralityTable_private();

    // TF1 *f_NBDGlauber = new TF1("f_NBDGlauber",
    //                              NBDGlauberConv,
    //                              0, 400,
    //                              3);   // 3 free parameters

    // // Parameter names
    // f_NBDGlauber->SetParName(0, "Norm");
    // f_NBDGlauber->SetParName(1, "#mu (mean charge/NN)");
    // f_NBDGlauber->SetParName(2, "k (NBD shape)");

    // f_NBDGlauber->SetParameters(
    //     8.36635e+06,
    //     4.82978,
    //     1.17235
    // );

    for (int i = 0; i < tree -> GetEntries(); i++){
        tree -> GetEntry(i);

        // if (MBDNS1_scaled_vtx10cm != 1){continue;}
        if (fabs(MBD_z_vtx) > 60) {continue;}
        if (MBD_charge_sum == 0){continue;}
        if (MBD_charge_sum <= 0.5){continue;}
        if (MBD_south_npmt < 1 || MBD_north_npmt < 1) {continue;}
        
        h1D_MBD_charge_sum -> Fill(MBD_charge_sum);
        
        h2D_MBDCharge_North_South -> Fill(MBD_north_charge_sum,MBD_south_charge_sum);
        h1D_MBD_z_vtx -> Fill(MBD_z_vtx);
        h2D_MBDz_MBDChargeAsym -> Fill(MBD_z_vtx,MBD_charge_asymm);

        // h1D_centrality -> Fill(
        //     GetCentralityBin_private(MBD_charge_sum)
        // );

        // h1D_centrality_2 -> Fill(
        //     100. * (fNBD->Integral(MBD_charge_sum,400)/fNBD->Integral(0,400))
        // );

        // h1D_centrality_3 -> Fill(
        //     int(100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400))) + 1
        // );

        // if (
        //     MBD_charge_sum >= 0 && MBD_charge_sum <= 12 
        // ) {
        //     std::cout<<"0-12, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
        // }
        // else if (
        //     MBD_charge_sum > 200 && MBD_charge_sum <= 300 
        // ) {
        //     std::cout<<"200-300, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
        // }
        // else if (
        //     MBD_charge_sum > 300 && MBD_charge_sum <= 400 
        // ){
        //     std::cout<<"300-400, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
        // }
        
        // if (fabs(MBD_z_vtx) < 10){
        //     h2D_INTTNClus_MBDChargeSum -> Fill(NClus, MBD_charge_sum);
        // }
    }

    TFile * file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
    h1D_MBD_charge_sum -> Write();
    h2D_INTTNClus_MBDChargeSum -> Write();
    h2D_MBDCharge_North_South -> Write();
    h1D_MBD_z_vtx -> Write();
    h2D_MBDz_MBDChargeAsym -> Write();
    h1D_centrality -> Write();
    h1D_centrality_2 -> Write();
    h1D_centrality_3 -> Write();

    file_out -> Close();

    system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_filename.c_str(), output_directory.c_str()));

    return 888;
}

// int macro(
//     int process_id = 0,
//     int runnumber = 82391
// ){

//     std::string job_index = std::to_string( process_id );
//     int job_index_len = 5;
//     job_index.insert(0, job_index_len - job_index.size(), '0');

//     string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/quick_hist";
//     string output_filename = Form("HistOut_%d_%s.root",runnumber,job_index.c_str());

//     system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), output_filename.c_str(), output_directory.c_str(), output_filename.c_str()));  

//     TFile * file_in = TFile::Open(Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/test_82391_INTTsurveyOnly_LoacalPos_PrivateCentrality/completed/Ntuple_00082391_standalone_ana551_%s.root",job_index.c_str()));
//     TTree * tree = (TTree*) file_in -> Get("EventTree");

//     bool is_min_bias;
//     float MBD_south_charge_sum;
//     float MBD_north_charge_sum;
//     float MBD_charge_sum;
//     float MBD_charge_asymm;
//     float MBD_z_vtx;
//     int NClus;
//     int MBDNS1_scaled_vtx10cm;

//     tree->SetBranchStatus("*",0);
//     tree->SetBranchStatus("is_min_bias", 1);
//     tree->SetBranchStatus("MBD_south_charge_sum",1);
//     tree->SetBranchStatus("MBD_north_charge_sum",1);
//     tree->SetBranchStatus("MBD_charge_sum", 1);
//     tree->SetBranchStatus("MBD_charge_asymm", 1);
//     tree->SetBranchStatus("MBD_z_vtx", 1);
//     tree->SetBranchStatus("NClus", 1);
//     tree->SetBranchStatus("MBDNS1_scaled_vtx10cm", 1);


//     tree->SetBranchAddress("is_min_bias", &is_min_bias);
//     tree->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
//     tree->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
//     tree->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
//     tree->SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);
//     tree->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
//     tree->SetBranchAddress("NClus", &NClus);
//     tree->SetBranchAddress("MBDNS1_scaled_vtx10cm", &MBDNS1_scaled_vtx10cm);

//     const char *glauberFile =
//         "/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GlauberTest/Npart_output.root";

//     TFile *f_glauber = TFile::Open(glauberFile, "READ");
//     if (!f_glauber || f_glauber->IsZombie()) {
//         std::cerr << "[ERROR] Cannot open: " << glauberFile << std::endl;
//         return 777;
//     }

//     g_h1D_OO_Npart = dynamic_cast<TH1D*>(f_glauber->Get("h1D_OO_Npart"));
//     if (!g_h1D_OO_Npart) {
//         std::cerr << "[ERROR] Cannot retrieve h1D_OO_Npart from Glauber file." << std::endl;
//         return 777;
//     }

//     TFile *fglauber= TFile::Open("lemon_oo_hists.root");
//     hglauber_data= (TH1D*) fglauber->Get("hNpart");
//     if (!fglauber || fglauber->IsZombie() || !hglauber_data) {
//         std::cerr << "Failed to open lemon_oo_hists.root or load hNpart" << std::endl;
//         return 777;
//     }

//     double mu    = 4.87528;
//     double k     = 1.1651;
//     double alpha = 1.0;
//     double N     = 83.7634;

//     TF1 *fNBD = new TF1("nbd_data", NBDGlauberConv_data, 0, 270, 4);
//     fNBD->SetParameters(mu, k, alpha, N);
//     fNBD->SetParNames("mu","k","alpha","N");


//     TH1D * h1D_MBD_charge_sum = new TH1D("h1D_MBD_charge_sum","h1D_MBD_charge_sum;MBD charge sum;Counts",400,0,400);
//     TH2D * h2D_INTTNClus_MBDChargeSum = new TH2D("h2D_INTTNClus_MBDChargeSum","h2D_INTTNClus_MBDChargeSum;# of INTT clusters;MBD charge sum",400,0,2000,400,0,400);
//     TH2D * h2D_MBDCharge_North_South = new TH2D("h2D_MBDCharge_North_South","h2D_MBDCharge_North_South;MBD charge (north);MBD charge (south)",200,0,200,200,0,200);
//     TH1D * h1D_MBD_z_vtx = new TH1D("h1D_MBD_z_vtx","h1D_MBD_z_vtx;MBD z-vertex [cm];Counts",200,-100,100);
//     TH2D * h2D_MBDz_MBDChargeAsym = new TH2D("h2D_MBDz_MBDChargeAsym","h2D_MBDz_MBDChargeAsym;MBD z-vertex [cm];MBD charge asymmetry",200,-100,100,200,-2,2);

//     TH1D * h1D_centrality = new TH1D("h1D_centrality","h1D_centrality; Event Centrality;Counts",101,-0.5,100.5);
//     TH1D * h1D_centrality_2 = new TH1D("h1D_centrality_2","h1D_centrality_2; Event Centrality;Counts",101,-0.5,100.5);
//     TH1D * h1D_centrality_3 = new TH1D("h1D_centrality_3","h1D_centrality_3; Event Centrality;Counts",101,-0.5,100.5);

//     ReadCentralityTable_private();

//     TF1 *f_NBDGlauber = new TF1("f_NBDGlauber",
//                                  NBDGlauberConv,
//                                  0, 400,
//                                  3);   // 3 free parameters

//     // Parameter names
//     f_NBDGlauber->SetParName(0, "Norm");
//     f_NBDGlauber->SetParName(1, "#mu (mean charge/NN)");
//     f_NBDGlauber->SetParName(2, "k (NBD shape)");

//     f_NBDGlauber->SetParameters(
//         8.36635e+06,
//         4.82978,
//         1.17235
//     );

//     for (int i = 0; i < tree -> GetEntries(); i++){
//         tree -> GetEntry(i);

//         if (MBDNS1_scaled_vtx10cm != 1){continue;}
//         if (fabs(MBD_z_vtx) > 60) {continue;}
//         if (MBD_charge_sum == 0){continue;}

        
//         h1D_MBD_charge_sum -> Fill(MBD_charge_sum);
        
//         h2D_MBDCharge_North_South -> Fill(MBD_north_charge_sum,MBD_south_charge_sum);
//         h1D_MBD_z_vtx -> Fill(MBD_z_vtx);
//         h2D_MBDz_MBDChargeAsym -> Fill(MBD_z_vtx,MBD_charge_asymm);

//         h1D_centrality -> Fill(
//             GetCentralityBin_private(MBD_charge_sum)
//         );

//         // h1D_centrality_2 -> Fill(
//         //     100. * (fNBD->Integral(MBD_charge_sum,400)/fNBD->Integral(0,400))
//         // );

//         h1D_centrality_3 -> Fill(
//             int(100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400))) + 1
//         );

//         if (
//             MBD_charge_sum >= 0 && MBD_charge_sum <= 12 
//         ) {
//             std::cout<<"0-12, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
//         }
//         else if (
//             MBD_charge_sum > 200 && MBD_charge_sum <= 300 
//         ) {
//             std::cout<<"200-300, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
//         }
//         else if (
//             MBD_charge_sum > 300 && MBD_charge_sum <= 400 
//         ){
//             std::cout<<"300-400, for h1D_centrality_3,  MBD_charge_sum: "<<MBD_charge_sum<<Form(", %.3f / %.3f = %.5f",f_NBDGlauber->Integral(MBD_charge_sum,400),f_NBDGlauber->Integral(0,400),100. * (f_NBDGlauber->Integral(MBD_charge_sum,400)/f_NBDGlauber->Integral(0,400)))<<std::endl;
//         }
        
//         if (fabs(MBD_z_vtx) < 10){
//             h2D_INTTNClus_MBDChargeSum -> Fill(NClus, MBD_charge_sum);
//         }
//     }

//     TFile * file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
//     h1D_MBD_charge_sum -> Write();
//     h2D_INTTNClus_MBDChargeSum -> Write();
//     h2D_MBDCharge_North_South -> Write();
//     h1D_MBD_z_vtx -> Write();
//     h2D_MBDz_MBDChargeAsym -> Write();
//     h1D_centrality -> Write();
//     h1D_centrality_2 -> Write();
//     h1D_centrality_3 -> Write();

//     file_out -> Close();

//     system(Form("mv %s/%s %s/completed", output_directory.c_str(), output_filename.c_str(), output_directory.c_str()));

//     return 888;
// }