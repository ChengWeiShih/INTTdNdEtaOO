int test_macro(){

    TH1D * h1D_dNdEta = new TH1D("h1D_dNdEta","",5,-5,5);

    h1D_dNdEta -> SetBinContent(1,1);
    h1D_dNdEta -> SetBinContent(2,2);
    h1D_dNdEta -> SetBinContent(3,3);
    h1D_dNdEta -> SetBinContent(4,4);
    h1D_dNdEta -> SetBinContent(5,5);

    for (int i = 1; i <= 5; i++){
        std::cout<<"Bin Content: "<<h1D_dNdEta->GetBinContent(i)<<
        "Bin Error: "<<h1D_dNdEta->GetBinError(i)<<
        "Bin Center: "<<h1D_dNdEta->GetBinCenter(i)<<std::endl;
    }

    Th1D * 

    if (true) {
        int nBins = h1D_dNdEta->GetNbinsX();
        for (int i = 1; i <= nBins; i++) {
            double center_i = h1D_dNdEta->GetBinCenter(i);
            if (center_i <= 0. || fabs(center_i) < 0.000001) { continue; } // note : only process positive-η bins
 
            // note : find the mirror bin (center == -center_i)
            int mirror_bin = h1D_dNdEta->FindBin(-center_i);
            if (mirror_bin < 1 || mirror_bin > nBins) { 
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", fail to symmetry the bin: "<<i<<", the bin center: "<<center_i<<", the mirror bin: "<<mirror_bin<<std::endl;
                exit(1); 
            }

            double c_pos = h1D_dNdEta->GetBinContent(i);
            double c_neg = h1D_dNdEta->GetBinContent(mirror_bin);
            double e_pos = h1D_dNdEta->GetBinError(i);
            double e_neg = h1D_dNdEta->GetBinError(mirror_bin);

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
                // note : only pos bin has a valid error – use it alone
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", e_neg=0 "<<std::endl;
                avg   = c_pos;
                e_avg = e_pos;
            } else if (e_neg > 0.) {
                // note : only neg bin has a valid error – use it alone
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", e_pos=0 "<<std::endl;
                avg   = c_neg;
                e_avg = e_neg;
            } else {
                std::cout<<"In DoOnedNdEta: class_name: "<<class_name<<", bin: "<<i<<", both errors=0 "<<std::endl;
                // note : both errors are 0, fall back to plain average
                avg   = (c_pos + c_neg) / 2.;
                e_avg = 0.;
            }

            h1D_dNdEta->SetBinContent(i,          avg);
            h1D_dNdEta->SetBinError(i,            e_avg);
            h1D_dNdEta->SetBinContent(mirror_bin, avg);
            h1D_dNdEta->SetBinError(mirror_bin,   e_avg);
        }
    }

}