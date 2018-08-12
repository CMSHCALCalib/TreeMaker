#ifndef UTILITY_BXPUINFO_HH
#define UTILITY_BXPUINFO_HH

//simple class to give per bx expected pileup  reading in from a previously generated file
//Author: Sam Harper (RAL), Aug 2018

#include "TROOT.h"

#include <unordered_map>
#include <array>

class BXPUInfo {
public:
  static constexpr int kNrBX = 3564;
  using BXData=std::array<char,kNrBX+1>;
  
  class RunData{
  private:
    int runnr_;
    std::unordered_map<int,BXData> lumiData_;
    
  public:
    RunData(int runnr=0):runnr_(runnr){}
    const BXData* getBXData(int lumiSec)const;
    void addBXData(int lumiSec,BXData&& bxData);
  };

private: 
  std::unordered_map<int,RunData> runData_;
  
public:
  BXPUInfo(){}
  virtual ~BXPUInfo(){}
  int exptNrPUInt(int runnr,int lumiSec,int bx)const;
  std::pair<int,int> nrInTrain(int runnr,int lumiSec,int bx)const;
  int nrBXFromStartOfTrain(int runnr,int lumiSec,int bx)const;
  int nrBXFromEndOfTrain(int runnr,int lumiSec,int bx)const;
  const BXData* getBXData(int runnr,int lumiSec)const;
  void addBXData(int runnr,int lumiSec,BXData bxData);
  void addBXData(int runnr,int lumiSec,const std::vector<float>& bxData); //for easier python access

  //  bool isBXNthInTrain(int runnr,int lumiSec,int bx)const;

  ClassDef(BXPUInfo,1)

};

#endif
