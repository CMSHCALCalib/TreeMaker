import FWCore.ParameterSet.Config as cms

recHitTree = cms.EDAnalyzer("RecHitTree",
                            isData = cms.untracked.uint32(1),
                            HBHERecHitCollectionLabel  = cms.untracked.InputTag("hbheprereco"),
                            HFRecHitCollectionLabel    = cms.untracked.InputTag("hfreco"),
                            LumiScalersCollectionLabel = cms.untracked.InputTag("scalersRawToDigi"),
                            PileUpInfoLabel            = cms.untracked.InputTag("addPileupInfo"),
                            triggerResults             = cms.untracked.InputTag("TriggerResults::HLT"),
                            hltPaths                   = cms.untracked.vstring('HLT_ZeroBias_v',
                                                                               'HLT_ZeroBias_part',
                                                                               'HLT_ZeroBias_FirstCollisionInTrain_v',
                                                                               'HLT_ZeroBias_IsolatedBunches_v',
                                                                               'HLT_ZeroBias_FirstBXAfterTrain_v',
                                                                               'HLT_ZeroBias_FirstCollisionAfterAbortGap_v',
                                                                               'HLT_ZeroBias_LastCollisionInTrain_v')
)
