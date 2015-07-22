import ROOT
from ROOT import *

cutnum = 180

for cutnum <= 340:
        rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012D/Finished/'
        fdata = TFile(rootdir+"Filtered2D.root")
        #fdata.cd("genHistos/Corrected/DalitzCuts/Moderate/")
        histname = 'genHistos/Corrected/DalitzCuts/Moderate/Dalitz_Delta_'+str(cutnum)
        hist = fdata.Get(histname)
        hist.Fit("gaus")
        fitter = TVirtualFitter.GetFitter()
        for i in range(0,4):
                print(fitter.GetParameter(i))
        fitter.GetCovarianceMatrixElement(i,j)
        cutnum = cutnum + 10


#to define your own function
#expofitlin =TF1("expofitlin","expo(0) + pol1(2)")
