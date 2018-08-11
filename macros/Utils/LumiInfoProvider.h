#ifndef LUMIINFOPROVIDER_H
#define LUMIINFOPROVIDER_H

//Simple parser for the pileup json file which converts it into expected #pileup events
//Author: Sam Harper (RAL), August 2018

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

class PUInfo{
private:

  int runnr_;
  std::unordered_map<int,float> data_;
 
public:
  explicit PUInfo(int runnr):runnr_(runnr){}
  bool operator<(const PUInfo& rhs)const{return runnr_<rhs.runnr_;}
  bool operator<(const int rhs)const{return runnr_<rhs;}
  friend bool operator<(const int lhs,const PUInfo& rhs){return lhs<rhs.runnr_;}

  void fill(const boost::property_tree::ptree& puInfoTree){
    const auto& runData = puInfoTree.get_child(std::to_string(runnr_));
    for(auto& entry : runData){
      int index=0;
      int lumiSec=0;
      float instLumiPerBunch=0;
      for(auto& val : entry.second){
	if(index==0) lumiSec = val.second.get_value<float>();
	if(index==3) instLumiPerBunch = val.second.get_value<float>();
	index++;
      }
      if(lumiSec>0){
	data_.insert({lumiSec,instLumiPerBunch});
      }
    }
  }
  float getPerBunchLumi(int lumiSec)const{
    auto entry = data_.find(lumiSec);
    if(entry != data_.end()){
      return entry->second;
    }else return -1;
  }
};



class LumiInfoProvider {
private:
  std::vector<PUInfo> puInfo_;
  static constexpr float kMinBiasXsec_=69200;
public:  
  LumiInfoProvider(const std::string& puJsonName){
    boost::property_tree::ptree puTree;
    boost::property_tree::read_json(puJsonName,puTree);
    for(auto& entry : puTree){
      //  std::cout <<"entry "<<entry.first<<" : "<<entry.second.get_value<float>()<<std::endl; 
      
      puInfo_.emplace_back(PUInfo(std::stoi(entry.first)));
    }
    std::sort(puInfo_.begin(),puInfo_.end());
    for(auto& info : puInfo_) info.fill(puTree);
  }
  
  float getNrExptPUInt(int runnr,int lumiSec)const{
    auto res = std::equal_range(puInfo_.begin(),puInfo_.end(),runnr);
    if(res.first==res.second){
      std::cout <<"PrescaleProvider::prescale : warning run "<<runnr<<" not found "<<std::endl;
      return -1;
    }else{
      if(std::distance(res.first,res.second)>1) std::cout <<"PrescaleProvider::prescale : warning run "<<runnr<<" has multiple entries, this should not be possible "<<std::endl;
      return res.first->getPerBunchLumi(lumiSec)*kMinBiasXsec_;
    }
  }
    
};

#endif
