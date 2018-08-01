import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras
import os

#---------------
# My definitions
#---------------

sourceTag = "PoolSource"         # for global runs
rawTag    = cms.InputTag('rawDataCollector')
era       = eras.Run2_2018
GT        = ""
infile    = []


options = VarParsing.VarParsing('analysis')
options.register('inputType',
                 'data',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "input type")

options.register('year',
                 2018,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "year")

options.register('doReco',
                 'OFF',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "do reco or not")

options.register('noiseFilter',
                 'OFF',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "activate noise filters") #necessary at least for HB

options.parseArguments()

print "YEAR = ", options.year, " TYPE = ", options.inputType, " RECO = ", options.doReco, " NOISEFILTER = ", options.noiseFilter


if options.year == 2017:
    era = eras.Run2_2017
    if options.inputType == 'data':
        GT = "100X_dataRun2_Prompt_v1"
        from CMSHCALCalib.TreeMaker.DataFileList2017 import fileList
        infile = fileList

    if options.inputType == 'MC':
        GT = '92X_upgrade2017_realistic_v10'
        from CMSHCALCalib.TreeMaker.MCFileList2017 import fileList
        infile = fileList

if options.year == 2018:
    if options.inputType == 'data':
        GT = "101X_dataRun2_Prompt_v11"
        from CMSHCALCalib.TreeMaker.DataFileList2018 import fileList
        infile = fileList

    if options.inputType == 'MC':
        GT = '101X_upgrade2018_realistic_v8'
        from CMSHCALCalib.TreeMaker.MCFileList2018 import fileList
        infile = fileList




#-----------------------------------
# Standard CMSSW Imports/Definitions
#-----------------------------------
process = cms.Process('MyTree',era)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = GT

#----------
#process.es_ascii = cms.ESSource(
#    'HcalTextCalibrations',
#    input = cms.VPSet(
#        cms.PSet(
#            object = cms.string('ElectronicsMap'),
#            file = cms.FileInPath("HCALemap_2018_data_MWGR1test.txt")
#            )
#        )
#    )
#process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

#-----------
# get latest response correction (mostly energy scale from Isotrack)
if options.inputType == 'data' and options.year == 2018:
    process.load("CondCore.DBCommon.CondDBSetup_cfi")
    process.es_pool = cms.ESSource("PoolDBESSource",
                                   process.CondDBSetup,
                                   timetype = cms.string('runnumber'),
                                   toGet = cms.VPSet(
                                       cms.PSet(record = cms.string("HcalRespCorrsRcd"),
                                                tag = cms.string("HcalRespCorrs_2018_v3.1_data")
                                                )
                                       ),
                                   connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                                   authenticationMethod = cms.untracked.uint32(0)
                                   )
    process.es_prefer_es_pool = cms.ESPrefer( "PoolDBESSource", "es_pool" )


#-----------
# Log output
#-----------
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = ''
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
    )


#-----------------
# Files to process
#-----------------
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
    )

process.source = cms.Source(
    sourceTag,
    fileNames = cms.untracked.vstring(infile)
    )
#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('293765:264-293765:9999')

process.TFileService = cms.Service('TFileService', fileName = cms.string("recHitTree_"+options.inputType+"_year_"+str(options.year)+"_doReco_"+options.doReco+"_noiseF_"+options.noiseFilter+".root"))


#-----------------------------------------
# CMSSW/Hcal Related Module import
#-----------------------------------------
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff")
process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")
process.load("EventFilter.ScalersRawToDigi.ScalersRawToDigi_cfi")
# Load relevant hcal noise modules
process.load("RecoMET.METProducers.hcalnoiseinfoproducer_cfi")
process.load("CommonTools.RecoAlgos.HBHENoiseFilter_cfi")
process.load("CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi")

# To apply filter decision in CMSSW as an EDFilter
process.hcalnoise.fillCaloTowers = cms.bool(False)
process.hcalnoise.fillTracks = cms.bool(False)
process.hcalnoise.recHitCollName = cms.string("hbheprereco")
process.ApplyBaselineHBHENoiseFilter = cms.EDFilter("BooleanFlagFilter",
    inputLabel = cms.InputTag("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"),
    reverseDecision = cms.bool(False)
)
#process.HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Tight")


#set digi and analyzer
process.hcalDigis.InputLabel = rawTag
process.load("CMSHCALCalib.TreeMaker.recHitTree_cfi")
if(options.inputType == 'MC'):
    process.recHitTree.isData = cms.untracked.uint32(0)

process.digiPath = cms.Path(
    process.scalersRawToDigi
    *process.hcalDigis
)

process.noiseFilter = cms.Path(
    process.hcalnoise                                                                                                                                                           
    *process.HBHENoiseFilterResultProducer                                                                                                                                       
    *process.ApplyBaselineHBHENoiseFilter     
)

process.recoPath = cms.Path(
    process.horeco
    *process.hfprereco
    *process.hfreco
    *process.hbheprereco
)

process.ntuple = cms.Path(
    process.recHitTree
)

process.schedule = cms.Schedule(process.ntuple) #default in case doReco==OFF

if options.year == 2017 and options.inputType == 'MC':
    process.recHitTree.HBHERecHitCollectionLabel  = cms.untracked.InputTag("hbhereco")
    process.hcalnoise.recHitCollName = cms.string("hbhereco")

if options.doReco == "ON" and options.noiseFilter == "ON":
    if options.year == 2017:
        process.recoPath.insert(100,process.hbheplan1)
        process.recHitTree.HBHERecHitCollectionLabel  = cms.untracked.InputTag("hbheplan1")
        process.hcalnoise.recHitCollName = cms.string("hbheplan1")

    process.recoPath.insert(100,process.hcalnoise*process.HBHENoiseFilterResultProducer*process.ApplyBaselineHBHENoiseFilter)
    process.schedule.insert(0,process.recoPath)
    process.schedule.insert(0,process.digiPath)

if options.doReco == "ON" and options.noiseFilter == "OFF":
    if options.year == 2017:
        process.recoPath.insert(100,process.hbheplan1)
        process.recHitTree.HBHERecHitCollectionLabel  = cms.untracked.InputTag("hbheplan1")
        process.hcalnoise.recHitCollName = cms.string("hbheplan1")

    process.schedule.insert(0,process.recoPath)
    process.schedule.insert(0,process.digiPath)

