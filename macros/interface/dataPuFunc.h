#include "interface/BXPUInfo.h"
#include "interface/LumiInfoProvider.h"
#include "TFile.h"

#include <cmath>



float getNrExpPU(int runnr,int lumiSec);
bool passPUCuts(int runnr,int lumiSec,int nrPUInt,int minPU,int maxPU);
float getNrExpPU(int runnr,int lumiSec,int bx);
int nrBXFromStartOfTrain(int runnr,int lumiSec,int bx);
int nrBXFromEndOfTrain(int runnr,int lumiSec,int bx);
bool passPUCuts(int runnr,int lumiSec,int bx,int nrPUInt,int minPU,int maxPU);
