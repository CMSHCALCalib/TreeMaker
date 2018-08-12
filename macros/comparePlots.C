//simple macro to compare Data and MC data

void loopdir(TFile* file, std::vector<TH1*>& histovec, std::string folder)
{
  file->cd(folder.c_str());
  TIter next(gDirectory->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1")) continue;
    TH1 *h = (TH1*)key->ReadObj();
    histovec.push_back(h);
  }
}

void setDatastyle(TH1* histo)
{
  histo->SetMarkerSize(0);  
  histo->SetLineWidth(2);
}

void setMCstyle(TH1* histo)
{
  histo->SetLineColor(2);
  histo->SetMarkerColor(2);
}

void setRatiostyle(TH1* histo)
{
  histo->GetXaxis()->SetLabelSize(0.1);
  histo->GetYaxis()->SetLabelSize(0.1);
}

void setGlobalStyle()
{
  // For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(000000); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  tdrStyle->SetLabelSize(0.05, "XYZ");
}



void comparePlots(std::string plotsData, std::string plotsMC, std::string folder)
{
  gROOT->SetBatch(kTRUE);
  setGlobalStyle();

  std::vector<string> v_baseName = {"h_pileup",
				    "h_recHitEn_",
				    "h_recHitEnRAW_",
				    "h_recHitIPhi_","h_recHitIEta_",
				    "h_recHitNum_",
				    "h_recHitEnVsIPhi_","h_recHitEnVsIEta_",
				    "h_recHitChi2_",
				    "h_recHitTime_",
				    "h_recHitMaxEn_",
				    "h_recHitMaxIEta_", "h_recHitMaxIPhi_",
				    "h_recHitEnVsPileup_","h_recHitNumVsPileup_"};

  std::vector<int> v_setLog = {0,       //"h_pileup",					    
			       1,	//"h_recHitEn_",				    
			       1,	//"h_recHitEnRAW_",				    
			       0, 0,	//"h_recHitIPhi_","h_recHitIEta_",		    
			       0,	//"h_recHitNum_",				    
			       1, 1,	//"h_recHitEnVsIPhi_","h_recHitEnVsIEta_",	    
			       0,	//"h_recHitChi2_",				    
			       0,	//"h_recHitTime_",				    
			       1,	//"h_recHitMaxEn_",				    
			       0, 0,	//"h_recHitMaxIEta_", "h_recHitMaxIPhi_"};	    
                               0, 0};	//"h_recHitEnVsPileup_","h_recHitNumVsPileup_"};
  
  std::vector<int> v_setXmax = {0,      //"h_pileup",					
				150,	//"h_recHitEn_",				
				150,	//"h_recHitEnRAW_",				
				0, 0,	//"h_recHitIPhi_","h_recHitIEta_",		
				300,	//"h_recHitNum_",				
				0, 0,	//"h_recHitEnVsIPhi_","h_recHitEnVsIEta_",	
				0,	//"h_recHitChi2_",				
				0,	//"h_recHitTime_",				
				150,	//"h_recHitMaxEn_",				
				0, 0,	//"h_recHitMaxIEta_", "h_recHitMaxIPhi_"};	
                                0, 0};//"h_recHitEnVsPileup_","h_recHitNumVsPileup_"};
  
  std::vector<int> v_scaleContent = {1,
				     1,
				     1,
				     1, 1,
				     1,
				     0, 0,
				     1,
				     1,
				     1,
				     1, 1,
                                     0, 0};


  //open files
  TFile* f_plotsData = TFile::Open(plotsData.c_str());
  TFile* f_plotsMC = TFile::Open(plotsMC.c_str());
  
  std::vector<TH1*> h_Data;
  std::vector<TH1*> h_MC;
  
  loopdir(f_plotsData, h_Data, folder);
  loopdir(f_plotsMC, h_MC, folder);

  //need to renormalize to the number of events for a given PU bin
  float mcScale = h_Data.at(0)->Integral() / h_MC.at(0)->Integral();


  for(int itr=0; itr<h_Data.size(); ++itr)
    {
      std::string histoName = h_Data.at(itr)->GetName();
      std::cout << "histoName: " << histoName << std::endl;
	  
      // how to plot
      unsigned int pos = -1;
      for(unsigned int itr=0; itr<v_baseName.size(); ++itr)
	if(histoName.find(v_baseName[itr]) != std::string::npos)
	  {
	    pos = itr;
	    break;
	  }
      if(pos == -1)
	continue;
      
      int scaleContent = v_scaleContent[pos];
      int setXmax      = v_setXmax[pos];
      int setLog       = v_setLog[pos];

      // TMP: normalize to number of entries
      // float mcScale = h_Data.at(itr)->Integral() / h_MC.at(itr)->Integral();

      setDatastyle(h_Data.at(itr));
      setMCstyle(h_MC.at(itr));

      //legend
      TLegend* leg = new TLegend(0.82, 0.78, 1.03, 0.89);
      //leg->SetFillColor(kWhite);
      leg->AddEntry(h_Data.at(itr),"Data","L");
      leg->AddEntry(h_MC.at(itr),"MC","P");

      TCanvas* c1 = new TCanvas();
      c1 -> cd();
      TPad* p1 = new TPad("p1","p1",0., 0.25, 1., 1.);
      if(setLog)
	    p1->SetLogy();
      TPad* p2 = new TPad("p2","p2",0., 0., 1., 0.25);
      p1 -> Draw();
      p2 -> Draw();

      //draw first pad
      p1 -> cd();
      p1 -> SetGridx();
      p1 -> SetGridy();


      //set specific axis range
      if(setXmax != 0)
	{
	  h_Data.at(itr)->GetXaxis()->SetRangeUser(h_Data.at(itr)->GetXaxis()->GetBinLowEdge(1), setXmax);
	  h_MC.at(itr)->GetXaxis()->SetRangeUser(h_MC.at(itr)->GetXaxis()->GetBinLowEdge(1), setXmax);
	}
      

      //scale and compute ratio
      h_Data.at(itr)->Sumw2();
      h_MC.at(itr)->Sumw2();
      //scale to the same number of events
      if(scaleContent)
	h_MC.at(itr)->Scale(mcScale);

      h_Data.at(itr)->Draw("HISTO,E");
      h_MC.at(itr)->Draw("P,sames");
      leg->Draw("same");


      TH1F* ratioHisto;
      if(h_Data.at(itr)->GetXaxis()->GetXbins()->GetArray() != nullptr)
	ratioHisto = new TH1F("tmp","tmp",h_Data.at(itr)->GetNbinsX(),h_Data.at(itr)->GetXaxis()->GetXbins()->GetArray());
      else
	ratioHisto = new TH1F("tmp","tmp",h_Data.at(itr)->GetNbinsX(),
			      h_Data.at(itr)->GetBinLowEdge(1),
			      h_Data.at(itr)->GetBinLowEdge(h_Data.at(itr)->GetNbinsX())+h_Data.at(itr)->GetBinWidth(1));
      ratioHisto->Sumw2();
      
      for(int bin = 1; bin <= h_Data.at(itr)->GetNbinsX(); ++bin)
	{
	  if(h_Data.at(itr)->GetBinContent(bin) == 0. || h_MC.at(itr)->GetBinContent(bin) == 0.) continue;
	  
	  float valData = h_Data.at(itr)->GetBinContent(bin);
	  float valMC = h_MC.at(itr)->GetBinContent(bin);
	  float sigmaData = h_Data.at(itr)->GetBinError(bin);
	  float sigmaMC = h_MC.at(itr)->GetBinError(bin);
	  
	  ratioHisto -> SetBinContent(bin, valData/valMC);
	  ratioHisto -> SetBinError(bin, sqrt((sigmaData/valMC)*(sigmaData/valMC) + (valData*sigmaMC/valMC/valMC)*(valData*sigmaMC/valMC/valMC)));
	}
      
      p2 -> cd();
      p2 -> SetGridx();
      p2 -> SetGridy();
	  
      // set specific axis range
      if(setXmax != 0)
	ratioHisto->GetXaxis()->SetRangeUser(ratioHisto->GetXaxis()->GetBinLowEdge(1), setXmax);

      ratioHisto -> GetYaxis() -> SetRangeUser(0., 2.);
      setRatiostyle(ratioHisto);
      ratioHisto -> DrawCopy("P");
      
      TF1* line = new TF1("line", "1.", -1000000., 1000000.);
      line -> SetLineWidth(2);
      line -> SetLineColor(kRed);
      line -> Draw("same");

      std::string picName = h_Data.at(itr)->GetName();
      c1->Print(("plots/"+picName+".pdf").c_str());

      delete ratioHisto;
    }
}
