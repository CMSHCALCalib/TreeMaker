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
std::vector<int> puVector{0,20,30,40,50,100};

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


std::map<std::string, TH1*> InitHistograms(TFile* outFile)
{
  std::map<std::string, TH1*> histoMap;

  // prepare map for per-sub and per-depth plots
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

  // global plots
  std::string hName = "h_pileup";
  histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),100,0,100);
  histoMap[hName]->GetXaxis()->SetTitle("pileup");


  // loop over PU bins
  for(unsigned int puBin = 1; puBin<puVector.size(); ++puBin)
    {
      std::string suffix = "pu"+std::to_string(puVector[puBin-1])+"-"+std::to_string(puVector[puBin]);
      TDirectory* dir = outFile->mkdir(suffix.c_str());
      dir->cd();

      std::string hName = "h_pileup_"+suffix;
      histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),100,0,100);
      histoMap[hName]->GetXaxis()->SetTitle("pileup");


      // loop over depths
      for(auto& subDepth : subDetMap)
	{
         //depth+PU suffix
         std::string suffix = std::to_string(subDepth.first)+"_pu"+std::to_string(puVector[puBin-1])+"-"+std::to_string(puVector[puBin]);
    	 
         hName = "h_recHitIEta_d"+suffix;
         histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),84,-42,42);
         histoMap[hName]->GetXaxis()->SetTitle("recHitIEta");
         
         hName = "h_recHitEnVsIEta_d"+suffix;
         histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),84,-42,42);
         histoMap[hName]->GetXaxis()->SetTitle("recHitIEta");
         histoMap[hName]->GetYaxis()->SetTitle("recHitEn [GeV]");
    	 
	 // loop over subdet regions
         for(auto& subDet : subDepth.second)
           {
	     //depth+subdepth+pu suffix
	     suffix = subDet.second+"_pu"+std::to_string(puVector[puBin-1])+"-"+std::to_string(puVector[puBin]);
	     
	     hName = "h_recHitEn_"+suffix;
	     histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
	     histoMap[hName]->GetXaxis()->SetTitle("recHitEn [GeV]");

	     hName = "h_recHitEnVsPileup_"+suffix;
	     histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),100,0,100);
	     histoMap[hName]->GetYaxis()->SetTitle("recHitEn [GeV]");    
	     histoMap[hName]->GetXaxis()->SetTitle("pileup");    

	     hName = "h_recHitEnRAW_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
             histoMap[hName]->GetXaxis()->SetTitle("recHitEnRAW [GeV]");
             
             hName = "h_recHitIPhi_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),72,0,72);
             histoMap[hName]->GetXaxis()->SetTitle("recHitIPhi");
             
             hName = "h_recHitNum_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),400,0,4000);
             histoMap[hName]->GetXaxis()->SetTitle("number of recHits");

             hName = "h_recHitNumVsPileup_"+suffix;
             histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),100,0,100);
             histoMap[hName]->GetYaxis()->SetTitle("number of recHits");
             histoMap[hName]->GetXaxis()->SetTitle("pileup");
             
             hName = "h_recHitEnVsIPhi_"+suffix;
             histoMap[hName] = new TProfile(hName.c_str(),hName.c_str(),72,0,72);
             histoMap[hName]->GetXaxis()->SetTitle("recHitIPhi");
             histoMap[hName]->GetYaxis()->SetTitle("recHitEn [GeV]");
             
             hName = "h_recHitChi2_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),100,0,10);
             histoMap[hName]->GetXaxis()->SetTitle("log(recHitChi2)");
             
             hName = "h_recHitTime_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),60,-30,30);
             histoMap[hName]->GetXaxis()->SetTitle("recHitTime [ns]");
             
             hName = "h_recHitMaxEn_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),2500,0,5000);
             histoMap[hName]->GetXaxis()->SetTitle("recHitMaxEn [GeV]");
             
             hName = "h_recHitMaxIEta_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),84,-42,42);
             histoMap[hName]->GetXaxis()->SetTitle("recHitMaxIEta");
             
             hName = "h_recHitMaxIPhi_"+suffix;
             histoMap[hName] = new TH1F(hName.c_str(),hName.c_str(),72,0,72);
             histoMap[hName]->GetXaxis()->SetTitle("recHitMaxIPhi");
           }
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
  auto histoMap = InitHistograms(outFile);


  int nEntries = chain -> GetEntries();
  int nProcessed = 0;
  for(int entry = 0; entry < 500; ++entry)
    {
      if(entry % 1000 == 0)
	std::cout << "reading entry " << entry << " / " << nEntries << "\r" << std::flush;
      chain -> GetEntry(entry);

      // get PU weights
      float ww = 1;
      if (isData == false)
	ww = h_weights->GetBinContent(h_weights->FindBin(tt.pileup));
      
      // fill histograms
      int puBinMin = -1;
      int puBinMax = -1;
      for(unsigned int puBin = 1; puBin<puVector.size(); ++puBin)
	if(tt.pileup < puVector[puBin])
	  {
	    puBinMin = puVector[puBin-1];
	    puBinMax = puVector[puBin];
	    break;
	  }
      std::string puSuffix = "_pu"+std::to_string(puBinMin)+"-"+std::to_string(puBinMax);

      std::string hName = "h_pileup";
      histoMap[hName]->Fill(tt.pileup,ww);
      hName = "h_pileup"+puSuffix;
      histoMap[hName]->Fill(tt.pileup,ww);
      
      
      std::map<std::string,int> nRecHitsMap;
      std::map<std::string,int> maxRecHitsEnMap;
      std::map<std::string,int> maxRecHitsIEtaMap;
      std::map<std::string,int> maxRecHitsIPhiMap;

      for (unsigned int rhItr = 0; rhItr<tt.recHitSub->size(); ++rhItr)
	{
	  // basic rechit selections
	  if(tt.recHitSub->at(rhItr) == 1 && //HB
	     tt.recHitEn->at(rhItr) < 0.8)
	    continue;
	  if(tt.recHitSub->at(rhItr) == 2 && //HE
	     tt.recHitEn->at(rhItr) < ((tt.recHitDepth->at(rhItr) > 1) ? 0.2 : 0.1))
	    continue;

	  int side = (tt.recHitIEta->at(rhItr) < 0) ? -1 : 1;
	  int depth = tt.recHitDepth->at(rhItr);
	  std::string detSuffix = subDetMap[depth][side * tt.recHitSub->at(rhItr)];

	  hName = "h_recHitEn_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.recHitEn->at(rhItr),ww);
	  hName = "h_recHitEnVsPileup_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.pileup,tt.recHitEn->at(rhItr)*ww);
	  hName = "h_recHitEnRAW_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.recHitEnRAW->at(rhItr),ww);
	  hName = "h_recHitIEta_d"+std::to_string(depth)+puSuffix;
	  histoMap[hName]->Fill(tt.recHitIEta->at(rhItr),ww);
	  hName = "h_recHitIPhi_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.recHitIPhi->at(rhItr),ww);
	  hName = "h_recHitEnVsIEta_d"+std::to_string(depth)+puSuffix;
	  histoMap[hName]->Fill(tt.recHitIEta->at(rhItr),tt.recHitEn->at(rhItr)*ww);
	  hName = "h_recHitEnVsIPhi_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.recHitIPhi->at(rhItr),tt.recHitEn->at(rhItr)*ww);
	  hName = "h_recHitChi2_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(log(tt.recHitChi2->at(rhItr)),ww);
	  hName = "h_recHitTime_"+detSuffix+puSuffix;
	  histoMap[hName]->Fill(tt.recHitTime->at(rhItr),ww);

	  if(maxRecHitsEnMap.find(detSuffix+puSuffix) == maxRecHitsEnMap.end())
	    {
	      maxRecHitsEnMap[detSuffix+puSuffix] = tt.recHitEn->at(rhItr);
	      maxRecHitsIEtaMap[detSuffix+puSuffix] = tt.recHitIEta->at(rhItr);
	      maxRecHitsIPhiMap[detSuffix+puSuffix] = tt.recHitIPhi->at(rhItr);
	      nRecHitsMap[detSuffix+puSuffix] = 0;
	    }
	  else if(tt.recHitEn->at(rhItr) > maxRecHitsEnMap[detSuffix+puSuffix])
	    {
	      maxRecHitsEnMap[detSuffix+puSuffix] = tt.recHitEn->at(rhItr);
	      maxRecHitsIEtaMap[detSuffix+puSuffix] = tt.recHitIEta->at(rhItr);
	      maxRecHitsIPhiMap[detSuffix+puSuffix] = tt.recHitIPhi->at(rhItr);	      
	    }
	  ++nRecHitsMap[detSuffix+puSuffix];

	}

       for(auto& subDet : nRecHitsMap)
       	  {
	    std::string suffix = subDet.first;
       	    std::string hName = "h_recHitNum_"+suffix;
       	    histoMap[hName]->Fill(nRecHitsMap[suffix],ww);

       	    hName = "h_recHitNumVsPileup_"+suffix;
       	    histoMap[hName]->Fill(tt.pileup,nRecHitsMap[suffix]*ww);
          
       	    hName = "h_recHitMaxEn_"+suffix;
       	    histoMap[hName]->Fill(maxRecHitsEnMap[suffix],ww);
          
       	    hName = "h_recHitMaxIEta_"+suffix;
       	    histoMap[hName]->Fill(maxRecHitsIEtaMap[suffix],ww);
          
       	    hName = "h_recHitMaxIPhi_"+suffix;
       	    histoMap[hName]->Fill(maxRecHitsIPhiMap[suffix],ww);
       	  }

    }//end event loop
  std::cout << std::endl;


  outFile->Write();
  outFile->Close();
  return 0;
}
