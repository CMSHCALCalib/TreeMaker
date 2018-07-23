#! /usr/bin/env python

import os
import sys
import optparse
import datetime
import time

## prepare the file list first
#find /eos/cms/store/data/Run2018A/EGamma/ALCARECO/HcalCalIterativePhiSym-PromptReco-v1/000/ -type f | grep ".root" > fileList_egamma+singlemu.txt
#find /eos/cms/store/data/Run2018A/SingleMuon/ALCARECO/HcalCalIterativePhiSym-PromptReco-v1/000/ -type f | grep ".root" >> fileList_egamma+singlemu.txt

usage = "usage: python ../scripts/splitAndSubmit.py -e ../macros/makePlots -l listMCSingleNeutrino.txt -w pileUpWeights.root -q cmscaf1nd -o histograms --split 1"

parser = optparse.OptionParser(usage)
parser.add_option("-e", "--exec", dest="executable",
    help="name of the executable to be used",
    )

parser.add_option('-q', '--queue',       action='store',     dest='queue',       
    help='run in batch in queue specified as option (default -q cmslong)', 
    default='cmsan',
    metavar="QUEUE")

parser.add_option("-l", "--list", dest="list",
    help="path and name of the fileList",
    )

parser.add_option("-w", "--weights", dest="weights",
    help="root file containing the mc weights",
    metavar="OUTDIR")

parser.add_option("-o", "--output", dest="output",
    help="the root file outName",
    metavar="OUTDIR")

parser.add_option("--split", dest="filesperjob", type=int,
    help="files to analyze per job ",
    default=10)

(opt, args) = parser.parse_args()
################################################


###
pwd = os.environ['PWD']
current_time = datetime.datetime.now()
simpletimeMarker = "_%04d%02d%02d_%02d%02d%02d" % (current_time.year,current_time.month,current_time.day,current_time.hour,current_time.minute,current_time.second) 
timeMarker = "submit_%04d%02d%02d_%02d%02d%02d" % (current_time.year,current_time.month,current_time.day,current_time.hour,current_time.minute,current_time.second) 
workingDir = pwd+"/batch/"+timeMarker

os.system("mkdir -p "+workingDir)

path, exename = os.path.split(opt.executable)

inputlist = []
njobs_list = []

num_lines = sum(1 for line in open(opt.list, "r"))
ins = open(opt.list, "r")

isData = ('none' in opt.weights)

##loop over lists (one for datasets) to create splitted lists
count = 0
jobCount = 0
for line in  ins:
    count = count+1
    inputlist.append(line)
    if count%opt.filesperjob == 0 or count==num_lines:
        jobCount = jobCount+1
        os.system("mkdir "+workingDir+"/"+str(jobCount))

        with open(workingDir+"/"+str(jobCount)+"/fileList.txt", "w") as fo:
            for item in inputlist:
                fo.write("%s" % item)

        inputlist = []

        os.system("cp "+opt.executable+" "+workingDir)
        if(not isData):
            os.system("cp "+opt.weights+" "+workingDir)
        
        os.system("echo cd "+pwd+" > launch.sh")
        os.system("echo 'eval `scramv1 runtime -sh`\n' >> launch.sh")
        os.system("echo cd - >> launch.sh")

        if(not isData):
            os.system("echo  "+workingDir+"/"+exename+" "+workingDir+"/"+str(jobCount)+"/fileList.txt "+workingDir+"/"+opt.weights+" >> launch.sh")
            os.system("echo mv "+opt.output+"_MC.root "+workingDir+"/"+opt.output+"_MC_"+str(jobCount)+".root >> launch.sh")
        else:
            os.system("echo  "+workingDir+"/"+exename+" "+workingDir+"/"+str(jobCount)+"/fileList.txt >> launch.sh")
            os.system("echo mv "+opt.output+"_data.root "+workingDir+"/"+opt.output+"_data_"+str(jobCount)+".root >> launch.sh")

        os.system("chmod 755 launch.sh")
        os.system("mv launch.sh "+workingDir+"/"+str(jobCount))
        njobs_list.append("bsub -q "+opt.queue+" -o "+workingDir+"/"+str(jobCount)+"/log.out -e "+workingDir+"/"+str(jobCount)+"/log.err "+workingDir+"/"+str(jobCount)+"/launch.sh")
        
for job in njobs_list:
    print job






