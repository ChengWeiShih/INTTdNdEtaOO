// GPT_test.C
//
// Read the HepMC text file from sPH_sHIJING, use only lines starting with "H ",
// and histogram the first 9 integer values after the H tag.

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void GPT_test(
    const std::string &input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/MC/sPH_sHIJING",
    const std::string &input_file_name = "sHijing_OO_0_15fm-0000000001-00050.dat",
    const std::string &output_file_name = "sHijing_H_integer_histograms_50.root",
    const long long max_h_rows = 100000)
{
    const std::string input_path = input_directory + "/" + input_file_name;
    const std::string output_path = input_directory + "/" + output_file_name;

    std::ifstream fin(input_path.c_str());
    if (!fin.is_open()) {
        std::cerr << "[GPT_test] ERROR: cannot open input file: "
                  << input_path << std::endl;
        return;
    }

    TFile *fout = TFile::Open(output_path.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "[GPT_test] ERROR: cannot create output file: "
                  << output_path << std::endl;
        fin.close();
        return;
    }

    std::vector<TH1D *> h_col;
    h_col.reserve(9);
    for (int i = 0; i < 9; ++i) {
        TH1D *h = new TH1D(
            Form("h_H_col%d", i + 1),
            Form("H integer column %d;Column %d value;Entries", i + 1, i + 1),
            100, 0, 100);
        h_col.push_back(h);
    }

    std::vector<TH1D *> h_pair_sum;
    std::vector<TH2D *> h_corr;
    h_pair_sum.reserve(36);
    h_corr.reserve(36);

    for (int i = 0; i < 9; ++i) {
        for (int j = i + 1; j < 9; ++j) {
            TH1D *h_sum = new TH1D(
                Form("h_H_col%d_plus_col%d", i + 1, j + 1),
                Form("H col%d + col%d;Column %d + column %d;Entries",
                     i + 1, j + 1, i + 1, j + 1),
                200, 0, 200);
            h_pair_sum.push_back(h_sum);

            TH2D *h_2d = new TH2D(
                Form("h2_H_col%d_vs_col%d", i + 1, j + 1),
                Form("H col%d vs col%d;Column %d;Column %d",
                     i + 1, j + 1, i + 1, j + 1),
                100, 0, 100,
                100, 0, 100);
            h_corr.push_back(h_2d);
        }
    }

    std::string line;
    long long n_lines = 0;
    long long n_h_rows = 0;
    long long n_bad_h_rows = 0;

    while (std::getline(fin, line)) {
        ++n_lines;

        if (line.size() < 2 || line[0] != 'H' || line[1] != ' ') {
            continue;
        }

        std::istringstream iss(line);
        char tag = '\0';
        int h_values[9] = {0};
        iss >> tag;

        bool good_row = true;
        for (int i = 0; i < 9; ++i) {
            if (!(iss >> h_values[i])) {
                good_row = false;
                break;
            }
        }

        if (!good_row) {
            ++n_bad_h_rows;
            std::cerr << "[GPT_test] WARNING: bad H row at text line "
                      << n_lines << ": " << line << std::endl;
            continue;
        }

        ++n_h_rows;

        for (int i = 0; i < 9; ++i) {
            h_col[i]->Fill(h_values[i]);
        }

        int pair_index = 0;
        for (int i = 0; i < 9; ++i) {
            for (int j = i + 1; j < 9; ++j) {
                h_pair_sum[pair_index]->Fill(h_values[i] + h_values[j]);
                h_corr[pair_index]->Fill(h_values[i], h_values[j]);
                ++pair_index;
            }
        }

        if (max_h_rows > 0 && n_h_rows >= max_h_rows) {
            break;
        }
    }

    fin.close();

    auto pass_1d_save_filter = [](const TH1D *h) {
        if (!h) {
            return false;
        }

        if (h->GetNbinsX() < 33) {
            return false;
        }

        if (h->GetBinContent(31) <= 0 ||
            h->GetBinContent(32) <= 0 ||
            h->GetBinContent(33) <= 0) {
            return false;
        }

        for (int bin = 34; bin <= h->GetNbinsX() + 1; ++bin) {
            if (h->GetBinContent(bin) > 0) {
                return false;
            }
        }

        return true;
    };

    auto pass_2d_save_filter = [](const TH2D *h) {
        if (!h) {
            return false;
        }

        if (h->GetStdDev(1) == 0 || h->GetStdDev(2) == 0) {
            return false;
        }

        return true;
    };

    long long n_1d_tested = 0;
    long long n_1d_written = 0;
    long long n_1d_skipped = 0;
    long long n_2d_tested = 0;
    long long n_2d_written = 0;
    long long n_2d_skipped = 0;

    fout->cd();
    for (TH1D *h : h_col) {
        ++n_1d_tested;
        if (pass_1d_save_filter(h)) {
            h->Write();
            ++n_1d_written;
        } else {
            ++n_1d_skipped;
        }
    }
    for (TH1D *h : h_pair_sum) {
        ++n_1d_tested;
        if (pass_1d_save_filter(h)) {
            h->Write();
            ++n_1d_written;
        } else {
            ++n_1d_skipped;
        }
    }
    for (TH2D *h : h_corr) {
        ++n_2d_tested;
        if (pass_2d_save_filter(h)) {
            h->Write();
            ++n_2d_written;
        } else {
            ++n_2d_skipped;
        }
    }
    fout->Close();

    std::cout << "[GPT_test] Input file: " << input_path << std::endl;
    std::cout << "[GPT_test] Output file: " << output_path << std::endl;
    std::cout << "[GPT_test] Requested max good H rows: " << max_h_rows
              << " (-1 means all)" << std::endl;
    std::cout << "[GPT_test] Total text lines read: " << n_lines << std::endl;
    std::cout << "[GPT_test] Good H rows: " << n_h_rows << std::endl;
    std::cout << "[GPT_test] Bad H rows: " << n_bad_h_rows << std::endl;
    std::cout << "[GPT_test] 1D histograms tested: " << n_1d_tested
              << ", written: " << n_1d_written
              << ", skipped: " << n_1d_skipped << std::endl;
    std::cout << "[GPT_test] 2D histograms tested: " << n_2d_tested
              << ", written: " << n_2d_written
              << ", skipped: " << n_2d_skipped << std::endl;
}
