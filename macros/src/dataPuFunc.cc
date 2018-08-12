#include "interface/dataPuFunc.h"

float getNrExpPU(int runnr,int lumiSec)
{
   
  static LumiInfoProvider lumiInfoProvider("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/PileUp/pileup_latest.txt");
  
  return lumiInfoProvider.getNrExptPUInt(runnr,lumiSec);
  
}

bool passPUCuts(int runnr,int lumiSec,int nrPUInt,int minPU,int maxPU)
{
  if(runnr>1){
    nrPUInt = std::floor(getNrExpPU(runnr,lumiSec)+0.5);
  }
  if(nrPUInt>=minPU && nrPUInt<=maxPU) return true;
  else return false;

}
float getNrExpPU(int runnr,int lumiSec,int bx)
{
   
  static const BXPUInfo* bxPUInfo = nullptr;
  if(!bxPUInfo){
    std::cout << "Open file bxPUInfo.root" << std::endl;
    TFile* file = TFile::Open("/afs/cern.ch/work/d/deguio/HCAL/dataMCcomparison/CMSSW_10_2_0_dataMCcomparison/src/CMSHCALCalib/TreeMaker/macros/Utils/bxPUInfo.root");
    bxPUInfo = (const BXPUInfo*)file->Get("bxPUInfo");
  }  
  return bxPUInfo->exptNrPUInt(runnr,lumiSec,bx);
  
}

int nrBXFromStartOfTrain(int runnr,int lumiSec,int bx)
{ 
  static const BXPUInfo* bxPUInfo = nullptr;
  if(!bxPUInfo){
    TFile* file = TFile::Open("/afs/cern.ch/work/d/deguio/HCAL/dataMCcomparison/CMSSW_10_2_0_dataMCcomparison/src/CMSHCALCalib/TreeMaker/macros/Utils/bxPUInfo.root");
    bxPUInfo = (const BXPUInfo*)file->Get("bxPUInfo");
  }  
  return bxPUInfo->nrBXFromStartOfTrain(runnr,lumiSec,bx);
}
int nrBXFromEndOfTrain(int runnr,int lumiSec,int bx)
{ 
  static const BXPUInfo* bxPUInfo = nullptr;
  if(!bxPUInfo){
    TFile* file = TFile::Open("/afs/cern.ch/work/d/deguio/HCAL/dataMCcomparison/CMSSW_10_2_0_dataMCcomparison/src/CMSHCALCalib/TreeMaker/macros/Utils/bxPUInfo.root");
    bxPUInfo = (const BXPUInfo*)file->Get("bxPUInfo");
  }  
  return bxPUInfo->nrBXFromEndOfTrain(runnr,lumiSec,bx);
}

bool passPUCuts(int runnr,int lumiSec,int bx,int nrPUInt,int minPU,int maxPU)
{
  if(runnr>1){
    nrPUInt = std::floor(getNrExpPU(runnr,lumiSec,bx)+0.5);
  }
  if(nrPUInt>=minPU && nrPUInt<=maxPU) return true;
  else return false;

}
