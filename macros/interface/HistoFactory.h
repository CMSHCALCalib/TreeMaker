#ifndef HISTOFACTORY_H
#define HISTOFACTORY_H

//Just an histogram container for the data/MC comparison
//Authorn: Federico De Guio, August 2018

#include "TH1.h"
#include "TProfile.h"
#include "TFile.h"

#include <iostream>
#include <string>
#include <unordered_map>


class HistoFactory {
 private:
  int debug_;
  TFile* outFile_;
  //contains the histograms
  std::unordered_map<std::string, std::unordered_map<std::string,TH1*>> hMap_;
  std::unordered_map<std::string, std::string> tMap_;

  //defines the binning for histo types
  std::vector<int> puVec_;
  std::vector<int> ietaVec_;
  std::vector<std::string> subVec_;
  int depthNum_;

 public:
  HistoFactory(std::vector<int> puVec,
	       std::vector<int> ietaVec,
	       TFile* outFile, int debug=0)
    {
      puVec_   = puVec;
      ietaVec_ = ietaVec;
      outFile_ = outFile;
      debug_   = debug;

      subVec_   = {"HBM","HBP","HEM","HEP","HFM","HFP"};
      depthNum_ = 7;
    }

  std::unordered_map<std::string, TH1*> getHistoVec(std::string hName)
    {
      return hMap_[hName];
    }

  void initHisto(std::string hOrigName,
		 std::string hType,
		 int nBins, int min, int max,
		 std::string hTitleX, std::string hTitleY="");

  TH1* bookHisto(std::string hName,
		 int nBins, int min, int max,
		 std::string hTitleX, std::string hTitleY="");

  void fill(std::string hOrigName, std::string hName, double xx, double yy=0, double ww=1);
  void fill(std::string hOrigName, int pu, int det, int depth, int ieta, double xx, double yy=0, double ww=1);


};

#endif
