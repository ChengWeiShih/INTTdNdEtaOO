#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <TCanvas.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TH1D.h>
#include <TLine.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>

namespace
{
    constexpr const char* kMotherDir =
        "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/Run25/dNdEtaOO/CarryOverHit/82420/InttDoublets_2BCO_NoTrig";
    constexpr const char* kHistName = "h1D_ClusEtaInttZ_normalized";

    // const std::vector<std::pair<int, int>> kBunchRanges = {
    //     {0, 4},   {5, 9},   {10, 14}, {15, 19}, {20, 24}, {25, 29},
    //     {30, 34}, {35, 39}, {40, 44}, {45, 49}, {50, 54}, {55, 59},
    //     {60, 64}, {65, 69}, {70, 74}, {75, 79}, {80, 84}, {85, 89},
    //     {90, 94}, {95, 99}, {100, 104}, {105, 110}}
    // ;

    const std::vector<std::pair<int, int>> kBunchRanges = {
        {0,   1},
        {2,   3},
        {4,   5},
        {6,   7},
        {8,   9},
        {10, 11},
        {12, 13},
        {14, 15},
        {16, 17},
        {18, 19},
        {20, 21},
        {22, 23},
        {24, 25},
        {26, 27},
        {28, 29},
        {30, 31},
        {32, 33},
        {34, 35},
        {36, 37},
        {38, 39},
        {40, 41},
        {42, 43},
        {44, 45},
        {46, 47},
        {48, 49},
        {50, 51},
        {52, 53},
        {54, 55},
        {56, 57},
        {58, 59},
        {60, 61},
        {62, 63},
        {64, 65},
        {66, 67},
        {68, 69},
        {70, 71},
        {72, 73},
        {74, 75},
        {76, 77},
        {78, 79},
        {80, 81},
        {82, 83},
        {84, 85},
        {86, 87},
        {88, 89},
        {90, 91},
        {92, 93},
        {94, 95},
        {96, 97},
        {98, 99},
        {100, 101},
        {102, 103},
        {104, 105},
        {106, 107},
        {108, 109},
        {110, 111}
    };

    const std::vector<std::string> kChargeCutLabels = {
        "150to300",
        "100to149",
        "50to99",
        "NoCut",
        "10to49"
    };

    const std::array<int, 5> kColors = {{kCyan, kBlue, kRed, kGreen, kMagenta}};
    const std::array<int, 5> kMarkers = {{20, 21, 22, 33, 34}};

    TString BuildPrimaryFileName(int rangeIndex, const std::string& chargeCutLabel)
    {
        const auto& bunchRange = kBunchRanges.at(rangeIndex);

        if (chargeCutLabel == "NoCut") {
            return Form(
                "%s/BunchNumber_range_%d/completed/"
                "Output_Data_INTTDoublets_StreamTrig_UsedMBDz_BunchNumberRange%dto%d_"
                "VtxZRangeN25to25_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082420_merged.root",
                kMotherDir, rangeIndex, bunchRange.first, bunchRange.second);
        }

        return Form(
            "%s/BunchNumber_range_%d/completed/"
            "Output_Data_INTTDoublets_StreamTrig_UsedMBDz_MBDChargeCut%s_BunchNumberRange%dto%d_"
            "VtxZRangeN25to25_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082420_merged.root",
            kMotherDir, rangeIndex, chargeCutLabel.c_str(), bunchRange.first, bunchRange.second);
    }

    TString BuildFallbackFileName(int rangeIndex, const std::string& chargeCutLabel)
    {
        const auto& bunchRange = kBunchRanges.at(rangeIndex);

        if (chargeCutLabel == "NoCut") {
            return Form(
                "%s/BunchNumber_range_%d/completed/"
                "Data_INTTDoublets_StreamTrig_UsedMBDz_BunchNumberRange%dto%d_"
                "VtxZRangeN25to25_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082420_merged.root",
                kMotherDir, rangeIndex, bunchRange.first, bunchRange.second);
        }

        return Form(
            "%s/BunchNumber_range_%d/completed/"
            "Data_INTTDoublets_StreamTrig_UsedMBDz_MBDChargeCut%s_BunchNumberRange%dto%d_"
            "VtxZRangeN25to25_ClusQAAdc0PhiSize128_DeltaPhiCut0p150_00082420_merged.root",
            kMotherDir, rangeIndex, chargeCutLabel.c_str(), bunchRange.first, bunchRange.second);
    }

    TString ResolveInputFile(int rangeIndex, const std::string& chargeCutLabel)
    {
        const TString primary = BuildPrimaryFileName(rangeIndex, chargeCutLabel);
        if (!gSystem->AccessPathName(primary)) {
            return primary;
        }

        // const TString fallback = BuildFallbackFileName(rangeIndex, chargeCutLabel);
        // if (!gSystem->AccessPathName(fallback)) {
        //     return fallback;
        // }

        std::cout << "Missing both candidate files for range index " << rangeIndex
                  << ", charge cut " << chargeCutLabel << std::endl;
        std::cout << "  Tried: " << primary << std::endl;
        // std::cout << "  Tried: " << fallback << std::endl;
        return "";
    }

    std::pair<double, double> GetEtaZeroBinContentAndError(const TH1D* hist)
    {
        if (!hist) {
            return {0., 0.};
        }

        const int binIndex = hist->GetXaxis()->FindBin(0.);
        return {hist->GetBinContent(binIndex), hist->GetBinError(binIndex)};
    }

    double GetAverageOfFirstNPoints(const TGraphErrors* graph, int nPointsToUse)
    {
        if (!graph) {
            return 0.;
        }

        const int nPoints = std::min(graph->GetN(), nPointsToUse);
        if (nPoints <= 0) {
            return 0.;
        }

        double ySum = 0.;
        double x = 0.;
        double y = 0.;
        for (int i = 0; i < nPoints; ++i) {
            graph->GetPoint(i, x, y);
            ySum += y;
        }

        return ySum / nPoints;
    }
}

int PlotBunchNumberEtaZeroDependenceImpl()
{
    gStyle->SetOptStat(0);

    std::vector<TGraphErrors*> graphs;
    graphs.reserve(kChargeCutLabels.size());
    std::vector<TLine*> averageLines;
    averageLines.reserve(kChargeCutLabels.size());

    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::lowest();

    for (std::size_t iLabel = 0; iLabel < kChargeCutLabels.size(); ++iLabel) {
        auto* graph = new TGraphErrors();
        graph->SetName(Form("g_%s", kChargeCutLabels[iLabel].c_str()));
        graph->SetTitle(kChargeCutLabels[iLabel].c_str());
        graph->SetLineColor(kColors[iLabel]);
        graph->SetMarkerColor(kColors[iLabel]);
        graph->SetMarkerStyle(kMarkers[iLabel]);
        graph->SetMarkerSize(1.4);
        graph->SetLineWidth(2);

        for (std::size_t rangeIndex = 0; rangeIndex < kBunchRanges.size(); ++rangeIndex) {
            const TString fileName = ResolveInputFile(static_cast<int>(rangeIndex), kChargeCutLabels[iLabel]);
            if (fileName.IsNull()) {
                continue;
            }

            TFile* fileIn = TFile::Open(fileName, "READ");
            if (!fileIn || fileIn->IsZombie()) {
                std::cout << "Failed to open: " << fileName << std::endl;
                if (fileIn) {
                    fileIn->Close();
                }
                continue;
            }

            auto* hist = dynamic_cast<TH1D*>(fileIn->Get(kHistName));
            if (!hist) {
                std::cout << "Missing histogram " << kHistName << " in " << fileName << std::endl;
                fileIn->Close();
                continue;
            }

            const auto [value, valueErr] = GetEtaZeroBinContentAndError(hist);
            const auto& bunchRange = kBunchRanges[rangeIndex];
            const double bunchAverage = 0.5 * (bunchRange.first + bunchRange.second);

            const int pointIndex = graph->GetN();
            graph->SetPoint(pointIndex, bunchAverage, value);
            graph->SetPointError(pointIndex, 0., valueErr);

            yMin = std::min(yMin, value - valueErr);
            yMax = std::max(yMax, value + valueErr);

            fileIn->Close();
        }

        const double avgFirst10 = GetAverageOfFirstNPoints(graph, 10);
        auto* avgLine = new TLine(-2., avgFirst10, 112., avgFirst10);
        avgLine->SetLineColor(kColors[iLabel]+2);
        avgLine->SetLineStyle(2);
        avgLine->SetLineWidth(2);
        averageLines.push_back(avgLine);

        yMin = std::min(yMin, avgFirst10);
        yMax = std::max(yMax, avgFirst10);
        graphs.push_back(graph);
    }

    if (graphs.empty()) {
        std::cout << "No graphs were created." << std::endl;
        return 1;
    }

    if (yMin == std::numeric_limits<double>::max() || yMax == std::numeric_limits<double>::lowest()) {
        yMin = 0.;
        yMax = 1.;
    }

    const double span = (yMax > yMin) ? (yMax - yMin) : 1.;
    yMin -= 0.10 * span;
    yMax += 0.15 * span;

    auto* canvas = new TCanvas("c_bunch_number_eta_zero_dependence", "Eta-zero bunch number dependence", 1250, 700);
    canvas->SetRightMargin(0.30);
    canvas->cd();

    graphs.front()->Draw("APEL");
    graphs.front()->SetTitle("Intt Clusters per evt. per #eta (|#eta|<0.5)");
    graphs.front()->GetXaxis()->SetTitle("Average FPHX BCO (Bunch number range)");
    graphs.front()->GetYaxis()->SetTitle("# INTT clusters (per evt. per #eta)");
    graphs.front()->GetXaxis()->SetLimits(-2., 112.);
    graphs.front()->GetYaxis()->SetRangeUser(yMin, yMax);

    for (std::size_t i = 1; i < graphs.size(); ++i) {
        graphs[i]->Draw("PLE SAME");
    }
    for (auto* avgLine : averageLines) {
        avgLine->Draw("SAME");
    }

    auto* legend = new TLegend(0.72, 0.20, 0.98, 0.88);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    for (int i = 0; i < kChargeCutLabels.size(); i++){
        legend->AddEntry(graphs[i], Form("MBDChargeSum: %s", kChargeCutLabels[i].c_str()), "lp");
    }
    legend->Draw();

    const TString outputPdf = Form("%s/BunchNumber_Clustereta0_dependence.pdf", kMotherDir);
    canvas->SaveAs(outputPdf);

    std::cout << "Saved plot to " << outputPdf << std::endl;
    return 0;
}

void PlotBunchNumberClusEtaZeroDependence()
{
    PlotBunchNumberEtaZeroDependenceImpl();
}
