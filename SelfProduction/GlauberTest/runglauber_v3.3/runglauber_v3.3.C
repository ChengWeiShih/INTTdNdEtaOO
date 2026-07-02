/*!
 * @file runglauber.C
 * @brief Monte Carlo Glauber Model for Heavy-Ion Collisions
 * @author Constantin Loizides and others
 * @version 3.3.2
 * @date 2026-02-03
 *
 * @mainpage TGlauberMC v3.3.2
 * 
 * @section intro_sec Introduction
 * 
 * TGlauberMC is a Monte Carlo Glauber model implementation for heavy-ion collisions.
 * It simulates the geometry of nuclear collisions and calculates quantities like
 * the number of participating nucleons, binary collisions, eccentricity, etc.
 *
 * Three main classes are provided:
 * - \ref TGlauNucleon class to represent a nucleon
 * - \ref TGlauNucleus class to represent a nucleus
 * - \ref TGlauberMC top-level class to run the Glauber model
 * 
 * The code is maintained at the TGlauberMC page on HepForge: https://tglaubermc.hepforge.org
 * 
 * @section install_sec Installation
 * 
 * To run the code, you need to have ROOT v6 (http://root.cern.ch) installed.
 * It is recommended to use the provided rootlogon.C script in the same directory.
 * 
 * If you do not use the \ref rootlogon.C, at the ROOT prompt, enter:
 * @code
 * root [0] gSystem->Load("libMathMore")
 * root [1] .L runglauber_v3.3.C+
 * @endcode
 * 
 * If you do not have libMathMore, comment out "#define HAVE_MATHMORE" in the code.
 * 
 * @section usage_sec Usage
 * 
 * The most convenient starting point is the provided runAndSaveNtuple() function:
 * @code
 * root [2] runAndSaveNtuple(10000, "Pbpnrw", "Pbpnrw", 68.0, 0, 0.4, 0.0, 0.3)
 * @endcode
 * 
 * This will generate 10,000 Glauber Monte Carlo events for Pb+Pb collisions with
 * σNN = 68.0 mb and minimum nucleon distance of 0.4 fm, omega = 0.3, and store the 
 * results in a ROOT tree.
 * If the provided value for σNN is negative, it will be interpreted as beam energy
 * to compute σNN and σNN^hard.
 * 
 * The list of supported nuclei can be found in the \ref TGlauNucleus::Lookup function in the code.
 *
 * Nuclei names that start with "TR" are taken from TrNucGen (available at https://trnucgen.web.cern.ch).
 * For this to work, the TrNucGen library must be available in a subdirectory called "trnucgen"
 * (or a symbolic link must be present that points to it). 
 *
 * Nuclei names that start with "input" are read from a text file with the following structure:
 * @code
 * # Nucleus generator version: 1.0.2
 * # Nucleus type: TR_CARBON_V1
 * # Nucleus id: 7
 * # Nucleus name: C1
 * # Nucleus description: carbon12(v1)
 * # Number of nucleons: 12
 * # Number of protons: 6
 * # Number of events: 10000
 * # Per line all nuclei, in the format x1, y1, z1, c1, x2, y2, z2, c2, ... 
 * @endcode
 * where the position (x, y, z) and charge c are listed per line for a nucleus configuration.
 * The format of the file is also explained in the example provided with TrNucGen.
 *
 * @section features_sec New Features in v3.3
 * 
 * The main changes related to the previous version (3.2) are:
 * 
 * - Interface to any nucleus configuration provided by Trajectum via the new TrNucGen library
 * - Support to read nucleus configurations from text files
 * - Larger list of pre-defined nucleus configurations (see TGlauNucleus::Lookup function)
 * - Possibility to use overlap functions using TRENTO, HIJING and PYTHIA parameterizations
 * - Calculation of the number of multiple interactions from the nucleon-nucleon overlap function
 * - σNN parameterization for RHIC and LHC energies
 * - Dedicated parameterizations for neutron-neutron (proton-proton) and neutron-proton
 *   interactions relevant at Hades/CBM energies
 * 
 * @section func_sec List of all global functions provided
 * 
 * A number of provided global functions:
 * - \ref runAndSaveNtuple to run the Glauber Monte Carlo and save results to an ntuple
 * - \ref runAndSaveNucleons to run the Glauber Monte Carlo and save nucleon positions to a ROOT file
 * - \ref runAndOutputLemonTree to run the Glauber Monte Carlo and save results to a LemonTree
 * - \ref runAndSmearNtuple to run the Glauber Monte Carlo and save Gaussian-smeared results to an ntuple, for various initial conditions including TRENTO IC
 * - \ref runAndCalcDens to run the Glauber Monte Carlo and save the density histograms
 * - \ref getNNProf and \ref getNNProfDist to get the nucleon-nucleon interaction probability and impact parameter distribution for the Gamma distribution
 * - \ref getNNHijing and \ref getNNHijingDist to get the nucleon-nucleon interaction probability and impact parameter distribution based on the HIJING model
 * - \ref getNNPythia and \ref getNNPythiaDist to get the nucleon-nucleon interaction probability and impact parameter distribution based on the PYTHIA model
 * - \ref getNNTrento and \ref getNNTrentoDist to get the nucleon-nucleon interaction probability and impact parameter distribution based on the TRENTO model
 * - \ref getSigmaNNvsEnergy and\ref getSigmaNN to get the nucleon-nucleon cross section as a function of energy and nucleon-nucleon cross section at a given energy
 * - \ref getSigmaNPvsEnergy_Bystricky and\ref getSigmaNP_Bystricky to get the proton-neutron cross section as a function of energy and proton-neutron cross section at a given energy
 * - \ref getSigmaPPvsEnergy_Bystricky and \ref getSigmaPP_Bystricky to get the proton-proton cross section as a function of energy and proton-proton cross section at a given energy
 * - \ref getSigmaHardvsEnergy and \ref getSigmaHard to get the hard-scattering cross section as a function of energy and hard-scattering cross section at a given energy
 * These also exist as static functions in the TGlauberMC class, eg TGlauberMC::GetNNProf() etc.
 *
 * @section refs_sec References
 * 
 * - TGlauberMC v3.3: https://tglaubermc.hepforge.org
 * - TrNucGen: https://trnucgen.web.cern.ch
 * - Published documentation:
 *  - "Glauber predictions for oxygen and neon collisions at the LHC", https://arxiv.org/abs/2507.05853
 *  - "Improved Monte Carlo Glauber predictions at present and future nuclear colliders", https://arxiv.org/abs/1710.07098
 *  - "Improved version of the PHOBOS Glauber Monte Carlo", https://arxiv.org/abs/1408.2549
 *  - "The PHOBOS Glauber Monte Carlo", https://arxiv.org/abs/0805.4411
 * 
 * @section version_sec Version History
 *
 * v3.3.2: Add Trento smearing in runAndSmearNtuple macro, added static functions to TGlauberMC
 * to eventually replace the global functions as getSigmaNN, compute MPI distribution per event,
 * add static functions to TGlauberMC to read ntuple, get centrality distribution and add 
 * centrality branch to ntuple written to a new file.
 *
 * v3.3.1: Added possibility to set default value for omega in rootlogon.C (via 
 * TGlauberMC::SetDefOmega), increased ranges for O and Ne nuclei, added Osat as
 * replacement for Opar2
 *
 * v3.3: Interface to TrNucGen (https://trnucgen.web.cern.ch), and support to read 
 * nucleus configurations from text files, more pre-defined nucleus configurations
 * (see Lookup function), calculations using TRENTO, HIJING and PYTHIA based NN profiles 
 * (default is still HS approximation) and calculation of the number of multiple parton 
 * interactions (MPI), possibity to get sigmaNN from parameterized fit (getSigmaNN),
 * use of different NN and NP crossections at Hades energies, 
 * several improvements in TGlauNucleus, see https://arxiv.org/abs/2507.05853
 *
 * v3.2: Incorporates changes from v2.7, see https://arxiv.org/abs/1710.07098v3
 *
 * v3.1:
 * Fixes related to spherical nuclei, as well as consistent set of reweighted profiles 
 * for Cu, Au and Xe, see https://arxiv.org/abs/1710.07098v2
 *
 * v3.0:
 * Major update to include separate profile for protons and neutrons, placement of nucleon 
 * dof on lattice, as well as reweighted profiles for recentering, 
 * see https://arxiv.org/abs/1710.07098v1
 *
 v2.7:
 * New macro "runAndOutputLemonTree" for IP-Jazma input (1808.01276), as well as nucleon 
 * configurations for He4, C, and O from wavefunction calculations, clarified use of Hulthen
 * for deuteron, harmonic oscillator param for O, and new mode to use GlauberGribov also 
 * in AA (enable with SetCalcAAGG)
 *
 * v2.6:
 * Includes runAndCalcDens macro, as well as definition for Al, and fixes beta4 for Si2,
 * see https://arxiv.org/abs/1408.2549v8
 *
 * v2.5:
 * Include core/corona determination in Npart, and if requested for area from mc and eccentricity,
 * as well as various Xe parameterizations including deformation,
 * see https://arxiv.org/abs/1408.2549v7
 *
 * v2.4: 
 * Minor update to include Xenon and fix of the TGlauberMC::Draw function, 
 * see https://arxiv.org/abs/1408.2549v4
 *
 * v2.3: 
 * Small bugfixes, see https://arxiv.org/abs/1408.2549v3
 *
 * v2.2:
 * Minor update to provide higher harmonic eccentricities up to n=5, and the average
 * nucleon--nucleon impact parameter (bNN) in tree output. 
 *
 * v2.1: 
 * Minor update to include more proton pdfs, see https://arxiv.org/abs/1408.2549v2
 *
 * v2.0: 
 * First major update with inclusion of Tritium, Helium-3, and Uranium, as well as the 
 * treatment of deformed nuclei and Glauber-Gribov fluctuations of the proton in p+A 
 * collisions, see https://arxiv.org/abs/1408.2549v1
 *
 * v1.1: 
 * First public release of the PHOBOS MC Glauber, see https://arxiv.org/abs/0805.4411
 *
 * @section license_sec License
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#define HAVE_MATHMORE

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TBits.h>
#include <TCanvas.h>
#include <TEllipse.h>
#include <TF1.h>
#include <TF2.h>
#include <TFile.h>
#include <TKey.h>
#include <TH2D.h>
#include <TLine.h>
#include <TMath.h>
#include <TNamed.h>
#include <TNtuple.h>
#include <TObjArray.h>
#include <TRandom.h>
#include <TRotation.h>
#include <TString.h>
#include <TSystem.h>
#include <TVector3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#ifdef HAVE_MATHMORE
#include <Math/SpecFuncMathMore.h>
#endif
#ifdef USE_TRNUCGEN
#define floatingpoint double
#include "nucleusgenerator.h"
#else
//#warning"Need libtrnucgen.so for trajectum models"
#endif
using namespace std;
#endif

#ifndef _runglauber_
#if !defined(__CINT__) || defined(__MAKECINT__)
#define _runglauber_ 3
#endif

/**
 * @brief Get the nucleon-nucleon interaction probability based on Gamma distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon interaction probability
 * based on the Gamma distribution. It is used to determine the probability
 * of a collision between two nucleons as a function of their impact parameter.
 * Deprecated: Use TGlauberMC::GetNNProf instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param omega Omega parameter for the NN profile (0-2, default is 0.3)
 * @param G G parameter for the NN profile (default is 1)
 * @return TF1* Nucleon-nucleon collision profile function
 */
TF1 *getNNProf(Double_t signn=68.0, Double_t omega=0.3, Double_t G=1);

/**
 * @brief Get the nucleon-nucleon impact parameter distribution based on Gamma distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon impact parameter distribution
 * based on the Gamma distribution. 
 * Deprecated: Use TGlauberMC::GetNNProfDist instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param omega Omega parameter for the NN profile (0-2, default is 0.3)
 * @param G G parameter for the NN profile (default is 1)
 * @return TF1* Nucleon-nucleon collision profile distribution function
 */
TF1 *getNNProfDist(Double_t signn=68.0, Double_t omega=0.3, Double_t G=1);

/**
 * @brief Get the HIJING-based nucleon-nucleon interaction probability
 * 
 * This function returns a TF1 object representing the nucleon-nucleon interaction probability
 * based on the HIJING model. It is used to determine the probability
 * of a collision between two nucleons as a function of their impact parameter.
 * Deprecated: Use TGlauberMC::GetNNHijing instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param mu Mu parameter for the profile (default is 3.9 from HIJING)
 * @return TF1* HIJING profile function
 */
TF1 *getNNHijing(Double_t signn=68.0, Double_t mu=3.9);

/**
 * @brief Get the HIJING-based nucleon-nucleon impact parameter distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon impact parameter distribution
 * based on the HIJING model. 
 * Deprecated: Use TGlauberMC::GetNNHijingDist instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param mu Mu parameter for the profile (default is 3.9 from HIJING)
 * @return TF1* HIJING profile distribution function
 */
TF1 *getNNHijingDist(Double_t signn=68.0, Double_t mu=3.9);

/**
 * @brief Get the PYTHIA-based nucleon-nucleon interaction probability
 * 
 * This function returns a TF1 object representing the nucleon-nucleon interaction probability
 * based on the PYTHIA model. It is used to determine the probability
 * of a collision between two nucleons as a function of their impact parameter.
 * Deprecated: Use TGlauberMC::GetNNPythia instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param m M parameter for the profile (default is 1.85 from PYTHIA)
 * @param rp Rp parameter for the profile (default is 1 from PYTHIA)
 * @return TF1* PYTHIA profile function
 */
TF1 *getNNPythia(Double_t signn=68.0, Double_t m=1.85, Double_t rp=1);

/**
 * @brief Get the PYTHIA-based nucleon-nucleon impact parameter distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon impact parameter distribution
 * based on the PYTHIA model.
 * Deprecated: Use TGlauberMC::GetNNPythiaDist instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param m M parameter for the profile (default is 1.85 from PYTHIA)
 * @param rp Rp parameter for the profile (default is 1 from PYTHIA)
 * @return TF1* PYTHIA profile distribution function
 */
TF1 *getNNPythiaDist(Double_t signn=68.0, Double_t m=1.85, Double_t rp=1);

/**
 * @brief Get the TRENTO-based nucleon-nucleon interaction probability
 * 
 * This function returns a TF1 object representing the nucleon-nucleon interaction probability
 * based on the TRENTO model. It is used to determine the probability
 * of a collision between two nucleons as a function of their impact parameter.
 * Deprecated: Use TGlauberMC::GetNNTrento instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param w W parameter for the profile (default is 0.5 from TRENTO)
 * @return TF1* TRENTO profile function
 */
TF1 *getNNTrento(Double_t signn=68.0, Double_t w=0.5) ;

/**
 * @brief Get the TRENTO-based nucleon-nucleon impact parameter distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon impact parameter distribution
 * based on the TRENTO model.
 * Deprecated: Use TGlauberMC::GetNNTrentoDist instead.
 * 
 * @param signn Nucleon-nucleon cross section in mb
 * @param w W parameter for the profile (default is 0.5 from TRENTO)
 * @return TF1* TRENTO profile distribution function
 */
TF1 *getNNTrentoDist(Double_t signn=68.0, Double_t w=0.5);

/**
 * @brief Get the nucleon-nucleon cross section vs energy function
 * 
 * This function returns a TGraph object representing the nucleon-nucleon cross section
 * vs energy. The parameterization is valid for energies from 10 GeV to 100 TeV
 * (from https://arxiv.org/abs/2011.14909).
 * Deprecated: Use TGlauberMC::GetSigmaNNvsEnergy instead.
 *
 * @return TF1* Nucleon-nucleon cross section vs energy
 */
TF1 *getSigmaNNvsEnergy();

/**
 * @brief Get the nucleon-nucleon cross section at a given energy
 * 
 * This function returns the nucleon-nucleon cross section at a given collision energy
 * based on a parameterized fit. The parameterization is valid for energies from 10 GeV to 100 TeV
 * (from https://arxiv.org/abs/2011.14909).
 * Deprecated: Use TGlauberMC::GetSigmaNN instead.
 *
 * @param energy Collision energy in GeV
 * @return Double_t Nucleon-nucleon cross section in mb
 */
Double_t getSigmaNN(Double_t energy=5360);

/**
 * @brief Get the neutron-proton cross section vs energy function
 * 
 * This function returns a TGraph object representing the neutron-proton cross section
 * vs energy. The parameterization is valid for energies from 0.3 GeV to 4.2 GeV
 * (from https://inspirehep.net/literature/244855).
 * Deprecated: Use TGlauberMC::GetSigmaNPvsEnergy_Bystricky instead.
 *
 * @return TGraph* Neutron-proton cross section vs energy
 */
TGraph *getSigmaNPvsEnergy_Bystricky();

/**
 * @brief Get the neutron-proton cross section at a given energy
 * 
 * This function returns the neutron-proton cross section at a given collision energy
 * based on the Bystricky parameterization. This is particularly useful for low-energy
 * collisions (below 10 GeV) relevant for HADES/CBM experiments.
 * Deprecated: Use TGlauberMC::GetSigmaNP_Bystricky instead.
 *
 * @param energy Collision energy in GeV (valid range: 0.3-4.2 GeV)
 * @return Double_t Neutron-proton cross section in mb
 */
Double_t getSigmaNP_Bystricky(Double_t energy);

/**
 * @brief Get the proton-proton cross section vs energy function
 * 
 * This function returns a TGraph object representing the proton-proton cross section
 * vs energy. The parameterization is valid for energies from 0.28 GeV to 425 GeV
 * (from https://inspirehep.net/literature/244855).
 * Deprecated: Use TGlauberMC::GetSigmaPPvsEnergy_Bystricky instead.
 *
 * @return TGraph* Proton-proton cross section vs energy
 */
TGraph *getSigmaPPvsEnergy_Bystricky();

/**
 * @brief Get the proton-proton cross section at a given energy
 * 
 * This function returns the proton-proton cross section at a given collision energy
 * based on the Bystricky parameterization. This is particularly useful for low-energy
 * collisions (below 10 GeV) relevant for HADES/CBM experiments.
 * Deprecated: Use TGlauberMC::GetSigmaPP_Bystricky instead.
 *
 * @param energy Collision energy in GeV (valid range: 0.28-425 GeV)
 * @return Double_t Proton-proton cross section in mb
 */
Double_t getSigmaPP_Bystricky(Double_t energy);

/**
 * @brief Get the hard-scattering cross section vs energy function
 * 
 * This function returns a TGraph object representing the hard-scattering cross section
 * vs energy obtained from HIJING.
 * Deprecated: Use TGlauberMC::GetSigmaHardvsEnergy instead.
 *
 * @return TGraph* Hard-scattering cross section vs energy
 */
TGraph *getSigmaHardvsEnergy();

/**
 * @brief Get the hard-scattering cross section at a given energy
 * 
 * This function returns the hard-scattering cross section at a given collision energy
 * based on the HIJING model.
 * Deprecated: Use TGlauberMC::GetSigmaHard instead.
 *
 * @param energy Collision energy in GeV (valid range: 10-100 TeV)
 * @return Double_t Hard-scattering cross section in mb
 */
Double_t getSigmaHard(Double_t energy=5360);

/** 
 * @brief Run Glauber Monte Carlo and save results to an ntuple
 * 
 * This is the most convenient starting point for Glauber Monte Carlo simulations.
 * It generates the requested number of events for given nucleus types, σNN, ω, and
 * minimum distance values, storing the event-by-event computed quantities in a ROOT tree.
 * If the provided value for σNN is negative, it will be interpreted as beam energy
 * to compute σNN and σNN^hard.
 * 
 * @param n Number of events to generate
 * @param sysA Name of nucleus A (see TGlauNucleus::Lookup for supported nuclei)
 * @param sysB Name of nucleus B (see TGlauNucleus::Lookup for supported nuclei)
 * @param signn Nucleon-nucleon cross section in mb (if negative, interpreted as collision energy in GeV)
 * @param sigwidth Standard deviation of the nucleon-nucleon cross section (for Glauber-Gribov fluctuations)
 * @param mind Minimum distance between nucleons in fm
 * @param omega Omega parameter for the NN profile (0 for hard-sphere, 0-2 for Gamma distribution, 7 for HIJING, 8 for PYTHIA, 9-11 for TRENTO)
 * @param noded Node distance for lattice placement (≤0 for continuous distribution)
 * @param fname Output file name (if NULL, a default name is generated)
 */
void runAndSaveNtuple(const Int_t n,
                      const char *sysA        = "Pbpnrw",
                      const char *sysB        = "Pbpnrw",
                      const Double_t signn    = -5360, // computes sigmaNN from energy
                      const Double_t sigwidth = -1,
                      const Double_t mind     = 0.4,
                      const Double_t omega    = 0.0,
                      const Double_t noded    = -1,
                      const char *fname       = 0);

/**
 * @brief Run Glauber Monte Carlo and save nucleon positions
 * 
 * This function runs the Glauber Monte Carlo and saves the nucleon positions
 * for each event in a ROOT file. This is useful for visualizing the nucleon
 * distributions and for further analysis of the collision geometry.
 * 
 * @param n Number of events to generate
 * @param sysA Name of nucleus A (see TGlauNucleus::Lookup for supported nuclei)
 * @param sysB Name of nucleus B (see TGlauNucleus::Lookup for supported nuclei)
 * @param signn Nucleon-nucleon cross section in mb
 * @param sigwidth Standard deviation of the nucleon-nucleon cross section (for Glauber-Gribov fluctuations)
 * @param mind Minimum distance between nucleons in fm
 * @param verbose Verbosity level (0=minimal, 1=detailed)
 * @param bmin Minimum impact parameter in fm
 * @param bmax Maximum impact parameter in fm
 * @param fname Output file name (if NULL, a default name is generated)
 */
void runAndSaveNucleons(const Int_t n,                    
                        const char *sysA        = "Pbpnrw",           
                        const char *sysB        = "Pbpnrw",           
                        const Double_t signn    = 68.0,           
                        const Double_t sigwidth = -1,
                        const Double_t mind     = 0.4,
                        const Bool_t verbose    = 0,
			                  const Double_t bmin     = 0.0,
			                  const Double_t bmax     = 20.0,
                        const char *fname       = 0);

/**
 * @brief Run Glauber Monte Carlo and save smeared ntuple
 * 
 * This function runs the Glauber Monte Carlo and saves the smeared ntuple
 * for each event in a ROOT file. This is useful for visualizing the smeared nucleon
 * distributions and for further analysis of the collision geometry.
 * The smeared eccentricities are computed using 
 * 
 * @param n Number of events to generate
 * @param p Parameter for the overlap computation (default is 1, set to 0 for TRENTO IC)
 * @param sysA Name of nucleus A (see TGlauNucleus::Lookup for supported nuclei)
 * @param sysB Name of nucleus B (see TGlauNucleus::Lookup for supported nuclei)
 * @param signn Nucleon-nucleon cross section in mb
 * @param mind Minimum distance between nucleons in fm
 * @param omega Omega parameter for the NN profile (0 for hard-sphere, 0-2 for Gamma distribution, 7 for HIJING, 8 for PYTHIA, 9-11 for TRENTO)
 * @param w W parameter for the profile (default is 0.5 from TRENTO)
 * @param k k parameter for the Gamma distribution (default is 1.4 from TRENTO)
 * @param bmin Minimum impact parameter in fm
 * @param bmax Maximum impact parameter in fm
 * @param fname Output file name (if NULL, a default name is generated)
 */
void runAndSmearNtuple(const Int_t n,
                       const Double_t p     = 1,
                       const char *sysA     = "Pbpnrw",
                       const char *sysB     = "Pbpnrw",
                       const Double_t signn = 68.0,
                       const Double_t mind  = 0.4,
                       const Double_t omega = 0.3,
                       const Double_t w     = 0.5,
                       const Double_t k     = 1.4,
		                   const Double_t bmin  = 0.0,
		                   const Double_t bmax  = 20.0,
                       const char *fname    = 0);

/**
 * @brief Run Glauber Monte Carlo and save LemonTree
 * 
 * This function runs the Glauber Monte Carlo and saves the LemonTree
 * for each event in a ROOT file. This is useful for visualizing the LemonTree
 * distributions and for further analysis of the collision geometry.
 * 
 * @param n Number of events to generate
 * @param sigs Smearing parameter in fm (width of Gaussian smearing)
 * @param sysA Name of nucleus A (see TGlauNucleus::Lookup for supported nuclei)
 * @param sysB Name of nucleus B (see TGlauNucleus::Lookup for supported nuclei)
 * @param signn Nucleon-nucleon cross section in mb
 * @param mind Minimum distance between nucleons in fm
 * @param bmin Minimum impact parameter in fm
 * @param bmax Maximum impact parameter in fm
 * @param ogrid If true, output energy density grid
 * @param fname Output file name (if NULL, a default name is generated)
 */
void runAndOutputLemonTree(const Int_t n,
			   const Double_t sigs  = 0.4,
			   const char *sysA     = "p",
			   const char *sysB     = "Pbpnrw",
			   const Double_t signn = 67.6,
			   const Double_t mind  = 0.4,
			   const Double_t bmin  = 0.0,
			   const Double_t bmax  = 20.0,
			   const Bool_t   ogrid = 0,
			   const char *fname    = 0);

/**
 * @brief Run Glauber Monte Carlo and save density histograms
 * 
 * This function runs the Glauber Monte Carlo and saves the density histograms
 * for each event in a ROOT file. This is useful for visualizing the density
 * distributions and for further analysis of the collision geometry.
 * 
 * @param n Number of events to generate
 * @param alpha Two-component model parameter (fraction of binary collisions)
 * @param sysA Name of nucleus A (see TGlauNucleus::Lookup for supported nuclei)
 * @param sysB Name of nucleus B (see TGlauNucleus::Lookup for supported nuclei)
 * @param signn Nucleon-nucleon cross section in mb
 * @param mind Minimum distance between nucleons in fm
 * @param fname Output file name (if NULL, a default name is generated)
 */
void runAndCalcDens(const Int_t n,
		    const Double_t alpha = 0.1,
		    const char *sysA     = "Pbpnrw",
		    const char *sysB     = "Pbpnrw",
		    const Double_t signn = 68.0,
		    const Double_t mind  = 0.4,
		    const char *fname    = "glau_dens_hists.root");

/**
 * \class TGlauNucleon
 * @brief Class representing a single nucleon in the Glauber model
 * 
 * This class represents a single nucleon (proton or neutron) in the Glauber model.
 * It stores the position, type (proton or neutron), and collision information of the nucleon.
 */
class TGlauNucleon : public TObject
{
  protected:
    Double32_t fX;            ///< X position of nucleon
    Double32_t fY;            ///< Y position of nucleon
    Double32_t fZ;            ///< Z position of nucleon
    Int_t      fType;         ///< 0 = neutron, 1 = proton
    Bool_t     fInNucleusA;   ///< =1 from nucleus A, =0 from nucleus B
    Int_t      fNColl;        ///< Number of binary collisions
    Double32_t fEn;           ///< Energy
  public:
    TGlauNucleon() : fX(0), fY(0), fZ(0), fInNucleusA(0), fNColl(0), fEn(0) {}                  ///< Default constructor
    virtual   ~TGlauNucleon() {}                                                                ///< Destructor
    void       Collide()                                  {++fNColl;}                           ///< Increment the number of collisions for this nucleon
    Double_t   Get2CWeight(Double_t x) const              {return 2.*(0.5*(1-x)+0.5*x*fNColl);} ///< Get the two-component weight for this nucleon
    Double_t   GetEnergy()             const              {return fEn;}                         ///< Get the energy of the nucleon
    Int_t      GetNColl()              const              {return fNColl;}                      ///< Get the number of binary collisions for this nucleon
    Int_t      GetType()               const              {return fType;}                       ///< Get the type of the nucleon
    Double_t   GetX()                  const              {return fX;}                          ///< Get the x-coordinate of the nucleon
    Double_t   GetY()                  const              {return fY;}                          ///< Get the y-coordinate of the nucleon
    Double_t   GetZ()                  const              {return fZ;}                          ///< Get the z-coordinate of the nucleon
    Bool_t     IsNeutron()             const              {return (fType==0);}                  ///< Returns true if the nucleon is a neutron
    Bool_t     IsInNucleusA()          const              {return fInNucleusA;}                 ///< Returns true if the nucleon as belonging to nucleus A
    Bool_t     IsInNucleusB()          const              {return !fInNucleusA;}                ///< Returns true if the nucleon as belonging to nucleus B
    Bool_t     IsProton()              const              {return (fType==1);}                  ///< Returns true if the nucleon is a proton
    Bool_t     IsSpectator()           const              {return !fNColl;}                     ///< Returns true if the nucleon is a spectator
    Bool_t     IsWounded()             const              {return fNColl>0;}                    ///< Returns true if the nucleon is wounded
    void       Reset()                                    {fNColl=0;}                           ///< Reset the number of collisions for this nucleon
    void       RotateXYZ(Double_t phi, Double_t theta);
    void       RotateXYZ_3D(Double_t psiX, Double_t psiY, Double_t psiZ);
    void       SetEnergy(Double_t en)                     {fEn = en;}                           ///< Set the energy of the nucleon
    void       SetInNucleusA()                            {fInNucleusA=1;}                      ///< Set the nucleon as belonging to nucleus A
    void       SetInNucleusB()                            {fInNucleusA=0;}                      ///< Set the nucleon as belonging to nucleus B
    void       SetNColl(Int_t nc)                         {fNColl = nc;}                        ///< Set the number of collisions for this nucleon
    void       SetType(Bool_t b)                          {fType = b;}                          ///< Set the type of the nucleon
    void       SetXYZ(Double_t x, Double_t y, Double_t z) {fX=x; fY=y; fZ=z;}                   ///< Set the position of the nucleon
    ClassDef(TGlauNucleon,4) ///< TGlauNucleon class
};

/// @cond
ClassImp(TGlauNucleon)
/// @endcond

/**
 * @brief Rotate the nucleon in 2D around the origin
 * @param phi Rotation angle around Z axis in radians
 * @param theta Rotation angle around Z axis in radians
 */
void TGlauNucleon::RotateXYZ(Double_t phi, Double_t theta)
{
  TVector3 v(fX,fY,fZ);
  TVector3 vr;
  vr.SetMagThetaPhi(1,theta,phi);
  v.RotateUz(vr);
  fX = v.X();
  fY = v.Y();
  fZ = v.Z();
}

/**
 * @brief Rotate the nucleon in 3D around the origin
 * @param psiX Rotation angle around X axis in radians
 * @param psiY Rotation angle around Y axis in radians
 * @param psiZ Rotation angle around Z axis in radians
 */
void TGlauNucleon::RotateXYZ_3D(Double_t psiX, Double_t psiY, Double_t psiZ)
{
  TVector3 v(fX,fY,fZ);
  v.RotateX(psiX);
  v.RotateY(psiY);
  v.RotateZ(psiZ);
  fX = v.X();
  fY = v.Y();
  fZ = v.Z();
}

/**
 * @class TGlauNucleus
 * @brief Class representing a nucleus in the Glauber model
 * 
 * This class represents a nucleus in the Glauber model.
 * It stores the position, type (proton or neutron), and collision information of the nucleus.
 * The class supports many different nucleus types and density profiles, including:
 * - Woods-Saxon (3-parameter Fermi) distribution
 * - Harmonic oscillator parameterization
 * - Deformed nuclei with β₂, β₃, β₄ parameters
 * - Reweighted profiles for proper recentering
 * - Reading nucleon configurations from files
 * - Interface to TrNucGen library for Trajectum nuclei
 * - Possibility to create nucleons from provided text files read in a TGraph object
 * 
 * The key function is \ref TGlauNucleus::Lookup to define the nucleus and its density profile
 * and \ref TGlauNucleus::ThrowNucleons to throw nucleons according to given density profile.
 */
class TGlauNucleus : public TNamed
{
  private:
    Int_t      fN;                   ///< Number of nucleons
    Int_t      fZ;                   ///< Number of protons
    Double_t   fR;                   ///< Radius of the 3pF function
    Double_t   fA;                   ///< Thickness of the 3pF function 
    Double_t   fW;                   ///< Shape parameter of the 3pF function
    Double_t   fR2;                  ///< Radius of the 3pF function (for p and n separately)
    Double_t   fA2;                  ///< Thickness of the 3pF function (for p and n separately)
    Double_t   fW2;                  ///< Shape parameter of the 3pF function (for p and n separately)
    Double_t   fBeta2;               ///< Beta2 (deformed nuclei) 
    Double_t   fBeta3;               ///< Beta3 (deformed nuclei) 
    Double_t   fBeta4;               ///< Beta4 (deformed nuclei) 
    Double_t   fGamma;               ///< Gamma (deformed nuclei) 
    Double_t   fMinDist;             ///< Minimum separation distance (default 0.4 fm)
    Double_t   fNodeDist;            ///< Average node distance (set to <=0 if you do not want the "crystal lattice")
    Double_t   fSmearing;            ///< Node smearing (relevant if fNodeDist>0)
    Int_t      fRecenter;            ///< =1 by default (0=no recentering, 1=recenter all, 2=recenter displacing only one nucleon, 3=recenter by rotating around z and shift in z, 4=recenter by rotation only, 5=recenter in transverse plane)
    Int_t      fLattice;             ///< =0 use HCP by default (1=PCS, 2=BCC, 3=FCC)
    Double_t   fSmax;                ///< Maximum magnitude of cms shift tolerated (99, ie all by default) 
    Int_t      fF;                   ///<! Type of (radial) distribution function
    Int_t      fTrials;              ///<! Store trials needed to complete nucleus
    Int_t      fNonSmeared;          ///<! Store number of non-smeared-node nucleons
    Double_t   fWeight;              ///<! Weight of nucleus (for event-by-event weighting)
    TF1*       fFunc1;               ///<! Probability density function rho(r)
    TF1*       fFunc2;               ///<! Probability density function rho(r) -> if set 1 is for p, 2 is for n
    TF2*       fFunc3;               ///<! Probability density function rho(r,theta) for deformed nuclei
    TObjArray* fNucleons;            ///<! Array of nucleons
    Double_t   fPhiRot;              ///<! Angle phi for nucleus
    Double_t   fThetaRot;            ///<! Angle theta for nucleus
    Double_t   fXRot;                ///<! Angle around X axis for nucleus
    Double_t   fYRot;                ///<! Angle around Y axis for nucleus
    Double_t   fZRot;                ///<! Angle around Z axis for nucleus
    Float_t*** fNucArr;              ///<! Array of events, up to ~20 nucleons (only for small nuclei), 3 coordinates
    Int_t      fNucCounter;          ///<! Event counter
    TBits     *fIsUsed;              ///<! Bits for lattice use  
    Double_t   fMaxR;                ///<! Maximum radius (15fm)
    TGraph    *fInputDens;           ///<! Input density function (if provided via data points)
    void       AllocateNucleons();   
    void       RandomizeNucleons();
    void       AllocateNucArr();
    void       ReadNucArr(const char* fname);
    void       FreeNucArr();
    void       SelectFromNucArr();
    void       Lookup(const char* name);
    Bool_t     TestMinDist(Int_t n, Double_t x, Double_t y, Double_t z) const;

  public:
    TGlauNucleus(const char* iname="Pb", Int_t iN=0, Double_t iR=0, Double_t ia=0, Double_t iw=0, TF1* ifunc=0);
    virtual ~TGlauNucleus();
    Double_t   CalcMinDist()      const;
    Double_t   CalcRmsRadius()    const;
    void       Draw(Option_t* option="") { if (fFunc1) fFunc1->Draw(option); else if (fFunc2) fFunc2->Draw(option); else if (fFunc3) fFunc3->Draw(option); else TObject::Draw(option);} ///< Draw the nucleus density profile
    void       Draw(Double_t xs, Int_t colp, Int_t cols);
    Double_t   GetA()             const {return fA;}           ///< Get the thickness of the 3pF function
    TF1*       GetFunc1()         const {return GetFuncP();}   ///< Get the 3pF function for protons
    TF1*       GetFunc2()         const {return GetFuncN();}   ///< Get the 3pF function for neutrons
    TF2*       GetFunc3()         const {return GetFuncDef();} ///< Get the deformed nucleus function
    TF1*       GetFuncP()         const {return fFunc1;}       ///< Get the 3pF function for protons
    TF1*       GetFuncN()         const {return fFunc2;}       ///< Get the 3pF function for neutrons
    TF2*       GetFuncDef()       const {return fFunc3;}       ///< Get the deformed nucleus function
    TF1*       GetDens(Bool_t n=0)const;
    Double_t   GetSqrtMeanR2()    const;
    Double_t   GetMinDist()       const {return fMinDist;}     ///< Get the minimum separation distance
    Int_t      GetN()             const {return fN;}           ///< Get the number of nucleons
    Double_t   GetNodeDist()      const {return fNodeDist;}    ///< Get the average node distance
    TObjArray *GetNucleons()      const {return fNucleons;}    ///< Get the array of nucleons
    Int_t      GetRecenter()      const {return fRecenter;}    ///< Get the recentering method
    Double_t   GetR()             const {return fR;}           ///< Get the radius of the 3pF function
    Double_t   GetPhiRot()        const {return fPhiRot;}      ///< Get the phi rotation angle
    Double_t   GetThetaRot()      const {return fThetaRot;}    ///< Get the theta rotation angle
    Int_t      GetTrials()        const {return fTrials;}      ///< Get the number of trials needed to complete nucleus
    Int_t      GetNonSmeared()    const {return fNonSmeared;}  ///< Get the number of non-smeared-node nucleons
    Double_t   GetShiftMax()      const {return fSmax;}        ///< Get the maximum shift tolerance
    Double_t   GetW()             const {return fW;}           ///< Get the shape parameter of the 3pF function
    Double_t   GetWeight()        const {return fWeight;}      ///< Get the weight of the nucleus
    Double_t   GetXRot()          const {return fXRot;}        ///< Get the x rotation angle
    Double_t   GetYRot()          const {return fYRot;}        ///< Get the y rotation angle
    Double_t   GetZRot()          const {return fZRot;}        ///< Get the z rotation angle
    Int_t      GetZ()             const {return fZ;}           ///< Get the number of protons
    void       SetA(Double_t ia, Double_t ia2=-1);
    void       SetBeta(Double_t b2, Double_t b3, Double_t b4, Double_t g);  ///< Set the beta parameters for the deformed nucleus function
    void       SetBeta(Double_t b2, Double_t b4);                           ///< Set the beta parameters for the deformed nucleus function
    void       SetGamma(Double_t g);                                        ///< Set the gamma parameter for the deformed nucleus function
    void       SetLattice(Int_t i)               {fLattice=i;}              ///< Set the lattice type
    void       SetMinDist(Double_t min)          {fMinDist=min;}            ///< Set the minimum separation distance
    void       SetN(Int_t in)                    {fN=in;}                   ///< Set the number of nucleons
    void       SetNodeDist(Double_t nd)          {fNodeDist=nd;}            ///< Set the average node distance
    void       SetR(Double_t ir, Double_t ir2=-1);                          ///< Set the radius of the 3pF function
    void       SetRecenter(Int_t b)              {fRecenter=b;}             ///< Set the recentering method
    void       SetShiftMax(Double_t s)           {fSmax=s;}                 ///< Set the maximum shift tolerance
    void       SetSmearing(Double_t s)           {fSmearing=s;}             ///< Set the node smearing
    void       SetW(Double_t iw);                
    void       SetWeight(Double_t w)             {fWeight=w;}               ///< Set the weight of the nucleus
    TVector3  &ThrowNucleons(Double_t xshift=0.);
    ClassDef(TGlauNucleus,7) // TGlauNucleus class
};

/**
 * @class TGlauberMC
 * @brief Top-level class to run the Glauber model
 * 
 * This class represents the top-level class for the Glauber model.
 * It stores the nuclei, and the way to collide them, and collects all the event information
 * provided by the \ref TGlauberMC::Event class, including the number of wounded nucleons, 
 * binary collisions, and impact parameter, etc.
 *
 * The key functions are the \ref TGlauberMC::NextEvent function to generate a new event
 * and the \ref TGlauberMC::Run() function to generate events stored in a TNtuple.
 */
class TGlauberMC : public TNamed
{
  public:
    /**
     * @brief Class representing an event in the Glauber model
     * 
     * This class represents an event in the Glauber model.
     * It stores the event information, including the number of wounded nucleons, binary collisions, and impact parameter.
     */
    class Event {
      public:
        Float_t Npart;       ///< Number of wounded (participating) nucleons in current event
        Float_t Ncoll;       ///< Number of binary collisions in current event
        Float_t Nhard;       ///< Number of hard collisions in current event (based on fHardFrac)
        Float_t Nmpi;        ///< Number of MPI 
        Float_t B;           ///< Impact parameter (b)
        Float_t BNN;         ///< Average NN impact parameter
        Float_t Ncollpp;     ///< Ncoll pp
        Float_t Ncollpn;     ///< Ncoll pn
        Float_t Ncollnn;     ///< Ncoll nn
        Float_t VarX;        ///< Variance of x of wounded nucleons
        Float_t VarY;        ///< Variance of y of wounded nucleons
        Float_t VarXY;       ///< Covariance of x and y of wounded nucleons
        Float_t NpartA;      ///< Number of wounded (participating) nucleons in Nucleus A
        Float_t NpartB;      ///< Number of wounded (participating) nucleons in Nucleus B
        Float_t Npart0;      ///< Number of singly-wounded (participating) nucleons
        Float_t NpartAn;     ///< Number of wounded (participating) neutrons in Nucleus A
        Float_t NpartBn;     ///< Number of wounded (participating) neutrons in Nucleus B
        Float_t Npart0n;     ///< Number of singly-wounded (participating) neutrons
        Float_t AreaW;       ///< Area defined by width of participants
        Float_t SpecA;       ///< Spectator neutrons in nucleus A
        Float_t SpecB;       ///< Spectator neutrons in nucleus B
        Float_t Weight;      ///< Weight of event (needed for e-by-e weighting)
        Float_t Psi1;        ///< Psi1
        Float_t Ecc1;        ///< Eps1
        Float_t Psi2;        ///< Psi2
        Float_t Ecc2;        ///< Eps2
        Float_t Psi3;        ///< Psi3
        Float_t Ecc3;        ///< Eps3
        Float_t Psi4;        ///< Psi4
        Float_t Ecc4;        ///< Eps4
        Float_t Psi5;        ///< Psi5
        Float_t Ecc5;        ///< Eps5
        Float_t AreaA;       ///< Area defined by "and" of participants
        Float_t AreaO;       ///< Area defined by "or" of participants
        Float_t X0;          ///< Production point in x
        Float_t Y0;          ///< Production point in y
        Float_t Phi0;        ///< Direction in phi
        Float_t Length;      ///< Length in phi0
        Float_t MeanX;       ///< <x> of wounded nucleons
        Float_t MeanY;       ///< <y> of wounded nucleons
        Float_t MeanX2;      ///< <x^2> of wounded nucleons
        Float_t MeanY2;      ///< <y^2> of wounded nucleons
        Float_t MeanXY;      ///< <xy> of wounded nucleons
        Float_t MeanXSystem; ///< <x> of all nucleons
        Float_t MeanYSystem; ///< <y> of all nucleons  
        Float_t MeanXA;      ///< <x> of nucleons in nucleus A
        Float_t MeanYA;      ///< <y> of nucleons in nucleus A
        Float_t MeanXB;      ///< <x> of nucleons in nucleus B
        Float_t MeanYB;      ///< <y> of nucleons in nucleus B
        Float_t PhiA;        ///< Phi angle nucleus A
        Float_t ThetaA;      ///< Theta angle nucleus B
        Float_t PhiB;        ///< Phi angle nucleus B
        Float_t ThetaB;      ///< Theta angle nucleus B
        void    Reset()      {Npart=0;Ncoll=0;Nhard=0;Nmpi=0;B=0;BNN=0;Ncollpp=0;Ncollpn=0;Ncollnn=0;VarX=0;VarY=0;VarXY=0;NpartA=0;NpartB=0;Npart0=0;NpartAn=0;NpartBn=0;Npart0n=0;AreaW=0;SpecA=0;SpecB=0;Weight=0;
                              Psi1=0;Ecc1=0;Psi2=0;Ecc2=0;Psi3=0;Ecc3=0;Psi4=0;Ecc4=0;Psi5=0;Ecc5=0;
                              AreaA=0;AreaO=0;X0=0;Y0=0;Phi0=0;Length=0;
                              MeanX=0;MeanY=0;MeanX2=0;MeanY2=0;MeanXY=0;MeanXSystem=0;MeanYSystem=0;MeanXA=0;MeanYA=0;MeanXB=0;MeanYB=0;
                              PhiA=0;ThetaA=0;PhiB=0;ThetaB=0;} // order must match that given in vars below
        ClassDef(TGlauberMC::Event, 2)
    };

  protected:
    TGlauNucleus  fANucleus;       ///< Nucleus A
    TGlauNucleus  fBNucleus;       ///< Nucleus B
    Double_t      fXSect;          ///< Nucleon-nucleon cross section
    Double_t      fXSectNP;        ///< Proton-Neutron cross section (needed for Hades energies)
    Double_t      fXSectOmega;     ///< StdDev of Nucleon-nucleon cross section
    Double_t      fXSectLambda;    ///< Jacobian from tot to inelastic (Strikman)
    Double_t      fXSectEvent;     ///< Event value of Nucleon-nucleon cross section
    TObjArray*    fNucleonsA;      ///<! Array of nucleons in nucleus A
    TObjArray*    fNucleonsB;      ///<! Array of nucleons in nucleus B
    TObjArray*    fNucleons;       ///<! Array which joins Nucleus A & B
    Int_t         fAN;             ///< Number of nucleons in nucleus A
    Int_t         fBN;             ///< Number of nucleons in nucleus B
    TNtuple*      fNt;             ///<! Ntuple for results (created, but not deleted in destructor)
    Double_t      fEvents;         ///< Number of events with at least one collision
    Double_t      fTotalEvents;    ///< All events within selected impact parameter range
    Double_t      fBmin;           ///< Minimum impact parameter to be generated
    Double_t      fBmax;           ///< Maximum impact parameter to be generated
    Double_t      fHardFrac;       ///< Fraction of cross section used for Nhard (def=0.65)
    Int_t         fDetail;         ///< Detail to store (99=all by default)
    Bool_t        fCalcArea;       ///< If true calculate overlap area via grid (slow, off by default)
    Bool_t        fCalcLength;     ///< If true calculate path length (slow, off by default)
    Bool_t        fDoCore;         ///< If true calculate area and eccentricy only for core participants (off by default)
    Bool_t        fDoAAGG;         ///< If true do Glauber Gribov also for AA
    Double_t      fSigH;           ///< Sigma hard process
    Bool_t        fShadow;         ///< If true use shadowed cross section
    Double_t      fOmega;          ///< Omega parameter for NN profile (default=0, use 0-2 for Gamma distributions, 7 for HIJING, 8 for PYTHIA, 9-10 for Trento)
    Int_t         fMaxNpartFound;  ///< Largest value of Npart obtained
    Double_t      fPsiN[10];       ///<! Psi N
    Double_t      fEccN[10];       ///<! Ecc N
    Double_t      f2Cx;            ///<! Two-component x
    TF1          *fPTot;           ///<! Cross section distribution
    TF1          *fNNProf;         ///<! NN profile (hard-sphere == 0 by default)
    Event         fEv;             ///<! Glauber event (results of calculation stored in tree)
    Bool_t        fBC[999][999];   ///<! Array to record binary collision
    Int_t         fMPI[99];        ///<! Array to record distribution of mpi for the current event
    Bool_t        CalcResults(Double_t bgen);
    Bool_t        CalcEvent(Double_t bgen);
    static Double_t gDefOmega;     ///< default omega (-1, corresponds to hard-sphere approximation)

  public:
    TGlauberMC(const char* NA = "Pb", const char* NB = "Pb", Double_t xsect = 42, Double_t xsectsigma=0, Double_t xsectnp=0);
    virtual ~TGlauberMC() {delete fNt; fNt=0; delete fNucleons; fNucleons=0; delete fPTot; fPTot=0; if (fOmega!=99) delete fNNProf; fNNProf=0;}

    Double_t            CalcDens(TF1 &prof, Double_t xval, Double_t yval) const;
    void                Draw(Option_t* option="");
    Double_t            GetB()                 const {return fEv.B;}                ///< Get the impact parameter of the current event
    Double_t            GetBNN()               const {return fEv.BNN;}              ///< Get the average nucleon-nucleon impact parameter of the current event
    Double_t            GetBmax()              const {return fBmax;}                ///< Get the maximum impact parameter to be generated
    Double_t            GetBmin()              const {return fBmin;}                ///< Get the minimum impact parameter to be generated
    Double_t            GetEcc(Int_t i=2)      const {return fEccN[i];}             ///< Get the i-th harmonic order eccentricity for the current event
    Double_t            GetHardFrac()          const {return fHardFrac;}            ///< Get the fraction of cross section used for Nhard (def=0.65)
    Double_t            GetMeanX()             const {return fEv.MeanX;}            ///< Get the mean x-coordinate of participants for the current event
    Double_t            GetMeanXParts()        const {return fEv.MeanX;}            ///< Get the mean x-coordinate of participants for the current event
    Double_t            GetMeanXSystem()       const {return fEv.MeanXSystem;}      ///< Get the mean x-coordinate of all nucleons for the current event 
    Double_t            GetMeanY()             const {return fEv.MeanY;}            ///< Get the mean y-coordinate of participants for the current event
    Double_t            GetMeanYParts()        const {return fEv.MeanY;}            ///< Get the mean y-coordinate of participants for the current event
    Double_t            GetMeanYSystem()       const {return fEv.MeanYSystem;}      ///< Get the mean y-coordinate of all nucleons for the current event
    Double_t            GetPsi(Int_t i=2)      const {return fPsiN[i];}             ///< Get the i-th harmonic order participant plane angle for the current event
    Double_t            GetSx2()               const {return fEv.VarX;}             ///< Get the variance of x-coordinate of participants for the current event
    Double_t            GetSxy()               const {return fEv.VarXY;}            ///< Get the covariance of x and y coordinates of participants for the current event
    Double_t            GetSy2()               const {return fEv.VarY;}             ///< Get the variance of y-coordinate of participants for the current event
    Double_t            GetTotXSect()          const;
    Double_t            GetTotXSectErr()       const;
    Double_t            GetXSectEvent()        const {return fXSectEvent;}          ///< Get the nucleon-nucleon cross section for the current event
    Int_t               GetNcoll()             const {return fEv.Ncoll;}            ///< Get the number of binary collisions for the current event
    Int_t               GetNcollnn()           const {return fEv.Ncollnn;}          ///< Get the number of neutron-neutron binary collisions for the current event
    Int_t               GetNcollpn()           const {return fEv.Ncollpn;}          ///< Get the number of neutron-proton binary collisions for the current event
    Int_t               GetNcollpp()           const {return fEv.Ncollpp;}          ///< Get the number of proton-proton binary collisions for the current event
    Int_t               GetNhard()             const {return fEv.Nhard;}            ///< Get the number of hard collisions for the current event
    Int_t               GetNpart()             const {return fEv.Npart;}            ///< Get the number of wounded (participating) nucleons for the current event
    Int_t               GetNmpi()              const {return fEv.Nmpi;}             ///< Get the number of multiple parton interactions for the current event
    Int_t               GetNpart0()            const {return fEv.Npart0;}           ///< Get the number of singly-wounded (participating) nucleons for the current event
    Int_t               GetNpartA()            const {return fEv.NpartA;}           ///< Get the number of wounded (participating) nucleons in nucleus A for the current event
    Int_t               GetNpartB()            const {return fEv.NpartB;}           ///< Get the number of wounded (participating) nucleons in nucleus B for the current event
    Int_t               GetNpart0n()           const {return fEv.Npart0n;}          ///< Get the number of singly-wounded (participating) neutrons for the current event
    Int_t               GetNpartAn()           const {return fEv.NpartAn;}          ///< Get the number of wounded (participating) neutrons in nucleus A for the current event
    Int_t               GetNpartBn()           const {return fEv.NpartBn;}          ///< Get the number of wounded (participating) neutrons in nucleus B for the current event
    Int_t               GetNpart0p()           const {return fEv.Npart0-fEv.Npart0n;}  ///< Get the number of singly-wounded (participating) protons for the current event
    Int_t               GetNpartAp()           const {return fEv.NpartA-fEv.NpartAn;}  ///< Get the number of wounded (participating) protons in nucleus A for the current event
    Int_t               GetNpartBp()           const {return fEv.NpartB-fEv.NpartBn;}  ///< Get the number of wounded (participating) protons in nucleus B for the current event
    Int_t               GetNpartFound()        const {return fMaxNpartFound;}       ///< Get the largest value of Npart obtained
    Double_t            GetOmega()             const {return fOmega;}               ///< Get the Omega parameter for the NN profile
    Double_t            GetSpecA()             const {return fEv.SpecA;}            ///< Get the number of spectator neutrons in nucleus A for the current event
    Double_t            GetSpecB()             const {return fEv.SpecB;}            ///< Get the number of spectator neutrons in nucleus B for the current event
    Double_t            GetWeight()            const {return fEv.Weight;}           ///< Get the weight of the current event
    TF1*                GetXSectDist()         const {return fPTot;}                ///< Get the cross section distribution
    TGlauNucleus*       GetNucleusA()                {return &fANucleus;}           ///< Get a pointer to the nucleus A
    TGlauNucleus*       GetNucleusB()                {return &fBNucleus;}           ///< Get a pointer to the nucleus B
    TNtuple*            GetNtuple()            const {return fNt;}                  ///< Get a pointer to the ntuple
    TObjArray          *GetNucleons();
    const Event        &GetEvent()             const {return fEv;}                  ///< Get a reference to the current event
    const Event        *GetEvent()                   {return &fEv;}                 ///< Get a pointer to the current event
    const TGlauNucleus* GetNucleusA()          const {return &fANucleus;}           ///< Get a pointer to the nucleus A
    const TGlauNucleus* GetNucleusB()          const {return &fBNucleus;}           ///< Get a pointer to the nucleus B
    Int_t               GetMPI(Int_t n)        const {return fMPI[n];}              ///< Get the amount of sub collisions with n MPI 
    Bool_t              IsBC(Int_t i, Int_t j) const {return fBC[i][j];}            ///< Check if there is a binary collision between the i-th and j-th nucleon
    Bool_t              NextEvent(Double_t bgen=-1);
    void                Reset()                      {delete fNt; fNt=0; }          ///< Delete the ntuple
    Bool_t              ReadNextEvent(Bool_t calc=1, const char *fname=0);        
    void                Run(Int_t nevents,Double_t b=-1);
    void                Set2Cx(Double_t x)           {f2Cx = x;}                    ///< Set the two-component x
    void                SetBmax(Double_t bmax)       {fBmax = bmax;}                ///< Set the maximum impact parameter to be generated
    void                SetBmin(Double_t bmin)       {fBmin = bmin;}                ///< Set the minimum impact parameter to be generated
    void                SetCalcAAGG(Bool_t b)        {fDoAAGG = b;}                 ///< Set whether to use Glauber-Gribov for AA collisions
    void                SetCalcArea(Bool_t b)        {fCalcArea = b;}               ///< Set whether to calculate overlap area
    void                SetCalcCore(Bool_t b)        {fDoCore = b;}                 ///< Set whether to calculate core/corona
    void                SetCalcLength(Bool_t b)      {fCalcLength = b;}             ///< Set whether to calculate path length
    void                SetDetail(Int_t d)           {fDetail = d;}                 ///< Set the detail to store (99=all by default)
    void                SetHardFrac(Double_t f)      {fHardFrac=f;}                 ///< Set the fraction of cross section used for Nhard (def=0.65)
    void                SetLattice(Int_t i)          {fANucleus.SetLattice(i); fBNucleus.SetLattice(i);}   ///< Set the lattice type
    void                SetMinDistance(Double_t d)   {fANucleus.SetMinDist(d); fBNucleus.SetMinDist(d);}   ///< Set the minimum distance between nucleons
    void                SetNNProf(TF1 *f1)           {fNNProf = f1; fOmega=99;}     ///< Set the NN profile by directly providing the function
    void                SetOmega(Double_t omega=-1);                                ///< Set the Omega parameter for the NN profile (0 for hard-sphere, 0-2 for Gamma distribution, 7 for HIJING, 8 for PYTHIA, 9-11 for TRENTO)
    void                SetNodeDistance(Double_t d)  {fANucleus.SetNodeDist(d); fBNucleus.SetNodeDist(d);} ///< Set the node distance between nucleons
    void                SetRecenter(Int_t b)         {fANucleus.SetRecenter(b); fBNucleus.SetRecenter(b);} ///< Set the recentering parameter
    void                SetShiftMax(Double_t s)      {fANucleus.SetShiftMax(s); fBNucleus.SetShiftMax(s);} ///< Set the maximum shift tolerance
    void                SetShadowing(Bool_t b)       {fShadow=b;}                   ///< Set whether to use shadowed cross section
    void                SetSigmaHard(Double_t s)     {fSigH=s;}                     ///< Set the sigma hard process
    void                SetSmearing(Double_t s)      {fANucleus.SetSmearing(s); fBNucleus.SetSmearing(s);} ///< Set the smearing parameter for both nuclei
    void                SetXSect(Double_t sig)       {fXSect=sig;}                  ///< Set the nucleon-nucleon cross section
    void                SetXSectNP(Double_t sig)     {fXSectNP=sig;}                ///< Set the proton-neutron cross section
    void                SetXSectDist(TF1 *f1)        {fPTot = f1;}                  ///< Set the nucleon-nucleon cross section distribution function for Glauber-Gribov fluctuating cross sections
    const char         *Str()                  const;
    static TF1         *GetNNProf(Double_t snn, Double_t omega=0.3, Double_t G=1);
    static TF1         *GetNNProfDist(Double_t snn, Double_t omega=0.3, Double_t G=1);
    static TF1         *GetNNHijing(Double_t signn, Double_t mu=3.9);
    static TF1         *GetNNHijingDist(Double_t signn, Double_t mu=3.9);
    static TF1         *GetNNPythia(Double_t signn, Double_t m=1.85, Double_t rp=1);
    static TF1         *GetNNPythiaDist(Double_t signn, Double_t m=1.85, Double_t rp=1);
    static TF1         *GetNNTrento(Double_t signn, Double_t w=0.5);
    static TF1         *GetNNTrentoDist(Double_t signn, Double_t w=0.5);
    static TF1         *GetSigmaNNvsEnergy();
    static Double_t     GetSigmaNN(Double_t energy);
    static Double_t     GetSigmaNP_Bystricky(Double_t energy);
    static TGraph      *GetSigmaNPvsEnergy_Bystricky();
    static Double_t     GetSigmaPP_Bystricky(Double_t energy);
    static TGraph      *GetSigmaPPvsEnergy_Bystricky();
    static Double_t     GetSigmaHard(Double_t energy);
    static TGraph      *GetSigmaHardvsEnergy();
    static void         FillArea(TH2D *area, const TF1 *nnprof, const TGlauNucleus *nuc, TF1* wf=0,const Double_t shift=0, const Double_t ell=10, const Double_t gmax=3, const Double_t maxweight=1e-6);
    static void         EccFromOverlap(const TH2D *overlap, Double_t ecc[10], Double_t psi[10]);
    static Double_t     TrentoGenMean(const Double_t A, const Double_t B, const Double_t p);
    static TNtuple     *ReadNtuple(const char *fname, const char *name=0);
    static TGraph      *GetCentralityDist(TNtuple *nt, const char *bname="B");
    static void         AddCentralitBranch(const char *fin, const char *fout, const char *tname=0, const char *bname="B", const char *cname="bcen");
    static Double_t     GetDefOmega()                {return gDefOmega;}                           ///< Get the default Omega parameter for the NN profile
    static void         SetDefOmega(Double_t om)     {gDefOmega=om;}                               ///< Set the default Omega parameter for the NN profile
    static void         PrintVersion()               {cout << "TGlauberMC " << Version() << endl;} ///< Print the version of the code
    static const char  *Version()                    {return "v3.3.2";}                            ///< Get the version of the code
    ClassDef(TGlauberMC,8) // TGlauberMC class
};

//---------------------------------------------------------------------------------
TF1 *getNNProf(Double_t snn, Double_t omega, Double_t G) 
{ // is deprecated, use TGlauberMC::GetNNProf() instead
  return TGlauberMC::GetNNProf(snn,omega,G);
}

//---------------------------------------------------------------------------------
TF1 *getNNProfDist(Double_t snn, Double_t omega, Double_t G) 
{ // is deprecated, use TGlauberMC::GetNNProfDist() instead
  return TGlauberMC::GetNNProfDist(snn,omega,G);
}

//---------------------------------------------------------------------------------
TF1 *getNNHijingDist(Double_t signn, Double_t mu) 
{ // is deprecated, use TGlauberMC::GetNNHijingDist() instead
  return TGlauberMC::GetNNHijingDist(signn,mu);
}

//---------------------------------------------------------------------------------
TF1 *getNNHijing(Double_t signn, Double_t mu) 
{ // is deprecated, use TGlauberMC::GetNNHijing() instead
  return TGlauberMC::GetNNHijing(signn,mu);
}

//---------------------------------------------------------------------------------
TF1 *getNNPythiaDist(Double_t signn, Double_t m, Double_t rp) 
{ // is deprecated, use TGlauberMC::GetNNPythiaDist() instead
  return TGlauberMC::GetNNPythiaDist(signn,m,rp);
}

//---------------------------------------------------------------------------------
TF1 *getNNPythia(Double_t signn, Double_t m, Double_t rp) 
{ // is deprecated, use TGlauberMC::GetNNPythia() instead
  return TGlauberMC::GetNNPythia(signn,m,rp);
}

//---------------------------------------------------------------------------------
TF1 *getNNTrentoDist(Double_t signn, Double_t w) 
{ // is deprecated, use TGlauberMC::GetNNTrentoDist() instead
  return TGlauberMC::GetNNTrentoDist(signn,w);
}

//---------------------------------------------------------------------------------
TF1 *getNNTrento(Double_t signn, Double_t w) 
{ // is deprecated, use TGlauberMC::GetNNTrento() instead
  return TGlauberMC::GetNNTrento(signn,w);
}

//---------------------------------------------------------------------------------
TF1 *getSigmaNNvsEnergy()
{ // is deprecated, use TGlauberMC::GetSigmaNNvsEnergy() instead
  return TGlauberMC::GetSigmaNNvsEnergy();
}

//---------------------------------------------------------------------------------
Double_t getSigmaNN(Double_t energy)
{ // is deprecated, use TGlauberMC::GetSigmaNN() instead
  return TGlauberMC::GetSigmaNN(energy);
}

//---------------------------------------------------------------------------------
TGraph *getSigmaNPvsEnergy_Bystricky()
{ // is deprecated, use TGlauberMC::GetSigmaNPvsEnergy_Bystricky() instead
  return TGlauberMC::GetSigmaNPvsEnergy_Bystricky();
}

//---------------------------------------------------------------------------------
Double_t getSigmaNP_Bystricky(Double_t energy)
{ // is deprecated, use TGlauberMC::GetSigmaNP_Bystricky() instead
  return TGlauberMC::GetSigmaNP_Bystricky(energy);
}

//---------------------------------------------------------------------------------
TGraph *getSigmaPPvsEnergy_Bystricky()
{ // is deprecated, use TGlauberMC::GetSigmaPPvsEnergy_Bystricky() instead
  return TGlauberMC::GetSigmaPPvsEnergy_Bystricky();
}

//---------------------------------------------------------------------------------
Double_t getSigmaPP_Bystricky(Double_t energy)
{ // is deprecated, use TGlauberMC::GetSigmaPP_Bystricky() instead
  return TGlauberMC::GetSigmaPP_Bystricky(energy);
}

//---------------------------------------------------------------------------------
TGraph *getSigmaHardvsEnergy()
{ // is deprecated, use TGlauberMC::GetSigmaHardvsEnergy() instead
  return TGlauberMC::GetSigmaHardvsEnergy();
} 

//---------------------------------------------------------------------------------
Double_t getSigmaHard(Double_t energy)
{ // is deprecated, use TGlauberMC::GetSigmaHard() instead
  return TGlauberMC::GetSigmaHard(energy);
}

//---------------------------------------------------------------------------------
void runAndSaveNtuple(const Int_t n,
                      const char *sysA,
                      const char *sysB,
                      const Double_t signn,
                      const Double_t sigwidth,
                      const Double_t mind,
		                  const Double_t omega,
                      const Double_t noded,
                      const char *fname)
{
  TGlauberMC *mcg=new TGlauberMC(sysA,sysB,signn,sigwidth);
  mcg->SetMinDistance(mind);
  mcg->SetNodeDistance(noded);
  mcg->SetCalcLength(0);
  mcg->SetCalcArea(0);
  mcg->SetCalcCore(0);
  mcg->SetDetail(99);
  if ((omega>=0) && (omega<=11)) {
    mcg->SetOmega(omega);
  }
  TString name;
  if (fname) 
    name = fname; 
  else {
    name = Form("%s.root",mcg->Str());
  }
  mcg->Run(n);
  TFile out(name,"recreate",name,9);
  TNtuple  *nt=mcg->GetNtuple();
  nt->Write();
  out.Close();
}

//---------------------------------------------------------------------------------
void runAndSaveNucleons(const Int_t n,                    
                        const char *sysA,           
                        const char *sysB,           
                        const Double_t signn,
                        const Double_t sigwidth,
                        const Double_t mind,
                        const Bool_t verbose,
			                  const Double_t bmin,
			                  const Double_t bmax,
			                  const char *fname)
{
  TGlauberMC *mcg=new TGlauberMC(sysA,sysB,signn,sigwidth);
  mcg->SetMinDistance(mind);
  mcg->SetBmin(bmin);
  mcg->SetBmax(bmax);
  TFile *out=0;
  if (fname) 
    out=new TFile(fname,"recreate",fname,9);

  for (Int_t ievent=0; ievent<n; ++ievent) {
    //get an event with at least one collision
    mcg->Run(1);
    if (ievent%100==0)
      cout << "\r" << 100.*ievent/n << "% done" << flush;

    //access, save and (if wanted) print out nucleons
    TObjArray* nucleons=mcg->GetNucleons();
    if (!nucleons) 
      continue;
    if (out)
      nucleons->Write(Form("nucleonarray%d",ievent),TObject::kSingleKey);

    if (verbose) {
      cout<<endl<<endl<<"EVENT NO: "<<ievent<<endl;
      cout<<"B = "<<mcg->GetB()<<"  Npart = "<<mcg->GetNpart()<<endl<<endl;
      printf("Nucleus\t X\t Y\t Z\tNcoll\n");
      Int_t nNucls=nucleons->GetEntries();
      for (Int_t iNucl=0; iNucl<nNucls; ++iNucl) {
        TGlauNucleon *nucl=(TGlauNucleon *)nucleons->At(iNucl);
        Char_t nucleus='A';
        if (nucl->IsInNucleusB()) 
	        nucleus='B';
        Double_t x=nucl->GetX();
        Double_t y=nucl->GetY();
        Double_t z=nucl->GetZ();
        Int_t ncoll=nucl->GetNColl();
        printf("   %c\t%2.2f\t%2.2f\t%2.2f\t%3d\n",nucleus,x,y,z,ncoll);
      }
    }
  }
  cout << endl << "Done!" << endl;
  if (out) {
    TNtuple *nt = mcg->GetNtuple();
    nt->Write();
    if (verbose)
      out->ls();
    delete out;
  }
}

//---------------------------------------------------------------------------------
void runAndSmearNtuple(const Int_t n,
                       const Double_t p,
                       const char *sysA,
                       const char *sysB,
                       const Double_t signn,
                       const Double_t mind,
                       const Double_t omega,
                       const Double_t w,
                       const Double_t k,
                       const Double_t bmin,
		                   const Double_t bmax,
                       const char *fname)
{
  // Run Glauber and store ntuple with smeared eccentricities in file.

  TGlauberMC *mcg = new TGlauberMC(sysA,sysB,signn);
  mcg->SetMinDistance(mind);
  mcg->SetOmega(omega);
  mcg->SetBmin(bmin);
  mcg->SetBmax(bmax);

  TF1 *nnprof = 0;
  const Double_t gmax=5;
  nnprof = new TF1("trento","exp(-x^2/4/[0]^2)",0,gmax);
  nnprof->SetParameter(0,w);

  TF1 *gammaFunc = new TF1("gammaFunc", "ROOT::Math::gamma_pdf(x, [0], 1.0/[0])", 0, 8);
  gammaFunc->SetParameter(0, k);

  TNtuple *nt = new TNtuple("nt","nt",
                            "Npart:Ncoll:Nhard:Nmpi:B:BNN:Ncollpp:Ncollpn:Ncollnn:NpartA:NpartB:Npart0:NpartAn:NpartBn:Npart0n:SpecA:SpecB:Weight:Ecc1P:Ecc2P:Ecc3P:Ecc4P:Ecc5P:Ecc1G:Ecc2G:Ecc3G:Ecc4G:Ecc5G:S");
  nt->SetDirectory(0);

  const Int_t nbins=(Int_t)(bmax*20)+1;
  const Double_t ell=bmax;
  const Double_t da=2*ell*2*ell/nbins/nbins;
  TH2D *areaA = new TH2D("hAreaA",";x (fm);y (fm)",nbins,-ell,ell,nbins,-ell,ell);
  areaA->Sumw2();
  areaA->SetDirectory(0);
  TH2D *areaB = new TH2D("hAreaB",";x (fm);y (fm)",nbins,-ell,ell,nbins,-ell,ell);
  areaB->Sumw2();
  areaB->SetDirectory(0);
  TH2D *overlap = new TH2D("hOverlap",";x (fm);y (fm)",nbins,-ell,ell,nbins,-ell,ell);
  overlap->Sumw2();
  overlap->SetDirectory(0);

  for (Int_t ievent=0; ievent<n; ++ievent) {
    while (!mcg->NextEvent()) {}
    const TGlauNucleus *nucA   = mcg->GetNucleusA();
    mcg->FillArea(areaA, nnprof, nucA, gammaFunc, 0, ell, gmax, 1e-6);
    const TGlauNucleus *nucB   = mcg-> GetNucleusB();
    mcg->FillArea(areaB, nnprof, nucB, gammaFunc, 0, ell, gmax, 1e-6);
    const Int_t n = overlap->GetNcells();
    Float_t sum=0;
    for (Int_t bin = 0; bin<n; ++bin) {
      const Double_t TA = areaA->GetBinContent(bin);
      const Double_t TB = areaB->GetBinContent(bin);
      const Double_t m = mcg->TrentoGenMean(TA,TB,p);
      overlap->AddBinContent(bin,m);
      sum += m;
    }
    Double_t ecc[10]={0}, psi[10]={0};
    mcg->EccFromOverlap(overlap, ecc, psi);
    if ((ievent>0)&&(ievent%1000)==0) 
      cout << "Event # " << ievent << " x-sect = " << mcg->GetTotXSect() << " +- " << mcg->GetTotXSectErr() << " b        \r" << flush;
    //cout << "Npart A " << mcg->GetNpartA() << " Npart B " << mcg->GetNpartB() << " Ecc2 " << mcg->GetEcc(2) << " Ecc2 " << ecc[2] << endl;
    Float_t v[29]; Int_t i=0;
    v[i++] = mcg->GetNpart();
    v[i++] = mcg->GetNcoll();
    v[i++] = mcg->GetNhard();
    v[i++] = mcg->GetNmpi();
    v[i++] = mcg->GetB();
    v[i++] = mcg->GetBNN();
    v[i++] = mcg->GetNcollpp();
    v[i++] = mcg->GetNcollpn();
    v[i++] = mcg->GetNcollnn();
    v[i++] = mcg->GetNpartA();
    v[i++] = mcg->GetNpartB();
    v[i++] = mcg->GetNpart0();
    v[i++] = mcg->GetNpartAn();
    v[i++] = mcg->GetNpartBn();
    v[i++] = mcg->GetNpart0n();
    v[i++] = mcg->GetSpecA();
    v[i++] = mcg->GetSpecB();
    v[i++] = mcg->GetWeight();
    v[i++] = mcg->GetEcc(1);
    v[i++] = mcg->GetEcc(2);
    v[i++] = mcg->GetEcc(3);
    v[i++] = mcg->GetEcc(4);
    v[i++] = mcg->GetEcc(5);
    v[i++] = ecc[1]; // Gaussian smeared values
    v[i++] = ecc[2];
    v[i++] = ecc[3];
    v[i++] = ecc[4];
    v[i++] = ecc[5];
    v[i++] = sum*da;
    nt->Fill(v);
    if (0) {
      TCanvas *c1 = new TCanvas;
      areaA->Draw("colz");
      c1->Print(Form("areaA_%d.png",ievent));
      TCanvas *c2 = new TCanvas;
      areaB->Draw("colz");
      c2->Print(Form("areaB_%d.png",ievent));
      TCanvas *c3 = new TCanvas;
      overlap->Draw("colz");
      c3->Print(Form("overlap_%d.png",ievent));
    }
    areaA->Reset();
    areaB->Reset();
    overlap->Reset();
  }

  cout << "Event # " << n << " x-sect = " << mcg->GetTotXSect() << " +- " << mcg->GetTotXSectErr() << " b        \r" << flush;
  TString title(Form("%s+%s, x-sect=%.1f mb, str=%s-w%.1f-k%.1f, totx-sect=%.2f mb",
                mcg->GetNucleusA()->GetName(),mcg->GetNucleusB()->GetName(),signn,mcg->Str(),w,k,mcg->GetTotXSect()*1e3));
  nt->SetTitle(title);
  TString name;
  if (fname) 
    name = fname; 
  else {
    name = Form("%s.root",mcg->Str());
  }
  TFile *out = TFile::Open(name,"recreate",name,9);
  if (out) {
    cout << "\nWriting to file " << name << endl;
    nt->Write();
    out->Close();
    delete out;
  }
}

//---------------------------------------------------------------------------------
void runAndOutputLemonTree(const Int_t n,
                       const Double_t sigs,
                       const char *sysA,
                       const char *sysB,
                       const Double_t signn,
                       const Double_t mind,
		                   const Double_t bmin,
		                   const Double_t bmax,
		                   const Bool_t   ogrid,
                       const char *fname)
{
  // Run Glauber and store Lemon TTree in format needed for IP-Jazma input 

  TGlauberMC *mcg = new TGlauberMC(sysA,sysB,signn);
  mcg->SetMinDistance(mind);
  mcg->SetBmin(bmin);
  mcg->SetBmax(bmax);
  
  TFile *out = TFile::Open(fname,"recreate",fname,9);
  if (!out) return;

  // create new TTree with MC Glauber information for input to IP-Jazma
  const Int_t lemonmaxNucleons = 500;
  Int_t       lemonnpart;
  Int_t       lemonncoll;
  Int_t       lemonnparta;
  Int_t       lemonnpartb;  
  Float_t     lemonb;                        // collision impact parameter
  Float_t     lemoneccgaus[10];
  Float_t     lemoneccpoint[10];  
  Int_t       lemonnproj;
  Int_t       lemonntarg;
  Float_t     lemonxproj[lemonmaxNucleons];  // x,y,z coordinates for all nucleons
  Float_t     lemonyproj[lemonmaxNucleons];  // note these must be in the global coordinate frame
  Float_t     lemonzproj[lemonmaxNucleons];
  Float_t     lemonxtarg[lemonmaxNucleons];  // x,y,z coordinates for all nucleons
  Float_t     lemonytarg[lemonmaxNucleons];  // note these must be in the global coordinate frame
  Float_t     lemonztarg[lemonmaxNucleons];
  
  TTree *lemon = new TTree("lemon","lemon");
  lemon->Branch("npart",&lemonnpart,"npart/I");
  lemon->Branch("nparta",&lemonnparta,"nparta/I");
  lemon->Branch("npartb",&lemonnpartb,"npartb/I");  
  lemon->Branch("ncoll",&lemonncoll,"ncoll/I");
  lemon->Branch("b",&lemonb,"b/F");
  lemon->Branch("eccgaus",lemoneccgaus,"eccgaus[10]/F");
  lemon->Branch("eccpoint",lemoneccpoint,"eccpoint[10]/F");  
  lemon->Branch("nproj",&lemonnproj,"nproj/I");
  lemon->Branch("ntarg",&lemonntarg,"ntarg/I");  
  lemon->Branch("xproj",lemonxproj,"xproj[500]/F");
  lemon->Branch("yproj",lemonyproj,"yproj[500]/F");
  lemon->Branch("zproj",lemonyproj,"zproj[500]/F");  
  lemon->Branch("xtarg",lemonxtarg,"xtarg[500]/F");
  lemon->Branch("ytarg",lemonytarg,"ytarg[500]/F");
  lemon->Branch("ztarg",lemonytarg,"ztarg[500]/F");    
  lemon->SetDirectory(out);

  const Int_t NSAMP = 100;
  TF1 *rad = new TF1("rad","x*TMath::Exp(-x*x/(2.*[0]*[0]))",0.0,3*sigs);
  rad->SetParameter(0,sigs);
  TF2* smearing_function = new TF2("smear_tf2", "TMath::Exp(-(x*x+y*y)/(2.*[0]*[0]))/(2*TMath::Pi()*[0]*[0])", 0, 10*sigs, 0, 10*sigs);
  smearing_function->SetParameter(0,sigs);
  
  for (Int_t ievent=0; ievent<n; ++ievent) {
    while (!mcg->NextEvent()) {}

    const TGlauNucleus *nucA   = mcg->GetNucleusA();
    const TObjArray *nucleonsA = nucA->GetNucleons();
    const Int_t AN             = nucA->GetN();
    const TGlauNucleus *nucB   = mcg-> GetNucleusB();
    const TObjArray *nucleonsB = nucB->GetNucleons();
    const Int_t BN             = nucB->GetN();

    Double_t sinphi[10] = {0};
    Double_t cosphi[10] = {0};
    Double_t rn[10]     = {0};
    Double_t ecc[10]    = {0};
    Double_t psi[10]    = {0};

    for (Int_t s=0; s<NSAMP; ++s) {
      Int_t ni = 0;
      Double_t xvals[1000] = {0};
      Double_t yvals[1000] = {0};
      for (Int_t i = 0; i<AN; ++i) {
        TGlauNucleon *nucleonA=(TGlauNucleon*)(nucleonsA->At(i));
        if (!nucleonA->IsWounded())
          continue;
        Double_t sr = rad->GetRandom();
        Double_t sp = gRandom->Uniform(-TMath::Pi(), +TMath::Pi());
        xvals[ni]   = nucleonA->GetX() + sr*TMath::Cos(sp);
        yvals[ni]   = nucleonA->GetY() + sr*TMath::Sin(sp);
        ++ni;
      }
      for (Int_t i = 0; i<BN; ++i) {
        TGlauNucleon *nucleonB=(TGlauNucleon*)(nucleonsB->At(i));
        if (!nucleonB->IsWounded())
          continue;
        Double_t sr = rad->GetRandom();
        Double_t sp = gRandom->Uniform(-TMath::Pi(), +TMath::Pi());
        xvals[ni]   = nucleonB->GetX() + sr*TMath::Cos(sp);
        yvals[ni]   = nucleonB->GetY() + sr*TMath::Sin(sp);
        ++ni;
      }

      Double_t MeanX  = 0;
      Double_t MeanY  = 0;
      for (Int_t i = 0; i<ni; ++i) {
        MeanX  += xvals[i];
        MeanY  += yvals[i];
      }
      MeanX  /= ni;
      MeanY  /= ni;

      for (Int_t j = 1; j<9; ++j) {
        for (Int_t i = 0; i<ni; ++i) {
          Double_t x   = xvals[i] - MeanX;
          Double_t y   = yvals[i] - MeanY;
          Double_t r   = TMath::Sqrt(x*x+y*y);
          Double_t phi = TMath::ATan2(y,x);
          Double_t w = j;
          if (j==1)
            w = 3; // use r^3 weighting for Ecc1/Psi1
          cosphi[j] += TMath::Power(r,w)*TMath::Cos(j*phi);
          sinphi[j] += TMath::Power(r,w)*TMath::Sin(j*phi);
          rn[j]     += TMath::Power(r,w);
        }
      }
    }
    for (Int_t j = 1; j<9; ++j) {
      psi[j] = (TMath::ATan2(sinphi[j],cosphi[j]) + TMath::Pi())/j;
      ecc[j] = TMath::Sqrt(sinphi[j]*sinphi[j] + cosphi[j]*cosphi[j]) / rn[j];
    }

    // fill lemon TTree variables for this event
    lemonnpart   = mcg->GetNpart();
    lemonnparta  = mcg->GetNpartA();
    lemonnpartb  = mcg->GetNpartB();    
    lemonncoll   = mcg->GetNcoll();
    lemonb       = mcg->GetB(); 
    lemoneccpoint[0] = 0.0;
    lemoneccpoint[1] = mcg->GetEcc(1);
    lemoneccpoint[2] = mcg->GetEcc(2);
    lemoneccpoint[3] = mcg->GetEcc(3);
    lemoneccpoint[4] = mcg->GetEcc(4);
    lemoneccpoint[5] = mcg->GetEcc(5);
    lemoneccpoint[6] = mcg->GetEcc(6);    
    lemoneccpoint[7] = mcg->GetEcc(7);    
    lemoneccpoint[8] = mcg->GetEcc(8);    
    lemoneccpoint[9] = mcg->GetEcc(9);    
    lemoneccgaus[0] = 0.0;
    lemoneccgaus[1] = ecc[1];    
    lemoneccgaus[2] = ecc[2];
    lemoneccgaus[3] = ecc[3];
    lemoneccgaus[4] = ecc[4];
    lemoneccgaus[5] = ecc[5];
    lemoneccgaus[6] = ecc[6];
    lemoneccgaus[7] = ecc[7];
    lemoneccgaus[8] = ecc[8];
    lemoneccgaus[9] = ecc[9];
    for (Int_t i = 0; i<AN; ++i) {
      TGlauNucleon *nucleonA=(TGlauNucleon*)(nucleonsA->At(i));
      lemonxproj[i] = nucleonA->GetX();
      lemonyproj[i] = nucleonA->GetY();
      lemonzproj[i] = nucleonA->GetZ();      
    }
    for (Int_t i = 0; i<BN; ++i) {
      TGlauNucleon *nucleonB=(TGlauNucleon*)(nucleonsB->At(i));
      lemonxtarg[i] = nucleonB->GetX();
      lemonytarg[i] = nucleonB->GetY();
      lemonztarg[i] = nucleonB->GetZ();      
    }
    lemon->Fill();
    //======================================================================
    // also include option to write out nucleon smeared energy density map
    //======================================================================
    if (ogrid) {
      const Int_t nbins = 1000; 
      const Int_t nbinsx = nbins;
      const Int_t nbinsy = nbins;
      const Double_t max_x = 7.5;
      // now create an energy density distribution (a.u.)
      TH2D* inited_hist = new TH2D(Form("inited_event%i",ievent), ";x;y;E [a.u.]", nbinsx, -max_x, max_x, nbinsy, -max_x, max_x);
      for (Int_t ybin=1; ybin<=nbinsy; ybin++) {
        for (Int_t xbin=1; xbin<=nbinsx; xbin++) {
          // get the center of the bin
          const Double_t xval = inited_hist->GetXaxis()->GetBinCenter(xbin);
          const Double_t yval = inited_hist->GetYaxis()->GetBinCenter(ybin);
          long double content = 0.;  // sum the contributions from all wounded nucleons
          
          for (Int_t i = 0; i<AN; ++i) {
            TGlauNucleon *nucleonA=(TGlauNucleon*)(nucleonsA->At(i));
            if (!nucleonA->IsWounded()) continue;   // skip non-wounded nucleons
            content += smearing_function->Eval(nucleonA->GetX() - xval, nucleonA->GetY() - yval);
          }
          for (Int_t i = 0; i<BN; ++i) {
            TGlauNucleon *nucleonB=(TGlauNucleon*)(nucleonsB->At(i));
            if (!nucleonB->IsWounded()) continue;   // skip non-wounded nucleons
            content += smearing_function->Eval(nucleonB->GetX() - xval, nucleonB->GetY() - yval);
          }
          inited_hist->SetBinContent(xbin, ybin, content);          
        }
      }
      inited_hist->Write();
      if (inited_hist) delete inited_hist;
    }
  } // end loop over events

  out->Write();
  out->Close();
  delete out;
}

//---------------------------------------------------------------------------------
void runAndCalcDens(
        const Int_t n,
		    const Double_t alpha,
		    const char *sysA,
		    const char *sysB,
		    const Double_t signn,
		    const Double_t mind,
		    const char *fname)
{
  // Run Glauber and store per event a density profile in x and y, calculated from participant and binary positions
  // with relative weight given by alpha.

  TGlauberMC *mcg = new TGlauberMC(sysA,sysB,signn);
  mcg->SetMinDistance(mind);

  TFile *out = 0;
  TCanvas *c1 = 0;
  if (fname) {
    out = TFile::Open(fname,"recreate",fname,9);
    if (!out)
      return;
  } else {
    c1 = new TCanvas;
  }

  const Int_t NSAMP = 100;
  const Double_t wp = (1-alpha)/2;
  const Double_t wb = alpha;
  const Double_t sigs = TMath::Sqrt(signn/20/TMath::Pi()); //from arXiv::0711.3724

  TF1 *rad = new TF1("rad","x*TMath::Exp(-x*x/(2.*[0]*[0]))",0.0,3*sigs);
  rad->SetParameter(0,sigs);

  TH2F *h2f = new TH2F("h2f",";x (fm);y (fm)",121,-15.5,15.5,121,-15.5,15.5);
  h2f->SetStats(0);
  
  for (Int_t ievent=0; ievent<n; ++ievent) {
    while (!mcg->NextEvent()) {}
    h2f->Reset();
    h2f->SetName(Form("Event_%d",ievent));
    h2f->SetTitle(Form("Npart=%d, Ncoll=%d",mcg->GetNpart(), mcg->GetNcoll()));

    const TGlauNucleus *nucA   = mcg->GetNucleusA();
    const TObjArray *nucleonsA = nucA->GetNucleons();
    const Int_t AN             = nucA->GetN();
    const TGlauNucleus *nucB   = mcg-> GetNucleusB();
    const TObjArray *nucleonsB = nucB->GetNucleons();
    const Int_t BN             = nucB->GetN();

    for (Int_t i = 0; i<AN; ++i) {
      TGlauNucleon *nucleonA=(TGlauNucleon*)(nucleonsA->At(i));
      if (!nucleonA->IsWounded())
      	continue;
      Double_t xA=nucleonA->GetX();
      Double_t yA=nucleonA->GetY();
      for (Int_t s=0; s<NSAMP; ++s) {
	      Double_t sr = rad->GetRandom();
	      Double_t sp = gRandom->Uniform(-TMath::Pi(), +TMath::Pi());
	      h2f->Fill(xA+sr*TMath::Cos(sp),yA+sr*TMath::Sin(sp),wp);
      }
    }

    for (Int_t j = 0; j<BN; ++j) {
      TGlauNucleon *nucleonB=(TGlauNucleon*)(nucleonsB->At(j));
      if (!nucleonB->IsWounded())
	      continue;
      Double_t xB=nucleonB->GetX();
      Double_t yB=nucleonB->GetY();
      for (Int_t s=0; s<NSAMP; ++s) {
	      Double_t sr = rad->GetRandom();
	      Double_t sp = gRandom->Uniform(-TMath::Pi(), +TMath::Pi());
	      h2f->Fill(xB+sr*TMath::Cos(sp),yB+sr*TMath::Sin(sp),wp);
      }
    }

    if (alpha>0) {
      for (Int_t i = 0; i<AN; ++i) {
        TGlauNucleon *nucleonA=(TGlauNucleon*)(nucleonsA->At(i));
        if (!nucleonA->IsWounded())
          continue;
        Double_t xA=nucleonA->GetX();
        Double_t yA=nucleonA->GetY();
        for (Int_t j = 0; j<BN; ++j) {
          TGlauNucleon *nucleonB=(TGlauNucleon*)(nucleonsB->At(j));
          if (!mcg->IsBC(i,j))
            continue;
          Double_t xB=nucleonB->GetX();
          Double_t yB=nucleonB->GetY();
          Double_t dX=(xA+xB)/2;
          Double_t dY=(yA+yB)/2;
          for (Int_t s=0; s<NSAMP; ++s) {
            Double_t sr = rad->GetRandom();
            Double_t sp = gRandom->Uniform(-TMath::Pi(), +TMath::Pi());
            h2f->Fill(dX+sr*TMath::Cos(sp),dY+sr*TMath::Sin(sp),wb);
          }
        }
      }
    }
    h2f->Scale(1./h2f->Integral());
    if (out) {
      h2f->Write();
    } else {
      h2f->Draw("colz");
      c1->Update();
      gSystem->Sleep(1000);
    }
  }
  if (out) {
    out->Write();
    out->Close();
    delete out;
  }
}

/** \cond */
ClassImp(TGlauNucleus)
/** \endcond */

/** \brief Constructor for TGlauNucleus
 * when iN is 0, the nucleus is set up from the name given in iname
 * \param iname name of the nucleus
 * \param iN number of nucleons
 * \param iR radius of the nucleus
 * \param ia asymmetry parameter
 * \param iw width parameter
 * \param ifunc function for the density profile
 */
TGlauNucleus::TGlauNucleus(const char* iname, Int_t iN, Double_t iR, Double_t ia, Double_t iw, TF1* ifunc) : 
  TNamed(iname,""),
  fN(iN),fR(iR),fA(ia),fW(iw),fR2(0),fA2(0),fW2(0),fBeta2(0),fBeta4(0),
  fMinDist(0.4),fNodeDist(0.0),fSmearing(0.0),fRecenter(1),fLattice(0),fSmax(99),
  fF(0),fTrials(0),fNonSmeared(0),fWeight(0),fFunc1(ifunc),fFunc2(0),fFunc3(0),fNucleons(0),
  fPhiRot(0),fThetaRot(0),fNucArr(0),fNucCounter(-1),fIsUsed(0),fMaxR(15),
  fInputDens(0)
{
  if (fN==0) {
    cout << "Setting up nucleus " << iname << endl;
    Lookup(iname);
  }
}

/// Destructor for TGlauNucleus
TGlauNucleus::~TGlauNucleus()
{
  if (fIsUsed)
    delete fIsUsed;
  if (fNucleons)
    delete fNucleons;
  delete fFunc1;
  delete fFunc2;
  delete fFunc3;
  delete fInputDens;
  FreeNucArr();
}

/// Allocate the nucleons for the nucleus
void TGlauNucleus::AllocateNucleons()
{
  if (fN<=0) 
    return;
  if (fNucleons==0) {
    fNucleons=new TObjArray(fN);
    fNucleons->SetOwner();
  } else {
    if (fNucleons->GetEntries()>=fN)
      return;
    fNucleons->Clear();
    fNucleons->Expand(fN);
  }
  for (Int_t i=0; i<fN; ++i) {
    TGlauNucleon *nucleon=new TGlauNucleon(); 
    nucleon->SetType(0);
    if (i<fZ) 
      nucleon->SetType(1);
    fNucleons->Add(nucleon); 
  } 
}

/// Randomize the nucleons for the nucleus
void TGlauNucleus::RandomizeNucleons()
{
  for (Int_t i=0,iz=0; i<fN; ++i) {
    TGlauNucleon *nucleon=(TGlauNucleon*)fNucleons->At(i);
    Double_t frac=double(fZ-iz)/double(fN-i);
    Double_t rn=gRandom->Uniform(0,1);
    if (rn<frac) {
      nucleon->SetType(1);
      ++iz;
    } else {
      nucleon->SetType(0);
    }
  }
}

/// Allocate the nucleon configurations for the nucleus
void TGlauNucleus::AllocateNucArr()
{
  FreeNucArr();
  if (fNucCounter<=0) return;
  fNucArr = new Float_t**[fNucCounter];
  for (Int_t i = 0; i < fNucCounter; i++) {
    fNucArr[i] = new Float_t*[fN];
    for (Int_t j = 0; j < fN; j++) {
      fNucArr[i][j] = new Float_t[4]; 
    }
  }
}

/// Free the nucleon configurations for the nucleus
void TGlauNucleus::FreeNucArr()
{
  if (!fNucArr) return;
  for (Int_t i = 0; i < fNucCounter; i++) {
    for (Int_t j = 0; j < fN; j++) {
      delete[] fNucArr[i][j];
    }
    delete[] fNucArr[i];
  }
  delete[] fNucArr;
  fNucArr = 0;
  fNucCounter = -1;
}

/// Read the nucleon configurations for the nucleus
void TGlauNucleus::ReadNucArr(const char *fname=0)
{
  if (fNucCounter>0) {
    cout << "Warning: Should only call this function once (fNucCounter=" << fNucCounter << ")" << endl;
    return;
  }
  TString filename;
  Int_t extraInfo = 0;
  if (fname) {
    filename = fname;
    extraInfo = 1;
  } else {
    TString tmpname = GetName();
    if (tmpname=="He3") {
      filename = "he3_plaintext.dat";
      fNucCounter = 13699;
      extraInfo = 2;
    } else if (tmpname=="H3") {
      filename = "h3_plaintext.dat";
      fNucCounter = 13729;
      extraInfo = 2;
    } else if (tmpname=="He4") {
      filename = "he4_plaintext.dat";
      fNucCounter = 50626;
    } else if (tmpname=="C") {
      filename = "carbon_plaintext.dat";
      fNucCounter = 9999;
      extraInfo = 3;
    } else if (tmpname=="O") {
      filename = "oxygen_plaintext.dat";
      fNucCounter = 10000;
    } else {
      filename = GetName();
    }
  }

  ifstream myfile;
  myfile.open(filename);
  if (!myfile.is_open()) {
    TString datfile = Form("dat/%s",filename.Data());
    myfile.open(datfile.Data());
  }
  if (!myfile) {
    cerr << "ERROR:  no file for nucleon configurations found with name = " << filename << endl;
    gSystem->Exit(123);
  }
  if (fN==0) { // get all information from the file
    string line;
    Int_t nprotons = 0;
    Int_t nnucleons = 0;
    Int_t numEvents = 0;
    char nucleusName[99];
    while (getline(myfile, line)) {
      if (line.find("# Number of events:") != string::npos) {
        sscanf(line.c_str(), "# Number of events: %d", &numEvents);
        break; // stop reading after the number of events
      } else if (line.find("# Number of protons:") != string::npos) {
        sscanf(line.c_str(), "# Number of protons: %d", &nprotons);
      } else if (line.find("# Number of nucleons:") != string::npos) {
        sscanf(line.c_str(), "# Number of nucleons: %d", &nnucleons);
      } else if (line.find("# Nucleus name:") != string::npos) {
        sscanf(line.c_str(), "# Nucleus name: %s", nucleusName);
        SetTitle(GetName());
        SetName(nucleusName);
      }
    }
    fN = nnucleons;
    fZ = nprotons;
    if (fN<=0 || fZ<=0 || fZ>fN) {
      cerr << "ERROR: number of nucleons (fN=" << fN << ") or protons (fZ=" << fZ << ") are not consistent" << endl;
      gSystem->Exit(123);
    }
    fNucCounter = numEvents;
    cout << "Found " << numEvents << " nucleon configurations in the file" << endl;
    cout << "Number of protons: " << nprotons << endl;
    cout << "Number of nucleons: " << nnucleons << endl;
  } else if (fNucCounter<=0) { // this is no longer really needed
    Int_t numEvents = 0;
    string line;
    while (getline(myfile, line)) {
      if (!line.empty()) numEvents++;
    }
    myfile.clear();
    myfile.seekg(0);
    cout <<"Found "<<numEvents<<" nucleon configurations in the file"<<endl;
    fNucCounter = numEvents;
  }
  // read in the data from the file
  AllocateNucArr();
  cout << "Reading file " << filename << " for " << fNucCounter << " nucleon configurations with fN = " << fN << endl;
  Int_t inputcounter = 0;
  while (myfile) {
    if (inputcounter >= fNucCounter) break;
    Float_t foo;
    for (Int_t i = 0; i<fN; ++i) {
      if (extraInfo==3) {
        myfile >> foo >> foo; // dummy info not used
      }
      myfile >> fNucArr[inputcounter][i][0] >> fNucArr[inputcounter][i][1] >> fNucArr[inputcounter][i][2];
      fNucArr[inputcounter][i][3] = -1; // no isospin info
      if (extraInfo==1) {
        myfile >> fNucArr[inputcounter][i][3]; // isospin info
      } else if (extraInfo==2) {
        myfile >> foo >> foo >> foo >> foo; // extra isospin info not used
      }
    }
    ++inputcounter;
  }
  myfile.close();
}

/// Select the nucleon configurations for the nucleus
void TGlauNucleus::SelectFromNucArr()
{
  if (fNucleons==0) {
    cerr << "ERROR: nucleons not set, should not happen!" << endl;
    gSystem->Exit(123);
  }
  fTrials = 1;
  Bool_t ok = false;
  while (!ok) {
    Int_t nucidx=gRandom->Uniform(0,fNucCounter-1);
    ok = true;
    for (Int_t i = 0; i<fN; ++i) {
      TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
      nucleon->Reset();
      Double_t x = fNucArr[nucidx][i][0];
      Double_t y = fNucArr[nucidx][i][1];
      Double_t z = fNucArr[nucidx][i][2];
      Int_t isospin = fNucArr[nucidx][i][3];
      if (isospin>=0)
        nucleon->SetType(isospin);
      nucleon->SetXYZ(x,y,z);
      nucleon->RotateXYZ(fPhiRot,fThetaRot);
      if (!TestMinDist(i,nucleon->GetX(),nucleon->GetY(),nucleon->GetZ())) {
        ++fTrials;
        ok = false;
        break;
      }
    }
  }
}

/// Calculate the minimum separation distance between the nucleons
Double_t TGlauNucleus::CalcMinDist() const
{
  Double_t minr = 1e10;
  for (Int_t i = 0; i<fN; ++i) {
    TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleons->At(i));
    Double_t xA = nucleonA->GetX();
    Double_t yA = nucleonA->GetY();
    Double_t zA = nucleonA->GetZ();
    for (Int_t j = i+1; j<fN; ++j) {
      TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleons->At(j));
      Double_t xB = nucleonB->GetX();
      Double_t yB = nucleonB->GetY();
      Double_t zB = nucleonB->GetZ();
      Double_t d = (xA-xB)*(xA-xB) + (yA-yB)*(yA-yB) + (zA-zB)*(zA-zB);
      if (d<minr)
        minr = d;
    }
  }
  return TMath::Sqrt(minr);
}

/// Calculate the root-mean-square radius of the nucleus
Double_t TGlauNucleus::CalcRmsRadius() const
{
  Double_t rrms = 0;
  for (Int_t i = 0; i<fN; ++i) {
    TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
    Double_t x = nucleon->GetX();
    Double_t y = nucleon->GetY();
    Double_t z = nucleon->GetZ();
    rrms += x*x+y*y+z*z;
  }
  rrms /= fN;
  return TMath::Sqrt(rrms);
}

/// Draw the nucleus
void TGlauNucleus::Draw(Double_t xs, Int_t colp, Int_t cols)
{
  Double_t r = 0.5*TMath::Sqrt(xs/TMath::Pi()/10.);
  TEllipse en;
  en.SetLineStyle(1);
  en.SetLineWidth(1);
  en.SetFillStyle(1001);
  for (Int_t i = 0; i<fNucleons->GetEntries(); ++i) {
    TGlauNucleon* gn = (TGlauNucleon*) fNucleons->At(i);
    if (!gn->IsSpectator()) {
      en.SetFillColor(colp);
      en.DrawEllipse(gn->GetX(),gn->GetY(),r,r,0,360,0,"");
    } else {
      en.SetFillColor(cols);
      en.SetFillStyle(1001);
      en.DrawEllipse(gn->GetX(),gn->GetY(),r,r,0,360,0,"");
    }
  }
}

/**  
* \brief Get the nuclear density function
* \param n normalize to 4π if true
* \return the nuclear density function
*/
TF1* TGlauNucleus::GetDens(Bool_t n) const
{
  if (!fFunc1)
    return 0;
  TF1 *f = 0;
  if (fInputDens)
    f = new TF1(Form("%s_dens",fFunc1->GetName()),[&](double*x, double *p){return p[0]*fInputDens->Eval(x[0]);}, 0.0, fMaxR, 1);  
  else
    f = new TF1(Form("%s_dens",fFunc1->GetName()),Form("[0]*(%s)/x/x",fFunc1->GetName()),0,fMaxR);
  f->SetNpx(1000);
  f->SetParameter(0,1);
  for (Int_t i=0;i<fFunc1->GetNpar();++i)
    f->SetParameter(i+1,fFunc1->GetParameter(i));
  Double_t norm = fZ/fFunc1->Integral(0,fMaxR,0.001);
  if (n)
    norm /= 4*TMath::Pi();
  f->SetParameter(0,norm);
  return f;
} 

/// Calculate the root-mean-square radius of the nucleus
Double_t TGlauNucleus::GetSqrtMeanR2() const
{
  if (!fFunc1)
    return 0;
  TF1 *f = 0;
  if (fInputDens)
    f = new TF1(Form("%s_dummy",fFunc1->GetName()),[&](double*x, double *){return fInputDens->Eval(x[0])*pow(x[0],4);}, 0.0, fMaxR, 0);
  else
    f = new TF1(Form("%s_dummy",fFunc1->GetName()),Form("x^2*%s",fFunc1->GetName()),0,fMaxR);
  for (Int_t i=0;i<fFunc1->GetNpar();++i)
    f->SetParameter(i,fFunc1->GetParameter(i));
  Double_t meanr2 = f->Integral(0,fMaxR,0.001);
  delete f;
  Double_t norm = fFunc1->Integral(0,fMaxR,0.001);
  Double_t ret = TMath::Sqrt(meanr2/norm);
  if (TMath::IsNaN(ret))
    ret = 0;
  return ret;
}


/**  
* \brief Lookup the nucleus
* 
* Pre-defines a variety of nuclei from several sources:
* - Most nuclei use parameters from DeVries et al., Atomic Data and Nuclear Data Tables 36, 495-536 (1987)
* - Light nuclei (He3, H3, He4, C, O) use configurations read from external files
* - Deuteron uses either 2-parameter Fermi (dpf) or Hulthen wavefunctions (dh, d)
* - Proton can use various form factors:
*   - Point-like (p)
*   - Gaussian (pg) 
*   - Dipole form factor (pdg)
* - Some nuclei (Cu2, Al, Si2) include deformation parameters beta2, beta4
* - Oxygen has multiple parameterizations:
*   - Woods-Saxon from Nuclear Physics A150 (1970) 631-654 (Odat)
*   - Harmonic oscillator fits (Oho, Oho2)
*   - NNLO_sat chiral interactions (Osat)
* - Heavy nuclei (Nb93, Zr96, Ru96, Ag107) use parameters from Landolt-Börnstein tables
* - Some nuclei include reweighting parameters (Curw, Cu2rw) for proper recentering
* - Supports nuclei from Trajectum using libtrnucgen (https://trnucgen.web.cern.ch)
* - Supports nuclei reading from input files or from a TGraph objects
*
* \param name name of the nucleus
*/
void TGlauNucleus::Lookup(const char* name)
{
  SetName(name);
  Double_t r0=0, r1=0, r2=0; // if not specified the parameters are from DeVries, ATOMIC DATA AND NUCLEAR DATA TABLES 36,495536 (1987)
  if      (TString(name) == "p")       {fN = 1;   fR = 0.234;      fA = 0;      fW =  0;       fF = 0;  fZ=1;}
  else if (TString(name) == "pi")      {fN = 1;   fR = 0.234;      fA = 0;      fW =  0;       fF = 0;  fZ=1;}
  else if (TString(name) == "pg")      {fN = 1;   fR = 0.514;      fA = 0;      fW =  0;       fF = 9;  fZ=1;} 
  else if (TString(name) == "pdg")     {fN = 1;   fR = 1;          fA = 0;      fW =  0;       fF = 10; fZ=1;} // from arXiv:1101.5953
  else if (TString(name) == "dpf")     {fN = 2;   fR = 0.01;       fA = 0.5882; fW =  0;       fF = 1;  fZ=1;} // deuteron 2pf (tuned to Hulthen)
  else if (TString(name) == "dh")      {fN = 2;   fR = 0.2283;     fA = 1.1765; fW =  0;       fF = 3;  fZ=1;} // deuteron Hulthen free
  else if (TString(name) == "d")       {fN = 2;   fR = 0.2283;     fA = 1.1765; fW =  0;       fF = 4;  fZ=1;} // deuteron Hulthen constrained
  else if (TString(name) == "He3")     {fN = 3;   fR = 0.00;       fA = 0.0000; fW =  0;       fF = 6;  fZ=1;} // read configurations from file
  else if (TString(name) == "H3")      {fN = 3;   fR = 0.00;       fA = 0.0000; fW =  0;       fF = 6;  fZ=2;} // read configurations from file
  else if (TString(name) == "He4")     {fN = 4;   fR = 0.00;       fA = 0.0000; fW =  0;       fF = 6;  fZ=2;} // read configurations from file
  else if (TString(name) == "C")       {fN = 12;  fR = 0.00;       fA = 0.0000; fW =  0;       fF = 6;  fZ=6;} // read configurations from file
  else if (TString(name) == "Npar")    {fN = 14;  fR = 2.570;      fA = 0.0572; fW =  -0.0180; fF = 1;  fZ=7;} 
  else if (TString(name) == "O")       {fN = 16;  fR = 0.00;       fA = 0.0000; fW =  0;       fF = 6;  fZ=8;} // read configurations from file
  else if (TString(name) == "Opar")    {fN = 16;  fR = 2.608;      fA = 0.513;  fW = -0.051;   fF = 1;  fZ=8;  fMaxR=7.5;} // WS parameterization
  else if (TString(name) == "Opar2")   {fN = 16;  fR = 1.850;      fA = 0.497;  fW =  0.912;   fF = 1;  fZ=8;  fMaxR=7.5;} // WS parameterization (from Trajectum OXYGEN_V4)
  else if (TString(name) == "Osat")    {fN = 16;  fR = 0;          fA = 0;      fW =  0;       fF = 90; fZ=8;  fMaxR=7.5; fInputDens=new TGraph("dat/16O_NNLO_sat.txt");}
  else if (TString(name) == "Odat")    {fN = 16;  fR = 2.608,      fA = 0.513;  fW = -0.051;   fF = 16; fZ=8;  fMaxR=7.5;} // WS parameterization (from Nuclear Physics A150 (1970) 631--654)
  else if (TString(name) == "Oho")     {fN = 16;  fR = 1.544;      fA = 1.833;  fW =  0;       fF = 15; fZ=8;  fMaxR=7.5;} // Harmonic oscillator parameterization
  else if (TString(name) == "Oho2")    {fN = 16;  fR = 1.506;      fA = 1.819;  fW =  0;       fF = 15; fZ=8;  fMaxR=7.5;} // Harmonic oscillator fit to data from Nuclear Physics A150 (1970) 631--654
  else if (TString(name) == "Ne")      {fN = 20;  fR = 2.805;      fA = 0.571;  fW =  0;       fF = 1;  fZ=10; fMaxR=8.5;}
  else if (TString(name) == "Ne2")     {fN = 20;  fR = 2.740;      fA = 0.572;  fW =  0;       fF = 1;  fZ=10; fMaxR=8.5;}
  else if (TString(name) == "Ne3")     {fN = 20;  fR = 2.791;      fA = 0.698;  fW = -0.168;   fF = 1;  fZ=10; fMaxR=8.5;}
  else if (TString(name) == "NeTr2")   {fN = 20;  fR = 2.8;        fA = 0.57;   fW =  0;       fF = 8;  fZ=10; fBeta2=0.721; fMaxR=10;} // WS parameterization (TR_NEON_V2 from Trajectum)
  else if (TString(name) == "NeTr3")   {fN = 20;  fR = 2.7243;     fA = 0.4982; fW =  0;       fF = 7;  fZ=10; fBeta2=0.4899; fBeta3=0.2160; fBeta4=0.3055; fGamma=0; fMaxR=10;} // WS parameterization (TR_NEON_V3 from Trajectum)
  else if (TString(name) == "Al")      {fN = 27;  fR = 3.34;       fA = 0.580;  fW =  0.0;     fF = 8;  fZ=13; fBeta2=-0.448; fBeta4=0.239; fMaxR=10;}
  else if (TString(name) == "Si")      {fN = 28;  fR = 3.34;       fA = 0.580;  fW = -0.233;   fF = 1;  fZ=14; fMaxR=10;}
  else if (TString(name) == "Si2")     {fN = 28;  fR = 3.34;       fA = 0.580;  fW =  0;       fF = 8;  fZ=14; fBeta2=-0.478; fBeta4=0.250; fMaxR=10;}
  else if (TString(name) == "S")       {fN = 32;  fR = 2.54;       fA = 2.191;  fW =  0.16;    fF = 2;  fZ=16; fMaxR=10;}
  else if (TString(name) == "Ar")      {fN = 40;  fR = 3.53;       fA = 0.542;  fW =  0;       fF = 1;  fZ=18; fMaxR=10;}
  else if (TString(name) == "Ca")      {fN = 40;  fR = 3.766;      fA = 0.586;  fW = -0.161;   fF = 1;  fZ=20; fMaxR=10;}
  else if (TString(name) == "Ni")      {fN = 58;  fR = 4.309;      fA = 0.517;  fW = -0.1308;  fF = 1;  fZ=28; fMaxR=10;}
  else if (TString(name) == "Cu")      {fN = 63;  fR = 4.20;       fA = 0.596;  fW =  0;       fF = 1;  fZ=29; fMaxR=10;}
  else if (TString(name) == "Curw ")   {fN = 63;  fR = 4.20;       fA = 0.596;  fW =  0;       fF = 12; fZ=29; r0=1.00898; r1=-0.000790403; r2=-0.000389897; fMaxR=10;} 
  else if (TString(name) == "Cu2")     {fN = 63;  fR = 4.20;       fA = 0.596;  fW =  0;       fF = 8;  fZ=29; fBeta2=0.162; fBeta4=-0.006; fMaxR=10;}  
  else if (TString(name) == "Cu2rw")   {fN = 63;  fR = 4.20;       fA = 0.596;  fW =  0;       fF = 14; fZ=29; fBeta2=0.162; fBeta4=-0.006; r0=1.01269; r1=-0.00298083; r2=-9.97222e-05; fMaxR=10;}  
  else if (TString(name) == "CuHN")    {fN = 63;  fR = 4.28;       fA = 0.5;    fW =  0;       fF = 1;  fZ=29; fMaxR=10;} // from arXiv:0904.4080v1
  // Nb93  <r^2>^(1/2)= 4.324    R= 4.9853(1)  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234  from Landolt-Börnstein
  else if (TString(name) == "Nb93LB")  {fN = 93; fR = 4.9853;      fA = 0.5234; fW =  0;       fF = 1;  fZ=41;}
  // Zr96  <r^2>^(1/2)= 4.349    R= 5.0212(2)  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234  from Landolt-Börnstein
  else if (TString(name) == "Zr96LB")  {fN = 96; fR = 5.0212;      fA = 0.5234; fW =  0;       fF = 1; fZ=40;}
  // Ru96  <r^2>^(1/2)= 4.393    R= 5.0845(1)  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234  from Landolt-Börnstein
  else if (TString(name) == "Ru96LB")  {fN = 96; fR = 5.0845;      fA = 0.5234; fW =  0;       fF = 1; fZ=44;}
  // Ag107  <r^2>^(1/2)= 4.544    R= 5.3006(1)  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234  from Landolt-Börnstein
  else if (TString(name) == "Ag107LB") {fN = 107; fR = 5.3006;     fA = 0.5234; fW =  0;       fF = 1;  fZ=47;}
  // Ag109  <r^2>^(1/2)= 4.565    R= 5.3306(1)  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234  from Landolt-Börnstein
  else if (TString(name) == "Ag109LB") {fN = 109; fR = 5.3306;     fA = 0.5234; fW =  0;       fF = 1; fZ=47;} 
   //from GiBUU Lenske et al.
  else if (TString(name) == "Ag107pn") {fN = 107; fR = 5.2731;     fA = 0.4749;  fW =  0;      fF = 11; fZ=47; fR2=5.4262;fA2=0.4776; fW2=0;}
  else if (TString(name) == "Ag109pn") {fN = 109; fR = 5.2943;     fA = 0.4729;  fW =  0;      fF = 11; fZ=47; fR2=5.4762;fA2=0.4788; fW2=0;}
  //from HFB14
  else if (TString(name) == "Ag107pnHFB14") {fN = 107; fR = 5.2875;fA = 0.4788;  fW =  0;      fF = 11; fZ=47; fR2=5.287; fA2=0.5498; fW2=0;}
  else if (TString(name) == "Ag109pnHFB14") {fN = 109; fR = 5.3160;fA = 0.4776;  fW =  0;      fF = 11; fZ=47; fR2=5.3246;fA2=0.5593; fW2=0;}
  // 50Sn  *stable isotopes
  else if (TString(name) == "Sn112")   {fN = 112; fR = 5.3714; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn114")   {fN = 114; fR = 5.3943; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn116")   {fN = 116; fR = 5.4173; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn117")   {fN = 117; fR = 5.1241; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn118")   {fN = 118; fR = 5.4391; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn119")   {fN = 119; fR = 5.4431; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn120")   {fN = 120; fR = 5.4588; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn122")   {fN = 122; fR = 5.4761; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn124")   {fN = 124; fR = 5.4907; fA = 0.5234; fW =  0;           fF = 1; fZ=50;}
  else if (TString(name) == "Sn112pr3"){fN = 112; fR = 4.962; fA = 2.638/(4.*TMath::Log(3.)); fW =  0.285; fF = 1; fZ=50;}
  else if (TString(name) == "Sn114pr3"){fN = 114; fR = 4.971; fA = 2.636/(4.*TMath::Log(3.)); fW =  0.320; fF = 1; fZ=50;}
  else if (TString(name) == "Sn116pr3"){fN = 116; fR = 5.062; fA = 2.625/(4.*TMath::Log(3.)); fW =  0.272; fF = 1; fZ=50;}
  else if (TString(name) == "Sn117pr3"){fN = 117; fR = 5.058; fA = 2.625/(4.*TMath::Log(3.)); fW =  0.295; fF = 1; fZ=50;}
  else if (TString(name) == "Sn118pr3"){fN = 118; fR = 5.072; fA = 2.623/(4.*TMath::Log(3.)); fW =  0.304; fF = 1; fZ=50;}
  else if (TString(name) == "Sn119pr3"){fN = 119; fR = 5.100; fA = 2.618/(4.*TMath::Log(3.)); fW =  0.290; fF = 1; fZ=50;}
  else if (TString(name) == "Sn120pr3"){fN = 120; fR = 5.110; fA = 2.619/(4.*TMath::Log(3.)); fW =  0.292; fF = 1; fZ=50;}
  else if (TString(name) == "Sn122pr3"){fN = 122; fR = 5.088; fA = 2.611/(4.*TMath::Log(3.)); fW =  0.378; fF = 1; fZ=50;}
  else if (TString(name) == "Sn124pr3"){fN = 124; fR = 5.150; fA = 2.615/(4.*TMath::Log(3.)); fW =  0.311; fF = 1; fZ=50;}
  // 50Sn  non-stable isotopes from Angeli2013  https://inspirehep.net/literature/1611365
  else if (TString(name) == "Sn108")   {fN =108; fR = 5.3274;      fA = 0.5234; fW =  0;       fF = 1;  fZ=50;} // Sn108  <r^2>^(1/2)= 4.5605  --> fR = 5.3274
  else if (TString(name) == "Sn132")   {fN =132; fR = 5.5387;      fA = 0.5234; fW =  0;       fF = 1;  fZ=50;} // Sn132  <r^2>^(1/2)= 4.7093  --> fR = 5.5387
  else if (TString(name) == "Xe")      {fN = 129; fR = 5.36;       fA = 0.59;   fW =  0;       fF = 1;  fZ=54;} // adapted from arXiv:1703.04278
  else if (TString(name) == "Xes")     {fN = 129; fR = 5.42;       fA = 0.57;   fW =  0;       fF = 1;  fZ=54;} // scale from Sb (Antimony, A=122, r=5.32) by 1.019 = (129/122)**0.333
  else if (TString(name) == "Xe2")     {fN = 129; fR = 5.36;       fA = 0.59;   fW =  0;       fF = 8;  fZ=54; fBeta2=0.161; fBeta4=-0.003;} // adapted from arXiv:1703.04278 and Z. Physik (1974) 270: 113
  else if (TString(name) == "Xe2a")    {fN = 129; fR = 5.36;       fA = 0.59;   fW =  0;       fF = 8;  fZ=54; fBeta2=0.18; fBeta4=0;} // ALICE parameters (see public note from 2018 at https://cds.cern.ch/collection/ALICE%20Public%20Notes?ln=en)
  else if (TString(name) == "Xerw")    {fN = 129; fR = 5.36;       fA = 0.59;   fW =  0;       fF = 12; fZ=54; r0=1.00911; r1=-0.000722999; r2=-0.0002663;}
  else if (TString(name) == "Xesrw")   {fN = 129; fR = 5.42;       fA = 0.57;   fW =  0;       fF = 12; fZ=54; r0=1.0096; r1=-0.000874123; r2=-0.000256708;}
  else if (TString(name) == "Xe2arw")  {fN = 129; fR = 5.36;       fA = 0.59;   fW =  0;       fF = 14; fZ=54; fBeta2=0.18; fBeta4=0; r0=1.01246; r1=-0.0024851; r2=-5.72464e-05;} 
  // W184 <r^2>^(1/2)= 5.373    R= 6.3599  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234   from Landolt-Börnstein
  else if (TString(name) == "W184LB")  {fN = 184; fR = 6.3599;     fA = 0.523;  fW =  0;       fF = 1;  fZ=74;}
  else if (TString(name) == "W")       {fN = 186; fR = 6.58;       fA = 0.480;  fW =  0;       fF = 1;  fZ=74;}
  // W186 <r^2>^(1/2)= 5.381    R= 6.3839  fixed t = 2.3 --> a = t/(4 ln(3)) = 0.5234   from Landolt-Börnstein
  else if (TString(name) == "W186LB")  {fN = 186; fR = 6.3839;     fA = 0.523;  fW =  0;       fF = 1;  fZ=74;}
  else if (TString(name) == "Au")      {fN = 197; fR = 6.38;       fA = 0.535;  fW =  0;       fF = 1;  fZ=79;}
  else if (TString(name) == "Aurw")    {fN = 197; fR = 6.38;       fA = 0.535;  fW =  0;       fF = 12; fZ=79; r0=1.00899; r1=-0.000590908; r2=-0.000210598;}
  else if (TString(name) == "Au2")     {fN = 197; fR = 6.38;       fA = 0.535;  fW =  0;       fF = 8;  fZ=79; fBeta2=-0.131; fBeta4=-0.031; }
  else if (TString(name) == "Au2rw")   {fN = 197; fR = 6.38;       fA = 0.535;  fW =  0;       fF = 14; fZ=79; fBeta2=-0.131; fBeta4=-0.031; r0=1.01261; r1=-0.00225517; r2=-3.71513e-05;}
  else if (TString(name) == "AuHN")    {fN = 197; fR = 6.42;       fA = 0.44;   fW =  0;       fF = 1;  fZ=79;} // from arXiv:0904.4080v1
  else if (TString(name) == "Au197LB")     {fN = 197; fR = 6.5541; fA = 0.523;  fW =  0;       fF = 1;  fZ=79;}
  // from muonic and HBF calc from Landolt-Börnstein
  else if (TString(name) == "Au4pn")       {fN = 197; fR = 6.538;  fA = 0.465;  fW =  0;       fF = 11; fZ=79; fR2=6.794; fA2=0.483; fW2=0;}   //from GiBUU Lenske et al.
  else if (TString(name) == "Au197pnHFB14"){fN = 197; fR = 6.5831; fA = 0.4628; fW =  0;       fF = 11; fZ=79; fR2=6.6604; fA2=0.5464; fW2=0;} //HFB14
  else if (TString(name) == "Pb")      {fN = 208; fR = 6.62;       fA = 0.546;  fW =  0;       fF = 1;  fZ=82;}
  else if (TString(name) == "Pbrw")    {fN = 208; fR = 6.62;       fA = 0.546;  fW =  0;       fF = 12; fZ=82; r0=1.00863; r1=-0.00044808; r2=-0.000205872;} //only Pb 207 was tested but should be the same for 208
  else if (TString(name) == "Pb*")     {fN = 208; fR = 6.624;      fA = 0.549;  fW =  0;       fF = 1;  fZ=82;}
  else if (TString(name) == "PbHN")    {fN = 208; fR = 6.65;       fA = 0.460;  fW =  0;       fF = 1;  fZ=82;}
  else if (TString(name) == "Pbpn")    {fN = 208; fR = 6.68;       fA = 0.447;  fW =  0;       fF = 11; fZ=82; fR2=6.69; fA2=0.56; fW2=0;}
  else if (TString(name) == "Pbpnrw")  {fN = 208; fR = 6.68;       fA = 0.447;  fW =  0;       fF = 13; fZ=82; fR2=6.69; fA2=0.56; fW2=0;}
  else if (TString(name) == "U")       {fN = 238; fR = 6.188;      fA = 0.54;   fW =  0;       fF = 5;  fZ=92; fBeta2=1.77;} // Uranium from Heinz & Kuhlman, nucl-th/0411054, fR is defined as 6.8*0.91, fW=6.8*0.26
  else if (TString(name) == "U2")      {fN = 238; fR = 6.67;       fA = 0.44;   fW =  0;       fF = 8;  fZ=92; fBeta2=0.280; fBeta4=0.093;}
  else if (TString(name).BeginsWith("TR_")) {fN = 0; fR = 0; fA = 0; fW = 0; fF = 99; fZ=0;}  // Trajectum models (needs libtrnucgen)
  else if (TString(name).BeginsWith("input")) {fN = 0; fR = 0; fA = 0; fW = 0; fF = 6; fZ=0;} // Use input file with nucleon configurations
  else {
    cout << "Warning: Could not find nucleus " << name << " in lookup table" << endl;
    return;
  }
  switch (fF) {
    case 0: // Proton exp
      fFunc1 = new TF1(name,"x^2*exp(-x/[0])",0,fMaxR);
      fFunc1->SetParameter(0,fR);
      break;
    case 1: // 3pF
      fFunc1 = new TF1(name,"x^2*(1+[2]*(x/[0])**2)/(1+exp((x-[0])/[1]))",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW);
      break;
    case 2: // 3pG
      fFunc1 = new TF1(name,"x^2*(1+[2]*(x/[0])^2)/(1+exp((x^2-[0]^2)/[1]^2))",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW);
      break;
    case 3: // Hulthen (see nucl-ex/0603010)
    case 4: // same but constrain the neutron opposite to the proton event-by-event
      fFunc1 = new TF1(name,"x^2*([0]*[1]*([0]+[1]))/(2*pi*(pow([0]-[1],2)))*pow((exp(-[0]*x)-exp(-[1]*x))/x,2)",0,fMaxR);
      fFunc1->SetParameters(fR,fA);
      break;
    case 5: // Ellipsoid (Uranium), box method
      break;
    case 6: // read from configuration file
      fFunc1 = 0;
      fMaxR = 0;
      ReadNucArr();
      break;
    case 7: // Deformed nuclei, box method
#ifndef HAVE_MATHMORE
      cerr << "Need libMathMore.so for deformed nuclei" << endl;
      gSystem->Exit(123);
#endif
      fFunc1 = 0; // no func: only need beta parameters and use uniform box distribution
      break;
    case 8: // Deformed nuclei, TF2 method
      fFunc3 = new TF2(name,"x^2*TMath::Sin(y)/(1+exp((x-[0]*(1+[2]*0.315*(3*pow(cos(y),2)-1.0)+[3]*0.105*(35*pow(cos(y),4)-30*pow(cos(y),2)+3)))/[1]))",0,fMaxR,0.0,TMath::Pi());
      fFunc3->SetNpx(120);
      fFunc3->SetNpy(120);
      fFunc3->SetParameters(fR,fA,fBeta2,fBeta4);
      break;
    case 9: // Proton gaus
      fFunc1 = new TF1(name,"x^2*exp(-x*x/[0]/[0]/2)",0,5);
      fFunc1->SetParameter(0,fR);
      break;
    case 10: // Proton dgaus
      fFunc1 = new TF1(name,"x^2*((1-[0])/[1]^3*exp(-(x/[1])^2)+[0]/(0.4*[1])^3*exp(-x^2/(0.4*[1])^2))",0,5);
      fFunc1->SetParameter(0,0.5);
      fFunc1->SetParameter(1,fR);
      break;
    case 11: // 3pF for proton and neutrons
      fFunc1 = new TF1(name,"x^2*(1+[2]*(x/[0])^2)/(1+exp((x-[0])/[1]))",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW);
      fFunc2 = new TF1(name,"x^2*(1+[2]*(x/[0])^2)/(1+exp((x-[0])/[1]))",0,fMaxR);
      fFunc2->SetParameters(fR2,fA2,fW2);
      break;
    case 12: // reweighted
      fFunc1 = new TF1(name,"x^2*(1+[2]*(x/[0])^2)/(1+exp((x-[0])/[1]))/([3]+[4]*x+[5]*x^2)",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW,r0,r1,r2); 
      fRecenter=1;
      fSmax=0.1;
      break;
    case 13: // Pb for proton and neutrons reweighted
      fFunc1 = new TF1(Form("%s_prot",name),"x^2*(1+[2]*(x/[0])**2)/(1+exp((x-[0])/[1]))/([3]+[4]*x+[5]*x^2)",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW,1.00866,-0.000461484,-0.000203571);
      fFunc2 = new TF1(Form("%s_neut",name),"x^2*(1+[2]*(x/[0])**2)/(1+exp((x-[0])/[1]))/([3]+[4]*x+[5]*x^2)",0,fMaxR);
      fFunc2->SetParameters(fR2,fA2,fW2,1.00866,-0.000461484,-0.000203571);
      fRecenter=1;
      fSmax=0.1;
      break;
    case 14: // Deformed nuclei, TF2 method, reweighted
      fFunc3 = new TF2(name,"x^2*TMath::Sin(y)/(1+exp((x-[0]*(1+[2]*0.315*(3*pow(cos(y),2)-1.0)+[3]*0.105*(35*pow(cos(y),4)-30*pow(cos(y),2)+3)))/[1]))/([4]+[5]*x+[6]*x^2)",0,fMaxR,0.0,TMath::Pi());
      fFunc3->SetNpx(120);
      fFunc3->SetNpy(120);
      fFunc3->SetParameters(fR,fA,fBeta2,fBeta4,r0,r1,r2);
      fRecenter=1;
      fSmax=0.1;
      break;
    case 15: // harmonic oscillator model 
      fFunc1 = new TF1(name,"x^2*(1+[0]*(x/[1])^2)*exp(-(x/[1])^2)",0,fMaxR);
      fFunc1->SetParameters(fR,fA);
      break;      
    case 16: // from Nuclear Physics A150 (1970) 631--654;
      fFunc1 = new TF1(name,"x^2*(1-[3]*sin([5]*x)*exp(-([4]*x)**2)/([5]*x)+[2]*(x/[0])**2)/(1+exp((x-[0])/[1]))",0,fMaxR);
      fFunc1->SetParameters(fR,fA,fW,0.102,0.35,2.76);
      break;
    case 90: // via TGraph input
      if (!fInputDens) {
        fInputDens = new TGraph(GetTitle());
        if (!fInputDens) {
          cerr << "ERROR: Could not create TGraph for input file given by " << GetTitle() << endl;
          gSystem->Exit(123);
        }
      }
      fFunc1 = new TF1(name,[&](double*x, double *p){return p[0]*fInputDens->Eval(x[0])*x[0]*x[0];}, 0.0, fMaxR, 1);
      fFunc1->SetParameter(0,1);
      break;
    case 99: // Trajectum models
#ifndef USE_TRNUCGEN 
      cerr << "ERROR: Need libtrnucgen.so for trajectum models" << endl;
      gSystem->Exit(123);
#endif
      break;
    default:
      cerr << "Warning: Could not find function type " << fF << endl;
      return;
  }
  if (fFunc1)   
    fFunc1->SetNpx(1000);
  return;
}

/**  
* \brief Set the thickness parameter
* \param ia thickness parameter
* \param ia2 thickness parameter for neutron distribution (if used)
*/
void TGlauNucleus::SetA(Double_t ia, Double_t ia2)
{
  fA  = ia;
  fA2 = ia2;
  switch (fF) {
    case 1:  // 3pF
    case 12: // 3pF with pol2 normalization
    case 2:  // 3pG
    case 7:  // Box method
    case 15: // harmonic oscillator model 
    case 5:  // Ellipsoid (Uranium)
      fFunc1->SetParameter(1,fA);
      break;
    case 8:
      fFunc3->SetParameter(1,fA);
      break;
    case 11: //p&n
      fFunc1->SetParameter(1,fA);//proton
      fFunc2->SetParameter(1,fA2);//neutron
      break;
    default:
      cerr << "Warning: fA not needed for function " << fF <<endl;
  }
}

/**  
* \brief Set the deformation parameters
* \param b2 deformation parameter
* \param b4 deformation parameter
*/
void TGlauNucleus::SetBeta(Double_t b2, Double_t b4) 
{
  fBeta2=b2; 
  fBeta4=b4;      
  if (fFunc3) {
    fFunc3->SetParameter(2,fBeta2);
    fFunc3->SetParameter(3,fBeta4);
  }
}

/**  
* \brief Set the deformation parameters
* \param b2 deformation parameter
* \param b3 deformation parameter
* \param b4 deformation parameter
* \param g gamma parameter
*/
void TGlauNucleus::SetBeta(Double_t b2, Double_t b3, Double_t b4, Double_t g) 
{
  fBeta2=b2; 
  fBeta3=b3; 
  fBeta4=b4; 
  fGamma=g;
  fF=7; 
}

/**  
* \brief Set the gamma parameter
* \param g gamma parameter
*/
void TGlauNucleus::SetGamma(Double_t g) 
{
  fGamma=g; 
  fF=7; 
}

/**  
* \brief Set the radius parameter
* \param ir radius parameter
* \param ir2 radius parameter for the neutron distribution (if used)
*/
void TGlauNucleus::SetR(Double_t ir, Double_t ir2)
{
  fR  = ir;
  fR2 = ir2;
  switch (fF) {
    case 0:  // Proton exp
    case 9:  // Proton gaus
    case 1:  // 3pF
    case 12: // 3pF with pol2 normalization
    case 2:  // 3pG
    case 15: // harmonic oscillator model 
    case 5:  // Ellipsoid (Uranium)
      fFunc1->SetParameter(0,fR);
      break;
    case 8:
      fFunc3->SetParameter(0,fR);
      break;
    case 10: // Proton
      fFunc1->SetParameter(1,fR);
      break;
    case 11: // p&n
      fFunc1->SetParameter(0,fR);//proton
      fFunc2->SetParameter(0,fR2);//neutron
      break;
    default:
      cerr << "Warning: fR not needed for function " << fF <<endl;
  }
}

/**  
* \brief Set the shape parameter
* \param iw shape parameter
*/
void TGlauNucleus::SetW(Double_t iw)
{
  fW = iw;
  switch (fF) {
    case 1: // 3pF
    case 2: // 3pG
      fFunc1->SetParameter(2,fW);
      break;
    default:
      cerr << "Warning: fW not needed for function " << fF <<endl;
  }
}

/**  
* \brief Test if the nucleon is within the minimum distance
* \param n number of nucleons
* \param x x coordinate of the nucleon
* \param y y coordinate of the nucleon
* \param z z coordinate of the nucleon
* \return true if the nucleon is within the minimum distance
*/
Bool_t TGlauNucleus::TestMinDist(Int_t n, Double_t x, Double_t y, Double_t z) const
{
  if (fMinDist<=0)
    return kTRUE;
  const Double_t md2 = fMinDist*fMinDist; 
  for (Int_t j = 0; j<n; ++j) {
    TGlauNucleon *other=(TGlauNucleon*)fNucleons->At(j);
    Double_t xo=other->GetX();
    Double_t yo=other->GetY();
    Double_t zo=other->GetZ();
    Double_t dist2 = (x-xo)*(x-xo)+
		                 (y-yo)*(y-yo)+
		                 (z-zo)*(z-zo);
    if (dist2<md2) {
      return kFALSE;
    }
  }
  return kTRUE;
}

/**  
* \brief Throw the nucleons
*
* Distributes the nucleons according to the specified density profiles.
*
* \param xshift shift of the nucleus
* \return the center of mass of the nucleus
*/
TVector3 &TGlauNucleus::ThrowNucleons(Double_t xshift)
{
  AllocateNucleons();
  RandomizeNucleons();

  cmscheck: /* start over here in case shift was too large */

  fTrials = 0;
  fNonSmeared = 0;
  fPhiRot = gRandom->Rndm()*2*TMath::Pi();
  const Double_t cosThetaRot = 2*gRandom->Rndm()-1;
  fThetaRot = TMath::ACos(cosThetaRot);
  fXRot = gRandom->Rndm()*2*TMath::Pi();
  fYRot = gRandom->Rndm()*2*TMath::Pi();
  fZRot = gRandom->Rndm()*2*TMath::Pi();

  const Bool_t hulthen = (fF==3||fF==4);
  
  if (fF==6) { // select configuration read from file
    SelectFromNucArr();
  } else if (fF==99) { // use trnucgen to generate nucleons
#ifdef USE_TRNUCGEN
    static tr::NucleusGenerator ngen;
    ngen.setMinDistance(fMinDist);
    ngen.setRecenter(0); 
    int nuctype = ngen.getNucleusType(GetName());
    if (nuctype == -1) {
      cerr << "ERROR: Failed to generate nucleons for " << GetName() << " with trnucgen; check if it is supported by looking at the trnucgen/nucleusgenerator.h file" << endl;
      gSystem->Exit(123);
    }
    bool res = false;
    int trials = 0;
    while (!res) {
      if (++trials > 100) {
        cerr << "ERROR: Failed to generate nucleons for " << GetName() << " check if the minimal distance (" << fMinDist << " fm) is supported by looking at the trnucgen/nucleusgenerator.cpp file" << endl;
        gSystem->Exit(123);
      }
      res = ngen.generate(GetName());
      fWeight = ngen.getWeight();
      if (TMath::IsNaN(fWeight)) {
        cerr << "Warning: Weight is NaN" << endl;
        res = false;
      }
    }
    fTrials = trials;
    fN = ngen.getNumNucleons();
    fZ = ngen.getNumProtons();
    AllocateNucleons();
    for (Int_t i = 0; i<fN; ++i) {
      TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
      nucleon->Reset();
      nucleon->SetXYZ(ngen.getNucleonPositionX(i),
                      ngen.getNucleonPositionY(i),
                      ngen.getNucleonPositionZ(i));
      if (ngen.getNucleonIsProton(i)) {
        nucleon->SetType(1);
      } else {
        nucleon->SetType(0);
      }
      //rotation is done in the nucleus generator
      //nucleon->RotateXYZ(fPhiRot,fThetaRot);
    }
    SetTitle(ngen.getNucleusDescription().c_str());
#else
    cerr << "ERROR: Should not end here because you do not have libtrnucgen.so" << endl;
#endif
  } else if (fN==1) { //special treatment for proton
    Double_t r = fFunc1->GetRandom();
    Double_t phi = gRandom->Rndm() * 2 * TMath::Pi();
    Double_t ctheta = 2*gRandom->Rndm() - 1;
    Double_t stheta = TMath::Sqrt(1-ctheta*ctheta);
    TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(0));
    nucleon->Reset();
    nucleon->SetXYZ(r * stheta * TMath::Cos(phi),
		                r * stheta * TMath::Sin(phi),
		                r * ctheta);
    fTrials = 1;
  } else if (fN==2 && hulthen) { //special treatment for Hulten
    Double_t r = fFunc1->GetRandom()/2;
    Double_t phi = gRandom->Rndm() * 2 * TMath::Pi();
    Double_t ctheta = 2*gRandom->Rndm() - 1;
    Double_t stheta = TMath::Sqrt(1-ctheta*ctheta);

    TGlauNucleon *nucleon1=(TGlauNucleon*)(fNucleons->At(0));
    TGlauNucleon *nucleon2=(TGlauNucleon*)(fNucleons->At(1));
    nucleon1->Reset();
    nucleon1->SetXYZ(r * stheta * TMath::Cos(phi),
                     r * stheta * TMath::Sin(phi),
                     r * ctheta);
    nucleon2->Reset();
    if (fF==4) { // place opposite of 1
      nucleon2->SetXYZ(-nucleon1->GetX(),
		                   -nucleon1->GetY(),
		                   -nucleon1->GetZ());
    } else {
      r = fFunc1->GetRandom()/2;
      phi = gRandom->Rndm() * 2 * TMath::Pi();
      ctheta = 2*gRandom->Rndm() - 1;
      stheta = TMath::Sqrt(1-ctheta*ctheta);
      nucleon2->SetXYZ(r * stheta * TMath::Cos(phi),
		                   r * stheta * TMath::Sin(phi),
		                   r * ctheta);
    }
    fTrials = 1;
  } else { // all other nuclei 
    const Double_t startingEdge  = 20; // throw nucleons within a cube of this size (fm)
    const Double_t startingEdgeX = startingEdge + fNodeDist*gRandom->Rndm() - 0.5*fNodeDist;
    const Double_t startingEdgeY = startingEdge + fNodeDist*gRandom->Rndm() - 0.5*fNodeDist;
    const Double_t startingEdgeZ = startingEdge + fNodeDist*gRandom->Rndm() - 0.5*fNodeDist;
    const Int_t nslots = 2*startingEdge/fNodeDist+1;
    if (fNodeDist>0) {
      if (fMinDist>fNodeDist) {
        cout << "Minimum distance (nucleon hard core diameter) [" 
          << fMinDist << "] cannot be larger than the nodal spacing of the grid [" 
          << fNodeDist << "]." << endl;
        cout << "Quitting...." << endl;
        gSystem->Exit(123);
      }
      if (!fIsUsed)
        fIsUsed = new TBits(nslots*nslots*nslots);
      else
        fIsUsed->ResetAllBits();
    }
    for (Int_t i = 0; i<fN; ++i) {
      TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
      nucleon->Reset();
      while (1) {
        ++fTrials;
        Bool_t nucleon_inside = 0;
        Double_t x=999, xsmeared=999;
        Double_t y=999, ysmeared=999;
        Double_t z=999, zsmeared=999;
        if (fF==5||fF==7) { // the extended way, throw in a box and test the weight
          while (!nucleon_inside) {
            x = (fMaxR)*(gRandom->Rndm() * 2 - 1);
            y = (fMaxR)*(gRandom->Rndm() * 2 - 1);
            z = (fMaxR)*(gRandom->Rndm() * 2 - 1);
            Double_t r = TMath::Sqrt(x*x+y*y);
            Double_t theta = TMath::ATan2(r,z);
            Double_t R = TMath::Sqrt(x*x+y*y+z*z);
            Double_t Rtheta = fR;
            if (fF==5)
              Rtheta += fBeta2*TMath::Cos(theta)*TMath::Cos(theta);
            else if (fF==7) {
#ifdef HAVE_MATHMORE
              if (fBeta2!=0) {
                if (fGamma!=0) {
                  Double_t phi = TMath::Abs(y)/y*TMath::ACos(x/r);
                  Double_t v2 = TMath::Sqrt(15./64/TMath::Pi())*TMath::Power(TMath::Sin(theta),2)*TMath::Cos(2*phi);
                  Double_t b2 = TMath::Cos(fGamma)*ROOT::Math::sph_legendre(2,0,theta) + TMath::Sin(fGamma)*v2;
                  Rtheta += fR*fBeta2*b2;
                } else {
                  Rtheta += fR*fBeta2*ROOT::Math::sph_legendre(2,0,theta);
                }
              }
              if (fBeta3!=0) {
                Rtheta += fR*fBeta3*ROOT::Math::sph_legendre(3,0,theta);
              }
              if (fBeta4!=0) {
                Rtheta += fR*fBeta4*ROOT::Math::sph_legendre(4,0,theta);
              }
#else
              cerr << "Should not end here because you do not have libMathMore" << endl;
#endif
            }
            Double_t prob = (1+fW*TMath::Power(R/Rtheta,2))/(1+TMath::Exp((R-Rtheta)/fA));
            if (gRandom->Rndm()<prob) 
              nucleon_inside=1;
          }
        } else if ((fF==8) || (fF==14)) { // use TF2
          Double_t r;
          Double_t theta;
          fFunc3->GetRandom2(r,theta);
          Double_t phi = 2*TMath::Pi()*gRandom->Rndm();
          x = r * TMath::Sin(phi) * TMath::Sin(theta);
          y = r * TMath::Cos(phi) * TMath::Sin(theta);
          z = r *                   TMath::Cos(theta);
        } else { // all other types
          TF1 *ff = fFunc1;
          if ((fFunc2) && (nucleon->GetType()==0) && (fF!=90))
            ff = fFunc2;
          if (fNodeDist<=0) { // "continuous" mode
            Double_t r = ff->GetRandom();
            Double_t phi = 2*TMath::Pi()*gRandom->Rndm();
            Double_t ctheta = 2*gRandom->Rndm() - 1 ;
            Double_t stheta = TMath::Sqrt(1-ctheta*ctheta);
            x = r * stheta * TMath::Cos(phi);
            y = r * stheta * TMath::Sin(phi);
            z = r * ctheta;
          } else { // "grid/lattice" mode
            Int_t iNode = Int_t((2*startingEdge/fNodeDist)*gRandom->Rndm());
            Int_t jNode = Int_t((2*startingEdge/fNodeDist)*gRandom->Rndm());
            Int_t kNode = Int_t((2*startingEdge/fNodeDist)*gRandom->Rndm());
            Int_t index=iNode*nslots*nslots+jNode*nslots+kNode;
            if (fIsUsed->TestBitNumber(index))
              continue;
            if (fLattice==1) {       // Primitive cubic system (PCS) -> https://en.wikipedia.org/wiki/Cubic_crystal_system
              x = fNodeDist*(iNode) - startingEdgeX;
              y = fNodeDist*(jNode) - startingEdgeY;
              z = fNodeDist*(kNode) - startingEdgeZ;
            } else if (fLattice==2) { //Body centered cubic (BCC) -> http://mathworld.wolfram.com/CubicClosePacking.html
              x = 0.5*fNodeDist*(-iNode+jNode+kNode) - 0.5*startingEdgeX;
              y = 0.5*fNodeDist*(+iNode-jNode+kNode) - 0.5*startingEdgeY;
              z = 0.5*fNodeDist*(+iNode+jNode-kNode) - 0.5*startingEdgeZ;
            } else if (fLattice==3) { //Face Centered Cubic (FCC) -> http://mathworld.wolfram.com/CubicClosePacking.html
              x = 0.5*fNodeDist*(jNode+kNode) - startingEdgeX;
              y = 0.5*fNodeDist*(iNode+kNode) - startingEdgeY;
              z = 0.5*fNodeDist*(iNode+jNode) - startingEdgeZ;
            } else {                  //Hexagonal close packing (HCP) -> https://en.wikipedia.org/wiki/Close-packing_of_equal_spheres
              x = 0.5*fNodeDist*(2*iNode+((jNode+kNode)%2))          - startingEdgeX;
              y = 0.5*fNodeDist*(TMath::Sqrt(3)*(jNode+(kNode%2)/3)) - startingEdgeY;
              z = 0.5*fNodeDist*(kNode*2*TMath::Sqrt(6)/3)           - startingEdgeZ;
            }
            const Double_t r2 = x*x + y*y + z*z;
            const Double_t r  = TMath::Sqrt(r2);
            if ((r>fMaxR)||(r2*gRandom->Rndm()>ff->Eval(r)))
              continue;
            if (fSmearing>0.0) {
              Int_t nAttemptsToSmear = 0;
              while (1) {
                xsmeared = x*gRandom->Gaus(1.0,fSmearing);
                ysmeared = y*gRandom->Gaus(1.0,fSmearing);
                zsmeared = z*gRandom->Gaus(1.0,fSmearing);
                nAttemptsToSmear++;
                if (TestMinDist(i,xsmeared,ysmeared,zsmeared)) {
                  x = xsmeared;
                  y = ysmeared;
                  z = zsmeared;
                  break;
                }
                if (nAttemptsToSmear>=99) {
                  cerr << "Could not place on this node :: [" << x <<","<< y <<","<< z <<"] r = " << TMath::Sqrt(x*x+y*y+z*z) << " fm; "
                    << "Node (" << iNode << "," << jNode << "," << kNode << ") not smeared !!!" << endl;
                  ++fNonSmeared;
                  break;
                }
              }
            }
            fIsUsed->SetBitNumber(index);
          } /* end "grid/lattice mode" */
        }
        nucleon->SetXYZ(x,y,z);
        if (fF==5||fF==7||fF==8||fF==14) 
          nucleon->RotateXYZ(fPhiRot,fThetaRot); // Uranium etc.
        if (fNodeDist>0) {
          nucleon->RotateXYZ_3D(fXRot,fYRot,fZRot);
          break;
        }
        if (TestMinDist(i,x,y,z))
          break;
      }
    }
  }    

  // calculate center of mass
  Double_t sumx=0;       
  Double_t sumy=0;       
  Double_t sumz=0;       
  for (Int_t i = 0; i<fN; ++i) {
    TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
    sumx += nucleon->GetX();
    sumy += nucleon->GetY();
    sumz += nucleon->GetZ();
  }
  sumx = sumx/fN;
  sumy = sumy/fN;
  sumz = sumz/fN;

  static TVector3 finalShift;
  finalShift.SetXYZ(sumx,sumy,sumz);
  if (finalShift.Mag()>fSmax)
    goto cmscheck;
  
  Double_t fsumx = 0;
  Double_t fsumy = 0;
  Double_t fsumz = 0;
  if (fRecenter==1) {
    fsumx = sumx;
    fsumy = sumy;
    fsumz = sumz;
  } else if (fRecenter==2) {
    TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(fN-1));
    Double_t x = nucleon->GetX() - fN*sumx;
    Double_t y = nucleon->GetY() - fN*sumy;
    Double_t z = nucleon->GetZ() - fN*sumz;
    nucleon->SetXYZ(x,y,z);
  } else if ((fRecenter==3)||(fRecenter==4)) {
    if (finalShift.Mag()>1e-3) {
      TVector3 zVec;
      zVec.SetXYZ(0,0,1);
      TVector3 shiftVec;
      shiftVec.SetXYZ(sumx,sumy,sumz);
      TVector3 orthVec;
      orthVec = shiftVec.Cross(zVec);
      TRotation myRot;
      myRot.Rotate(shiftVec.Angle(zVec),orthVec);
      TVector3 myNuc;
      for (Int_t i = 0; i<fN; ++i) {
        TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
        myNuc.SetXYZ(nucleon->GetX(),nucleon->GetY(),nucleon->GetZ());
        myNuc.Transform(myRot);
        nucleon->SetXYZ(myNuc.X(), myNuc.Y(), myNuc.Z());
      }
      if (fRecenter==3)
        fsumz = shiftVec.Mag();
    }
  } else if (fRecenter==5) {
    fsumx = sumx;
    fsumy = sumy;
  }

  // recenter and shift
  sumx=0;       
  sumy=0;       
  sumz=0;       
  for (Int_t i = 0; i<fN; ++i) {
    TGlauNucleon *nucleon=(TGlauNucleon*)(fNucleons->At(i));
    nucleon->SetXYZ(nucleon->GetX()-fsumx + xshift,
		                nucleon->GetY()-fsumy,
		                nucleon->GetZ()-fsumz);
    sumx += nucleon->GetX();
    sumy += nucleon->GetY();
    sumz += nucleon->GetZ();
  }
  sumx = sumx/fN;
  sumy = sumy/fN;
  sumz = sumz/fN;
  finalShift.SetXYZ(sumx,sumy,sumz);
  return finalShift;
}

/**  /cond */
ClassImp(TGlauberMC)
ClassImp(TGlauberMC::Event)
/**  endcond */
Double_t TGlauberMC::gDefOmega=-1;

/**  
* \brief Constructor for TGlauberMC
*
* Initializes the TGlauberMC object with the given nuclei and cross sections.
* If the value for the cross section is negative, it is interpreted as the collision energy in GeV, and the 
* cross section is calculated using the \ref getSigma and \ref getSigmaHard functions.
* If the value for the fluctuating cross section is positive, it is used to setup fluctuating cross sections.
* If the value for the proton-neutron cross section is negative, it is interpreted as the collision energy in GeV
* and the cross section is calculated using the \ref getSigmaNP function.
* Also sets the omega parameter to the default value (specfied in \ref gDefOmega)
*
* \param NA name of the nucleus A
* \param NB name of the nucleus B
* \param xsect nucleon-nucleon cross section
* \param xsectsigma fluctuating cross section
* \param xsectnp proton-neutron cross section
*/
TGlauberMC::TGlauberMC(const char* NA, const char* NB, Double_t xsect, Double_t xsectsigma, Double_t xsectnp) :
  fANucleus(NA),fBNucleus(NB),
  fXSect(xsect),fXSectNP(xsectnp),fXSectOmega(0),fXSectLambda(0),fXSectEvent(0),
  fNucleonsA(0),fNucleonsB(0),fNucleons(0),
  fAN(0),fBN(0),fNt(0),
  fEvents(0),fTotalEvents(0),fBmin(0),fBmax(20),fHardFrac(0.65),
  fDetail(99),fCalcArea(0),fCalcLength(0), fDoCore(0), fDoAAGG(1),
  fSigH(129.4), fShadow(0), fOmega(gDefOmega),
  fMaxNpartFound(0),f2Cx(0),fPTot(0),fNNProf(0),
  fEv()
{
  if (xsect<0) { // negative cross section, means energy is provided
    Double_t energy = -xsect;
    fSigH = GetSigmaHard(energy);
    if (energy < 10) { // low energy, use Bystricky parameterization
      fXSect = GetSigmaPP_Bystricky(energy);
      if (xsectnp<0)
        fXSectNP = GetSigmaNP_Bystricky(energy);
      cout << "Using sigma_NN=" << fXSect << " mb and sigma_NP=" << fXSectNP << " mb for energy=" << energy << " GeV" << endl;
    } else {
      fXSect = GetSigmaNN(energy);
      fXSectNP = 0;
      cout << "Using sigma_NN=" << fXSect << " mb and sigmaHard= " << fSigH << " mb for energy=" << energy << " GeV" << endl;
    }
  }
  if (xsectsigma>0) {
    fXSectOmega = xsectsigma;
    fXSectLambda = 1;
    fPTot = new TF1("fPTot","((x/[2])/(x/[2]+[0]))*exp(-(((x/[2])/[0]-1 )**2)/([1]*[1]))/[2]",0,300);
    fPTot->SetParameters(fXSect,fXSectOmega,fXSectLambda);
    fPTot->SetNpx(1000);
    fXSectLambda = fXSect/fPTot->GetHistogram()->GetMean();
    fPTot->SetParameters(fXSect,fXSectOmega,fXSectLambda);
    Double_t mean = fPTot->GetHistogram()->GetMean();
    cout << "Using fluctuating cross section with <sigma>=" << mean << "mb, using fXSectOmega=" << fXSectOmega << " and lambda=" << fXSectLambda << endl;
  }

  TString name(Form("Glauber_%s_%s",fANucleus.GetName(),fBNucleus.GetName()));
  TString title(Form("Glauber %s+%s Version %s",fANucleus.GetName(),fBNucleus.GetName(),Version()));
  SetName(name);
  SetTitle(title);
}

/**  
* \brief Calculate the next event
* \param bgen impact parameter
* \return true if the event is calculated successfully
*/
Bool_t TGlauberMC::CalcEvent(Double_t bgen)
{
  // calc next event
  if (!fNucleonsA) {
    fNucleonsA = fANucleus.GetNucleons();
    fAN = fANucleus.GetN();
    for (Int_t i = 0; i<fAN; ++i) {
      TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(i));
      nucleonA->SetInNucleusA();
    }
  }

  if (!fNucleonsB) {
    fNucleonsB = fBNucleus.GetNucleons();
    fBN = fBNucleus.GetN();
    for (Int_t i = 0; i<fBN; ++i) {
      TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
      nucleonB->SetInNucleusB();
    }
  }

  Double_t xsecA[999] = {0};
  Double_t xsecB[999] = {0};
  if (fPTot) {
    fXSectEvent = fPTot->GetRandom();
    if (fDoAAGG) {
      for (Int_t i = 0; i<fAN; ++i)
	      xsecA[i] = fPTot->GetRandom();
      for (Int_t i = 0; i<fBN; ++i)
	      xsecB[i] = fPTot->GetRandom();
    }
  } else {
    fXSectEvent = fXSect;
  }
  if (fOmega>0 && !fNNProf) {
    if (fOmega<2) {
      cout << "Setting up NN profile for omega=" << fOmega << endl;
      fNNProf = GetNNProf(fXSect, fOmega);
    } else if (fOmega==7) {
      cout << "Setting up NN profile for HIJING (omega==7)" << endl;
      fNNProf = GetNNHijing(fXSect);
    } else if (fOmega==8) {
      cout << "Setting up NN profile for PYTHIA (omega==8)" << endl;
      fNNProf = GetNNPythia(fXSect);
    } else if ((fOmega>=9)&&(fOmega<11)) {
      Double_t w = fOmega-9;
      cout << "Setting up NN profile for Trento (w=" << w << ")" << endl;
      fNNProf = GetNNTrento(fXSect,w);
    } else {
      cerr << "Value of omega is not supported: " << fOmega << endl;
      gSystem->Exit(1);
    }
  } else if (fOmega<0) {
    cout << "Using hard-sphere approximation (default);\n use SetOmega(omega) or SetDefOmega(omega) in rootlogon.C if you want NN overlap" << endl;
    fOmega = 0;
  }

  // "ball" diameter = distance at which two balls interact
  Double_t d2pp = (Double_t)fXSectEvent/(TMath::Pi()*10); // in fm^2
  Double_t d2np = (Double_t)fXSectNP/(TMath::Pi()*10);    // in fm^2
  Double_t d2   = d2pp;
  Double_t bh   = TMath::Sqrt(d2*fHardFrac);
  if (fNNProf) {
    Double_t xmin=0,xmax=0;
    fNNProf->GetRange(xmin,xmax);
    d2 = xmax*xmax;
  }

  Double_t sigS = fXSectEvent; // 57 mb for HIJING;
  //const Double_t b02  = 0.5 * sigS * 0.1 / TMath::Pi();
  Double_t sigHS = fSigH;
  if (fShadow) {
    const Double_t rrb   = TMath::Min(1., bgen * bgen / 35.2 / 1.44);
    const Double_t aphx  = 0.1 * 4./3. * 4.92 * TMath::Sqrt(1. - rrb);
    sigHS = fSigH - aphx * 103.65;
  }

  fEv.Reset();
  memset(fBC,0,sizeof(Bool_t)*999*999);
  Int_t nc=0,nh=0,njet=0;
  memset(fMPI,0,sizeof(Int_t)*99);
  for (Int_t i = 0; i<fBN; ++i) {
    TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
    Bool_t tB=nucleonB->GetType();
    for (Int_t j = 0; j<fAN; ++j) {
      TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(j));
      Double_t dx = nucleonB->GetX()-nucleonA->GetX();
      Double_t dy = nucleonB->GetY()-nucleonA->GetY();
      Double_t dij = dx*dx+dy*dy;
      Bool_t tA=nucleonA->GetType();
      if (fXSectNP>0) {
        if (tA!=tB) d2 = d2np;
        else        d2 = d2pp;
      } else if (fDoAAGG && fPTot){
        d2 = 0.5*(xsecA[j]+xsecB[i])/(TMath::Pi()*10);
      }
      if (dij>d2) 
        continue;
      Double_t bij = TMath::Sqrt(dij);
      if (fNNProf) {
        Double_t val = fNNProf->Eval(bij);
        Double_t ran = gRandom->Uniform();
        if (ran>val)
          continue;
        //from HIJING
        const Double_t ts = 2. *val;
        const Double_t es = TMath::Exp(-ts);
        const Double_t tt = ts * sigHS/sigS;
        const Double_t et = TMath::Exp(-tt);
        if (ran<et*(1-es)) {
          fMPI[0]++;
          continue;
        }
        Double_t xr = - TMath::Log(et + gRandom->Rndm()*(1.-et));
        Int_t nj = 0;
        while (1) {
          ++nj;
          xr -= TMath::Log(gRandom->Rndm());
          if (xr > tt)
            break;
        }
        if (nj>99) {
          cerr << "ERROR: nj is greater than 99: " << nj << endl;
          gSystem->Exit(123);
        }
        fMPI[nj]++;
        njet += nj;
      }
      fEv.Nmpi = njet;
      nucleonB->Collide();
      nucleonA->Collide();
      fBC[i][j] = 1;
      fEv.BNN  += bij;
      ++nc;
      if (bij<bh)
        ++nh;
      if (tA!=tB)
        ++fEv.Ncollpn;
      else if (tA==1)
        ++fEv.Ncollpp;
      else
        ++fEv.Ncollnn;
      if (nc==1) {
        fEv.X0 = (nucleonA->GetX()+nucleonB->GetX())/2;
        fEv.Y0 = (nucleonA->GetY()+nucleonB->GetY())/2;
      }
    }
  }
  fEv.B = bgen;
  Double_t w1 = fANucleus.GetWeight();
  Double_t w2 = fBNucleus.GetWeight();
  if (w1==0) w1 = 1;
  if (w2==0) w2 = 1;
  Double_t w = w1*w2;
  if (TMath::Abs(w)>1e6) {
    cout << "Warning: Weight is too large: " << w << endl;
    cout << "w1 = " << w1 << ", w2 = " << w2 << endl;
    cout << "fANucleus.GetName() = " << fANucleus.GetName() << endl;
    cout << "fBNucleus.GetName() = " << fBNucleus.GetName() << endl;
    gSystem->Exit(1);
  }
  fTotalEvents += w;
  if (nc>0) {
    fEvents += w;
    fEv.Weight    = w;
    fEv.Ncoll     = nc;
    fEv.Nhard     = nh;
    fEv.BNN      /= nc;
    return CalcResults(bgen);
  }
  return kFALSE;
}

/**  
* \brief Calculate the results for the given event
* \param bgen impact parameter
* \return true if the results are calculated successfully
*/
Bool_t TGlauberMC::CalcResults(Double_t bgen)
{
  // calc results for the given event
  Double_t sumW=0;
  Double_t sumWA=0;
  Double_t sumWB=0;

  Double_t sinphi[10] = {0};
  Double_t cosphi[10] = {0};
  Double_t rn[10]     = {0};

  const Int_t kNc = fDoCore; // used later for core/corona

  for (Int_t i = 0; i<fAN; ++i) {
    TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(i));
    Double_t xA=nucleonA->GetX();
    Double_t yA=nucleonA->GetY();
    fEv.MeanXSystem  += xA;
    fEv.MeanYSystem  += yA;
    fEv.MeanXA  += xA;
    fEv.MeanYA  += yA;
    if (nucleonA->IsWounded()) {
      Double_t w = nucleonA->Get2CWeight(f2Cx);
      ++fEv.Npart;
      if (nucleonA->IsNeutron())
        ++fEv.NpartAn;
      if (nucleonA->GetNColl()==1) {
	      ++fEv.Npart0;
        if (nucleonA->IsNeutron())
          ++fEv.Npart0n;
      }
      ++fEv.NpartA;
      sumW   += w;
      fEv.MeanX  += xA * w;
      fEv.MeanY  += yA * w;
      fEv.MeanX2 += xA * xA * w;
      fEv.MeanY2 += yA * yA * w;
      fEv.MeanXY += xA * yA * w;
    } else { // count spectator neutrons
      if (nucleonA->IsNeutron())
        ++fEv.SpecA;
    }
  }

  for (Int_t i = 0; i<fBN; ++i) {
    TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
    Double_t xB=nucleonB->GetX();
    Double_t yB=nucleonB->GetY();
    fEv.MeanXSystem  += xB;
    fEv.MeanYSystem  += yB;
    fEv.MeanXB  += xB;
    fEv.MeanYB  += yB;
    if (nucleonB->IsWounded()) {
      Double_t w = nucleonB->Get2CWeight(f2Cx);
      ++fEv.Npart;
      if (nucleonB->IsNeutron())
        ++fEv.NpartBn;
      if (nucleonB->GetNColl()==1) {
	      ++fEv.Npart0;
        if (nucleonB->IsNeutron())
          ++fEv.Npart0n;
      }
      ++fEv.NpartB;
      sumW   += w;
      sumWB  += w;
      fEv.MeanX  += xB * w;
      fEv.MeanY  += yB * w;
      fEv.MeanX2 += xB * xB * w;
      fEv.MeanY2 += yB * yB * w;
      fEv.MeanXY += xB * yB * w;
    } else { // count spectator neutrons
      if (nucleonB->IsNeutron())
        ++fEv.SpecB;
    }
  }
  if (fEv.Npart>0) {
    fEv.MeanX  /= sumW;
    fEv.MeanY  /= sumW;
    fEv.MeanX2 /= sumW;
    fEv.MeanY2 /= sumW;
    fEv.MeanXY /= sumW;
  } else {
    fEv.MeanX = 0;
    fEv.MeanY  = 0;
    fEv.MeanX2 = 0;
    fEv.MeanY2 = 0;
    fEv.MeanXY = 0;
  }

  if (fAN+fBN>0) {
    fEv.MeanXSystem /= (fAN + fBN);
    fEv.MeanYSystem /= (fAN + fBN);
  } else {
    fEv.MeanXSystem = 0;
    fEv.MeanYSystem = 0;
  }
  if (sumWA>0) {
    fEv.MeanXA /= sumWA;
    fEv.MeanYA /= sumWA;
  } else {
    fEv.MeanXA = 0;
    fEv.MeanYA = 0;
  }
  if (sumWB>0) {
    fEv.MeanXB /= sumWB;
    fEv.MeanYB /= sumWB;
  } else {
    fEv.MeanXB = 0;
    fEv.MeanYB = 0;
  }

  fEv.VarX  = fEv.MeanX2-(fEv.MeanX*fEv.MeanX);
  fEv.VarY  = fEv.MeanY2-(fEv.MeanY*fEv.MeanY);
  fEv.VarXY = fEv.MeanXY-fEv.MeanX*fEv.MeanY;
  Double_t tmpa = fEv.VarX*fEv.VarY-fEv.VarXY*fEv.VarXY;
  if (tmpa<0) 
    fEv.AreaW = -1;
  else 
    fEv.AreaW = TMath::Sqrt(tmpa);

  if (fEv.Npart>0) {
    // do full moments relative to meanX and meanY
    for (Int_t n = 1; n<10; ++n) {
      for (Int_t ia = 0; ia<fAN; ++ia) {
        TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(ia));
        if (nucleonA->GetNColl()<=kNc) 
          continue;
        Double_t xA=nucleonA->GetX() - fEv.MeanX;
        Double_t yA=nucleonA->GetY() - fEv.MeanY;
        Double_t r = TMath::Sqrt(xA*xA+yA*yA);
        Double_t phi = TMath::ATan2(yA,xA);
        Double_t w = n;
        if (n==1) 
          w = 3; // use r^3 weighting for Ecc1/Psi1
        Double_t rw = TMath::Power(r,w);
        cosphi[n] += rw*TMath::Cos(n*phi);
        sinphi[n] += rw*TMath::Sin(n*phi);
        rn[n] += rw;
      }
      for (Int_t ib = 0; ib<fBN; ++ib) {
        TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(ib));
        if (nucleonB->GetNColl()<=kNc) 
          continue;
        Double_t xB=nucleonB->GetX() - fEv.MeanX;
        Double_t yB=nucleonB->GetY() - fEv.MeanY;
        Double_t r = TMath::Sqrt(xB*xB+yB*yB);
        Double_t phi = TMath::ATan2(yB,xB);
        Double_t w = n;
        if (n==1)
          w = 3; // use r^3 weighting for Ecc1/Psi1
        Double_t rw = TMath::Power(r,w);
        cosphi[n] += rw*TMath::Cos(n*phi);
        sinphi[n] += rw*TMath::Sin(n*phi);
        rn[n] += rw;
      }
      cosphi[n] /= fEv.Npart;
      sinphi[n] /= fEv.Npart;
      rn[n] /= fEv.Npart;
      if (rn[n]>0) {
	      fPsiN[n] = (TMath::ATan2(sinphi[n],cosphi[n]) + TMath::Pi())/n;
	      fEccN[n] = TMath::Sqrt(sinphi[n]*sinphi[n]+cosphi[n]*cosphi[n])/rn[n];
      } else {
	      fPsiN[n] = -1;
	      fEccN[n] = -1;
      }
    }
    if (!kNc) { //silly test but useful to catch errors 
      Double_t t=TMath::Sqrt(TMath::Power(fEv.VarY-fEv.VarX,2)+4.*fEv.VarXY*fEv.VarXY)/(fEv.VarY+fEv.VarX)/fEccN[2];
      if (t<0.99||t>1.01)
        cout << "Warning: Expected t=1 but found t=" << t << endl;
    }
  }

  fEv.B      = bgen;
  fEv.PhiA   = fANucleus.GetPhiRot();
  fEv.ThetaA = fANucleus.GetThetaRot();
  fEv.PhiB   = fBNucleus.GetPhiRot();
  fEv.ThetaB = fBNucleus.GetThetaRot();
  fEv.Psi1   = fPsiN[1];
  fEv.Ecc1   = fEccN[1];
  fEv.Psi2   = fPsiN[2];
  fEv.Ecc2   = fEccN[2];
  fEv.Psi3   = fPsiN[3];
  fEv.Ecc3   = fEccN[3];
  fEv.Psi4   = fPsiN[4];
  fEv.Ecc4   = fEccN[4];
  fEv.Psi5   = fPsiN[5];
  fEv.Ecc5   = fEccN[5];

  if (fCalcArea) {
    const Int_t nbins=200;
    const Double_t ell=10;
    const Double_t da=2*ell*2*ell/nbins/nbins;
    const Double_t d2 = (Double_t)fXSectEvent/(TMath::Pi()*10); // in fm^2
    const Double_t r2 = d2/4.;
    const Double_t mx = fEv.MeanX;
    const Double_t my = fEv.MeanY;
    TH2D areaA("hAreaA",";x (fm);y (fm)",nbins,-ell,ell,nbins,-ell,ell);
    TH2D areaB("hAreaB",";x (fm);y (fm)",nbins,-ell,ell,nbins,-ell,ell);
    for (Int_t i = 0; i<fAN; ++i) {
      TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(i));
      if (!nucleonA->IsWounded())
        continue;
      if (nucleonA->GetNColl()==kNc)
        continue;
      Double_t x = nucleonA->GetX()-mx;
      Double_t y = nucleonA->GetY()-my;
      for (Int_t xi=1; xi<=nbins; ++xi) {
        for (Int_t yi=1; yi<=nbins; ++yi) {
          Int_t bin = areaA.GetBin(xi,yi);
          Double_t val=areaA.GetBinContent(bin);
          if (val>0)
            continue;
          Double_t dx=x-areaA.GetXaxis()->GetBinCenter(xi);
          Double_t dy=y-areaA.GetYaxis()->GetBinCenter(yi);
          if (dx*dx+dy*dy<r2)
            areaA.SetBinContent(bin,1);
        }
      }
    }
    for (Int_t i = 0; i<fBN; ++i) {
      TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
      if (!nucleonB->IsWounded())
        continue;
      if (nucleonB->GetNColl()==kNc)
        continue;
      Double_t x = nucleonB->GetX()-mx;
      Double_t y = nucleonB->GetY()-my;
      for (Int_t xi=1; xi<=nbins; ++xi) {
        for (Int_t yi=1; yi<=nbins; ++yi) {
          Int_t bin = areaB.GetBin(xi,yi);
          Double_t val=areaB.GetBinContent(bin);
          if (val>0)
            continue;
          Double_t dx=x-areaB.GetXaxis()->GetBinCenter(xi);
          Double_t dy=y-areaB.GetYaxis()->GetBinCenter(yi);
          if (dx*dx+dy*dy<r2)
            areaB.SetBinContent(bin,1);
        }
      }
    }
    Double_t overlap1=0;
    Double_t overlap2=0;
    for (Int_t xi=1; xi<=nbins; ++xi) {
      for (Int_t yi=1; yi<=nbins; ++yi) {
        Int_t bin = areaA.GetBin(xi,yi);
        Double_t vA=areaA.GetBinContent(bin);
        Double_t vB=areaB.GetBinContent(bin);
        if (vA>0&&vB>0)
          ++overlap1;
        if (vA>0||vB>0)
          ++overlap2;
      }
    }
    fEv.AreaO = overlap1*da;
    fEv.AreaA = overlap2*da;
  }

  if (fCalcLength) {
    const Double_t krhs = TMath::Sqrt(fXSectEvent/40./TMath::Pi());
    const Double_t ksg  = krhs/TMath::Sqrt(5);
    const Double_t kDL  = 0.1;
    TF1 rad("rad","2*pi/[0]/[0]*TMath::Exp(-x*x/(2.*[0]*[0]))",0.0,5*ksg); 
    rad.SetParameter(0,ksg);
    const Double_t minval = rad.Eval(5*ksg);
    fEv.Phi0         = gRandom->Uniform(0,TMath::TwoPi());
    Double_t kcphi0  = TMath::Cos(fEv.Phi0);
    Double_t ksphi0  = TMath::Sin(fEv.Phi0);
    Double_t x       = fEv.X0;
    Double_t y       = fEv.Y0;
    Double_t i0a     = 0;
    Double_t i1a     = 0;
    Double_t l       = 0;
    Double_t val     = CalcDens(rad,x,y);
    while (val>minval) {
      x     += kDL * kcphi0;
      y     += kDL * ksphi0;
      i0a   += val;
      i1a   += l*val;
      l+=kDL;
      val    = CalcDens(rad,x,y);
    }
    fEv.Length = 2*i1a/i0a;
  }

  if (fEv.Npart > fMaxNpartFound) 
    fMaxNpartFound = fEv.Npart;

  return kTRUE;
}

/**  
* \brief Calculate the density at the given point
* \param prof profile function
* \param xval x coordinate
* \param yval y coordinate
* \return the density at the given point
*/
Double_t TGlauberMC::CalcDens(TF1 &prof, Double_t xval, Double_t yval) const
{
  Double_t rmin=0,rmax=0;
  prof.GetRange(rmin,rmax);
  Double_t r2max = rmax*rmax;
  Double_t ret = 0;
  for (Int_t i = 0; i<fAN; ++i) {
    TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(i));
    if (!nucleonA->IsWounded())
      continue;
    Double_t x = nucleonA->GetX();
    Double_t y = nucleonA->GetY();
    Double_t r2=(xval-x)*(xval-x)+(yval-y)*(yval-y);
    if (r2>r2max)
      continue;
    ret += prof.Eval(TMath::Sqrt(r2));
  }
  for (Int_t i = 0; i<fBN; ++i) {
    TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
    if (!nucleonB->IsWounded())
      continue;
    Double_t x = nucleonB->GetX();
    Double_t y = nucleonB->GetY();
    Double_t r2=(xval-x)*(xval-x)+(yval-y)*(yval-y);
    if (r2>r2max)
      continue;
    ret += prof.Eval(TMath::Sqrt(r2));
  }
  return ret;
}

/**  
* \brief Draw the colliding nuclei separated by the impact parameter
* \param option option string
*/
void TGlauberMC::Draw(Option_t* option)
{
  static TH2F *h2f = new TH2F("hGlauberMC",";x (fm);y(fm)",1,-18,18,1,-12,12);
  h2f->Reset();
  h2f->SetStats(0);
  h2f->Draw();
  TEllipse e;
  e.SetFillColor(0);
  e.SetFillStyle(0);
  e.SetLineColor(1);
  e.SetLineStyle(2);
  e.SetLineWidth(1);
  e.DrawEllipse(GetB()/2,0,fBNucleus.GetR(),fBNucleus.GetR(),0,360,0);
  e.DrawEllipse(-GetB()/2,0,fANucleus.GetR(),fANucleus.GetR(),0,360,0);
  fANucleus.Draw(fXSect, kMagenta, kYellow);
  fBNucleus.Draw(fXSect, kMagenta, kOrange);
  TString opt(option);
  if (opt.IsNull())
    return;
  Double_t sy2 = GetSy2();
  Double_t sx2 = GetSx2();
  Double_t phase = 0;
  if (sy2<sx2) {
    Double_t d = sx2;
    sx2 = sy2;
    sy2 = d;
    phase = TMath::Pi()/2.;
  }
  Double_t x1 = (0.5*(sy2-sx2)+TMath::Sqrt(TMath::Power(sy2-sx2,2.)-4*TMath::Power(GetSxy(),2)));
  Double_t ang = TMath::ATan2(-GetSxy(),x1)+phase;
  TLine l;
  l.SetLineWidth(3);
  l.DrawLine(-10*TMath::Cos(ang),-10*TMath::Sin(ang),10*TMath::Cos(ang),10*TMath::Sin(ang));
}

/// Get the total cross section
Double_t TGlauberMC::GetTotXSect() const
{
  return (1.*fEvents/fTotalEvents)*TMath::Pi()*fBmax*fBmax/100;
}

/// Get the statistical error on the total cross section
Double_t TGlauberMC::GetTotXSectErr() const
{
  return GetTotXSect()/TMath::Sqrt((Double_t)fEvents) * TMath::Sqrt(Double_t(1.-fEvents/fTotalEvents));
}

/// Get a TObjArray of the nucleons
TObjArray *TGlauberMC::GetNucleons() 
{
  if (!fNucleonsA || !fNucleonsB) return 0;
  if (fNucleons) return fNucleons;

  fNucleonsA->SetOwner(0);
  fNucleonsB->SetOwner(0);
  TObjArray *allnucleons=new TObjArray(fAN+fBN);
  allnucleons->SetOwner();
  for (Int_t i = 0; i<fAN; ++i) {
    allnucleons->Add(fNucleonsA->At(i));
  }
  for (Int_t i = 0; i<fBN; ++i) {
    allnucleons->Add(fNucleonsB->At(i));
  }
  fNucleons = allnucleons;
  return allnucleons;
}

/**  
* \brief Calculate the next event
* \param bgen impact parameter
* \return true if the event is calculated successfully
*/
Bool_t TGlauberMC::NextEvent(Double_t bgen)
{
  if (bgen<0) 
    bgen = TMath::Sqrt((fBmax*fBmax-fBmin*fBmin)*gRandom->Rndm()+fBmin*fBmin);
  fANucleus.ThrowNucleons(-bgen/2.);
  fBNucleus.ThrowNucleons(bgen/2.);
  return CalcEvent(bgen);
}

/**  
* \brief Read the next event from a file
* When the file name is specified, the function is initialized. 
* The next calls should no longer specify the file name.
* \param calc calculate the event
* \param fname file name
* \return true if the event is read successfully
*/
Bool_t TGlauberMC::ReadNextEvent(Bool_t calc, const char *fname)
{
  static TFile *inf = 0;
  static Int_t iev  = 0;
  if (fname) {
    cout << "ReadNextEvent: Setting up file " << fname << endl;
    delete inf;
    inf = TFile::Open(fname);
    if (!inf) 
      return 0;
    if (!fNucleonsA) {
      fANucleus.ThrowNucleons();
      fNucleonsA = fANucleus.GetNucleons();
      fAN = fANucleus.GetN();
      for (Int_t i = 0; i<fAN; ++i) {
	      TGlauNucleon *nucleonA=(TGlauNucleon*)(fNucleonsA->At(i));
	      nucleonA->SetInNucleusA();
      }
    }
    if (!fNucleonsB) {
      fBNucleus.ThrowNucleons();
      fNucleonsB = fBNucleus.GetNucleons();
      fBN = fBNucleus.GetN();
      for (Int_t i = 0; i<fBN; ++i) {
	      TGlauNucleon *nucleonB=(TGlauNucleon*)(fNucleonsB->At(i));
	      nucleonB->SetInNucleusB();
      }
    }
    if (calc)
      return 1;
    fNt = dynamic_cast<TNtuple*>(inf->Get(Form("nt_%s_%s",fANucleus.GetName(),fBNucleus.GetName())));
    if (!fNt) {
      cerr << "ReadNextEvent: Could not find ntuple!" << endl;
      inf->ls();
      return 0;
    }
    fNt->SetBranchAddress("Npart",&fEv.Npart);
    fNt->SetBranchAddress("Ncoll",&fEv.Ncoll);
    fNt->SetBranchAddress("B",&fEv.B);
    fNt->SetBranchAddress("BNN",&fEv.BNN);
    fNt->SetBranchAddress("VarX",&fEv.VarX);
    fNt->SetBranchAddress("VarY",&fEv.VarY);
    fNt->SetBranchAddress("VarXY",&fEv.VarXY);
    fNt->SetBranchAddress("NpartA",&fEv.NpartA);
    fNt->SetBranchAddress("NpartB",&fEv.NpartB);
    fNt->SetBranchAddress("Npart0",&fEv.Npart0);
    fNt->SetBranchAddress("Psi1",&fEv.Psi1);
    fNt->SetBranchAddress("Ecc1",&fEv.Ecc1);
    fNt->SetBranchAddress("Psi2",&fEv.Psi2);
    fNt->SetBranchAddress("Ecc2",&fEv.Ecc2);
    fNt->SetBranchAddress("Psi3",&fEv.Psi3);
    fNt->SetBranchAddress("Ecc3",&fEv.Ecc3);
    fNt->SetBranchAddress("Psi4",&fEv.Psi4);
    fNt->SetBranchAddress("Ecc4",&fEv.Ecc4);
    fNt->SetBranchAddress("Psi5",&fEv.Psi5);
    fNt->SetBranchAddress("Ecc5",&fEv.Ecc5);
    return 1;
  }
  if ((!inf)||(!fNt&&!calc)) {
    cerr << "ReadNextEvent was not initialized" <<endl;
    return 0;
  }
  TObjArray *arr = dynamic_cast<TObjArray*>(inf->Get(Form("nucleonarray%d",iev)));
  if (!arr) {
    if (iev==0) {
      cerr << "ReadNextEvent could not read nucleon array for event " << iev << endl;
      return 0;
    }
    iev = 0;
    cerr << "ReadNextEvent resetting to first event" << endl;
    arr = dynamic_cast<TObjArray*>(inf->Get(Form("nucleonarray%d",iev)));
  }
  Double_t bgenA=0, bgenB=0;
  Int_t inA=0, inB=0;
  const Int_t nNucls = arr->GetEntries();
  for (Int_t iNucl=0; iNucl<nNucls; ++iNucl) {
    TGlauNucleon *nuclinp = static_cast<TGlauNucleon*>(arr->At(iNucl));
    TGlauNucleon *nuclout = 0;
    if (nuclinp->IsInNucleusB()) { 
      nuclout = static_cast<TGlauNucleon*>(fNucleonsB->At(inB));
      bgenB += nuclinp->GetX();
      ++inB;
    } else {
      nuclout = static_cast<TGlauNucleon*>(fNucleonsA->At(inA));
      bgenA += nuclinp->GetX();
      ++inA;
    }
    nuclout->Reset();
    nuclout->SetXYZ(nuclinp->GetX(),nuclinp->GetY(),nuclinp->GetZ());
    nuclout->SetType(nuclinp->GetType());
    nuclout->SetEnergy(nuclinp->GetEnergy());
    if (!calc)
      nuclout->SetNColl(nuclinp->GetNColl());
  }
  delete arr;
  Double_t bgen = bgenB/inB-bgenA/inA;
  if (calc) {
    Bool_t ret = CalcEvent(bgen);
    if (0) 
      cout << iev << ": " << fEv.B << " " << fEv.Npart << " " << fEv.Ncoll << " " << fEv.Npart0 << endl;
    ++iev;
    return ret;
  }
  Int_t ret = fNt->GetEntry(iev);
  if (ret<=0) 
    return 0;
  fEccN[1]=fEv.Ecc1;
  fEccN[2]=fEv.Ecc2;
  fEccN[3]=fEv.Ecc3;
  fEccN[4]=fEv.Ecc4;
  fEccN[5]=fEv.Ecc5;
  if (0) 
    cout << iev << ": " << fEv.B << " " << fEv.Npart << " " << fEv.Ncoll << " " << fEv.Npart0 << endl;
  if (0) { // test ntuple values vs re-calculated values
    Double_t npart = fEv.Npart;
    Double_t ncoll = fEv.Ncoll;
    Double_t ecc2  = fEv.Ecc2;
    CalcEvent(bgen);
    if (npart!=fEv.Npart) 
      cout << iev << " differ in npart " << npart << " " << fEv.Npart << endl;
    if (ncoll!=fEv.Ncoll) 
      cout << iev << " differ in ncoll " << ncoll << " " << fEv.Ncoll << endl;
    if (TMath::Abs(ecc2-fEv.Ecc2)>0.001) 
      cout << iev << " differ in ecc2 " << ecc2 << " " << fEv.Ecc2 << endl;
  }
  ++iev;
  return 1;
}

/**  
* \brief Run the Glauber model and store the results in an TNtuple
*
* If the TNtuple is not already created, it is created with the necessary branches
* according to the \ref fDetail level.
*
* \param nevents number of events
* \param b impact parameter
*/
void TGlauberMC::Run(Int_t nevents, Double_t b)
{
  if (fNt == 0) {
    TString name(Form("nt_%s_%s",fANucleus.GetName(),fBNucleus.GetName()));
    TString vars("Npart:Ncoll:Nhard:Nmpi:B:BNN:Ncollpp:Ncollpn:Ncollnn:VarX:VarY:VarXY:NpartA:NpartB:Npart0:NpartAn:NpartBn:Npart0n:AreaW:SpecA:SpecB:Weight");
    if (fDetail>1)
      vars+=":Psi1:Ecc1:Psi2:Ecc2:Psi3:Ecc3:Psi4:Ecc4:Psi5:Ecc5";
    if (fDetail>2)
      vars+=":AreaO:AreaA:X0:Y0:Phi0:Length";
    if (fDetail>3)
      vars+=":MeanX:MeanY:MeanX2:MeanY2:MeanXY:MeanXSystem:MeanYSystem:MeanXA:MeanYA:MeanXB:MeanYB";
    if (fDetail>4)
      vars+=":PhiA:ThetaA:PhiB:ThetaB";
    fNt = new TNtuple(name,name,vars);
    fNt->SetDirectory(0);
    TObjArray *l = fNt->GetListOfBranches();
    for (Int_t i=0; i<l->GetEntries(); ++i) {
      TBranch *br = dynamic_cast<TBranch*>(l->At(i));
      if (br)
        br->SetCompressionLevel(9);
    }
  }
  for (Int_t i = 0; i<nevents; ++i) {
    while (!NextEvent(b)) {}
    fNt->Fill((Float_t*)(&fEv.Npart));
    if ((i>0)&&(i%100)==0) 
      cout << "Event # " << i << " x-sect = " << GetTotXSect() << " +- " << GetTotXSectErr() << " b        \r" << flush;
  }
  TString title(Form("%s+%s, x-sect=%.1f mb, str=%s, totx-sect=%.2f mb",fANucleus.GetName(),fBNucleus.GetName(),fXSect,Str(),GetTotXSect()*1e3));
  fNt->SetTitle(title);
  if (nevents>99)
    cout << endl << "Done!" << endl;
}

/**  
* \brief Set the omega parameter
* If the omega parameter is negative or zero, the hard-sphere approximation is used.
* Otherwise, the following distributions are supported:
* - 0-2: Gamma distribution
* - 7: HIJING
* - 8: PYTHIA
* - 9-11: TRENTO (where the value is translated into d=omega-9)
*
* \param omega omega parameter
*/
void TGlauberMC::SetOmega(Double_t omega)
{
  if (omega<0) {
    cout << "Using hard-sphere approximation (default)" << endl;
    cout << " If you want NN overlap, use SetOmega(o) where " << endl;
    cout << "  - Gamma dist (0<o<2)" << endl;
    cout << "  - o=7: HIJING" << endl;
    cout << "   -o=8: PYTHIA" << endl;
    cout << "   -Trento (9<o<11), translates into d=o-9" << endl;
    fOmega = 0;
    return;
  }
  fOmega = omega;
}

/// Get the string representation of the TGlauberMC object
const char *TGlauberMC::Str() const {
  return Form("TGlauberMC%s_%s%s-snn%.1f-md%.1f-om%.1f-nd%.1f-rc%d-smax%.1f",Version(),fANucleus.GetName(),fBNucleus.GetName(),fXSect,fBNucleus.GetMinDist(),fOmega,fBNucleus.GetNodeDist(),fBNucleus.GetRecenter(),fBNucleus.GetShiftMax());
}

/**
 * @brief Get the nucleon-nucleon interaction probability based on Gamma distribution
 * 
 * This function returns a TF1 object representing the nucleon-nucleon interaction probability
 * based on the Gamma distribution. It is used to determine the probability
 * of a collision between two nucleons as a function of their impact parameter,
 * taken from https://arxiv.org/abs/1307.0636
 * 
 * @param snn Nucleon-nucleon cross section in mb
 * @param omega Omega parameter for the NN profile (0-2, default is 0.3)
 * @param G G parameter for the NN profile (default is 1)
 * @return TF1* Nucleon-nucleon collision profile function
 */
TF1 *TGlauberMC::GetNNProf(Double_t snn, Double_t omega, Double_t G) 
{ 
  if ((omega<0) || (omega>2))
    return 0;
  Double_t R2 = snn/10./TMath::Pi();
  TF1 *nnprof = new TF1("nnprofgamma","[2]*(1-TMath::Gamma([0],[1]*x^2))",0,5);
  nnprof->SetParameters(1./omega,G/omega/R2,G);
  return nnprof;
}

/**
  * @brief Get the nucleon-nucleon impact parameter distribution based on Gamma distribution
  * 
  * This function returns a TF1 object representing the nucleon-nucleon impact parameter distribution
  * based on the radial Gamma distribution, taken from https://arxiv.org/abs/1307.0636
  * 
  * @param snn Nucleon-nucleon cross section in mb
  * @param omega Omega parameter for the NN profile (0-2, default is 0.3)
  * @param G G parameter for the NN profile (default is 1)
  * @return TF1* Nucleon-nucleon collision profile distribution function
 */
TF1 *TGlauberMC::GetNNProfDist(Double_t snn, Double_t omega, Double_t G) 
{ 
  if ((omega<0) || (omega>2))
    return 0;
  Double_t R2 = snn/10./TMath::Pi();
  TF1 *nnprof = new TF1("nnprofgammadist","[2]*x*(1-TMath::Gamma([0],[1]*x^2))",0,5);
  nnprof->SetParameters(1./omega,G/omega/R2,G*TMath::TwoPi());
  return nnprof;
}

/**
  * @brief Get the HIJING-based nucleon-nucleon interaction probability
  * 
  * This function returns a TF1 object representing the nucleon-nucleon interaction probability
  * based on the HIJING model. It is used to determine the probability
  * of a collision between two nucleons as a function of their impact parameter.
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param mu Mu parameter for the profile (default is 3.9 from HIJING)
  * @return TF1* HIJING profile function
  */
TF1 *TGlauberMC::GetNNHijing(Double_t signn, Double_t mu) 
{
  Double_t b02  = 0.5 * signn * 0.1 / TMath::Pi(); //sigS=57mb
  b02 = TMath::Sqrt(b02);
  TF1* eik = new TF1("eik", "1-exp(-[2]*[0]^2/[1]*([0]*x)^3*TMath::BesselK(3,[0]*x))", 0, 10);
  eik->SetParameter(0, mu/b02);
  eik->SetParameter(1, 96);
  TF1 *dummy=GetNNHijingDist(signn,mu);
  eik->SetParameter(2,dummy->GetParameter(2));
  delete dummy;
  eik->SetLineWidth(4);
  eik->SetLineStyle(9);
  eik->SetLineColor(1);
  return eik;
}

/**
  * @brief Get the HIJING-based nucleon-nucleon impact parameter distribution
  * 
  * This function returns a TF1 object representing the nucleon-nucleon radial 
  * impact parameter distribution based on the HIJING model. 
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param mu Mu parameter for the profile (default is 3.9 from HIJING)
  * @return TF1* HIJING profile distribution function
  */
TF1 *TGlauberMC::GetNNHijingDist(Double_t signn, Double_t mu) 
{
  Double_t b02  = 0.5 * signn * 0.1 / TMath::Pi(); //sigS=57mb
  b02 = TMath::Sqrt(b02);
  TF1* eik = new TF1("eikdist", "2*pi*x*(1-exp(-[2]*[0]^2/[1]*([0]*x)^3*TMath::BesselK(3,[0]*x)))", 0, 10);
  eik->SetParameter(0, mu/b02);
  eik->SetParameter(1, 96);
  Double_t k=1;
  while (1) {
    eik->SetParameter(2,k);
    Double_t val=eik->Integral(0,5);
    Double_t diff=val-signn*0.1;
    if (diff<-0.01)
      k*=1.01;
    else if (diff>0.01)
      k*=0.99;
    else break;
  }
  eik->SetLineWidth(4);
  eik->SetLineStyle(9);
  eik->SetLineColor(1);
  return eik;
}

/**
  * @brief Get the PYTHIA-based nucleon-nucleon interaction probability
  * 
  * This function returns a TF1 object representing the nucleon-nucleon interaction probability
  * based on the PYTHIA model. It is used to determine the probability
  * of a collision between two nucleons as a function of their impact parameter.
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param m M parameter for the profile (default is 1.85 from PYTHIA)
  * @param rp Rp parameter for the profile (default is 1 from PYTHIA)
  * @return TF1* PYTHIA profile function
  */
TF1 *TGlauberMC::GetNNPythia(Double_t signn, Double_t m, Double_t rp) 
{
  TF1* pyt = new TF1("pyt", "1-exp(-[2]*TMath::Exp(-TMath::Power(x/[1],[0])))",0,10);
  pyt->SetParameters(m,rp,1); //m=1.85 Monash tune
  TF1 *dummy=GetNNPythiaDist(signn,m);
  pyt->SetParameter(2,dummy->GetParameter(2));
  delete dummy;
  pyt->SetLineWidth(4);
  pyt->SetLineStyle(5);
  pyt->SetLineColor(1);
  return pyt;
}

/**
  * @brief Get the PYTHIA-based nucleon-nucleon impact parameter distribution
  * 
  * This function returns a TF1 object representing the nucleon-nucleon radial
  * impact parameter distribution based on the PYTHIA model.
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param m M parameter for the profile (default is 1.85 from PYTHIA)
  * @param rp Rp parameter for the profile (default is 1 from PYTHIA)
  * @return TF1* PYTHIA profile distribution function
  */
TF1 *TGlauberMC::GetNNPythiaDist(Double_t signn, Double_t m, Double_t rp) 
{
  TF1* pyt = new TF1("pytdist", "2*pi*x*(1-exp(-[2]*TMath::Exp(-TMath::Power(x/[1],[0]))))",0,10);
  pyt->SetParameters(m,rp,1); //m=1.85 Monash tune
  Double_t k=1;
  while (1) {
    pyt->SetParameter(2,k);
    Double_t val=pyt->Integral(0,5);
    Double_t diff=val-signn*0.1;
    if (diff<-0.01)
      k*=1.01;
    else if (diff>0.01)
      k*=0.99;
    else break;
  }
  pyt->SetLineWidth(4);
  pyt->SetLineStyle(5);
  pyt->SetLineColor(1);
  return pyt;
}

/**
  * @brief Get the TRENTO-based nucleon-nucleon interaction probability
  * 
  * This function returns a TF1 object representing the nucleon-nucleon interaction probability
  * based on the TRENTO model. It is used to determine the probability
  * of a collision between two nucleons as a function of their impact parameter.
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param w W parameter for the profile (default is 0.5 from TRENTO)
  * @return TF1* TRENTO profile function
  */
TF1 *TGlauberMC::GetNNTrento(Double_t signn, Double_t w) 
{
  TF1* t = new TF1("trento","1-exp(-[0]*exp(-x^2/4/[1]^2))",0,6);
  t->SetParameters(1,w); 
  TF1 *dummy=GetNNTrentoDist(signn,w);
  t->SetParameter(0,dummy->GetParameter(0));
  delete dummy;
  t->SetLineWidth(4);
  t->SetLineStyle(9);
  t->SetLineColor(1);
  return t;
}

/**
  * @brief Get the TRENTO-based nucleon-nucleon impact parameter distribution
  * 
  * This function returns a TF1 object representing the nucleon-nucleon radial
  * impact parameter distribution based on the TRENTO model.
  * 
  * @param signn Nucleon-nucleon cross section in mb
  * @param w W parameter for the profile (default is 0.5 from TRENTO)
  * @return TF1* TRENTO profile distribution function
  */
TF1 *TGlauberMC::GetNNTrentoDist(Double_t signn, Double_t w) 
{
  const Double_t maxb = 6;
  TF1* t = new TF1("trentodist","2*pi*x*(1-exp(-[0]*exp(-x^2/4/[1]^2)))",0,maxb);
  t->SetParameters(1,w); 
  Double_t k=1;
  while (1) {
    t->SetParameter(0,k);
    Double_t val=t->Integral(0,maxb);
    Double_t diff=val-signn*0.1;
    if (diff<-0.01)
      k*=1.01;
    else if (diff>0.01)
      k*=0.99;
    else break;
  }
  t->SetLineWidth(4);
  t->SetLineStyle(9);
  t->SetLineColor(1);
  return t;
}

/**
  * @brief Get the nucleon-nucleon cross section vs energy function
  * 
  * This function returns a TGraph object representing the nucleon-nucleon cross section
  * vs energy. The parameterization is valid for energies from 10 GeV to 100 TeV
  * (from https://arxiv.org/abs/2011.14909).
  * 
  * @return TF1* Nucleon-nucleon cross section vs energy
  */
TF1 *TGlauberMC::GetSigmaNNvsEnergy()
{
  TF1 *lnNs = new TF1("lnNs","[0]+[1]*pow(log(x*x),[2])",5,100000, TF1::EAddToList::kDefault);
  lnNs->SetFillColor(19);
  lnNs->SetFillStyle(0);
  lnNs->SetLineColor(2);
  lnNs->SetLineWidth(3);
  lnNs->SetChisquare(17.60074);
  lnNs->SetNDF(24);
  lnNs->GetXaxis()->SetLabelFont(42);
  lnNs->GetXaxis()->SetTitleOffset(1);
  lnNs->GetXaxis()->SetTitleFont(42);
  lnNs->GetYaxis()->SetLabelFont(42);
  lnNs->GetYaxis()->SetTitleFont(42);
  lnNs->SetParameter(0,28.84374);
  lnNs->SetParError(0,0.5163632);
  lnNs->SetParLimits(0,0,0);
  lnNs->SetParameter(1,0.04584121);
  lnNs->SetParError(1,0.0167335);
  lnNs->SetParLimits(1,0,0);
  lnNs->SetParameter(2,2.374257);
  lnNs->SetParError(2,0.1231245);
  lnNs->SetParLimits(2,1,4);
  return lnNs;
}

/**
  * @brief Get the nucleon-nucleon cross section at a given energy
  * 
  * This function returns the nucleon-nucleon cross section at a given collision energy
  * based on a parameterized fit. The parameterization is valid for energies from 10 GeV to 100 TeV
  * (from https://arxiv.org/abs/2011.14909).
  * 
  * @param energy Collision energy in GeV
  * @return Double_t Nucleon-nucleon cross section in mb
  */
Double_t TGlauberMC::GetSigmaNN(Double_t energy)
{
  if (energy < 10) {
    cerr << "TGlauberMC::GetSigmaNN: energy out of range (10-100000 GeV): " << energy << endl;
    return 0.;
  }
  if (energy < 100) {
    cerr << "TGlauberMC::GetSigmaNN: warning, provided result may not be accuruate at energies lower than 100 GeV: " << energy << endl;
  }
  TF1 *lnNs1 = GetSigmaNNvsEnergy();
  Double_t sigma = lnNs1->Eval(energy);
  delete lnNs1;
  return sigma;
}

/**
  * @brief Get the neutron-proton cross section vs energy function
  * 
  * This function returns a TGraph object representing the neutron-proton cross section
  * vs energy. The parameterization is valid for energies from 0.3 GeV to 4.2 GeV
  * (from https://inspirehep.net/literature/244855).
  * 
  * @return TGraph* Neutron-proton cross section vs energy
  */
TGraph *TGlauberMC::GetSigmaNPvsEnergy_Bystricky()
{
  //Tkin (GeV)  sigma_inel(np) (mb) Error (mb)
  // from Bystricky_JPhysique48  Table VI
  const Double_t x[] = {
    0.300, 0.325, 0.350, 0.375, 0.400, 0.425, 0.450, 0.475, 0.500, 0.525, 0.550,
    0.575, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00, 1.05, 1.10,
    1.15, 1.20, 1.25, 1.30, 1.35, 1.40, 1.45, 1.50, 1.60, 1.70, 1.80, 1.90,
    2.00, 2.10, 2.20, 2.30, 2.40, 2.50, 2.60, 2.70, 2.80, 2.90, 3.00, 3.20,
    3.40, 3.60, 3.80, 4.00, 4.20
  };
  const Double_t y[] = {
    0.003, 0.046, 0.175, 0.419, 0.783, 1.26, 1.84, 2.50, 3.23, 4.00, 4.81,
    5.64, 6.48, 8.15, 9.77, 11.31, 12.76, 14.10, 15.33, 16.46, 17.48, 18.42,
    19.26, 20.03, 20.73, 21.36, 21.93, 22.44, 22.91, 23.34, 23.72, 24.39,
    24.95, 25.42, 25.81, 26.14, 26.43, 26.68, 26.90, 27.10, 27.28, 27.45,
    27.60, 27.75, 27.89, 28.03, 28.30, 28.57, 28.84, 29.13, 29.42, 29.73
  };
  const Double_t yerr[] = {
    0.001, 0.002, 0.005, 0.007, 0.009, 0.010, 0.013, 0.015, 0.018, 0.022, 0.025,
    0.029, 0.033, 0.041, 0.049, 0.058, 0.069, 0.080, 0.092, 0.11, 0.12, 0.13,
    0.15, 0.16, 0.17, 0.19, 0.20, 0.21, 0.22, 0.23, 0.24, 0.26, 0.28, 0.30,
    0.31, 0.33, 0.34, 0.36, 0.37, 0.39, 0.41, 0.43, 0.45, 0.47, 0.49, 0.52,
    0.57, 0.63, 0.69, 0.76, 0.83, 0.90
  };
  TGraphErrors *gr = new TGraphErrors(sizeof(x)/sizeof(x[0]),x,y,0,yerr);
  gr->SetName("sigma_NP_Bystricky");
  return gr;
}

/**
  * @brief Get the neutron-proton cross section at a given energy
  * 
  * This function returns the neutron-proton cross section at a given collision energy
  * based on the Bystricky parameterization. This is particularly useful for low-energy
  * collisions (below 10 GeV) relevant for HADES/CBM experiments.
  * 
  * @param energy Collision energy in GeV (valid range: 0.3-4.2 GeV)
  * @return Double_t Neutron-proton cross section in mb
  */
Double_t TGlauberMC::GetSigmaNP_Bystricky(Double_t energy)
{
  if ((energy < 0.3) || (energy > 4.2)) {
    cerr << "getSigmaNP_Bystricky: energy out of range (0.3-4.2 GeV): " << energy << endl;
    return 0.;
  }
  TGraph *gr = GetSigmaNPvsEnergy_Bystricky();
  Double_t sigma = gr->Eval(energy);
  delete gr;
  return sigma;
}

/**
  * @brief Get the proton-proton cross section vs energy function
  * 
  * This function returns a TGraph object representing the proton-proton cross section
  * vs energy. The parameterization is valid for energies from 0.28 GeV to 425 GeV
  * (from https://inspirehep.net/literature/244855).
  * 
  * @return TGraph* Proton-proton cross section vs energy
  */
TGraph *TGlauberMC::GetSigmaPPvsEnergy_Bystricky()
{
  //Tkin (GeV)  sigma_inel(pp) (mb) Error (mb)
  //from Bystricky_JPhysique48  Table VI
  const Double_t x[] = {
    0.28, 0.29, 0.30, 0.31, 0.32, 0.34, 0.36, 0.38, 0.40, 0.42, 0.44, 0.46, 0.48, 0.50, 0.52, 0.56, 0.60, 0.64, 0.68, 0.72, 0.76, 0.80, 0.84, 0.88, 0.92, 0.96, 1.00, 1.04, 1.08, 1.12, 1.16, 1.20, 1.24, 1.28, 1.32, 1.36, 1.40, 1.44, 1.48, 1.52, 1.56, 1.64, 1.68, 1.72, 1.76, 1.80, 1.84, 1.88, 1.92, 1.96, 2.00, 2.10, 2.20, 2.30, 2.40, 2.50, 2.60, 2.70, 2.80, 2.90, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 25.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 125.0, 150.0, 175.0, 200.0, 225.0, 250.0, 275.0, 300.0, 325.0, 350.0, 375.0, 400.0, 425.0
  };
  const Double_t y[] = {
    0.027, 0.051, 0.082, 0.120, 0.171, 0.321, 0.563, 0.921, 1.41, 2.05, 2.83, 3.74, 4.76, 5.87, 7.04, 9.48, 11.89, 14.14, 16.15, 17.89, 19.34, 20.53, 21.51, 22.33, 23.03, 23.81, 24.39, 24.89, 25.30, 25.66, 25.95, 26.21, 26.42, 26.59, 26.74, 26.86, 26.96, 27.04, 27.11, 27.17, 27.21, 27.28, 27.30, 27.32, 27.33, 27.34, 27.35, 27.36, 27.36, 27.36, 27.36, 27.37, 27.37, 27.38, 27.38, 27.40, 27.41, 27.43, 27.46, 27.49, 27.52, 27.97, 28.48, 28.92, 29.28, 29.55, 29.75, 29.89, 30.04, 30.10, 30.09, 30.05, 30.00, 29.88, 29.80, 29.78, 29.91, 30.13, 30.38, 30.66, 30.93, 31.19, 31.78, 32.23, 32.56, 32.79, 32.92, 32.97, 32.96, 32.89, 32.78, 32.63, 32.44, 32.24, 32.01
  };
  TGraph *gr = new TGraph(sizeof(x)/sizeof(x[0]),x,y);
  gr->SetName("sigma_PP _Bystricky");
  return gr;
}

/**
  * @brief Get the proton-proton cross section at a given energy
  * 
  * This function returns the proton-proton cross section at a given collision energy
  * based on the Bystricky parameterization. This is particularly useful for low-energy
  * collisions (below 10 GeV) relevant for HADES/CBM experiments.
  * 
  * @param energy Collision energy in GeV (valid range: 0.28-425 GeV)
  * @return Double_t Proton-proton cross section in mb
  */
Double_t TGlauberMC::GetSigmaPP_Bystricky(Double_t energy)
{
  if ((energy < 0.28) || (energy > 425.)) {
    cerr << "TGlauberMC::GetSigmaPP_Bystricky: energy out of range (0.28-425 GeV): " << energy << endl;
    return 0.;
  }
  TGraph *gr = GetSigmaPPvsEnergy_Bystricky();
  Double_t sigma = gr->Eval(energy);
  delete gr;
  return sigma;
}

/**
  * @brief Get the hard-scattering cross section vs energy function
  * 
  * This function returns a TGraph object representing the hard-scattering cross section
  * vs energy obtained from HIJING.
  * 
  * @return TGraph* Hard-scattering cross section vs energy
  */
TGraph *TGlauberMC::GetSigmaHardvsEnergy()
{
  const Double_t x[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
  const Double_t y[] = {4.41642385E-03, 0.188464478, 0.649059832, 1.25692534, 1.92962086, 2.63335800, 3.34105325, 4.05063772, 4.73708200, 5.42901564, 11.6562014, 16.9493980, 21.6152058, 25.8751488, 29.7382126, 33.4025536, 36.8270454, 40.0877686, 43.2026596, 69.1051025, 89.9556122, 107.744072, 124.087372, 138.940948, 153.014984, 166.308075, 178.939285, 190.986938, 294.216583, 379.379303, 455.911163, 525.982910, 592.784119, 656.306885, 717.265808, 774.954590, 830.319702};
  TGraph *gr = new TGraph(sizeof(x)/sizeof(x[0]),x,y);
  gr->SetFillColor(19);
  gr->SetFillStyle(0);
  gr->SetLineColor(2);
  gr->SetLineWidth(3);
  gr->SetName("sigma_hard_HIJING");
  return gr;
} 

/**
  * @brief Get the hard-scattering cross section at a given energy
  * 
  * This function returns the hard-scattering cross section at a given collision energy
  * based on the HIJING model.
  * 
  * @param energy Collision energy in GeV (valid range: 10-100 TeV)
  * @return Double_t Hard-scattering cross section in mb
  */
Double_t TGlauberMC::GetSigmaHard(Double_t energy)
{
  if (energy < 10)
    return 0;
  if (energy>100000) {
    cerr << "TGlauberMC::GetSigmaHard: warning provided value may not be accurate as energy out of range (10-100000 GeV): " << energy << endl;
  }
  TGraph *gr = GetSigmaHardvsEnergy();
  Double_t sigma = gr->Eval(energy);
  delete gr;
  return sigma;
}

/** 
 * @brief Fill the area with the nucleon-nucleon interaction probability
 * 
 * This function fills the area with a weight given by the weight function wf.
 * 
 * @param area Area to fill
 * @param nnprof NN profile function
 * @param nuc Nucleus
 * @param wf Weight function
 * @param shift Shift of the nucleus
 * @param ell Ellipse size
 * @param gmax Maximum distance from the nucleus
 * @param maxweight Maximum weight
 */
void TGlauberMC::FillArea(TH2D *area, const TF1 *nnprof, const TGlauNucleus *nuc, TF1* wf, const Double_t shift, const Double_t ell, const Double_t gmax, const Double_t maxweight)
{
  const TObjArray *nucleons = nuc->GetNucleons();
  const Int_t N            = nuc->GetN();
  for (Int_t i = 0; i<N; ++i) {
    TGlauNucleon *nucleon=(TGlauNucleon*)(nucleons->At(i));
    if (!nucleon->IsWounded())
      continue;
    Double_t x0=nucleon->GetX()+shift;
    Double_t y0=nucleon->GetY();
    Double_t x1 = TMath::Max(x0-gmax,-ell);
    Double_t x2 = TMath::Min(x0+gmax,ell);
    Double_t y1 = TMath::Max(y0-gmax,-ell);
    Double_t y2 = TMath::Min(y0+gmax,ell);
    Int_t binx1 = area->GetXaxis()->FindBin(x1);
    Int_t binx2 = area->GetXaxis()->FindBin(x2);
    Int_t biny1 = area->GetYaxis()->FindBin(y1);
    Int_t biny2 = area->GetYaxis()->FindBin(y2);
    Double_t wfval = 1;
    if (wf)
      wfval = wf->GetRandom();
    for (Int_t ix = binx1; ix <= binx2; ++ix) {
      const Double_t xCenter = area->GetXaxis()->GetBinCenter(ix);
      const Double_t dx = xCenter-x0;
      const Double_t dx2=dx*dx;
      for (Int_t iy = biny1; iy <= biny2; ++iy) {
        const Double_t yCenter = area->GetYaxis()->GetBinCenter(iy);
        const Double_t dy = yCenter-y0;
        const Double_t dy2=dy*dy;
        const Double_t weight = nnprof->Eval(TMath::Sqrt(dx2+dy2))*wfval;
        if (weight<maxweight)
          continue;
        Int_t bin = area->GetBin(ix, iy);
        area->AddBinContent(bin, weight);
      }
    }
  }
}

/**
  * @brief Calculate the mean of the overlap area
  * 
  * This function calculates the generalized mean of two areas according to the Trento model.
  * 
  * @param A Area of the first nucleus
  * @param B Area of the second nucleus
  * @param p Trento or power parameter
  * @return Double_t generalized m ean of the overlap area
  */
Double_t TGlauberMC::TrentoGenMean(const Double_t A, const Double_t B, const Double_t p)
{
  if (p==0)
    return TMath::Sqrt(A*B);
  Double_t ap = TMath::Power(A,p);
  Double_t bp = TMath::Power(B,p);
  Double_t ret = 0.5*(ap+bp);
  return TMath::Power(ret,1./p);
}

/**
  * @brief Calculate the eccentricities from the overlap area
  * 
  * This function calculates the eccentricities from the overlap area.
  * The eccentricities are calculated for n=1 to 9 and stored in the array
  * at positions 1 to 9. The 0th position is set to 0.
  * 
  * @param overlap Overlap area
  * @param ecc Eccentricities
  * @param psi Phis
  */
void TGlauberMC::EccFromOverlap(const TH2D *overlap, Double_t ecc[10], Double_t psi[10])
{
  const Int_t nx = overlap->GetNbinsX();
  const Int_t ny = overlap->GetNbinsY();
  Double_t mx=0,my=0,su=0;
  for (Int_t ix = 1; ix<=nx; ++ix) {
    const Double_t x = overlap->GetXaxis()->GetBinCenter(ix);
    for (Int_t iy = 1; iy<=ny; ++iy) {
      const Double_t y = overlap->GetYaxis()->GetBinCenter(iy);
      const Double_t s = overlap->GetBinContent(ix,iy);
      mx += x*s;
      my += y*s;
      su += s;
    }
  }
  mx /= su;
  my /= su;
  Double_t sinphi[10] = {0};
  Double_t cosphi[10] = {0};
  Double_t rn[10]     = {0};
  for (Int_t ix = 1; ix<=nx; ++ix) {
    const Double_t x = overlap->GetXaxis()->GetBinCenter(ix)-mx;
    for (Int_t iy = 1; iy<=ny; ++iy) {
      const Double_t y = overlap->GetYaxis()->GetBinCenter(iy)-my;
      const Double_t s = overlap->GetBinContent(ix,iy);
      Double_t r = TMath::Sqrt(x*x+y*y);
      Double_t phi = TMath::ATan2(y,x);
      for (Int_t n = 1; n<=9; ++n) {
        Double_t w = n;
        if (n==1)
          w = 3; // use r^3 weighting for Ecc1/Psi1
        Double_t rw = TMath::Power(r,w);
        cosphi[n] += rw*TMath::Cos(n*phi)*s;
        sinphi[n] += rw*TMath::Sin(n*phi)*s;
        rn[n] += rw*s;
      }
    }
  }
  ecc[0] = 0;
  psi[0] = 0;
  for (Int_t n = 1; n<=9; ++n) {
    ecc[n] = TMath::Sqrt(sinphi[n]*sinphi[n]+cosphi[n]*cosphi[n])/rn[n];
    psi[n] = (TMath::ATan2(sinphi[n],cosphi[n]) + TMath::Pi())/n;
  }
}

/**
  * @brief Read a ntuple from a file
  * 
  * This function reads a ntuple from a file. If a name is provided, the function
  * reads the ntuple with the given name. If no name is provided, the function
  * reads the first ntuple in the file.
  * 
  * @param fname File name
  * @param name Ntuple name
  * @return TNtuple* Ntuple
  */
TNtuple *TGlauberMC::ReadNtuple(const char *fname, const char *name)
{
  TFile *inf = TFile::Open(fname);
  if (!inf)
    return 0;
  if (name) {
    TNtuple *nt = dynamic_cast<TNtuple*>(inf->Get(name));
    if (!nt) {
      delete inf;
      return 0;
    }
    return nt;
  }
  Int_t nkeys = inf->GetListOfKeys()->GetEntries();
  for (Int_t i = 0; i<nkeys; ++i) {
    TKey *key = (TKey*)inf->GetListOfKeys()->At(i);
    TNtuple *nt = (TNtuple*)key->ReadObj();
    if (nt) {
      return nt;
    }
  }
  delete inf;
  return 0;
}

/**
  * @brief Get the centrality distribution from a ntuple
  * 
  * This function gets the centrality distribution from a ntuple.
  * The centrality distribution is calculated for the given branch name.
  * 
  * @param nt Ntuple
  * @param bname Branch name
  * @return TGraph* Centrality distribution
  */
TGraph *TGlauberMC::GetCentralityDist(TNtuple *nt, const char *bname)
{
  TH1D *h = new TH1D("h","h",100000,0,25);
  nt->Draw(Form("%s>>h",bname),"","goff");
  TGraph *ret = new TGraph;
  ret->SetName(Form("cen_%s",nt->GetName()));
  ret->SetPoint(0,0,0);
  Double_t counts = 0;
  Double_t norm = h->GetEntries();
  for (Int_t bin=1;bin<=h->GetNbinsX();++bin)
  {
      counts += h->GetBinContent(bin);
      ret->SetPoint(ret->GetN(),h->GetBinCenter(bin),counts/norm);
  }
  delete h;
  return ret;
}

/**
 * @brief Add a centrality branch to a ntuple from a file
 * 
 * This function adds a centrality branch to a ntuple from a file.
 * The centrality branch is calculated for the given branch name.
 * 
 * @param fin Input file name
 * @param fout Output file name
 * @param tname Ntuple name
 * @param bname Branch name
 * @param cname Centrality branch name
 * 
 * @return void
 */
void TGlauberMC::AddCentralitBranch(const char *fin, const char *fout, const char *tname, const char *bname, const char *cname)
{
  TNtuple *nt = ReadNtuple(fin, tname);
  if (!nt)
    return;
  TGraph *gcen=GetCentralityDist(nt, bname);
  if (!gcen)
    return;
  TFile *fileout= new TFile(fout, "UPDATE");
  if (!fileout)
    return;
  fileout->SetCompressionLevel(9);
  TTree *nt2 = nt->CloneTree();
  nt2->SetDirectory(fileout);
  //delete nt;
  //delete fin;
  Float_t bcen = 0;
  Float_t b = 0;
  TBranch *bcent = nt2 ->Branch(cname, &bcen, Form("%s/F",cname));
  nt2->SetBranchAddress(bname,&b);
  for (Int_t i=0;i<nt2->GetEntries();i++) {
    nt2->GetEntry(i);
    bcen = gcen->Eval(b)*100;
    //cout << "bin " << i << " b " << b << " bcen " << bcen << endl;
    bcent->Fill();
  }
  nt2->Write();
  fileout->Close();
  delete fileout;
  //delete fin;
}
#endif /*_runglauber_*/
