//c++ -o makePlots `root-config --cflags --ldflags --glibs` makePlots.C

#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TApplication.h"

#include <utility>
#include <string>
#include <iostream>
#include <fstream>

std::map<int,std::string> subDetMap;

TChain* loadChain(std::string fileList)
{
  TChain* chain = new TChain("recHitTree/RecHitTree","recHitTree/RecHitTree");

  if(fileList.find(".root") != std::string::npos)
    {
      chain->Add(fileList.c_str());
      std::cout << "+++ Added file: " << fileList << std::endl;
    }
  else
    {
      std::ifstream file(fileList);
      std::string str;
      while (std::getline(file, str))
        {
          chain->Add(str.c_str());
	  std::cout << "+++ Added file: " << str << std::endl;
        }
    }
  return chain;
}

struct TreeVars
{  
  unsigned int run, lumi, event, pileup;
  
  std::vector<float>* recHitEn;
  std::vector<float>* recHitEaux;
  std::vector<float>* recHitChi2;
  std::vector<float>* recHitEnRAW;
  std::vector<float>* recHitTime;
  std::vector<float>* recHitFlags;
  std::vector<int>* recHitIEta;
  std::vector<int>* recHitIPhi;
  std::vector<int>* recHitDepth;
  std::vector<int>* recHitSub;

};

void InitTreeVars(TTree* chain, TreeVars& tt)
{
  chain -> SetBranchAddress("run",&tt.run);
  chain -> SetBranchAddress("lumi",&tt.lumi);
  chain -> SetBranchAddress("event",&tt.event);
  chain -> SetBranchAddress("pileup",&tt.pileup);

  tt.recHitEn = 0;
  tt.recHitEaux = 0;
  tt.recHitChi2 = 0;
  tt.recHitEnRAW = 0;
  tt.recHitTime = 0;
  tt.recHitFlags = 0;
  chain -> SetBranchAddress("recHitEn",&tt.recHitEn);
  chain -> SetBranchAddress("recHitEaux",&tt.recHitEaux);
  chain -> SetBranchAddress("recHitChi2",&tt.recHitChi2);
  chain -> SetBranchAddress("recHitEnRAW",&tt.recHitEnRAW);
  chain -> SetBranchAddress("recHitTime",&tt.recHitTime);
  chain -> SetBranchAddress("recHitFlags",&tt.recHitFlags);
  
  tt.recHitIEta = 0;
  tt.recHitIPhi = 0;
  tt.recHitDepth = 0;
  tt.recHitSub = 0;
  chain -> SetBranchAddress("recHitIEta",&tt.recHitIEta);
  chain -> SetBranchAddress("recHitIPhi",&tt.recHitIPhi);
  chain -> SetBranchAddress("recHitDepth",&tt.recHitDepth);
  chain -> SetBranchAddress("recHitSub",&tt.recHitSub);
}


std::map<std::string, TH1*> InitHistograms()
{
  std::map<std::string, TH1*> histoMap;

  // global plots
  std::string hName = "h_pileup";
  histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),100,0,100);
  hName = "h_recHitIEta";
  histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),84,-42,42);
  // add axis name

  // per-sub plots
  subDetMap[1] = "HB";
  subDetMap[2] = "HE";
  subDetMap[4] = "HF";
  for(auto& sub : subDetMap)
    {
      hName = "h_recHitEn_"+sub.second;
      histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
      // add axis name

      hName = "h_recHitIPhi_"+sub.second;
      histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),72,0,72);
    }

  return histoMap;
}



int main(int argc, char** argv)
{
  gROOT->SetBatch(kTRUE);

  std::string fileList(argv[1]);
  std::string puWeights = "none";
  if(argc > 2)
    puWeights = argv[2];
  
  TFile* f_weights = 0;
  TH1F* h_weights = 0;
  bool isData = true;

  std::string outFileName = "histograms_";
  if ( puWeights.find("none") != std::string::npos )
      outFileName += "data";
  else
    {
      outFileName += "MC";
      f_weights = TFile::Open(puWeights.c_str());
      h_weights = (TH1F*)f_weights->Get("h_weights");
      isData = false;
    }
  outFileName+= ".root";
  TFile* outFile = new TFile(outFileName.c_str(),"RECREATE");

  // load chain
  TTree* chain = loadChain(fileList);

  //set variables
  TreeVars tt;
  InitTreeVars(chain,tt); 

  // book histos
  auto histoMap = InitHistograms();


  int nEntries = chain -> GetEntries();
  int nProcessed = 0;
  for(int entry = 0; entry < nEntries; ++entry)
    {
      if(entry % 1000 == 0)
	std::cout << "reading entry " << entry << " / " << nEntries << "\r" << std::flush;
      chain -> GetEntry(entry);

      // get PU weights
      float ww = 1;
      if (isData == false)
	ww = h_weights->GetBinContent(h_weights->FindBin(tt.pileup));

      // fill histograms
      histoMap["h_pileup"]->Fill(tt.pileup,ww);
      
      for (unsigned int rhItr = 0; rhItr<tt.recHitSub->size(); ++rhItr)
	{
	  std::string suffix = subDetMap[tt.recHitSub->at(rhItr)];

	  std::string hName = "h_recHitEn_"+suffix;
	  histoMap[hName]->Fill(tt.recHitEn->at(rhItr),ww);
	  hName = "h_recHitIEta";
	  histoMap[hName]->Fill(tt.recHitIEta->at(rhItr),ww);
	  hName = "h_recHitIPhi_"+suffix;
	  histoMap[hName]->Fill(tt.recHitIPhi->at(rhItr),ww);

	}


    }//end event loop
  std::cout << std::endl;




  // write histos to file
  for (auto& itr : histoMap)
    itr.second->Write();
  
  outFile->Close();
  return 0;
}
