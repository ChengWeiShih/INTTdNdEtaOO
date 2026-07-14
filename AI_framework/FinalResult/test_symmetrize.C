// note : Standalone test for the inverse-variance weighted symmetrization logic.
// note : Run with: root -l -b -q test_symmetrize.C

#include <iostream>
#include <cmath>
#include <TH1D.h>

// note : helper: print all bins of a histogram
void PrintHist(TH1D * h, const char * tag)
{
    std::cout << "\n--- " << tag << " ---" << std::endl;
    std::cout << Form("%-6s  %-8s  %-12s  %-12s", "Bin", "Center", "Content", "Error") << std::endl;
    for (int i = 1; i <= h->GetNbinsX(); i++) {
        std::cout << Form("%-6d  %-8.3f  %-12.6f  %-12.6f",
                          i,
                          h->GetBinCenter(i),
                          h->GetBinContent(i),
                          h->GetBinError(i))
                  << std::endl;
    }
}

int test_symmetrize()
{
    // Division : -Build test histogram (5, -5, 5) -> bin centers: -4, -2, 0, +2, +4---
    TH1D * h = new TH1D("h_test", "h_test", 6, -6., 6.);
    h->Sumw2(true);

    // note : set deliberately asymmetric content and errors
    //        bin 1: center = -4  (mirror of bin 5: center = +4)
    //        bin 2: center = -2  (mirror of bin 4: center = +2)
    //        bin 3: center =  0  (no mirror)
    h->SetBinContent(1,  10.0);  h->SetBinError(1,  2.0);   // center -5
    h->SetBinContent(2,  20.0);  h->SetBinError(2,  3.0);   // center -4
    h->SetBinContent(3,  15.0);  h->SetBinError(3,  4.0);   // center -2
    h->SetBinContent(4,  25.0);  h->SetBinError(4,  5.0);   // center +2
    h->SetBinContent(5,  12.0);  h->SetBinError(5,  1.0);   // center +4
    h->SetBinContent(6,  7.0);   h->SetBinError(6,  0.);   // center +5

    TH1D * h_before = (TH1D*) h->Clone("h_before");

    PrintHist(h, "BEFORE symmetrization");

    // Division : -Symmetrization (copy of DoOnedNdEta logic)---------------------------
    int nBins = h->GetNbinsX();
    for (int i = 1; i <= nBins; i++) {
        double center_i = h->GetBinCenter(i);
        if (center_i <= 0. || fabs(center_i) < 0.000001) { continue; } // note : only positive-eta bins

        // note : find the mirror bin
        int mirror_bin = h->FindBin(-center_i);
        if (mirror_bin < 1 || mirror_bin > nBins) {
            std::cout << "ERROR: no mirror bin for center = " << center_i
                      << "  (mirror_bin = " << mirror_bin << ")" << std::endl;
            return 1;
        }

        double c_pos = h->GetBinContent(i);
        double c_neg = h->GetBinContent(mirror_bin);
        double e_pos = h->GetBinError(i);
        double e_neg = h->GetBinError(mirror_bin);

        // note : inverse-variance weighted combination
        // note : w = 1/sigma^2,  combined = sum(w*c) / sum(w),  sigma_combined = 1/sqrt(sum(w))
        double avg   = 0.;
        double e_avg = 0.;

        if (e_pos > 0. && e_neg > 0.) {
            double w_pos = 1. / (e_pos * e_pos);
            double w_neg = 1. / (e_neg * e_neg);
            double w_sum = w_pos + w_neg;
            avg   = (w_pos * c_pos + w_neg * c_neg) / w_sum;
            e_avg = 1. / std::sqrt(w_sum);
        } else if (e_pos > 0.) {
            std::cout << "WARNING: bin " << i << " (center=" << center_i << "), e_neg=0 – using pos bin alone." << std::endl;
            avg   = c_pos;
            e_avg = e_pos;
        } else if (e_neg > 0.) {
            std::cout << "WARNING: bin " << i << " (center=" << center_i << "), e_pos=0 – using neg bin alone." << std::endl;
            avg   = c_neg;
            e_avg = e_neg;
        } else {
            std::cout << "WARNING: bin " << i << " (center=" << center_i << "), both errors=0 – plain average." << std::endl;
            avg   = (c_pos + c_neg) / 2.;
            e_avg = 0.;
        }

        h->SetBinContent(i,          avg);
        h->SetBinError(i,            e_avg);
        h->SetBinContent(mirror_bin, avg);
        h->SetBinError(mirror_bin,   e_avg);
    }

    PrintHist(h, "AFTER symmetrization");

    // Division : -Print expected values manually for verification----------------------
    std::cout << "\n--- EXPECTED (manual calculation) ---" << std::endl;
    // note : bins ±4 : w+ = 1/1=1.0,  w- = 1/4=0.25,  w_sum=1.25
    //        avg = (1.0*12 + 0.25*10) / 1.25 = 14.5/1.25 = 11.6
    //        e   = 1/sqrt(1.25)                          = 0.8944
    // double w4p = 1./(1.*1.), w4n = 1./(2.*2.);
    // double avg4 = (w4p*12. + w4n*10.) / (w4p+w4n);
    // double e4   = 1./std::sqrt(w4p+w4n);
    // std::cout << Form("Bins +-4 :  avg = %.6f  e = %.6f", avg4, e4) << std::endl;

    // // note : bins ±2 : w+ = 1/25,  w- = 1/9,  w_sum = 1/25+1/9
    // double w2p = 1./(5.*5.), w2n = 1./(3.*3.);
    // double avg2 = (w2p*25. + w2n*20.) / (w2p+w2n);
    // double e2   = 1./std::sqrt(w2p+w2n);
    // std::cout << Form("Bins +-2 :  avg = %.6f  e = %.6f", avg2, e2) << std::endl;

    TFile * file_out = new TFile("test_DoOnedNdEta/test_symmetrize.root", "RECREATE");
    h_before->Write();
    h->Write();
    file_out->Close();

    // delete h;
    return 0;
}
