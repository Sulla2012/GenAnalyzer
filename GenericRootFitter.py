import ROOT
from ROOT import *

rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012D/Finished/'
fdata = TFile(rootdir+"Filtered2D.root")
#fdata.cd("genHistos/Corrected/DalitzCuts/Moderate/")
hist = fdata.Get("genHistos/Corrected/DalitzCuts/Moderate/Dalitz_Delta_230")
hist.Fit("gaus")
fitter = TVirtualFitter.GetFitter()
for i in range(0,4):
        print(fitter.GetParameter(i))
fitter.GetCovarianceMatrixElement(i,j)

#to define your own function
#expofitlin =TF1("expofitlin","expo(0) + pol1(2)")
