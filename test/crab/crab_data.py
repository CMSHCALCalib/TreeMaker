from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.workArea = 'crab3Submission_Data_2307'
config.General.requestName = ''

config.section_('JobType')
config.JobType.psetName = '../recHitTree_cfg.py'
config.JobType.pyCfgParams = ['inputType=data', 'year=2018', 'doReco=ON', 'noiseFilter=ON']
config.JobType.pluginName = 'Analysis'
#config.JobType.outputFiles = [''] #use autocollection
#config.JobType.maxJobRuntimeMin = 2750 #45 h
#config.JobType.maxMemoryMB = 2500 #2.5 GB

config.section_('Data')
config.Data.inputDataset = ''
config.Data.unitsPerJob = 2 #without '' since it must be an int
config.Data.splitting = 'LumiBased'
config.Data.publication = False
#config.Data.runRange = '319849'
#config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Final/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt'
config.Data.lumiMask = 'MyCert.txt'
config.Data.outLFNDirBase = '/store/group/dpg_hcal/comm_hcal/deguio/dataMC_2018/'
config.Data.ignoreLocality = True

config.section_('User')

config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
config.Site.whitelist = ['T2_CH_CERN']



if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand
    from multiprocessing import Process
    
    def submit(config):
        res = crabCommand('submit', config = config)
        
    #########From now on that's what users should modify: this is the a-la-CRAB2 configuration part.

    requestNameList = ['ZeroBias_Run2018C-v1_RAW'
                       ]


    inputDatasetList = ['/ZeroBias/Run2018C-v1/RAW'
                        ]

    
    for req,dataset in zip(requestNameList,inputDatasetList):
        config.General.requestName = req
        config.Data.inputDataset = dataset
        print 'REQUEST:', req, 'DATASET:',dataset
        p = Process(target=submit, args=(config,))
        p.start()
        p.join()
