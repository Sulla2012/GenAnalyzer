import ROOT
from ROOT import *

rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012B/Finished/
fdata = TFile(rootdir+"Filtered3.root")
data = fdata.Get("Dalitz_Delta_230;1")
data.Fit("gaus")
fitter = TVirtualFitter.GetFitter()
for i in range(0,4):
	print(fitter.GetParameter(i))
fitter.GetCovarianceMatrixElement(i,j)

#to define your own function
#expofitlin =TF1("expofitlin","expo(0) + pol1(2)")
