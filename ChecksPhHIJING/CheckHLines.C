// CheckHLines.C
// Reads a HepMC2 .dat file and processes every line starting with "H ".
//
// The H line format is:
//   H  col0  col1  col2  col3  col4  col5  col6  col7  col8  f0 f1 f2 f3
//        (9 integers)                                          (4 floats, ignored)
//
// Produces:
//   (1) 9 TH1D for individual integer columns  – range (100, 0, 100)
//   (2) C(9,2)=36 TH1D for pairwise sums       – range (200, 0, 200)
//   (3) C(9,2)=36 TH2D correlation plots        – range (100, 0, 100) x (100, 0, 100)
//
// All histograms are saved to:
//   <input_directory>/CheckHLines_<input_file_stem>.root

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

void CheckHLines(
    std::string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/sPH_sHIJING",
    std::string input_file_name = "sHijing_OO_0_15fm-0000000001-00000.dat"
)
{
    static const int N_COL = 9;   // number of integer columns on each H line

    // -----------------------------------------------------------------------
    // Column labels (short names used in histogram names/titles)
    // -----------------------------------------------------------------------
    std::array<std::string, N_COL> col_label = {
        "col0", "col1", "col2", "col3", "col4",
        "col5", "col6", "col7", "col8"
    };

    // -----------------------------------------------------------------------
    // (1) Individual column histograms
    // -----------------------------------------------------------------------
    std::array<TH1D*, N_COL> h1D_col;
    for (int ic = 0; ic < N_COL; ic++) {
        std::string hname  = Form("h1D_%s", col_label[ic].c_str());
        std::string htitle = Form("H line %s;%s;Counts", col_label[ic].c_str(), col_label[ic].c_str());
        h1D_col[ic] = new TH1D(hname.c_str(), htitle.c_str(), 100, 0, 100);
    }

    // -----------------------------------------------------------------------
    // (2) Pair-sum TH1D  and  (3) Correlation TH2D
    // Enumerate all C(9,2) = 36 unique pairs (i < j)
    // -----------------------------------------------------------------------
    // Store pair indices for later iteration
    std::vector<std::pair<int,int>> pairs;
    for (int i = 0; i < N_COL; i++)
        for (int j = i + 1; j < N_COL; j++)
            pairs.push_back({i, j});

    int N_PAIRS = (int) pairs.size();   // 36

    std::vector<TH1D*> h1D_sum(N_PAIRS);
    std::vector<TH2D*> h2D_corr(N_PAIRS);

    for (int ip = 0; ip < N_PAIRS; ip++) {
        int i = pairs[ip].first;
        int j = pairs[ip].second;
        std::string label_i = col_label[i];
        std::string label_j = col_label[j];

        // Sum histogram
        std::string sname  = Form("h1D_sum_%s_plus_%s", label_i.c_str(), label_j.c_str());
        std::string stitle = Form("Sum %s+%s;%s+%s;Counts", label_i.c_str(), label_j.c_str(),
                                  label_i.c_str(), label_j.c_str());
        h1D_sum[ip] = new TH1D(sname.c_str(), stitle.c_str(), 200, 0, 200);

        // Correlation histogram
        std::string cname  = Form("h2D_corr_%s_vs_%s", label_i.c_str(), label_j.c_str());
        std::string ctitle = Form("Correlation %s vs %s;%s;%s",
                                  label_i.c_str(), label_j.c_str(),
                                  label_i.c_str(), label_j.c_str());
        h2D_corr[ip] = new TH2D(cname.c_str(), ctitle.c_str(),
                                 100, 0, 100,
                                 100, 0, 100);
    }

    // -----------------------------------------------------------------------
    // Read the file and fill histograms
    // -----------------------------------------------------------------------
    std::string input_path = input_directory + "/" + input_file_name;
    std::ifstream fin(input_path.c_str());
    if (!fin.is_open()) {
        std::cerr << "[CheckHLines] ERROR: Cannot open " << input_path << std::endl;
        return;
    }
    std::cout << "[CheckHLines] Reading: " << input_path << std::endl;

    long long n_H_lines = 0;
    std::string line;
    while (std::getline(fin, line)) {

        // Only process lines that start with "H "
        if (line.size() < 2 || line[0] != 'H' || line[1] != ' ') continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;   // consume "H"

        // Read the 9 integer columns
        std::array<int, N_COL> cols;
        bool ok = true;
        for (int ic = 0; ic < N_COL; ic++) {
            if (!(iss >> cols[ic])) { ok = false; break; }
        }
        if (!ok) {
            std::cerr << "[CheckHLines] WARNING: could not parse H line: " << line << std::endl;
            continue;
        }

        n_H_lines++;

        // (1) Fill individual histograms
        for (int ic = 0; ic < N_COL; ic++) {
            h1D_col[ic]->Fill(cols[ic]);
        }

        // (2) & (3) Fill pair sums and correlations
        for (int ip = 0; ip < N_PAIRS; ip++) {
            int i = pairs[ip].first;
            int j = pairs[ip].second;
            h1D_sum[ip]->Fill(cols[i] + cols[j]);
            h2D_corr[ip]->Fill(cols[i], cols[j]);
        }
    }
    fin.close();
    std::cout << "[CheckHLines] Processed " << n_H_lines << " H lines." << std::endl;

    // -----------------------------------------------------------------------
    // Build output file name from input file stem
    // -----------------------------------------------------------------------
    // Strip directory prefix from input_file_name, then remove extension
    std::string stem = input_file_name;
    size_t slash_pos = stem.rfind('/');
    if (slash_pos != std::string::npos) stem = stem.substr(slash_pos + 1);
    size_t dot_pos = stem.rfind('.');
    if (dot_pos != std::string::npos) stem = stem.substr(0, dot_pos);

    std::string output_path = input_directory + "/CheckHLines_" + stem + ".root";
    TFile * output_file = TFile::Open(output_path.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "[CheckHLines] ERROR: Cannot create output file: " << output_path << std::endl;
        return;
    }
    std::cout << "[CheckHLines] Writing output to: " << output_path << std::endl;

    output_file->cd();

    // (1) Individual
    for (int ic = 0; ic < N_COL; ic++) {
        h1D_col[ic]->Write();
    }

    // (2) & (3) Pairs
    for (int ip = 0; ip < N_PAIRS; ip++) {
        h1D_sum[ip]->Write();
        h2D_corr[ip]->Write();
    }

    output_file->Close();
    std::cout << "[CheckHLines] Done." << std::endl;
}
