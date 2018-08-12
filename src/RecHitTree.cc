// -*- C++ -*-
//
// Original Author:  Federico De Guio
//         Created:  Fri, 20 Jul 2018
//
//

//Include files to use TFileService
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//All include files from HcalRecHitsValidation
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Scalers/interface/LumiScalers.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h" 


// Hcal related stuff
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "CondFormats/DataRecord/interface/HcalChannelQualityRcd.h"
// severity level assignment for HCAL
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include <vector>
#include <utility>
#include <ostream>
#include <string>
#include <algorithm>
#include <cmath>

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>


//CaloTowers
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"

//
// class declaration
//

class RecHitTree : public edm::EDAnalyzer {
public:
  explicit RecHitTree(const edm::ParameterSet&);
  ~RecHitTree();
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  void fillHltResults(const edm::Handle<edm::TriggerResults> &, 
		      const edm::TriggerNames &);
  
  // ----------member data ---------------------------
  int isData;
  edm::EDGetTokenT<HBHERecHitCollection> tok_hbhe_;
  edm::EDGetTokenT<HORecHitCollection> tok_ho_;
  edm::EDGetTokenT<HFRecHitCollection> tok_hf_;

  edm::EDGetTokenT<LumiScalersCollection> tok_scal_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>> tok_puinfo_;

  edm::EDGetTokenT<edm::TriggerResults>   tok_triggerResults_;
  std::vector<std::string> hltPaths_;


  TTree *tt;

  int run;
  int lumi;
  int event;
  int bx;
  int pileup;

  std::vector<float>* recHitEn;
  std::vector<float>* recHitEaux;
  std::vector<float>* recHitEnRAW;
  std::vector<float>* recHitChi2;
  std::vector<int>*   recHitIEta;
  std::vector<int>*   recHitIPhi;
  std::vector<float>* recHitTime;
  std::vector<int>*   recHitDepth;
  std::vector<int>*   recHitFlags;
  std::vector<int>*   recHitSub;

  std::vector<int>* hltAccept;
  std::vector<int>* hltWasrun;
  std::vector<std::string>* hltNames;
};


void RecHitTree::fillHltResults(const edm::Handle<edm::TriggerResults>   & triggerResults, 
				const edm::TriggerNames                  & triggerNames  )
{
  
  for (unsigned int itrig=0; itrig < triggerNames.size(); ++itrig) 
    for (unsigned int name=0; name < hltPaths_.size(); ++name)
      if(triggerNames.triggerName(itrig).find(hltPaths_[name]) != std::string::npos)
	{
	  hltNames->push_back(triggerNames.triggerName(itrig));
	  hltAccept->push_back(triggerResults->accept(itrig));
	  hltWasrun->push_back(triggerResults->wasrun(itrig));
	  break;
	}
}


//
// constructors and destructor
//
RecHitTree::RecHitTree(const edm::ParameterSet& iConfig)
{
  isData = iConfig.getUntrackedParameter<uint>("isData");
  //Collections
  tok_hbhe_ = consumes<HBHERecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HBHERecHitCollectionLabel"));
  tok_hf_  = consumes<HFRecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HFRecHitCollectionLabel"));
  //  tok_ho_ = consumes<HORecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HORecHitCollectionLabel"));
  //  tok_calo_ = consumes<CaloTowerCollection>(iConfig.getUntrackedParameter<edm::InputTag>("CaloTowerCollectionLabel"));
  tok_scal_  = consumes<LumiScalersCollection>(iConfig.getUntrackedParameter<edm::InputTag>("LumiScalersCollectionLabel"));
  tok_puinfo_= consumes<std::vector<PileupSummaryInfo>>(iConfig.getUntrackedParameter<edm::InputTag>("PileUpInfoLabel"));
  tok_triggerResults_ = consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("triggerResults"));
  hltPaths_           = iConfig.getUntrackedParameter<std::vector<std::string> >("hltPaths");


  
  //Now we use the modification so that we can use the TFileService
  edm::Service<TFileService> fs;

  tt = fs->make<TTree>("RecHitTree","RecHitTree");

  tt->Branch("run", &run, "run/I");
  tt->Branch("lumi", &lumi, "lumi/I");
  tt->Branch("event", &event, "event/I");
  tt->Branch("bx",    &bx, "bx/I");
  tt->Branch("pileup", &pileup, "pileup/I");

  recHitEn   = new std::vector<float>;
  recHitEaux = new std::vector<float>;
  recHitChi2 = new std::vector<float>;
  recHitEnRAW = new std::vector<float>;
  recHitIEta = new std::vector<int>;
  recHitIPhi = new std::vector<int>;
  recHitDepth = new std::vector<int>;
  recHitTime = new std::vector<float>;
  recHitFlags = new std::vector<int>;
  recHitSub = new std::vector<int>;
  hltNames = new std::vector<std::string>;
  hltAccept = new std::vector<int>;
  hltWasrun = new std::vector<int>;
  tt->Branch("recHitEn","std::vector<float>", &recHitEn);
  tt->Branch("recHitEaux","std::vector<float>", &recHitEaux);
  tt->Branch("recHitChi2","std::vector<float>", &recHitChi2);
  tt->Branch("recHitEnRAW","std::vector<float>", &recHitEnRAW);
  tt->Branch("recHitIEta","std::vector<int>", &recHitIEta);
  tt->Branch("recHitIPhi","std::vector<int>", &recHitIPhi);
  tt->Branch("recHitDepth","std::vector<int>", &recHitDepth);
  tt->Branch("recHitTime","std::vector<float>", &recHitTime);
  tt->Branch("recHitFlags","std::vector<int>", &recHitFlags);
  tt->Branch("recHitSub","std::vector<int>", &recHitSub);
  tt->Branch("hltNames",  "std::vector<std::string>", &hltNames);
  tt->Branch("hltAccept", "std::vector<int>", &hltAccept);
  tt->Branch("hltWasrun", "std::vector<int>", &hltWasrun);
}


RecHitTree::~RecHitTree()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

//
// member functions
//

// ------------ method called for each event  ------------
void
RecHitTree::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;

   //clear vectors
   recHitEn->clear();
   recHitEaux->clear();
   recHitChi2->clear();
   recHitEnRAW->clear();
   recHitIEta->clear();
   recHitIPhi->clear();
   recHitDepth->clear();
   recHitTime->clear();
   recHitFlags->clear();
   recHitSub->clear();
   hltNames->clear();
   hltAccept->clear();
   hltWasrun->clear();   

   // HCAL channel status map ****************************************
   
   //edm::ESHandle<HcalChannelQuality> hcalChStatus;
   //iSetup.get<HcalChannelQualityRcd>().get( "withTopo", hcalChStatus ); 

   //theHcalChStatus = hcalChStatus.product();
 
   // Assignment of severity levels **********************************
   //edm::ESHandle<HcalSeverityLevelComputer> hcalSevLvlComputerHndl;
   //iSetup.get<HcalSeverityLevelComputerRcd>().get(hcalSevLvlComputerHndl);
   //theHcalSevLvlComputer = hcalSevLvlComputerHndl.product(); 
   

   run = iEvent.id().run();
   lumi = iEvent.id().luminosityBlock();
   event = iEvent.id().event();
   bx = iEvent.bunchCrossing();

   //fill HLT results
   edm::Handle<edm::TriggerResults>   triggerResults;
   iEvent.getByToken(tok_triggerResults_, triggerResults);
   edm::TriggerNames triggerNames = iEvent.triggerNames(*triggerResults);
   fillHltResults(triggerResults,triggerNames); 


   // Access PU info
   if(isData)
     {
       edm::Handle<LumiScalersCollection> scalcoll;
       iEvent.getByToken(tok_scal_, scalcoll);
       pileup = scalcoll->begin()->pileup();
     }
   else
     {
       edm::Handle<std::vector<PileupSummaryInfo>> pucoll;
       iEvent.getByToken(tok_puinfo_, pucoll);
       for (auto const& pu_info : *pucoll.product()) {
	 if (pu_info.getBunchCrossing() == 0) { //in-time PU
	   pileup = pu_info.getTrueNumInteractions();
	   //pileup = pu_info.getPU_NumInteractions();
	 }
       }
     }



   // CaloTowers
   //edm::Handle<CaloTowerCollection> towers;
   //iEvent.getByToken(tok_calo_,towers);
   //CaloTowerCollection::const_iterator cal;

   //for(cal = towers->begin(); cal != towers->end(); cal++){
   //  CaloTowerDetId idT = cal->id();
   //  caloTower_HadEt.push_back(cal->hadEt());
   //  caloTower_EmEt.push_back(cal->emEt());
   //  caloTower_ieta.push_back(idT.ieta());
   //  caloTower_iphi.push_back(idT.iphi());
   //}


   //HBHE RecHits
   edm::Handle<HBHERecHitCollection> hbhecoll;
   iEvent.getByToken(tok_hbhe_, hbhecoll);

   //   int severityLevel = 0;

   for(HBHERecHitCollection::const_iterator j=hbhecoll->begin(); j != hbhecoll->end(); j++){
     HcalDetId cell(j->id());
     //severityLevel = hcalSevLvl( (CaloRecHit*) &*j );
     //if(severityLevel > 8) continue;
     
     recHitEn->push_back(j->energy());
     recHitEaux->push_back(j->eaux());
     recHitChi2->push_back(j->chi2());
     recHitEnRAW->push_back(j->eraw());
     recHitIEta->push_back(cell.ieta());
     recHitIPhi->push_back(cell.iphi());
     recHitDepth->push_back(cell.depth());
     recHitTime->push_back(j->time());
     recHitFlags->push_back(j->flags());
     recHitSub->push_back(cell.subdet());
     
   } //HBHE

   //HF RecHits
   edm::Handle<HFRecHitCollection> hfcoll;
   iEvent.getByToken(tok_hf_, hfcoll);
   
   for( HFRecHitCollection::const_iterator j = hfcoll->begin(); j != hfcoll->end(); j++){
     HcalDetId cell(j->id());
     //severityLevel = hcalSevLvl( (CaloRecHit*) &*j );

     //ZS emulation
     //int auxwd = j->aux();
     //bool reject = true;

     //for(int TSidx = 0; TSidx < 3; TSidx++){
       //int TS2 = (auxwd >> (TSidx*7)) & 0x7F;
       //int TS3 = (auxwd >> (TSidx*7+7)) & 0x7F;
       //if(TS2+TS3 >= 10) reject = false;
     //}
     //if(reject) continue;
     //ZS emulation

     //if(severityLevel > 8) continue;


     recHitEn->push_back(j->energy());
     recHitEaux->push_back(-99);
     recHitChi2->push_back(-99);
     recHitEnRAW->push_back(-99);
     recHitIEta->push_back(cell.ieta());
     recHitIPhi->push_back(cell.iphi());
     recHitDepth->push_back(cell.depth());
     recHitTime->push_back(j->time());
     recHitFlags->push_back(j->flags());
     recHitSub->push_back(cell.subdet());
     
   } //HF

   //HO RecHits
   //edm::Handle<HORecHitCollection> hocoll;
   //iEvent.getByToken(tok_ho_, hocoll);
   //for( HORecHitCollection::const_iterator j = hocoll->begin(); j != hocoll->end(); j++){
     //HcalDetId cell(j->id());
     //severityLevel = hcalSevLvl( (CaloRecHit*) &*j );
     //if(severityLevel > 8) continue;
     //nrechits++;
     //recHitEn_HO.push_back(j->energy());
	 //recHitHO_En.push_back(j->energy());
	 //recHitHO_EnRAW.push_back(j->eraw());
	 //recHitHO_ieta.push_back(cell.ieta());
	 //recHitHO_iphi.push_back(cell.iphi());
	 //recHitHO_time.push_back(j->time());
       
   //} //HO

   //Fill Tree
   tt->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void 
RecHitTree::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecHitTree::endJob() 
{
}


//int 
//RecHitTree::hcalSevLvl(const CaloRecHit* hit){
 
//   const DetId id = hit->detid();

//   const uint32_t recHitFlag = hit->flags();
//   const uint32_t dbStatusFlag = theHcalChStatus->getValues(id)->getValue();

//   int severityLevel = theHcalSevLvlComputer->getSeverityLevel(id, recHitFlag, dbStatusFlag);

//   return severityLevel;

//}

//define this as a plug-in
DEFINE_FWK_MODULE(RecHitTree);
