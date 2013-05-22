//////////////////////////////////////////////////////////////////////////////////////////
//
// name: AAComputation.cc
// date: 22 May 13
// auth: Zach Hartwig
//
// desc: 
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __AAComputation_hh__
#define __AAComputation_hh__ 1

#include <TObject.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TBox.h>
#include <TSpectrum.h>
#include <TLine.h>
#include <TFile.h>
#include <TTree.h>
#include <TRandom.h>
#include <TColor.h>
#include <TGProgressBar.h>
#include <TF1.h>

#include <string>
#include <vector>
using namespace std;

#include "ADAQRootGUIClasses.hh"
#include "ADAQAnalysisSettings.hh"
#include "ADAQAnalysisParallelResults.hh"
#include "ADAQAnalysisTypes.hh"

#ifndef __CINT__
#include <boost/array.hpp>
#endif



class AAComputation : public TObject
{
public:
  AAComputation(string, bool);
  ~AAComputation();

  static AAComputation *GetInstance();

  // Pointer set methods
  void SetProgressBarPointer(TGHProgressBar *PB) { ProcessingProgressBar = PB; }
  void SetADAQSettings(ADAQAnalysisSettings *AAS) { ADAQSettings = AAS; }

  
  ///////////////////////////
  // Data computation methods
  
  // File I/O methods
  bool LoadADAQRootFile(string);
  bool LoadACRONYMRootFile(string);
  bool SaveHistogramData(string, string, string);
  void CreateDesplicedFile();

  // Waveform creation
  TH1F *CalculateRawWaveform(int, int);
  TH1F *CalculateBSWaveform(int, int, bool CurrentWaveform=false);
  TH1F *CalculateZSWaveform(int, int, bool CurrentWaveform=false);
  double CalculateBaseline(vector<int> *);  
  double CalculateBaseline(TH1F *);

  // Waveform processing 
  bool FindPeaks(TH1F *);
  void FindPeakLimits(TH1F *);
  void IntegratePeaks();
  void FindPeakHeights();
  void RejectPileup(TH1F *);
  void IntegratePearsonWaveform(bool PlotPearsonIntegration=true);
  void CalculateCountRate();

  // Spectrum creation
  void CreateSpectrum();
  void CalculateSpectrumBackground();

  // Spectrum processing
  void FindSpectrumPeaks();
  void IntegrateSpectrum();
  TGraph *CalculateSpectrumDerivative();

  // Spectrum calibration
  bool SetCalibrationPoint(int, int, double, double);
  bool SetCalibration(int);
  bool ClearCalibration(int);

  // Pulse shape discrimination processing
  TH2F *CreatePSDHistogram();
  void CalculatePSDIntegrals(bool);
  bool ApplyPSDFilter(double, double);
  void CreatePSDFilter(int, int);
  void ClearPSDFilter(int);
  void CreatePSDHistogramSlice(int, int);

  // Processing methods
  void UpdateProcessingProgress(int);
  void ProcessWaveformsInParallel(string);
  double* SumDoubleArrayToMaster(double*, size_t);
  double SumDoublesToMaster(double);


  ////////////////////////////////////////
  // Public access methods for member data

  // Waveform peak data
  vector<PeakInfoStruct> GetPeakInfoVec() {return PeakInfoVec;}

  // Spectra
  TH1F *GetSpectrum() {return Spectrum_H;}
  TH1F *GetSpectrumBackground() {return SpectrumBackground_H;}
  TH1F *GetSpectrumWithoutBackground() {return SpectrumDeconvolved_H;}

  // Spectra calibrations
  vector<TGraph *> GetSpectraCalibrations() { return SpectraCalibrations; }
  vector<bool> GetUseSpectraCalibrations() { return UseSpectraCalibrations; }
  
  // Spectra analysis
  TH1F *GetSpectrumIntegral() { return SpectrumIntegral_H; }
  TF1 *GetSpectrumFit() { return SpectrumFit_F; }
  double GetSpectrumIntegralValue() { return SpectrumIntegralValue; }
  double GetSpectrumIntegralError() { return SpectrumIntegralError; }

  // Pulse shape discrimination histograms
  TH2F *GetPSDHistogram() { return PSDHistogram_H; }
  TH1D *GetPSDHistogramSlice() { return PSDHistogramSlice_H; }

  // Pulse shape discrimination filters
  vector<TGraph *> GetPSDFilters() { return PSDFilters; }
  vector<bool> GetUsePSDFilters() { return UsePSDFilters; }

  // ADAQ file data
  string GetADAQFileName() { return ADAQFileName; }
  int GetADAQNumberOfWaveforms() {return ADAQWaveformTree->GetEntries();}
  ADAQRootMeasParams *GetADAQMeasurementParameters() {return ADAQMeasParams;}

  // ACRONYM file data
  string GetACRONYMFileName() { return ACRONYMFileName; }

  // Booleans
  bool GetADAQFileLoaded() { return ADAQFileLoaded; }
  bool GetACRONYMFileLoaded() { return ACRONYMFileLoaded; }
  bool GetSpectrumExists() { return SpectrumExists; }
  bool GetSpectrumDerivativeExists() { return SpectrumDerivativeExists; }
  bool GetPSDHistogramExists() { return PSDHistogramExists; }
  bool GetPSDHIstogramSliceExists() { return PSDHistogramSliceExists; }


  ////////////////
  // Miscellaneous

  void CreateNewPeakFinder(int NumPeaks){
    if(PeakFinder) delete PeakFinder;
    PeakFinder = new TSpectrum(NumPeaks);
  }
  
  
private:

  static AAComputation *TheComputationManager;

  TGHProgressBar *ProcessingProgressBar;
  ADAQAnalysisSettings *ADAQSettings;

  // Bools to specify architecture type
  bool SequentialArchitecture, ParallelArchitecture;


  ///////////
  // File I/O

  ADAQRootMeasParams *ADAQMeasParams;
  TTree *ADAQWaveformTree;
  TFile *ADAQRootFile, *ACRONYMRootFile;
  string ADAQFileName, ACRONYMFileName;
  bool ADAQFileLoaded, ACRONYMFileLoaded;
  ADAQAnalysisParallelResults *ADAQParResults;
  bool ADAQParResultsLoaded;


  //////////////////////
  // Waveforms variables

  TH1F *Waveform_H[8];

  // Readout for ADAQ waveforms 
  vector<int> *WaveformVecPtrs[8];
  vector<int> Time, RawVoltage;
  int RecordLength;

  // Peak finding machinery
  TSpectrum *PeakFinder;
  int NumPeaks;
  vector<PeakInfoStruct> PeakInfoVec;
  vector<int> PeakIntegral_LowerLimit, PeakIntegral_UpperLimit;
#ifndef __CINT__
  vector< boost::array<int,2> > PeakLimits;
#endif

  // Waveform processing range
  int WaveformStart, WaveformEnd;
  

  /////////////////////
  // Spectrum variables

  TH1F *Spectrum_H;
  TH1F *SpectrumDerivative_H;
  TGraph *SpectrumDerivative_G;
  TH1F *SpectrumBackground_H, *SpectrumDeconvolved_H;
  TH1F *SpectrumIntegral_H;
  TF1 *SpectrumFit_F;
  double SpectrumIntegralValue, SpectrumIntegralError;

  
  // Objects that are used in energy calibration of the pulse spectra
  vector<TGraph *> SpectraCalibrations;
  vector<bool> UseSpectraCalibrations;
  vector<ADAQChannelCalibrationData> CalibrationData;


  ////////////////
  // PSD variables


  // Variables for PSD histograms and filter
  TH2F *PSDHistogram_H, *MasterPSDHistogram_H;
  TH1D *PSDHistogramSlice_H;

  double PSDFilterPolarity;
  vector<TGraph *> PSDFilters;
  vector<bool> UsePSDFilters;
  int PSDNumFilterPoints;
  vector<int> PSDFilterXPoints, PSDFilterYPoints;


  ///////////
  // Booleans
  bool SpectrumExists, SpectrumDerivativeExists;
  bool PSDHistogramExists, PSDHistogramSliceExists;


  ///////////
  // Parallel

  int MPI_Size, MPI_Rank;
  bool IsMaster, IsSlave;

    // Strings for specifying binaries and ROOT files
  string ADAQHOME, USER;
  string ParallelBinaryName;
  string ParallelProcessingFName;
  string ParallelProgressFName;
  bool ParallelVerbose;
  // ROOT TFile to hold values need for parallel processing
  TFile *ParallelProcessingFile;

  // Variables used to specify whether to print to stdout
  bool Verbose;

  // ROOT TH1F to hold aggregated spectra from parallel processing
  TH1F *MasterHistogram_H;

  // Variables to hold waveform processing values
  double Baseline;

  // Maximum bit value of CAEN X720 digitizer family (4095)
  int V1720MaximumBit;

  // Number of data channels in the ADAQ ROOT files
  int NumDataChannels;

  // Aggregated total waveform peaks found during processing
  int TotalPeaks;
  
  // Aggregated total number of deuterons from the RFQ
  double TotalDeuterons;

  // Bin widths for spectrum integration algorithms
  double GaussianBinWidth, CountsBinWidth;
  
  // Create a TColor ROOT object to handle pixel-2-color conversions
  TColor *ColorManager;

  // A ROOT random number generator (RNG)
  TRandom *RNG;

  // Define the class to ROOT
  ClassDef(AAComputation, 1)
};

#endif