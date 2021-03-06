import ROOT
from ROOT import *


print("Begining Moderate Dalitz Cut")
cutnum = 180

listGoodness = []

while cutnum <= 340:
        bufferString = "Dalitz Delta Medium "+str(cutnum)
        print("File being fit: "+str(bufferString))
        rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012B/Finished/'
        fdata = TFile(rootdir+"Filtered20B.root")
        #fdata.cd("genHistos/Corrected/DalitzCuts/Moderate/")
        bufferString = "genHistos/Corrected/DalitzCuts/Moderate/Dalitz_Delta_"+str(cutnum)
        hist = fdata.Get(bufferString)
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
        background = TF1("background", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x",160,350)

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

        top = TF1("top", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x+[5]*exp(-(x-[6])**2/(2*[7]**2))", 160,350)
        top.SetParName(5,"Amp")
        top.SetParName(6,"Mean")
        top.SetParName(7,"Sigma")
        top.SetParameter(5,1000)
        top.SetParameter(6,100)
        top.SetParameter(7,10)

        #Force the Amplitude of the Guassian to be non-negative
        top.SetParLimits(5,0,1000000000000)

        top.FixParameter(0,a)
        top.FixParameter(1,b)
        top.FixParameter(2,c)
        top.FixParameter(3,d)
        top.FixParameter(4,e)

        hist.Fit("top", "R")

        amp = top.GetParameter(5)
        ampErr = top.GetParError(5)
        goodness = (amp-ampErr)/ampErr
        bufferString = str(goodness)
        print("Goodness = "+bufferString)
        listGoodness.append(goodness)
        #Advance to next hist
        cutnum = cutnum + 10

bestGood = max(listGoodness)
bufferString = str(bestGood)
print("Best goodness = " +bufferString)

bufferList = ["Very Soft", "Soft", "Hard"]

for cut in bufferList:
        
        print("Begining " +cut+" Dalitz Cut")
        cutnum = 180

        listGoodness = []

        while cutnum <= 340:
                bufferString = "Dalitz Delta " +cut+str(cutnum)
                print("File being fit: "+str(bufferString))
                rootdir = '/cms/data18/scherer/LumiFilteredScouting/2012B/Finished/'
                fdata = TFile(rootdir+"Filtered20B.root")
                #fdata.cd("genHistos/Corrected/DalitzCuts/Moderate/")
                bufferString = "genHistos/Corrected/DalitzCuts/"+cut+"/"+cut"_Dalitz_Delta_"+str(cutnum)
                hist = fdata.Get(bufferString)
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
                background = TF1("background", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x",160,350)

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

                top = TF1("top", "[0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x+[5]*exp(-(x-[6])**2/(2*[7]**2))", 160,350)
                top.SetParName(5,"Amp")
                top.SetParName(6,"Mean")
                top.SetParName(7,"Sigma")
                top.SetParameter(5,1000)
                top.SetParameter(6,100)
                top.SetParameter(7,10)

                #Force the Amplitude of the Guassian to be non-negative
                top.SetParLimits(5,0,1000000000000)

                top.FixParameter(0,a)
                top.FixParameter(1,b)
                top.FixParameter(2,c)
                top.FixParameter(3,d)
                top.FixParameter(4,e)

                hist.Fit("top", "R")

                amp = top.GetParameter(5)
                ampErr = top.GetParError(5)
                goodness = (amp-ampErr)/ampErr
                bufferString = str(goodness)
                print("Goodness = "+bufferString)
                listGoodness.append(goodness)
                #Advance to next hist
                cutnum = cutnum + 10

        bestGood = max(listGoodness)
        bufferString = str(bestGood)
        print("Best goodness "+cut+" = " +bufferString)




