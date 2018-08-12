#include "interface/HistoFactory.h"

std::string getPartition(int& sub, int& ieta)
{
  std::string partition = "";

  if(sub == 1)
    partition = "HB";
  if(sub == 2)
    partition = "HE";
  if(sub == 4)
    partition = "HF";

  if(ieta < 0)
    partition += "M";
  else
    partition += "P";

  return partition;
}

std::pair<int,int> findBin(int& val,std::vector<int>& vec)
{
  int binMin = -1;
  int binMax = -1;
  for(unsigned int bin = 1; bin<vec.size(); ++bin)
    if(val <= vec[bin])
      {
	binMin = vec[bin-1];
	binMax = vec[bin];
	break;
      }

  return (std::make_pair(abs(binMin),abs(binMax)));
}

void HistoFactory::initHisto(std::string hOrigName,
			     std::string hType,
			     int nBins, int min, int max,
			     std::string hTitleX, std::string hTitleY)
{
  outFile_->cd();
  std::unordered_map<std::string,TH1*> tempMap;

  for(auto isub=subVec_.begin(); isub!=subVec_.end(); ++isub)
    for(unsigned int depth=1; depth<depthNum_+1; ++depth)
      for(unsigned int ieta=1; ieta<ietaVec_.size(); ++ieta)
	for(unsigned int ipu=1; ipu<puVec_.size(); ++ipu)
	  {
	    std::string hName = "";

	    if(hType.find("Sub") != std::string::npos)
	      hName += "_"+*isub;
	    
	    if(hType.find("Depth") != std::string::npos)
	      {
		//don't book non-existing detphs
		if(((isub->find("HB") != std::string::npos) || (isub->find("HF") != std::string::npos)) && depth > 2)
		  continue;	       
		hName += "_d"+std::to_string(depth);
	      }
	    
	    if(hType.find("Ieta") != std::string::npos)
	      {
		//create ieta bins for HE only
		if(isub->find("HE") != std::string::npos)
		  hName += "_ieta"+std::to_string(ietaVec_.at(ieta-1))+"-"+std::to_string(ietaVec_.at(ieta));
	      }
	    
	    if(hType.find("Pu") != std::string::npos)
	      {
		//create pu bins
		std::string puSuffix = "_pu"+std::to_string(puVec_.at(ipu-1))+"-"+std::to_string(puVec_.at(ipu));
		hName += puSuffix;

		TDirectory* dir = outFile_->GetDirectory(puSuffix.c_str());
		if(!dir)
		  dir = outFile_->mkdir(puSuffix.c_str());
		dir->cd();
	      }
		
	    if(tempMap.find(hName) == tempMap.end())
	      {
		tempMap[hName] = bookHisto(hOrigName+hName, nBins, min, max, hTitleX, hTitleY);
		if(debug_) std::cout << "HistoFactory::initHisto ==>> hName: " << hOrigName+hName << std::endl;
	      }
	  }

  //fill the histo containers
  hMap_[hOrigName] = tempMap;
  tMap_[hOrigName] = hType;
}

TH1* HistoFactory::bookHisto(std::string hName,
			     int nBins, int min, int max,
			     std::string hTitleX, std::string hTitleY)
{
  TH1* hTemp;
  if(hTitleY != "")
    {
      hTemp = new TProfile(hName.c_str(), hName.c_str(), nBins, min, max);
      hTemp->GetXaxis()->SetTitle(hTitleX.c_str());
      hTemp->GetYaxis()->SetTitle(hTitleY.c_str());
    }
  else
    {
      hTemp = new TH1D(hName.c_str(), hName.c_str(), nBins, min, max);
      hTemp->GetXaxis()->SetTitle(hTitleX.c_str());
      hTemp->GetYaxis()->SetTitle("Entries");
    } 

  return hTemp;
}

void HistoFactory::fill(std::string hOrigName, std::string hName, double xx, double yy, double ww)
{
  if(debug_ && hMap_.find(hOrigName) == hMap_.end())
    {
      std::cout << "==>> " << hOrigName << " ==>> HISTOGRAM NOT BOOKED <<==" << std::endl;
      return;
    }
  
  if(debug_) std::cout << "HistoFactory::fill ==>  hName: " << hOrigName+hName
		       << " xx: " << xx
		       << " yy: " << yy
		       << " ww: " << ww << std::endl;
  
  if(std::string(hMap_[hOrigName][hName]->GetYaxis()->GetTitle()) != "Entries")
    hMap_[hOrigName][hName]->Fill(xx, yy*ww); //TProfile
  else
    hMap_[hOrigName][hName]->Fill(xx, ww);    //TH1D

}

void HistoFactory::fill(std::string hOrigName, int pu, int det, int depth, int ieta, double xx, double yy, double ww)
{
  if(debug_ && hMap_.find(hOrigName) == hMap_.end())
    {
      std::cout << "==>> " << hOrigName << " ==>> HISTOGRAM NOT BOOKED <<==" << std::endl;
      return;      
    }

  std::string hName = "";

  std::string sub = getPartition(det, ieta);
  if(tMap_[hOrigName].find("Sub") != std::string::npos)
    hName += "_"+sub;

  if(tMap_[hOrigName].find("Depth") != std::string::npos)
    hName += "_d"+std::to_string(depth);

  if(tMap_[hOrigName].find("Ieta") != std::string::npos && sub.find("HE") != std::string::npos)
      {
	std::pair<int,int> boundIeta = findBin(ieta,ietaVec_);
	hName += "_ieta"+std::to_string(boundIeta.first)+"-"+std::to_string(boundIeta.second);
      }

  if(tMap_[hOrigName].find("Pu") != std::string::npos)
    {
      std::pair<int,int> boundPu = findBin(pu,puVec_);
      hName += "_pu"+std::to_string(boundPu.first)+"-"+std::to_string(boundPu.second);
    }

  
  if(debug_) std::cout << "HistoFactory::fill ==>  hName: " << hOrigName+hName
		       << " pu: " << pu
		       << " det: " << det
		       << " depth: " << depth
		       << " ieta: " << ieta
		       << " xx: " << xx
		       << " yy: " << yy
		       << " ww: " << ww << std::endl;

  if(std::string(hMap_[hOrigName][hName]->GetYaxis()->GetTitle()) != "Entries")
    hMap_[hOrigName][hName]->Fill(xx, yy*ww); //TProfile
  else
    hMap_[hOrigName][hName]->Fill(xx, ww);    //TH1D
}
