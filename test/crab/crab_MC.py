from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.workArea = 'crab3Submission_2207_3'
config.General.requestName = ''

config.section_('JobType')
config.JobType.psetName = '../recHitTree_cfg.py'
config.JobType.pyCfgParams = ['inputType=MC', 'year=2018', 'doReco=OFF', 'noiseFilter=OFF']
config.JobType.pluginName = 'Analysis'
#config.JobType.outputFiles = [''] #use autocollection
#config.JobType.maxJobRuntimeMin = 2750 #45 h
#config.JobType.maxMemoryMB = 2500 #2.5 GB

config.section_('Data')
config.Data.inputDataset = ''
config.Data.splitting = 'Automatic'
config.Data.publication = False
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

    requestNameList = ['SingleNeutrino_RunIISpring18DRPremix-NZS_100X_upgrade2018_realistic_v10-v2_GEN-SIM-RECO'
                       ]


    inputDatasetList = ['/SingleNeutrino/RunIISpring18DRPremix-NZS_100X_upgrade2018_realistic_v10-v2/GEN-SIM-RECO'
                        ]

    
    for req,dataset in zip(requestNameList,inputDatasetList):
        config.General.requestName = req
        config.Data.inputDataset = dataset
        print 'REQUEST:', req, 'DATASET:',dataset
        p = Process(target=submit, args=(config,))
        p.start()
        p.join()
