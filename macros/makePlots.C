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

std::map<int,std::map<int,std::string>> subDetMap;

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
  histoMap[hName]->GetXaxis()->SetTitle("pileup");

  // per-sub and per-depth plots
  for(int idepth = 1; idepth<8; ++idepth)
    {
      if(idepth < 3)
	{
	  subDetMap[idepth][1] = "HBP_d"+std::to_string(idepth);
	  subDetMap[idepth][-1] = "HBM_d"+std::to_string(idepth);
	  subDetMap[idepth][4] = "HFP_d"+std::to_string(idepth);
	  subDetMap[idepth][-4] = "HFM_d"+std::to_string(idepth);
	}
      subDetMap[idepth][2] = "HEP_d"+std::to_string(idepth);
      subDetMap[idepth][-2] = "HEM_d"+std::to_string(idepth);
    }
  
  for(auto& subDepth : subDetMap)
    {
      hName = "h_recHitIEta_d"+std::to_string(subDepth.first);
      histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),84,-42,42);
      histoMap[hName]->GetXaxis()->SetTitle("recHitIEta");
      
      hName = "h_recHitEnVsIEta_d"+std::to_string(subDepth.first);
      histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),84,-42,42);
      histoMap[hName]->GetXaxis()->SetTitle("recHitIEta");
      histoMap[hName]->GetYaxis()->SetTitle("recHitEn [GeV]");

      for(auto& subDet : subDepth.second)
        {
	  hName = "h_recHitEn_"+subDet.second;
	  histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
          histoMap[hName]->GetXaxis()->SetTitle("recHitEn [GeV]");
      
	  hName = "h_recHitEnRAW_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
          histoMap[hName]->GetXaxis()->SetTitle("recHitEnRAW [GeV]");
          
          hName = "h_recHitIPhi_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),72,0,72);
          histoMap[hName]->GetXaxis()->SetTitle("recHitIPhi");
          
          hName = "h_recHitNum_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),200,0,4000);
          histoMap[hName]->GetXaxis()->SetTitle("number of recHits");
          
          hName = "h_recHitEnVsIPhi_"+subDet.second;
          histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),72,0,72);
          histoMap[hName]->GetXaxis()->SetTitle("recHitIPhi");
          histoMap[hName]->GetYaxis()->SetTitle("recHitEn [GeV]");
          
          hName = "h_recHitChi2_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),100,0,10);
          histoMap[hName]->GetXaxis()->SetTitle("log(recHitChi2)");
          
          hName = "h_recHitTime_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),60,-30,30);
          histoMap[hName]->GetXaxis()->SetTitle("recHitTime [ns]");
          
          hName = "h_recHitMaxEn_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
          histoMap[hName]->GetXaxis()->SetTitle("recHitMaxEn [GeV]");
          
          hName = "h_recHitMaxIEta_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),84,-42,42);
          histoMap[hName]->GetXaxis()->SetTitle("recHitMaxIEta");
          
          hName = "h_recHitMaxIPhi_"+subDet.second;
          histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),72,0,72);
          histoMap[hName]->GetXaxis()->SetTitle("recHitMaxIPhi");
        }
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
  for(int entry = 0; entry < 1000; ++entry)
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
      
      std::map<std::string,int> nRecHitsMap;
      std::map<std::string,int> maxRecHitsEnMap;
      std::map<std::string,int> maxRecHitsIEtaMap;
      std::map<std::string,int> maxRecHitsIPhiMap;

      for (unsigned int rhItr = 0; rhItr<tt.recHitSub->size(); ++rhItr)
	{
	  // basic rechit selections
	  // if(tt.recHitEn->at(rhItr) < )

	  int side = (tt.recHitIEta->at(rhItr) < 0) ? -1 : 1;
	  int depth = tt.recHitDepth->at(rhItr);
	  std::string suffix = subDetMap[depth][side * tt.recHitSub->at(rhItr)];

	  std::string hName = "h_recHitEn_"+suffix;
	  histoMap[hName]->Fill(tt.recHitEn->at(rhItr),ww);
	  hName = "h_recHitEnRAW_"+suffix;
	  histoMap[hName]->Fill(tt.recHitEnRAW->at(rhItr),ww);
	  hName = "h_recHitIEta_d"+std::to_string(depth);
	  histoMap[hName]->Fill(tt.recHitIEta->at(rhItr),ww);
	  hName = "h_recHitIPhi_"+suffix;
	  histoMap[hName]->Fill(tt.recHitIPhi->at(rhItr),ww);
	  hName = "h_recHitEnVsIEta_d"+std::to_string(depth);
	  histoMap[hName]->Fill(tt.recHitIEta->at(rhItr),tt.recHitEn->at(rhItr)*ww);
	  hName = "h_recHitEnVsIPhi_"+suffix;
	  histoMap[hName]->Fill(tt.recHitIPhi->at(rhItr),tt.recHitEn->at(rhItr)*ww);
	  hName = "h_recHitChi2_"+suffix;
	  histoMap[hName]->Fill(log(tt.recHitChi2->at(rhItr)),ww);
	  hName = "h_recHitTime_"+suffix;
	  histoMap[hName]->Fill(tt.recHitTime->at(rhItr),ww);

	  if(maxRecHitsEnMap.find(suffix) == maxRecHitsEnMap.end())
	    {
	      maxRecHitsEnMap[suffix] = tt.recHitEn->at(rhItr);
	      maxRecHitsIEtaMap[suffix] = tt.recHitIEta->at(rhItr);
	      maxRecHitsIPhiMap[suffix] = tt.recHitIPhi->at(rhItr);
	      nRecHitsMap[suffix] = 0;
	    }
	  else if(tt.recHitEn->at(rhItr) > maxRecHitsEnMap[suffix])
	    {
	      maxRecHitsEnMap[suffix] = tt.recHitEn->at(rhItr);
	      maxRecHitsIEtaMap[suffix] = tt.recHitIEta->at(rhItr);
	      maxRecHitsIPhiMap[suffix] = tt.recHitIPhi->at(rhItr);	      
	    }
	  ++nRecHitsMap[suffix];

	}


      for(auto& subDepth : subDetMap)
	for(auto& subDet : subDepth.second)
	  {
	    std::string hName = "h_recHitNum_"+subDet.second;
	    histoMap[hName]->Fill(nRecHitsMap[subDet.second],ww);
	    
	    hName = "h_recHitMaxEn_"+subDet.second;
	    histoMap[hName]->Fill(maxRecHitsEnMap[subDet.second],ww);
	    
	    hName = "h_recHitMaxIEta_"+subDet.second;
	    histoMap[hName]->Fill(maxRecHitsIEtaMap[subDet.second],ww);
	    
	    hName = "h_recHitMaxIPhi_"+subDet.second;
	    histoMap[hName]->Fill(maxRecHitsIPhiMap[subDet.second],ww);
	  }

    }//end event loop
  std::cout << std::endl;


  // write histos to file
  for (auto& itr : histoMap)
    itr.second->Write();
  
  outFile->Close();
  return 0;
}
