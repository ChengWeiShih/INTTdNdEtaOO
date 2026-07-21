#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TRandom3.h>
#include <TString.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

void NormalizeTH2DByMajorityAverage(TH2D *hist, const double trimFraction = 0.10)
{
  if (hist == nullptr)
  {
    throw std::runtime_error("NormalizeTH2DByMajorityAverage: null input histogram");
  }

  std::vector<double> binContents;
  for (int ix = 1; ix <= hist->GetNbinsX(); ++ix)
  {
    for (int iy = 1; iy <= hist->GetNbinsY(); ++iy)
    {
      const double content = hist->GetBinContent(ix, iy);
      if (std::isfinite(content) && content > 0.0)
      {
        binContents.push_back(content);
      }
    }
  }

  if (binContents.empty())
  {
    throw std::runtime_error("NormalizeTH2DByMajorityAverage: no positive finite bins found");
  }

  std::sort(binContents.begin(), binContents.end());

  int nTrim = static_cast<int>(trimFraction * static_cast<double>(binContents.size()));
  if (2 * nTrim >= static_cast<int>(binContents.size()))
  {
    nTrim = 0;
  }

  double sum = 0.0;
  int nUsed = 0;
  for (int i = nTrim; i < static_cast<int>(binContents.size()) - nTrim; ++i)
  {
    sum += binContents[i];
    ++nUsed;
  }

  const double majorityAverage = sum / static_cast<double>(nUsed);
  const double normalizationFactor = 1.0 / majorityAverage;
  hist->Scale(normalizationFactor);

  std::cout << "Normalize " << hist->GetName()
            << ": majority average = " << majorityAverage
            << ", factor = " << normalizationFactor
            << ", bins used = " << nUsed << " / " << binContents.size()
            << std::endl;
}

TH2D *GetBestAgreementRatio(const TH2D *referenceHist,
                            const TH2D *histToScale,
                            const double minScale = 0.1,
                            const double maxScale = 3.0,
                            const int nScanSteps = 2900,
                            const double unityTolerance = 0.10,
                            const double trimFraction = 0.10,
                            const std::string &ratioHistName = "hRatio_after_best_scaling")
{
  if (referenceHist == nullptr || histToScale == nullptr)
  {
    throw std::runtime_error("GetBestAgreementRatio: null input histogram");
  }

  if (referenceHist->GetNbinsX() != histToScale->GetNbinsX() ||
      referenceHist->GetNbinsY() != histToScale->GetNbinsY())
  {
    throw std::runtime_error("GetBestAgreementRatio: input TH2Ds do not have the same dimensions");
  }

  if (minScale <= 0.0 || maxScale <= minScale || nScanSteps <= 0)
  {
    throw std::runtime_error("GetBestAgreementRatio: invalid scan range");
  }

  TH2D *referenceNorm = static_cast<TH2D *>(referenceHist->Clone("hReference_majorityNorm_for_scan"));
  TH2D *histToScaleNorm = static_cast<TH2D *>(histToScale->Clone("hToScale_majorityNorm_for_scan"));

  NormalizeTH2DByMajorityAverage(referenceNorm, trimFraction);
  NormalizeTH2DByMajorityAverage(histToScaleNorm, trimFraction);

  double bestScale = minScale;
  int bestNCloseToUnity = -1;
  double bestMeanAbsLogRatio = std::numeric_limits<double>::infinity();
  int bestNBinsUsed = 0;

  for (int i = 0; i <= nScanSteps; ++i)
  {
    const double scale = minScale + (maxScale - minScale) * static_cast<double>(i) / static_cast<double>(nScanSteps);
    int nCloseToUnity = 0;
    int nBinsUsed = 0;
    double sumAbsLogRatio = 0.0;

    for (int ix = 1; ix <= referenceNorm->GetNbinsX(); ++ix)
    {
      for (int iy = 1; iy <= referenceNorm->GetNbinsY(); ++iy)
      {
        const double denominator = referenceNorm->GetBinContent(ix, iy);
        const double numerator = scale * histToScaleNorm->GetBinContent(ix, iy);

        if (denominator <= 0.0 || numerator <= 0.0)
        {
          continue;
        }

        const double ratio = numerator / denominator;
        ++nBinsUsed;
        sumAbsLogRatio += std::fabs(std::log(ratio));

        if (std::fabs(ratio - 1.0) < unityTolerance)
        {
          ++nCloseToUnity;
        }
      }
    }

    const double meanAbsLogRatio = (nBinsUsed > 0)
                                     ? sumAbsLogRatio / static_cast<double>(nBinsUsed)
                                     : std::numeric_limits<double>::infinity();

    const bool moreBinsClose = nCloseToUnity > bestNCloseToUnity;
    const bool sameBinsBetterAverage = nCloseToUnity == bestNCloseToUnity &&
                                       meanAbsLogRatio < bestMeanAbsLogRatio;

    if (moreBinsClose || sameBinsBetterAverage)
    {
      bestScale = scale;
      bestNCloseToUnity = nCloseToUnity;
      bestMeanAbsLogRatio = meanAbsLogRatio;
      bestNBinsUsed = nBinsUsed;
    }
  }

  TH2D *ratioHist = static_cast<TH2D *>(referenceNorm->Clone(ratioHistName.c_str()));
  ratioHist->Reset("ICES");
  ratioHist->SetTitle(Form("Best ratio: %.6g * %s / %s",
                           bestScale,
                           histToScaleNorm->GetName(),
                           referenceNorm->GetName()));

  for (int ix = 1; ix <= referenceNorm->GetNbinsX(); ++ix)
  {
    for (int iy = 1; iy <= referenceNorm->GetNbinsY(); ++iy)
    {
      const double denominator = referenceNorm->GetBinContent(ix, iy);
      if (denominator == 0.0)
      {
        ratioHist->SetBinContent(ix, iy, 0.0);
        continue;
      }

      const double numerator = bestScale * histToScaleNorm->GetBinContent(ix, iy);
      ratioHist->SetBinContent(ix, iy, numerator / denominator);
    }
  }

  std::cout << "Best residual scale after normalization = " << bestScale << std::endl;
  std::cout << "Bins close to unity = " << bestNCloseToUnity
            << " / " << bestNBinsUsed
            << " for tolerance " << unityTolerance << std::endl;
  std::cout << "Mean |log(ratio)| = " << bestMeanAbsLogRatio << std::endl;

  return ratioHist;
}

void MakeBestScaledRatioHist()
{
  TH2D *hReference = new TH2D("hReference", "Reference;X;Y", 10, 0, 10, 10, 0, 10);
  TH2D *hToScale = new TH2D("hToScale", "Histogram to scale;X;Y", 10, 0, 10, 10, 0, 10);

  TRandom3 rand(0);
  const double trueScaleDifference = 0.72;

  for (int ix = 1; ix <= hReference->GetNbinsX(); ++ix)
  {
    for (int iy = 1; iy <= hReference->GetNbinsY(); ++iy)
    {
      const double x = hReference->GetXaxis()->GetBinCenter(ix);
      const double y = hReference->GetYaxis()->GetBinCenter(iy);
      const double shape = 50.0 + 4.0 * x + 2.0 * y + 8.0 * std::sin(0.4 * x);

      hReference->SetBinContent(ix, iy, rand.Gaus(shape, 2.0));
      hToScale->SetBinContent(ix, iy, rand.Gaus(trueScaleDifference * shape, 2.0) + 10000.0);
    }
  }

  hReference->SetBinContent(10, 10, 1000000.0);
  hToScale->SetBinContent(5, 5, 0.0);

  TH2D *hReferenceNorm = static_cast<TH2D *>(hReference->Clone("hReference_norm"));
  TH2D *hToScaleNorm = static_cast<TH2D *>(hToScale->Clone("hToScale_norm"));
  NormalizeTH2DByMajorityAverage(hReferenceNorm);
  NormalizeTH2DByMajorityAverage(hToScaleNorm);

  TH2D *hRatio = GetBestAgreementRatio(hReference,
                                       hToScale,
                                       0.1,
                                       3.0,
                                       2900,
                                       0.10,
                                       0.10,
                                       "hRatio_after_best_scaling");

  TH1D *h1RatioContent = new TH1D("h1_ratio_content",
                                  "Bin contents of hRatio_after_best_scaling;Ratio;Entries",
                                  100,
                                  0,
                                  5);

  for (int ix = 1; ix <= hRatio->GetNbinsX(); ++ix)
  {
    for (int iy = 1; iy <= hRatio->GetNbinsY(); ++iy)
    {
      h1RatioContent->Fill(hRatio->GetBinContent(ix, iy));
    }
  }

  TFile *outFile = new TFile("MakeBestScaledRatioHist_output.root", "RECREATE");
  hReferenceNorm->Write();
  hToScaleNorm->Write();
  hRatio->Write();
  h1RatioContent->Write();
  outFile->Close();

  std::cout << "Saved hReference_norm, hToScale_norm, hRatio_after_best_scaling, and h1_ratio_content to MakeBestScaledRatioHist_output.root" << std::endl;
}
