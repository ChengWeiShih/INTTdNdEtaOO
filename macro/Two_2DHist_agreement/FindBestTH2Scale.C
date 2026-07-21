#include <TH2D.h>
#include <TMath.h>
#include <TRandom3.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

struct TH2ScaleResult
{
  double scale = 1.0;
  int nBinsUsed = 0;
  int nBinsCloserThanTolerance = 0;
  double meanAbsLogRatio = std::numeric_limits<double>::infinity();
};

bool HaveSameTH2Binning(const TH2D *histA, const TH2D *histB)
{
  if (!histA || !histB)
  {
    return false;
  }

  if (histA->GetNbinsX() != histB->GetNbinsX() ||
      histA->GetNbinsY() != histB->GetNbinsY())
  {
    return false;
  }

  const TAxis *xA = histA->GetXaxis();
  const TAxis *xB = histB->GetXaxis();
  const TAxis *yA = histA->GetYaxis();
  const TAxis *yB = histB->GetYaxis();

  return TMath::AreEqualRel(xA->GetXmin(), xB->GetXmin(), 1e-12) &&
         TMath::AreEqualRel(xA->GetXmax(), xB->GetXmax(), 1e-12) &&
         TMath::AreEqualRel(yA->GetXmin(), yB->GetXmin(), 1e-12) &&
         TMath::AreEqualRel(yA->GetXmax(), yB->GetXmax(), 1e-12);
}

TH2ScaleResult ScoreTH2Scale(const TH2D *referenceHist,
                             const TH2D *histToScale,
                             const double scale,
                             const double unityTolerance = 0.10)
{
  TH2ScaleResult result;
  result.scale = scale;

  if (!HaveSameTH2Binning(referenceHist, histToScale))
  {
    throw std::runtime_error("ScoreTH2Scale: input TH2Ds do not have the same binning");
  }

  double sumAbsLogRatio = 0.0;

  for (int ix = 1; ix <= referenceHist->GetNbinsX(); ++ix)
  {
    for (int iy = 1; iy <= referenceHist->GetNbinsY(); ++iy)
    {
      const double referenceContent = referenceHist->GetBinContent(ix, iy);
      const double scaledContent = scale * histToScale->GetBinContent(ix, iy);

      if (referenceContent <= 0.0 || scaledContent <= 0.0)
      {
        continue;
      }

      const double ratio = scaledContent / referenceContent;
      const double absLogRatio = std::fabs(std::log(ratio));

      ++result.nBinsUsed;
      sumAbsLogRatio += absLogRatio;

      if (std::fabs(ratio - 1.0) < unityTolerance)
      {
        ++result.nBinsCloserThanTolerance;
      }
    }
  }

  if (result.nBinsUsed > 0)
  {
    result.meanAbsLogRatio = sumAbsLogRatio / static_cast<double>(result.nBinsUsed);
  }

  return result;
}

TH2ScaleResult FindBestTH2Scale(const TH2D *referenceHist,
                                const TH2D *histToScale,
                                const double minScale = 0.1,
                                const double maxScale = 3.0,
                                const int nScanSteps = 2900,
                                const double unityTolerance = 0.10)
{
  if (minScale <= 0.0 || maxScale <= minScale || nScanSteps <= 0)
  {
    throw std::runtime_error("FindBestTH2Scale: invalid scale scan range");
  }

  TH2ScaleResult bestResult;
  bestResult.scale = minScale;

  for (int i = 0; i <= nScanSteps; ++i)
  {
    const double scale = minScale + (maxScale - minScale) * static_cast<double>(i) / static_cast<double>(nScanSteps);
    const TH2ScaleResult result = ScoreTH2Scale(referenceHist, histToScale, scale, unityTolerance);

    const bool moreBinsNearUnity = result.nBinsCloserThanTolerance > bestResult.nBinsCloserThanTolerance;
    const bool sameBinsButBetterAverage = result.nBinsCloserThanTolerance == bestResult.nBinsCloserThanTolerance &&
                                          result.meanAbsLogRatio < bestResult.meanAbsLogRatio;

    if (moreBinsNearUnity || sameBinsButBetterAverage)
    {
      bestResult = result;
    }
  }

  return bestResult;
}

TH2D *MakeScaledCopy(const TH2D *inputHist, const double scale, const std::string &newName)
{
  TH2D *scaledHist = static_cast<TH2D *>(inputHist->Clone(newName.c_str()));
  scaledHist->Scale(scale);
  return scaledHist;
}

void FindBestTH2Scale()
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
      hToScale->SetBinContent(ix, iy, rand.Gaus(trueScaleDifference * shape, 2.0));
    }
  }

  const double toleranceAroundUnity = 0.10;
  const TH2ScaleResult beforeScaling = ScoreTH2Scale(hReference, hToScale, 1.0, toleranceAroundUnity);
  const TH2ScaleResult bestScaling = FindBestTH2Scale(hReference, hToScale, 0.1, 3.0, 2900, toleranceAroundUnity);

  TH2D *hScaled = MakeScaledCopy(hToScale, bestScaling.scale, "hToScale_bestScaled");

  std::cout << "Before scaling:" << std::endl;
  std::cout << "  scale = " << beforeScaling.scale << std::endl;
  std::cout << "  bins with ratio within " << toleranceAroundUnity << " of unity = "
            << beforeScaling.nBinsCloserThanTolerance << " / " << beforeScaling.nBinsUsed << std::endl;
  std::cout << "  mean |log(ratio)| = " << beforeScaling.meanAbsLogRatio << std::endl;

  std::cout << "Best scaling:" << std::endl;
  std::cout << "  scale = " << bestScaling.scale << std::endl;
  std::cout << "  bins with ratio within " << toleranceAroundUnity << " of unity = "
            << bestScaling.nBinsCloserThanTolerance << " / " << bestScaling.nBinsUsed << std::endl;
  std::cout << "  mean |log(ratio)| = " << bestScaling.meanAbsLogRatio << std::endl;

  std::cout << "Created scaled histogram: " << hScaled->GetName() << std::endl;
}
