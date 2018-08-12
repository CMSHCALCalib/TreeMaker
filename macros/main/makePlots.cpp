//c++ -I/cvmfs/cms.cern.ch/slc6_amd64_gcc700/external/boost/1.63.0-omkpbe4/include -o makePlots `root-config --cflags --ldflags --glibs` makePlots.C

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

#include "interface/LumiInfoProvider.h"
#include "interface/BXPUInfo.h"
#include "interface/HistoFactory.h"
#include "interface/dataPuFunc.h"

std::vector<int> puVector{0,20,30,40,50,100};
std::vector<int> ietaVector{16,24,29};

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
  unsigned int run, lumi, event, bx, pileup;
  
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
  chain -> SetBranchAddress("bx",&tt.bx);
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


  //book histos
  HistoFactory myHistos(puVector,ietaVector,outFile);

  myHistos.initHisto("h_pileupGlo","Global", 100,0,100,"pileup");
  myHistos.initHisto("h_pileupBin","Pu",100,0,100,"pileup");

  myHistos.initHisto("h_recHitIEta",    "DepthPu",84,-42,42,"recHitIEta");
  myHistos.initHisto("h_recHitEnVsIEta","DepthPu",       84,-42,42,"recHitIEta","recHitEn [GeV]");
  myHistos.initHisto("h_recHitIPhi",   "SubDepthIetaPu",72,0,72,"recHitIPhi");
  myHistos.initHisto("h_recHitEnVsIPhi","SubDepthIetaPu",72,0,72,"recHitIPhi","recHitEn [GeV]");
  
  myHistos.initHisto("h_recHitEn",   "SubDepthIetaPu",2500,0,5000,"recHitEn [GeV]");
  myHistos.initHisto("h_recHitEnRAW","SubDepthIetaPu",2500,0,5000,"recHitEnRAW [GeV]");
  myHistos.initHisto("h_recHitEnVsPu","SubDepthIeta",2500,0,5000,"recHitEn [GeV]","pileup");

  myHistos.initHisto("h_recHitNum","SubDepthIetaPu",400,0,4000,"number of recHits");
  myHistos.initHisto("h_recHitNumVsPu","SubDepthIeta",400,0,4000,"number of recHits","pileup");

  myHistos.initHisto("h_recHitChi2","SubDepthIetaPu",1000,0,100,"log(recHitChi2)");
  myHistos.initHisto("h_recHitTime","SubDepthIetaPu",60,-30,30,"recHitTime [ns]");
  myHistos.initHisto("h_recHitMaxEn","SubDepthIetaPu",2500,0,5000,"recHitMaxEn [GeV]");


  myHistos.initHisto("h_recHitEnNoPuTMP","SubDepthIeta",2500,0,5000,"recHitEn [GeV]");
  myHistos.initHisto("h_recHitEnTMP","SubDepthIetaPu",2500,0,5000,"recHitEn [GeV]");
  

  //loop over events
  int nEntries = chain -> GetEntries();
  if(argc > 3)
    nEntries = atoi(argv[3]);

  for(int entry = 0; entry < nEntries; ++entry)
    {
      if(entry % 1000 == 0)
   	std::cout << "reading entry " << entry << " / " << nEntries << "\r" << std::flush;
      chain -> GetEntry(entry);
      
      // get PU weights
      int truePU = -1;
      float ww = 1;
      if (isData == false)
   	{
   	  ww = h_weights->GetBinContent(h_weights->FindBin(tt.pileup));
   	  truePU = tt.pileup;
   	}
      else
   	truePU = getNrExpPU(tt.run,tt.lumi,tt.bx);
      
      std::unordered_map<std::string,int> nRecHitsMap;
      std::unordered_map<std::string,int> maxRecHitsEnMap;
      std::unordered_map<std::string,int> maxRecHitsIEtaMap;
      std::unordered_map<std::string,int> maxRecHitsIPhiMap;


      // fill global histograms
      myHistos.fill("h_pileupGlo",-1,-1,-1,-1, truePU, -1 ,ww);
      myHistos.fill("h_pileupBin",truePU,-1,-1,-1, truePU, -1 ,ww);
      

      // loop over rechits
      for (unsigned int rhItr = 0; rhItr<tt.recHitSub->size(); ++rhItr)
   	{
   	  // basic rechit selections
   	  if(tt.recHitSub->at(rhItr) == 1 && //HB
   	     tt.recHitEn->at(rhItr) < 0.8)
   	    continue;
   	  if(tt.recHitSub->at(rhItr) == 2 && //HE
   	     tt.recHitEn->at(rhItr) < ((tt.recHitDepth->at(rhItr) > 1) ? 0.2 : 0.1))
   	    //tt.recHitEn->at(rhItr) < 0.8)
   	    continue;

	  float en = tt.recHitEn->at(rhItr);
	  float enRAW = tt.recHitEnRAW->at(rhItr);
	  float time = tt.recHitTime->at(rhItr);
	  float chi2 = tt.recHitChi2->at(rhItr);

	  int ieta = tt.recHitIEta->at(rhItr);
	  int iphi = tt.recHitIPhi->at(rhItr);
	  int depth = tt.recHitDepth->at(rhItr);

	  int det = tt.recHitSub->at(rhItr);


	  myHistos.fill("h_recHitIEta",     truePU,det,depth,ieta, ieta,-1 ,ww);
	  myHistos.fill("h_recHitEnVsIEta", truePU,det,depth,ieta, ieta,en ,ww);
	  myHistos.fill("h_recHitIPhi",     truePU,det,depth,ieta, iphi,-1 ,ww);
	  myHistos.fill("h_recHitEnVsIPhi", truePU,det,depth,ieta, iphi,en ,ww);

	  myHistos.fill("h_recHitEn",       truePU,det,depth,ieta, en,-1 ,ww);
	  myHistos.fill("h_recHitEnVsPu",   truePU,det,depth,ieta, en,truePU ,ww);
	  myHistos.fill("h_recHitEnRAW",    truePU,det,depth,ieta, enRAW,-1 ,ww);

	  myHistos.fill("h_recHitTime",       truePU,det,depth,ieta, time,-1 ,ww);
	  myHistos.fill("h_recHitChi2",       truePU,det,depth,ieta, log10(chi2),-1 ,ww);

	  myHistos.fill("h_recHitEnTMP",    truePU,det,depth,ieta, en,-1 ,ww);
	  myHistos.fill("h_recHitEnNoPuTMP",truePU,det,depth,ieta, en,-1 ,ww);
	}

      //fill max and nrechits
      auto recHitEnMapPU = myHistos.getHistoVec("h_recHitEnTMP");
      for(auto& hist: recHitEnMapPU)
	if(hist.second->GetEntries() != 0)
	  {
	    myHistos.fill("h_recHitNum",        hist.first, hist.second->GetEntries(),-1 ,ww);
	    int lastBin = hist.second->FindLastBinAbove();
	    myHistos.fill("h_recHitMaxEn",      hist.first, hist.second->GetXaxis()->GetBinCenter(lastBin),-1 ,ww);
	    hist.second->Reset();
	  }

      auto recHitEnMap = myHistos.getHistoVec("h_recHitEnNoPuTMP");
      for(auto& hist: recHitEnMap)
	if(hist.second->GetEntries() != 0)
	  {
	    myHistos.fill("h_recHitNumVsPu",    hist.first, hist.second->GetEntries(),truePU ,ww);
	    hist.second->Reset();
	  }
      

    }//end event loop
  std::cout << std::endl;

  outFile->Write();
  outFile->Close();
  return 0;
}
