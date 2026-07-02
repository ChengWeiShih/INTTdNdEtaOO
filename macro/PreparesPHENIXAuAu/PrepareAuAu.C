// PrepareAuAu.C
// Reads HepData_Fig5.txt (sPHENIX Au+Au Fig.5 data),
// averages dN_ch/deta over |eta| < 0.3, normalizes by Npart/2,
// and propagates the combined uncertainty from dNdEta and Npart.
//
// Output vectors (one entry per centrality class, 15 total):
//   vec_x      : Npart  (centnparttable)
//   vec_x_err  : delta(Npart) (centnparterror)
//   vec_y      : (dN_ch/deta)_{|eta|<0.3} / (Npart/2)
//   vec_y_err  : nor_dNdEta * sqrt( (avg_err/avg_val)^2 + (delta_Npart/Npart)^2 )

#include <TFile.h>
#include <TGraphErrors.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// --------------------------------------------------------------------------
// Main function
// --------------------------------------------------------------------------
void PrepareAuAu(
    std::string input_filename = "HepData_Fig5.txt",
    double      eta_cut        = 0.3          // select |eta_center| < eta_cut
)
{
    // -----------------------------------------------------------------------
    // sPHENIX Au+Au Npart table (15 centrality classes)
    // -----------------------------------------------------------------------
    std::vector<double> centnparttable = {
        359.3, 331.2, 297.0, 257.3, 219.0,
        185.7, 156.0, 130.0, 107.1,  87.1,
         69.5,  54.2,  41.4,  30.7,  22.1
    };
    std::vector<double> centnparterror = {
        2.1, 2.9, 3.2, 3.8, 4.3,
        4.6, 5.0, 5.2, 5.2, 5.1,
        5.0, 4.7, 4.4, 3.9, 3.3
    };

    // -----------------------------------------------------------------------
    // Output vectors
    // -----------------------------------------------------------------------
    std::vector<double> vec_x;
    std::vector<double> vec_x_err;
    std::vector<double> vec_y;
    std::vector<double> vec_y_err;

    // -----------------------------------------------------------------------
    // Parse HepData_Fig5.txt
    // -----------------------------------------------------------------------
    std::ifstream fin(input_filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "[PrepareAuAu] ERROR: Cannot open " << input_filename << std::endl;
        return;
    }

    int    cent_class_idx = -1;   // index into centnparttable (0-based)
    bool   in_cent_block  = false;

    // Accumulators for the current centrality class
    double sum_val   = 0.;
    double sum_err   = 0.;
    int    n_rows    = 0;

    std::string line;
    while (std::getline(fin, line)) {

        // Skip empty lines and separator lines
        if (line.empty() || line.find("---") != std::string::npos) {
            // Separator: flush the current block if we were collecting
            if (in_cent_block && n_rows > 0) {
                // -- Compute nor_dNdEta --
                double avg_val = sum_val / (double) n_rows;
                double avg_err = sum_err / (double) n_rows;
                double npart   = centnparttable[cent_class_idx];
                double npart_e = centnparterror[cent_class_idx];

                double nor_dNdEta = avg_val / (npart / 2.0);

                // Error propagation:
                //   A = relative uncertainty of dNdEta alone
                //   B = relative uncertainty of Npart
                double A = avg_err / avg_val;
                double B = npart_e / npart;
                double nor_dNdEta_err = nor_dNdEta * std::sqrt(A * A + B * B);

                vec_x.push_back(npart);
                vec_x_err.push_back(npart_e);
                vec_y.push_back(nor_dNdEta);
                vec_y_err.push_back(nor_dNdEta_err);

                std::cout << Form("[PrepareAuAu] Cent class %02d | Npart=%.1f | n_rows=%d | avg_dNdEta=%.4f | nor_dNdEta=%.4f +- %.4f",
                                  cent_class_idx, npart, n_rows, avg_val, nor_dNdEta, nor_dNdEta_err)
                          << std::endl;

                // Reset accumulators
                sum_val = 0.; sum_err = 0.; n_rows = 0;
                in_cent_block = false;
            }
            continue;
        }

        // Centrality header line: "Centrality: X-Y"
        if (line.find("Centrality:") != std::string::npos) {
            cent_class_idx++;
            in_cent_block = true;
            std::cout << "[PrepareAuAu] --> " << line << std::endl;
            continue;
        }

        // Data line: "Eta: [lo,hi], value= V +- E"
        if (line.find("Eta:") != std::string::npos && in_cent_block) {
            // Parse: Eta: [lo,hi], value= V +- E
            double eta_lo, eta_hi, val, err;
            // Example: "Eta: [-0.3,-0.1], value= 723.514 +- 44.2671"
            // Use sscanf for robust parsing
            if (std::sscanf(line.c_str(),
                            " Eta: [%lf,%lf], value= %lf +- %lf",
                            &eta_lo, &eta_hi, &val, &err) != 4) {
                std::cerr << "[PrepareAuAu] WARNING: could not parse line: " << line << std::endl;
                continue;
            }

            // Central eta value of the bin
            double eta_center = (eta_lo + eta_hi) / 2.0;

            // Select bins with |eta_center| < eta_cut
            if (std::fabs(eta_center) < eta_cut) {
                sum_val += val;
                sum_err += err;
                n_rows++;
            }
        }
    }
    fin.close();

    // Flush the last block if the file doesn't end with a separator
    if (in_cent_block && n_rows > 0) {
        double avg_val = sum_val / (double) n_rows;
        double avg_err = sum_err / (double) n_rows;
        double npart   = centnparttable[cent_class_idx];
        double npart_e = centnparterror[cent_class_idx];

        double nor_dNdEta     = avg_val / (npart / 2.0);
        double A              = avg_err / avg_val;
        double B              = npart_e / npart;
        double nor_dNdEta_err = nor_dNdEta * std::sqrt(A * A + B * B);

        vec_x.push_back(npart);
        vec_x_err.push_back(npart_e);
        vec_y.push_back(nor_dNdEta);
        vec_y_err.push_back(nor_dNdEta_err);

        std::cout << Form("[PrepareAuAu] Cent class %02d | Npart=%.1f | n_rows=%d | nor_dNdEta=%.4f +- %.4f",
                          cent_class_idx, npart, n_rows, nor_dNdEta, nor_dNdEta_err)
                  << std::endl;
    }

    // -----------------------------------------------------------------------
    // Summary printout
    // -----------------------------------------------------------------------
    std::cout << "\n[PrepareAuAu] Summary (" << vec_x.size() << " centrality classes):\n";
    std::cout << Form("  %-5s  %-10s  %-10s  %-12s  %-12s\n",
                      "idx", "Npart", "dNpart", "nor_dNdEta", "nor_dNdEta_err");
    for (int i = 0; i < (int) vec_x.size(); i++) {
        std::cout << Form("  %-5d  %-10.2f  %-10.2f  %-12.5f  %-12.5f\n",
                          i, vec_x[i], vec_x_err[i], vec_y[i], vec_y_err[i]);
    }

    // -----------------------------------------------------------------------
    // (Optional) Build a TGraphErrors and print it
    // -----------------------------------------------------------------------
    TCanvas * c1 = new TCanvas("c1", "c1", 600, 400);
    TGraphErrors * grE_AuAu_Npart_NorMiddNdEta = new TGraphErrors(
        (int) vec_x.size(),
        vec_x.data(),
        vec_y.data(),
        vec_x_err.data(),
        vec_y_err.data()
    );
    grE_AuAu_Npart_NorMiddNdEta->SetName("grE_AuAu_Npart_NorMiddNdEta");
    grE_AuAu_Npart_NorMiddNdEta->SetTitle(
        Form(";N_{part};#LT(dN_{ch}/d#eta)/(N_{part}/2)#GT|_{|#eta|<%.1f}", eta_cut)
    );
    grE_AuAu_Npart_NorMiddNdEta->SetMarkerStyle(20);
    grE_AuAu_Npart_NorMiddNdEta->SetMarkerSize(1);
    grE_AuAu_Npart_NorMiddNdEta->SetMarkerColor(1);
    grE_AuAu_Npart_NorMiddNdEta->SetFillColorAlpha(1, 0.5);
    grE_AuAu_Npart_NorMiddNdEta->SetLineColorAlpha(1, 0);
    grE_AuAu_Npart_NorMiddNdEta->SetLineWidth(0);

    std::cout << "\n[PrepareAuAu] TGraphErrors built: "
              << grE_AuAu_Npart_NorMiddNdEta->GetName()
              << "  N=" << grE_AuAu_Npart_NorMiddNdEta->GetN()
              << std::endl;
    c1->cd();
    grE_AuAu_Npart_NorMiddNdEta->Draw("AP2");
    c1->SaveAs(Form("grE_AuAu_Npart_NorMiddNdEta.png"));
}
