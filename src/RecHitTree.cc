// -*- C++ -*-
//
// Package:    METPlots/RecHitTree
// Class:      RecHitTree
// 
/**\class RecHitTree RecHitTree.cc METPlots/RecHitTree/plugins/RecHitTree.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  kenneth call
//         Created:  Wed, 15 Jul 2015 16:05:14 GMT
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
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalSourcePositionData.h"

#include <DataFormats/EcalDetId/interface/EBDetId.h>
#include <DataFormats/EcalDetId/interface/EEDetId.h>
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
 
#include "DataFormats/HcalDetId/interface/HcalElectronicsId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
 
#include <vector>
#include <utility>
#include <ostream>
#include <string>
#include <algorithm>
#include <cmath>
 
#include "DataFormats/DetId/interface/DetId.h"
// channel status
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
 
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/DataRecord/interface/HcalChannelQualityRcd.h"
 
// severity level assignment for HCAL
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"
 
// severity level assignment for ECAL
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
//All include files from HcalRecHitsValidation

// system include files
#include <memory>

// user include files
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalSourcePositionData.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/DataRecord/interface/HcalChannelQualityRcd.h"

// severity level assignment for HCAL
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"

#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>

#include <vector>
#include <utility>
#include <ostream>
#include <string>
#include <algorithm>

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

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------

  edm::EDGetTokenT<HBHERecHitCollection> tok_hbhe_;
//  edm::EDGetTokenT<HORecHitCollection> tok_ho_;
  edm::EDGetTokenT<HFRecHitCollection> tok_hf_;

//  edm::EDGetTokenT<reco::VertexCollection> tok_vertexc_;
//  edm::EDGetTokenT<CaloTowerCollection> tok_calo_;

  TTree *tt1;

  //int nrechits;
  //int nvertx;

  // run:lumi:event
  int run;
  int lumi;
  int event;

  //std::vector<float> recHitEn_HB;
  //std::vector<float> recHitEn_HE;
  //std::vector<float> recHitEn_HF;
  //std::vector<float> recHitEn_HO;  

  std::vector<float> recHitHB_En;
  std::vector<float> recHitHB_Eaux;
  std::vector<float> recHitHB_Chi2;
  std::vector<float> recHitHB_EnRAW;
  std::vector<int>   recHitHB_ieta;
  std::vector<int>   recHitHB_iphi;
  std::vector<float> recHitHB_time;
  std::vector<int>   recHitHB_depth;

  std::vector<float> recHitHE_En;
  std::vector<float> recHitHE_Eaux;
  std::vector<float> recHitHE_Chi2;
  std::vector<float> recHitHE_EnRAW;
  std::vector<int>   recHitHE_ieta;
  std::vector<int>   recHitHE_iphi;
  std::vector<float> recHitHE_time;
  std::vector<int>   recHitHE_depth;

  std::vector<float> recHitHF_En;
  std::vector<float> recHitHF_EnRAW;
  std::vector<int>   recHitHF_ieta;
  std::vector<int>   recHitHF_iphi;
  std::vector<float> recHitHF_time;
  std::vector<int>   recHitHF_depth;


   // for checking the status of ECAL and HCAL channels stored in the DB 
  //const HcalChannelQuality* theHcalChStatus;
  // calculator of severety level for HCAL
  //const HcalSeverityLevelComputer* theHcalSevLvlComputer;
  //int hcalSevLvl(const CaloRecHit* hit);

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
RecHitTree::RecHitTree(const edm::ParameterSet& iConfig)

{
  //Collections
  tok_hbhe_ = consumes<HBHERecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HBHERecHitCollectionLabel"));
  tok_hf_  = consumes<HFRecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HFRecHitCollectionLabel"));
  //tok_ho_ = consumes<HORecHitCollection>(iConfig.getUntrackedParameter<edm::InputTag>("HORecHitCollectionLabel"));
  //tok_vertexc_ = consumes<reco::VertexCollection>(iConfig.getUntrackedParameter<edm::InputTag>("VertexCollectionLabel"));
  //tok_calo_ = consumes<CaloTowerCollection>(iConfig.getUntrackedParameter<edm::InputTag>("CaloTowerCollectionLabel"));
  
  //now do what ever initialization is needed

  //Now we use the modification so that we can use the TFileService
  edm::Service<TFileService> fs;

  tt1 = fs->make<TTree>("RecHitTree","RecHitTree");

  //branches

  //tt1->Branch("nRecHits", &nrechits, "nRecHits/I");
  //tt1->Branch("nVertices", &nvertx, "nVertices/I");

  tt1->Branch("run", &run, "run/I");
  tt1->Branch("lumi", &lumi, "lumi/I");
  tt1->Branch("event", &event, "event/I");

  //tt1->Branch("recHitEn_HB","std::vector<float>", &recHitEn_HB, 32000, 0);
  //tt1->Branch("recHitEn_HE","std::vector<float>", &recHitEn_HE, 32000, 0);
  //tt1->Branch("recHitEn_HF","std::vector<float>", &recHitEn_HF, 32000, 0);
  //tt1->Branch("recHitEn_HO","std::vector<float>", &recHitEn_HO, 32000, 0);

  tt1->Branch("recHitHB_En","std::vector<float>", &recHitHB_En, 32000, 0);
  tt1->Branch("recHitHB_Eaux","std::vector<float>", &recHitHB_Eaux, 32000, 0);
  tt1->Branch("recHitHB_Chi2","std::vector<float>", &recHitHB_Chi2, 32000, 0);
  tt1->Branch("recHitHB_EnRAW","std::vector<float>", &recHitHB_EnRAW, 32000, 0);
  tt1->Branch("recHitHB_ieta","std::vector<int>", &recHitHB_ieta, 32000, 0);
  tt1->Branch("recHitHB_iphi","std::vector<int>", &recHitHB_iphi, 32000, 0);
  tt1->Branch("recHitHB_depth","std::vector<int>", &recHitHB_depth, 32000, 0);
  tt1->Branch("recHitHB_time","std::vector<float>", &recHitHB_time, 32000, 0);

  tt1->Branch("recHitHE_En","std::vector<float>", &recHitHE_En, 32000, 0);
  tt1->Branch("recHitHE_Eaux","std::vector<float>", &recHitHE_Eaux, 32000, 0);
  tt1->Branch("recHitHE_Chi2","std::vector<float>", &recHitHE_Chi2, 32000, 0);
  tt1->Branch("recHitHE_EnRAW","std::vector<float>", &recHitHE_EnRAW, 32000, 0);
  tt1->Branch("recHitHE_ieta","std::vector<int>", &recHitHE_ieta, 32000, 0);
  tt1->Branch("recHitHE_iphi","std::vector<int>", &recHitHE_iphi, 32000, 0);
  tt1->Branch("recHitHE_depth","std::vector<int>", &recHitHE_depth, 32000, 0);
  tt1->Branch("recHitHE_time","std::vector<float>", &recHitHE_time, 32000, 0);

  tt1->Branch("recHitHF_En","std::vector<float>", &recHitHF_En, 32000, 0);
  tt1->Branch("recHitHF_EnRAW","std::vector<float>", &recHitHF_EnRAW, 32000, 0);
  tt1->Branch("recHitHF_ieta","std::vector<int>", &recHitHF_ieta, 32000, 0);
  tt1->Branch("recHitHF_iphi","std::vector<int>", &recHitHF_iphi, 32000, 0);
  tt1->Branch("recHitHF_depth","std::vector<int>", &recHitHF_depth, 32000, 0);
  tt1->Branch("recHitHF_time","std::vector<float>", &recHitHF_time, 32000, 0);

  //tt1->Branch("recHitHO_En","std::vector<float>", &recHitHO_En, 32000, 0);
  //tt1->Branch("recHitHO_EnRAW","std::vector<float>", &recHitHO_EnRAW, 32000, 0);
  //tt1->Branch("recHitHO_ieta","std::vector<int>", &recHitHO_ieta, 32000, 0);
  //tt1->Branch("recHitHO_iphi","std::vector<int>", &recHitHO_iphi, 32000, 0);
  //tt1->Branch("recHitHO_time","std::vector<float>", &recHitHO_time, 32000, 0);

  //tt1->Branch("caloTower_HadEt","std::vector<float>", &caloTower_HadEt, 32000, 0);
  //tt1->Branch("caloTower_EMEt","std::vector<float>", &caloTower_EmEt, 32000, 0);
  //tt1->Branch("caloTower_ieta","std::vector<int>", &caloTower_ieta, 32000, 0);
  //tt1->Branch("caloTower_iphi","std::vector<int>", &caloTower_iphi, 32000, 0);


  //tt1->Branch("recHitEn_HB2","std::Vector<float>", &recHitEn_HB2, 32000, 0);

  //tt1->Branch("recHitEn_HE1","std::vector<float>", &recHitEn_HE1, 32000, 0);
  //tt1->Branch("recHitEn_HE2","std::Vector<float>", &recHitEn_HE2, 32000, 0);
  //tt1->Branch("recHitEn_HE3","std::Vector<float>", &recHitEn_HE3, 32000, 0);

  //tt1->Branch("recHitEn_HF1", "std::vector<float>", &recHitEn_HF1, 32000, 0);
  //tt1->Branch("recHitEn_HF2", "std::vector<float>", &recHitEn_HF2, 32000, 0);


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


   // HCAL channel status map ****************************************
   
   //edm::ESHandle<HcalChannelQuality> hcalChStatus;
   //iSetup.get<HcalChannelQualityRcd>().get( "withTopo", hcalChStatus ); 

   //theHcalChStatus = hcalChStatus.product();
 
   // Assignment of severity levels **********************************
   //edm::ESHandle<HcalSeverityLevelComputer> hcalSevLvlComputerHndl;
   //iSetup.get<HcalSeverityLevelComputerRcd>().get(hcalSevLvlComputerHndl);
   //theHcalSevLvlComputer = hcalSevLvlComputerHndl.product(); 
   

   //Clear out the RecHit vectors
   //recHitEn_HB.clear();
   //recHitEn_HE.clear();
   //recHitEn_HF.clear();
   //recHitEn_HO.clear();

   //recHitEn_HB1.clear();
   //recHitEn_HE1.clear();
   //recHitEn_HF1.clear();

   //recHitEn_HB2.clear();
   //recHitEn_HE2.clear();
   //recHitEn_HF2.clear();

   //recHitEn_HE3.clear();

   recHitHB_En.clear();
   recHitHB_Eaux.clear();
   recHitHB_Chi2.clear();
   recHitHB_EnRAW.clear();
   recHitHB_ieta.clear();
   recHitHB_iphi.clear();
   recHitHB_depth.clear();
   recHitHB_time.clear();

   recHitHE_En.clear();
   recHitHE_Eaux.clear();
   recHitHE_Chi2.clear();
   recHitHE_EnRAW.clear();
   recHitHE_ieta.clear();
   recHitHE_iphi.clear();
   recHitHE_depth.clear();
   recHitHE_time.clear();

   recHitHF_En.clear();
   recHitHF_EnRAW.clear();
   recHitHF_ieta.clear();
   recHitHF_iphi.clear();
   recHitHF_depth.clear();
   recHitHF_time.clear();

   //recHitHO_En.clear();
   //recHitHO_EnRAW.clear();
   //recHitHO_ieta.clear();
   //recHitHO_iphi.clear();
   //recHitHO_time.clear();

   //caloTower_HadEt.clear();
   //caloTower_EmEt.clear();
   //caloTower_ieta.clear();
   //caloTower_iphi.clear();

   //run:lumi:event
   run = iEvent.id().run();
   lumi = iEvent.id().luminosityBlock();
   event = iEvent.id().event();

   //Clear the nvertx
   //nvertx = 0;
   //nrechits = 0;

   //Good Vertices
   //edm::Handle<VertexCollection> vertexcoll;
   //iEvent.getByToken(tok_vertexc_, vertexcoll);

   //for(VertexCollection::const_iterator vitr = vertexcoll->begin(); vitr != vertexcoll->end(); vitr++){
     //if(vitr->isFake()) continue;
     //if(vitr->ndof() <= 4) continue;
     //if(vitr->z() > 24) continue;
     //if(vitr->z() < -24) continue;
     //if(vitr->position().Rho()>=2) continue;
     //if(vitr->position().Rho()<=-2) continue;

     //nvertx++;
     
   //} //Good Vertices

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

   int depth = 0;
   int severityLevel = 0;

   for(HBHERecHitCollection::const_iterator j=hbhecoll->begin(); j != hbhecoll->end(); j++){
     HcalDetId cell(j->id());
     depth = cell.depth();
     //severityLevel = hcalSevLvl( (CaloRecHit*) &*j );
     //if(severityLevel > 8) continue;
     
     if(cell.subdet() == HcalBarrel){
       //nrechits++;
       //recHitEn_HB.push_back(j->energy());
         recHitHB_En.push_back(j->energy());
         recHitHB_Eaux.push_back(j->eaux());
         recHitHB_Chi2.push_back(j->chi2());
	 recHitHB_EnRAW.push_back(j->eraw());
	 recHitHB_ieta.push_back(cell.ieta());
	 recHitHB_iphi.push_back(cell.iphi());
	 recHitHB_depth.push_back(depth);
	 recHitHB_time.push_back(j->time());
       					    
     }//HB

     if(cell.subdet() == HcalEndcap){
       //nrechits++;
       //recHitEn_HE.push_back(j->energy());
	 recHitHE_En.push_back(j->energy());
         recHitHE_Eaux.push_back(j->eaux());
         recHitHE_Chi2.push_back(j->chi2());
	 recHitHE_EnRAW.push_back(j->eraw());
	 recHitHE_ieta.push_back(cell.ieta());
	 recHitHE_iphi.push_back(cell.iphi());
	 recHitHE_depth.push_back(depth);
	 recHitHE_time.push_back(j->time());
     }//HE     
       
   } //HBHE

   //HF RecHits
   edm::Handle<HFRecHitCollection> hfcoll;
   iEvent.getByToken(tok_hf_, hfcoll);
   
   for( HFRecHitCollection::const_iterator j = hfcoll->begin(); j != hfcoll->end(); j++){
     HcalDetId cell(j->id());
     depth = cell.depth();
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

     //nrechits++;
     //recHitEn_HF.push_back(j->energy());
     recHitHF_En.push_back(j->energy());
     //recHitHF_EnRAW.push_back(j->eraw());
     recHitHF_ieta.push_back(cell.ieta());
     recHitHF_iphi.push_back(cell.iphi());
     recHitHF_depth.push_back(depth);
     recHitHF_time.push_back(j->time());
     
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
   tt1->Fill();
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

// ------------ method called when starting to processes a run  ------------
/*
void 
RecHitTree::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecHitTree::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecHitTree::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecHitTree::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecHitTree::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
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
