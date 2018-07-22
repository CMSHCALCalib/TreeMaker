//simple macro to compare 2015 and 2016 data

void loopdir(TFile* file, std::vector<TH1*>& histovec)
{
  TIter next(file->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1")) continue;
    TH1 *h = (TH1*)key->ReadObj();
    histovec.push_back(h);
  }
}

void set2015style(TH1* histo)
{
  histo->SetMarkerSize(0);  
}

void set2016style(TH1* histo)
{
  histo->SetLineColor(2);
  histo->SetMarkerColor(2);
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
}



void comparePlots(std::string plots2015, std::string plots2016)
{
  setGlobalStyle();
  int setLog[] = {1,1,0,0,1,1,0,0,0,0};
  ADD SIMILAR FOR REBIN
  
  //open files
  TFile* f_plots2015 = TFile::Open(plots2015.c_str());
  TFile* f_plots2016 = TFile::Open(plots2016.c_str());

  std::vector<TH1*> h_2015;
  std::vector<TH1*> h_2016;

  loopdir(f_plots2015, h_2015);
  loopdir(f_plots2016, h_2016);

  for(int itr=0; itr<h_2015.size(); ++itr)
    {
      std::cout << "histoName: " << h_2015.at(itr)->GetName() << std::endl;

      set2015style(h_2015.at(itr));
      set2016style(h_2016.at(itr));

      //legend
      TLegend* leg = new TLegend(0.82, 0.78, 1.03, 0.89);
      //leg->SetFillColor(kWhite);
      leg->AddEntry(h_2015.at(itr),"2015 Data","L");
      leg->AddEntry(h_2016.at(itr),"2016 Data","P");

      TCanvas* c1 = new TCanvas();
      c1 -> cd();
      TPad* p1 = new TPad("p1","p1",0., 0.25, 1., 1.);
      if(setLog[itr] == 1)
	p1->SetLogy();
      TPad* p2 = new TPad("p2","p2",0., 0., 1., 0.25);
      p1 -> Draw();
      p2 -> Draw();

      //draw first pad
      p1 -> cd();
      p1 -> SetGridx();
      p1 -> SetGridy();

      //scale and compute ratio
      h_2015.at(itr)->Sumw2();
      h_2016.at(itr)->Sumw2();
      h_2015.at(itr)->Scale(h_2016.at(itr)->Integral()/h_2015.at(itr)->Integral());

      h_2015.at(itr)->Draw("HISTO,E");
      h_2016.at(itr)->Draw("P,sames");
      leg->Draw("same");


      TH1F* ratioHisto;
      if(h_2015.at(itr)->GetXaxis()->GetXbins()->GetArray() != nullptr)
	ratioHisto = new TH1F("tmp","tmp",h_2015.at(itr)->GetNbinsX(),h_2015.at(itr)->GetXaxis()->GetXbins()->GetArray());
      else
	ratioHisto = new TH1F("tmp","tmp",h_2015.at(itr)->GetNbinsX(),
			                  h_2015.at(itr)->GetBinLowEdge(1),
			                  h_2015.at(itr)->GetBinLowEdge(h_2015.at(itr)->GetNbinsX())+h_2015.at(itr)->GetBinWidth(1));
      ratioHisto->Sumw2();
      
      for(int bin = 1; bin <= h_2015.at(itr)->GetNbinsX(); ++bin)
	{
	  if(h_2015.at(itr)->GetBinContent(bin) == 0. || h_2016.at(itr)->GetBinContent(bin) == 0.) continue;

	  float val2015 = h_2015.at(itr)->GetBinContent(bin);
	  float val2016 = h_2016.at(itr)->GetBinContent(bin);
	  float sigma2015 = h_2015.at(itr)->GetBinError(bin);
	  float sigma2016 = h_2016.at(itr)->GetBinError(bin);

	  ratioHisto -> SetBinContent(bin, val2015/val2016);
	  ratioHisto -> SetBinError(bin, sqrt((sigma2015/val2016)*(sigma2015/val2016) + (val2015*sigma2016/val2016/val2016)*(val2015*sigma2016/val2016/val2016)));
	}
      
      p2 -> cd();
      p2 -> SetGridx();
      p2 -> SetGridy();

      // For the axis labels:
      tdrStyle->SetLabelSize(0.1, "XYZ");
  
      ratioHisto -> GetYaxis() -> SetRangeUser(0., 2.);
      ratioHisto -> DrawCopy("P");
      
      TF1* line = new TF1("line", "1.", -1000000., 1000000.);
      line -> SetLineWidth(2);
      line -> SetLineColor(kRed);
      line -> Draw("same");

      delete ratioHisto;
    }
}
