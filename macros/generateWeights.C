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
	  std::cout << "+++ Added file: " << fileList << std::endl;
	}
    }
  return chain;
}





void generateWeights(std::string dataFileList, std::string mcFileList)
{
  gROOT->SetBatch(kTRUE);
  TFile* outFile = new TFile("pileUpWeights.root","RECREATE");
  
  TTree* dataTree = loadChain(dataFileList);
  TTree* mcTree = loadChain(mcFileList);

  //book histo
  TH1F* h_puData = new TH1F("h_puData","h_puData",100,0,100);
  TH1F* h_puMC = new TH1F("h_puMC","h_puMC",100,0,100);

  TH1F* h_weights = new TH1F("h_weights","h_weights",100,0,100);

  dataTree->Draw("pileup >> h_puData","","");
  mcTree->Draw("pileup >> h_puMC","","");

  for(unsigned int bin=1; bin<h_puData->GetNbinsX(); ++bin)
    {
      int num = h_puData->GetBinContent(bin);
      int den = h_puMC->GetBinContent(bin);
      h_weights->SetBinContent(bin, den == 0 ? 0 : num/den);
    }
  
  h_weights->Write();
  h_puData->Write();
  h_puMC->Write();
  outFile->Close();
  return;
}
