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





void generateWeights(std::string dataFileList, std::string mcFileList)
{
  gROOT->SetBatch(kTRUE);
  TFile* outFile = new TFile("pileUpWeights_calc.root","RECREATE");
  
  TFile* f_pileupProfile = TFile::Open(dataFileList.c_str());
  TTree* mcTree = loadChain(mcFileList);
  

  //book histo
  TH1F* h_puData = (TH1F*)f_pileupProfile->Get("h_pileupGlo");
  TH1F* h_puMC = new TH1F("h_puMC","h_puMC",100,0,100);

  TH1F* h_weights = new TH1F("h_weights","h_weights",100,0,100);

  //  dataTree->Draw("pileup >> h_puData","","");
  mcTree->Draw("pileup >> h_puMC","","");

  h_puData->Scale(1/h_puData->Integral());
  h_puMC->Scale(1/h_puMC->Integral());

  for(unsigned int bin=1; bin<h_puData->GetNbinsX(); ++bin)
    {
      float num = h_puData->GetBinContent(bin);
      float den = h_puMC->GetBinContent(bin);
      h_weights->SetBinContent(bin, den == 0 ? 0 : num/den);

      std::cout << bin << " data = " << num << " MC = " << den << std::endl;
    }
  
  outFile->cd();
  h_weights->Write();
  h_puData->Write();
  h_puMC->Write();
  outFile->Close();
  return;
}
