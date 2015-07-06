// system include files
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFolder.h"						
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
							/////////////////////
							//class declaration//
							////////////////////

std::string int_to_string(const int a){
  std::stringstream ss;
  ss << a;
  std::string str = ss.str();
  return str;
}

//parse an input of two rows and return a vector comprised of the second row
std::vector<std::float> parse2(std::istream& in)
{
    std::vector<std::float> output;
    std::float num;
    while( in ) {
        std::cin >> num;  // throwaway 1
        std::cin >> num;  // data2
        output.push_back(num);
    }
    return output;
}
//parse an input of two rows and return a vector comprised of the first row

std::vector<std::int> parse1(std::istream& in)
{
    std::vector<std::int> output;
    std::int num;
    while( in ) {
        std::cin >> num;  // throwaway 1
        output.push_back(num);
        std::cin >> num;  // data2
    }
    return output;
}

class GenAnalyzer : public edm::EDAnalyzer {
	public:
		explicit GenAnalyzer(const edm::ParameterSet&);
		~GenAnalyzer();

		
	private:
		virtual void beginJob() ;
		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		virtual void endJob() ;
		int counter;

	// ----------member data ---------------------------
	edm::InputTag src_;
	edm::InputTag src2_;
//	edm::InputTag srcCA8_;
//	edm::InputTag srcAK5_;
	double scale;
	bool isMC;

								////////////////////////////////
								////HISTOGRAM INITIALIZATION////
								////////////////////////////////

	TH2D * h_Scale_Vs_Pt;
	TH1D * h_FilterMultiplicity;
	TH1D * h_Event_HT;
	TH1D * h_NewPT;
	TH1D * h_NewPT_4;

	TH2D * h_Dalitz_Mid_Low[26];
	TH2D * h_Dalitz_High_Mid[26];
	TH2D * h_Dalitz_High_Low[26];
	TH2D * h_Dalitz_All[26];
	TH1D * h_Mid_CosTheta[26];
	TH1D * h_High_CosTheta[26];


};



///////////////////////////////////////////////////////////////////////////////////////////////////////////  CORRECETED HISTOGRAMS
///
/// constructors and destructor
///
GenAnalyzer::GenAnalyzer(const edm::ParameterSet& iConfig) :
  src_( iConfig.getParameter<edm::InputTag>( "src" ) ), 			// Obtain input
  	src2_( iConfig.getParameter<edm::InputTag>( "srcGen" ) )  			// Obtain input
//	srcCA8_( iConfig.getParameter<edm::InputTag>("srcCA8") ),
//	srcAK5_( iConfig.getParameter<edm::InputTag>("srcAK5") )
{
	//now do what ever initialization is needed
	edm::Service<TFileService> fs;						// Output File 
	scale = iConfig.getParameter<double>("Scale");
	isMC = iConfig.getParameter<bool>("ISMC");
	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

TFileDirectory Select = fs->mkdir( "Selection_Plots" );
TFileDirectory High_Low = fs->mkdir( "Dalitz_High_Low" );
TFileDirectory High_Mid = fs->mkdir( "Dalitz_High_Mid" );
TFileDirectory Mid_Low = fs->mkdir( "Dalitz_Mid_Low" );
TFileDirectory All = fs->mkdir( "Dalitz_All" );
TFileDirectory MidCos = fs->mkdir( "Mid_CosTheta" );
TFileDirectory HighCos = fs->mkdir( "High_CosTheta" );

	h_Scale_Vs_Pt =         Select.make<TH2D> ("Scale_Vs_Pt", " Jet Correction Scale Factor Vs Intial Pt " , 500, 0, 500.0, 50, 0.0, 15.0);   
	h_FilterMultiplicity =  Select.make<TH1D> ("FilterJetMultiplicity", "Jet Multiplicity (Checking Filter cut)", 100, 0.0, 100.0);
	h_Event_HT =            Select.make<TH1D> ("HT_Event" , "Event HT", 500, 0.0, 2500.0);
	h_NewPT = 		Select.make<TH1D> ("NewPT_all" , "Pt~ = Pt/HT for All Jets", 50, 0.0, 1);
	h_NewPT_4 = 		Select.make<TH1D> ("NewPT_4thJet" , "Pt~ = Pt/HT for 4th Jet (for 4 Jet events Only)", 50, 0.0, 1.0); 



	for(int j=0; j<26; ++j){
	  	int i = (300 + (100*j));
		std::string k = int_to_string(i);
		h_Dalitz_Mid_Low[j]  = Mid_Low.make<TH2D>   (("Mid_Low_"+k).c_str(), ("Mid-Low : HT Min "+k).c_str(), 50, 0, 0.5, 50, 0, 1); 
		h_Dalitz_High_Mid[j] = High_Mid.make<TH2D>  (("High_Mid_"+k).c_str(), ("High-Mid : HT Min "+k).c_str(), 50, 0, 0.5, 50, 0, 1);
		h_Dalitz_High_Low[j] = High_Low.make<TH2D>  (("High_Low_"+k).c_str(), ("High-Low : HT Min "+k).c_str(), 50, 0, 0.5, 50, 0, 1);
		h_Dalitz_All[j]     = All.make<TH2D>       (("Dalitz_All_"+k).c_str(), ("Dalitz: All Three : HT Min "+k).c_str(), 50, 0, 0.5, 50, 0, 1);
		h_Mid_CosTheta[j]   = MidCos.make<TH1D>    (("Mid_CostTheta_"+k).c_str(), ("Mid-CosTheta :  HT Min "+k).c_str(), 25, -1, 1);
		h_High_CosTheta[j]  = HighCos.make<TH1D>   (("High_CosTheta_"+k).c_str(), ("High-CosTheta : HT Min "+k).c_str(), 25, -1, 1);
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}

bool ComparePt(TLorentzVector a, TLorentzVector b) { return a.Pt() > b.Pt(); }


GenAnalyzer::~GenAnalyzer()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)
}





//
// member functions
//

// ------------ method called for each event  ------------
void GenAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	using namespace std;
	using namespace edm;
	using namespace reco;
	using namespace boost;

//	bool isMC = false;
	//bool isMC = true;

	// Way to call GenParticle
	/*edm::Handle<std::vector<reco::GenParticle>> particles;
	const std::vector<reco::GenParticle> & p = *particles;
	if(isMC){
	  iEvent.getByLabel( src2_ , particles );
	  p = *particles;
	  }*/
	 
	

	// Way to call CALOJETS
	edm::Handle<std::vector<reco::CaloJet>> jets;
	iEvent.getByLabel( src_ , jets );
	const std::vector<reco::CaloJet> & jet = *jets;
	
	//this section filters out runs with luminosities over the cutoff 
	  
	parse(std::cin);
	std::ifstream file("scoutinglumi.tsv");
	
	std::vector<std::int> runs;
	std::vector<std::float> runlumi;
	
	runs = parse1(file);
	runlumi = parse2(file);
	
	//get the run number
	runNumber = iEvent.id().run();
	
	// Set the cuttoff Luminosity for the run
	int lumiCutoff;
	lumiCutoff = 100000;
	
	//find the interator corresponding to the run number
	int runtag;
	runtag = find(runs.begin(),runs.end(), runNumber);
	
	float luminosity;
	luminosity = runlumi.at(runtag);
	
	if(luminosity>lumiCutoff)
	{
		return;
	}
	
	
	//end filter
	
	

	// Way to cal PFJETS
//	edm::Handle<std::vector<reco::PFJet>> jets;
//	iEvent.getByLabel(srcAK5_ , jets);
//	const std::vector<reco::PFJet> & j = *jets; 	// AODSIM


const JetCorrector* corrector = JetCorrector::getJetCorrector("ak5CaloL2L3Residual", iSetup);
vector<reco::CaloJet> j;
TLorentzVector plotjet;
TLorentzVector FilterJet;
int FilterJetsNumber = 0;
for(unsigned int i = 0; i < jets->size(); ++i){
	reco::CaloJet uncorrjet = jet[i];
	double scaleL2L3  = corrector->correction(uncorrjet,iEvent,iSetup);
	plotjet.SetPxPyPzE(uncorrjet.px(),uncorrjet.py(),uncorrjet.pz(),uncorrjet.energy());
	h_Scale_Vs_Pt->Fill( plotjet.Pt(), scaleL2L3 ,scale); 
	if( plotjet.Pt() > 15){
		uncorrjet.scaleEnergy(scaleL2L3);
	}
	j.push_back(uncorrjet);

}

for(unsigned int i=0; i < jets->size(); ++i){
	FilterJet.SetPxPyPzE(j[i].px(),j[i].py(),j[i].pz(),j[i].energy());
	if( (fabs(FilterJet.Eta()) < 2.5) && (FilterJet.Pt() > 20) ){
		FilterJetsNumber++;
	}
	}
h_FilterMultiplicity->Fill(FilterJetsNumber,scale);

vector< TLorentzVector > CutJets;
double Event_HT = 0;
double NewPT[4];
double tmpDalitzVal[3];
double tmpDalitzValNorm[3];
double Mlow =0;
double Mmid =0;
double Mhigh =0;
double MlowNorm =0;
double MmidNorm =0;
double MhighNorm =0;
double Muh = 0;
double Mum = 0;
double Mijk =0;
tmpDalitzVal[0] = 0;
tmpDalitzVal[1] = 0;
tmpDalitzVal[2] = 0;
tmpDalitzValNorm[0] = 0;
tmpDalitzValNorm[1] = 0;
tmpDalitzValNorm[2] = 0;
NewPT[0] = 0;
NewPT[1] = 0;
NewPT[2] = 0;
NewPT[3] = 0;

for(unsigned int i=0; i < jets->size() ; ++i){
	TLorentzVector tmpJET;
	tmpJET.SetPxPyPzE(j[i].px(),j[i].py(),j[i].pz(),j[i].energy());
	Event_HT += tmpJET.Pt();
	
	if(( tmpJET.Pt() > 30) && (fabs(tmpJET.Eta()) < 2.4) ){
		CutJets.push_back(tmpJET);
	}
}
h_Event_HT->Fill(Event_HT);

if( (CutJets.size() == 4) && (Event_HT > 300) ){

	for(unsigned int i=0; i < CutJets.size(); ++i){
		NewPT[i] = (CutJets[i].Pt())/(Event_HT);
		h_NewPT->Fill( NewPT[i]);
	}

	h_NewPT_4->Fill( NewPT[3] );

	if(NewPT[3] > .1){


//		for(unsigned int i = 1; i < CutJets.size() -2 ; ++i){
//			for( unsigned int j = i+1; j < CutJets.size() -1 ; ++j){
//				for( unsigned int k = j+1; j < CutJets.size(); ++k){
					tmpDalitzValNorm[0] = ((CutJets[1] + CutJets[2]).M()*(CutJets[1] + CutJets[2]).M())/( (((CutJets[1] + CutJets[2] + CutJets[3]).M())*((CutJets[1] + CutJets[2] +CutJets[3] ).M())) + (CutJets[1].M())*(CutJets[1].M()) + (CutJets[2].M())*(CutJets[2].M()) + (CutJets[3].M())*(CutJets[3].M()) ) ;

					tmpDalitzValNorm[1] = ((CutJets[1] + CutJets[3]).M()*(CutJets[1] + CutJets[3]).M())/( (((CutJets[1] + CutJets[2] + CutJets[3]).M())*((CutJets[1] + CutJets[2] +CutJets[3] ).M())) + (CutJets[1].M())*(CutJets[1].M()) + (CutJets[2].M())*(CutJets[2].M()) + (CutJets[3].M())*(CutJets[3].M()) ) ;

					tmpDalitzValNorm[2] = ((CutJets[3] + CutJets[2]).M()*(CutJets[3] + CutJets[2]).M())/( (((CutJets[1] + CutJets[2] + CutJets[3]).M())*((CutJets[1] + CutJets[2] +CutJets[3] ).M())) + (CutJets[1].M())*(CutJets[1].M()) + (CutJets[2].M())*(CutJets[2].M()) + (CutJets[3].M())*(CutJets[3].M()) ) ;

//				}
//			}
//		}

			tmpDalitzVal[0] = (CutJets[1] + CutJets[2]).M();
			tmpDalitzVal[1] = (CutJets[1] + CutJets[3]).M();
			tmpDalitzVal[2] = (CutJets[2] + CutJets[3]).M();


	Mijk = ( (CutJets[1] + CutJets[2] + CutJets[3] ).M() );

		if( (tmpDalitzVal[0] > tmpDalitzVal[1]) && (tmpDalitzVal[0]  > tmpDalitzVal[2]) ){
			if ( tmpDalitzVal[1] > tmpDalitzVal[2]){
				MlowNorm = tmpDalitzValNorm[2];
				MmidNorm = tmpDalitzValNorm[1];					
				MhighNorm = tmpDalitzValNorm[0];

				Mlow = tmpDalitzVal[2];
				Mmid = tmpDalitzVal[1];					
				Mhigh = tmpDalitzVal[0];

				Muh = CutJets[3].M();  
				Mum = CutJets[2].M();
			}		
			else {	
				MlowNorm = tmpDalitzValNorm[1];
				MmidNorm = tmpDalitzValNorm[2];					
				MhighNorm = tmpDalitzValNorm[0];

				Mlow = tmpDalitzVal[1];
				Mmid = tmpDalitzVal[2];					
				Mhigh = tmpDalitzVal[0];

				Muh = CutJets[3].M();  
				Mum = CutJets[1].M();
			}
		}	
		
		if( (tmpDalitzVal[1] > tmpDalitzVal[2]) && (tmpDalitzVal[1]  > tmpDalitzVal[0]) ){
			if ( tmpDalitzVal[0] > tmpDalitzVal[2]){
				MlowNorm = tmpDalitzValNorm[2];
				MmidNorm = tmpDalitzValNorm[0];					
				MhighNorm = tmpDalitzValNorm[1];

				Mlow = tmpDalitzVal[2];
				Mmid = tmpDalitzVal[0];					
				Mhigh = tmpDalitzVal[1];

				Muh = CutJets[2].M();  
				Mum = CutJets[3].M();
			}
			else{	
				MlowNorm = tmpDalitzValNorm[0];
				MmidNorm = tmpDalitzValNorm[2];					
				MhighNorm = tmpDalitzValNorm[1];

				Mlow = tmpDalitzVal[0];
				Mmid = tmpDalitzVal[2];					
				Mhigh = tmpDalitzVal[1];

				Muh = CutJets[2].M();  
				Mum = CutJets[1].M();
			}
		}	
		
		if( (tmpDalitzVal[2] > tmpDalitzVal[0]) && (tmpDalitzVal[2]  > tmpDalitzVal[1]) ){
			if ( tmpDalitzVal[0] > tmpDalitzVal[1]){
				MlowNorm = tmpDalitzValNorm[1];
				MmidNorm = tmpDalitzValNorm[0];					
				MhighNorm = tmpDalitzValNorm[2];
	
				Mlow = tmpDalitzVal[1];
				Mmid = tmpDalitzVal[0];					
				Mhigh = tmpDalitzVal[2];
	
				Muh = CutJets[1].M();  
				Mum = CutJets[3].M();
			}
			else{	
				MlowNorm = tmpDalitzValNorm[0];
				MmidNorm = tmpDalitzValNorm[1];					
				MhighNorm = tmpDalitzValNorm[2];
	
				Mlow = tmpDalitzVal[0];
				Mmid = tmpDalitzVal[1];					
				Mhigh = tmpDalitzVal[2];
	
				Muh = CutJets[1].M();  
				Mum = CutJets[2].M();
			}
		}	
	}
	
}

double Mid_CosTheta = 0;
double High_CosTheta = 0;

if( (Event_HT > 300) && (Mlow > 0) && (Mmid > 0) && (Mhigh > 0) && (Muh > 0) && (Mum > 0) && (Mijk > 0) ){
	Mid_CosTheta = ( 1 - 2*((Mmid*Mmid)*(Mlow*Mlow))/( ( (Mijk*Mijk) - (Mlow*Mlow) )*( (Mlow*Mlow) - (Mum*Mum) ) )      ) ; 
	High_CosTheta = ( 1 - 2*(Mhigh*Mhigh)*(Mlow*Mlow)/( ( (Mijk*Mijk) - (Mlow*Mlow) )*( (Mlow*Mlow) - (Muh*Muh) ) )   ); 
	
	for( unsigned int i = 0; i < 26; ++i){
		unsigned int j = ( i * 100);
		if( (Event_HT > (300 + j)) && (Event_HT < (400 + j) ) ){
			h_Dalitz_Mid_Low[i]->Fill( MlowNorm , MmidNorm );
			h_Dalitz_High_Mid[i]->Fill( MmidNorm , MhighNorm );
			h_Dalitz_High_Low[i]->Fill( MlowNorm , MhighNorm );
			h_Dalitz_All[i]->Fill( MlowNorm, MmidNorm );
			h_Dalitz_All[i]->Fill( MmidNorm, MhighNorm);
			h_Dalitz_All[i]->Fill( MlowNorm, MhighNorm);
			h_Mid_CosTheta[i]->Fill( Mid_CosTheta  );
			h_High_CosTheta[i]->Fill( High_CosTheta  );
		}	
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

// ------------ method called once each job just after ending the event loop  ------------
void 
GenAnalyzer::beginJob() {
}
void 
GenAnalyzer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenAnalyzer);
