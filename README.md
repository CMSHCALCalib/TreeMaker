# TreeMaker and utilities to compare data and MC

### Setup CMSSW

    cmsrel CMSSW_10_2_0
    cd CMSSW_10_2_0/src
    cmsenv
    git cms-init
    git clone https://github.com/CMSHCALCalib/TreeMaker CMSHCALCalib/TreeMaker
    scram b -j4
    cd CMSHCALCalib/TreeMaker/test


### Run the ntuple on data/MC and save recHit information

    cmsRun recHitTree_cfg.py inputType=data year=2018 doReco=ON noiseFilter=ON //data
    cmsRun recHitTree_cfg.py inputType=MC year=2018 doReco=OFF noiseFilter=OFF //MC


### Compile and run the plot maker on top of the ntuples

    cd CMSHCALCalib/TreeMaker/macros
    make
    ./bin/makePlots ../test/listTest.txt none 1000  #for data
    ./bin/makePlots ../test/listTest.txt pileUpWeights.root 1000  #for MC


### Datasets used:

    /SingleNeutrino/RunIISpring18DR-NZSNoPU_100X_upgrade2018_realistic_v10-v2/GEN-SIM-RECO --> use the reco information
    /ZeroBias/Run2018C-v1/RAW --> rechits aren't available on ZeroBias, so need to run the hcal local reco




### Need to generate the PU weigth file to perform a proper comparison

    .x ../macros/generateWeights.C("test.root","listMCSingleNeutrino_2018_truePU.txt")


### Compare the plots

    .x ../macros/comparePlots.C("histograms_data.root","histograms_MC.root","_pu0-20")



### Status as of Jul 2018
