import ROOT
from ROOT import *

rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012D/Finished/'
fdata = TFile(rootdir+"Filtered2D.root")
#fdata.cd("genHistos/Corrected/DalitzCuts/Moderate/")
hist = fdata.Get("genHistos/Corrected/DalitzCuts/Moderate/Dalitz_Delta_230")
#Fancy Background
#background = TF1("background", '[0]*((1-x/8000)**([1])/((x/8000)**([2]+[3]*log(x/8000))))')

#background.SetParName(0,"P0")
#background.SetParName(1,"P1")
#background.SetParName(3,"P3")
#background.SetParName(2,"P2")
#background.SetParameter(0,1)
#background.SetParameter(1,5)
#background.SetParameter(2,10)
#background.SetParameter(3,1)

#Test Background
background = TF1("background", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x",150,250)

background.SetParName(0,"P0")
background.SetParName(1,"P1")
background.SetParName(2,"P2")
background.SetParameter(0,1)
background.SetParameter(1,-50)
background.SetParameter(2,-10)

#background.SetParameteres(1.,1.,1.,1.)
hist.Fit("background", "R")
fitter = TVirtualFitter.GetFitter()
for i in range(0,4):
        print(fitter.GetParameter(i))


#top = TF1("top", "a+b*x+c*x**2+d*x**3+e*x**4+expo(5)", 150, 250)
a = fitter.GetParameter(0)
b = fitter.GetParameter(1)
c = fitter.GetParameter(2)
d = fitter.GetParameter(3)
e = fitter.GetParameter(4)

top = TF1("top", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x+[5]*exp(-(x-[6])**2/(2*[7]**2))", 150,250)
top.SetParName(5,"Amp")
top.SetParName(6,"Mean")
top.SetParName(7,"Sigma")
top.SetParameter(5,1000)
top.SetParameter(6,100)
top.SetParameter(7,10)

top.FixParameter(0,a)
top.FixParameter(1,b)
top.FixParameter(2,c)
top.FixParameter(3,d)
top.FixParameter(4,e)

hist.Fit("top", "R")

#top = TF1("top", "a+b*x+c*x**2+d*x**3+e*x**4+expo(5)", 150, 250)

#top = TF1("top", "a+b*x+c*x**2+d*x**3+e*x**4+expo(5)", 150, 250)
#top = TF1("top", "a+b*x+c*x**2+d*x**3+e*x**4+expo(5)", 150, 250)
#fitter.GetCovarianceMatrixElement(i,j)

#to define your own function
#expofitlin =TF1("expofitlin","expo(0) + pol1(2)")
