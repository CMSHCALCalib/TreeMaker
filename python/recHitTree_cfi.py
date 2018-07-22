import FWCore.ParameterSet.Config as cms

recHitTree = cms.EDAnalyzer("RecHitTree",
                            isData = cms.untracked.uint32(1),
                            HBHERecHitCollectionLabel = cms.untracked.InputTag("hbheprereco"),
                            HFRecHitCollectionLabel   = cms.untracked.InputTag("hfreco"),
                            LumiScalersCollectionLabel = cms.untracked.InputTag("scalersRawToDigi"),
                            PileUpInfoLabel = cms.untracked.InputTag("addPileupInfo")
                            )
