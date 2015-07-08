// system include files
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <array>

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

//	bool isMC = false;
	//bool isMC = true;

	// Way to call GenParticle
	/*edm::Handle<std::vector<reco::GenParticle>> particles;
	const std::vector<reco::GenParticle> & p = *particles;
	if(isMC){
	  iEvent.getByLabel( src2_ , particles );
	  p = *particles;
	  }*/


int buffer [] = {193834, 193835, 193836, 193878, 193898, 193904, 193905, 193917, 193919, 193922, 193925, 193928, 193998, 193999, 194027, 194050, 194051, 194052, 194075, 194076, 194108, 194115, 194116, 194117, 194119, 194120, 194150, 194151, 194153, 194199, 194210, 194223, 194224, 194225, 194270, 194303, 194304, 194305, 194314, 194315, 194317, 194424, 194427, 194428, 194429, 194439, 194455, 194464, 194479, 194480, 194533, 194535, 194619, 194627, 194631, 194643, 194644, 194691, 194699, 194702, 194704, 194711, 194712, 194735, 194778, 194785, 194786, 194787, 194788, 194789, 194790, 194795, 194825, 194896, 194897, 194912, 194914, 194915, 195013, 195014, 195015, 195016, 195099, 195109, 195110, 195111, 195112, 195113, 195114, 195115, 195147, 195163, 195164, 195165, 195249, 195251, 195265, 195266, 195303, 195304, 195378, 195379, 195385, 195387, 195390, 195396, 195397, 195398, 195399, 195522, 195523, 195524, 195526, 195527, 195528, 195529, 195530, 195532, 195540, 195551, 195552, 195633, 195634, 195644, 195645, 195647, 195649, 195655, 195656, 195658, 195660, 195664, 195747, 195748, 195749, 195757, 195758, 195773, 195774, 195775, 195776, 195841, 195868, 195913, 195915, 195916, 195917, 195918, 195919, 195923, 195925, 195926, 195927, 195929, 195930, 195936, 195937, 195947, 195948, 195950, 195963, 195970, 196019, 196023, 196027, 196046, 196047, 196048, 196090, 196094, 196095, 196096, 196097, 196098, 196099, 196103, 196107, 196122, 196131, 196135, 196197, 196199, 196200, 196201, 196202, 196203, 196218, 196239, 196249, 196250, 196251, 196252, 196334, 196349, 196350, 196351, 196352, 196353, 196357, 196359, 196361, 196362, 196363, 196364, 196430, 196431, 196432, 196433, 196437, 196438, 196452, 196453, 196458, 196495, 196497, 196501, 196508, 196509, 196531, 197919, 197920, 197922, 197923, 197924, 197925, 197926, 197929, 198022, 198023, 198040, 198041, 198044, 198045, 198046, 198047, 198048, 198049, 198050, 198063, 198116, 198202, 198207, 198208, 198210, 198211, 198212, 198213, 198214, 198215, 198229, 198230, 198249, 198268, 198269, 198270, 198271, 198272, 198346, 198372, 198405, 198409, 198412, 198421, 198425, 198442, 198443, 198445, 198446, 198448, 198449, 198451, 198453, 198455, 198457, 198460, 198461, 198462, 198466, 198468, 198471, 198485, 198486, 198487, 198522, 198523, 198588, 198589, 198590, 198603, 198609, 198898, 198899, 198900, 198901, 198902, 198903, 198941, 198954, 198955, 198969, 199008, 199011, 199021, 199103, 199275, 199276, 199282, 199306, 199318, 199319, 199336, 199356, 199409, 199428, 199429, 199435, 199436, 199563, 199564, 199565, 199566, 199567, 199568, 199569, 199570, 199571, 199572, 199573, 199574, 199608, 199698, 199699, 199701, 199703, 199739, 199745, 199751, 199752, 199753, 199754, 199804, 199812, 199832, 199833, 199834, 199862, 199863, 199864, 199867, 199868, 199875, 199876, 199877, 199960, 199961, 199967, 199973, 200041, 200042, 200049, 200075, 200091, 200152, 200160, 200161, 200174, 200176, 200177, 200178, 200180, 200185, 200186, 200187, 200188, 200190, 200228, 200229, 200243, 200244, 200245, 200253, 200368, 200369, 200381, 200466, 200473, 200491, 200515, 200518, 200519, 200525, 200532, 200599, 200600, 200601, 200602, 200781, 200782, 200786, 200795, 200798, 200960, 200961, 200976, 200990, 200991, 200992, 201005, 201062, 201097, 201113, 201114, 201115, 201159, 201164, 201168, 201173, 201174, 201176, 201191, 201193, 201195, 201196, 201197, 201199, 201200, 201202, 201228, 201229, 201278, 201366, 201383, 201384, 201390, 201391, 201393, 201404, 201409, 201410, 201411, 201412, 201413, 201417, 201418, 201419, 201420, 201423, 201424, 201425, 201434, 201444, 201446, 201448, 201455, 201456, 201461, 201463, 201465, 201467, 201469, 201473, 201475, 201476, 201530, 201535, 201554, 201602, 201611, 201612, 201613, 201624, 201625, 201657, 201658, 201668, 201669, 201671, 201676, 201678, 201679, 201692, 201705, 201706, 201707, 201708, 201718, 201727, 201729, 201794, 201802, 201803, 201804, 201805, 201806, 201807, 201808, 201809, 201810, 201812, 201813, 201815, 201816, 201817, 201818, 201819, 201824, 202000, 202012, 202013, 202014, 202016, 202044, 202045, 202046, 202047, 202050, 202051, 202054, 202060, 202074, 202075, 202084, 202086, 202087, 202088, 202090, 202091, 202092, 202093, 202116, 202178, 202205, 202207, 202208, 202209, 202237, 202272, 202299, 202301, 202303, 202304, 202305, 202314, 202328, 202331, 202332, 202333, 202389, 202469, 202471, 202472, 202476, 202477, 202478, 202504, 202792, 202793, 202794, 202868, 202869, 202873, 202876, 202880, 202909, 202972, 202973, 203002, 203708, 203709, 203739, 203742, 203777, 203778, 203780, 203830, 203832, 203833, 203834, 203835, 203836, 203853, 203894, 203909, 203912, 203980, 203981, 203985, 203986, 203987, 203991, 203992, 203994, 204100, 204101, 204113, 204114, 204238, 204250, 204484, 204506, 204511, 204541, 204544, 204545, 204550, 204551, 204552, 204553, 204554, 204555, 204563, 204564, 204565, 204566, 204567, 204576, 204577, 204599, 204600, 204601, 204792, 204795, 204797, 204798, 204799, 204800, 204803, 204809, 204812, 204900, 205086, 205111, 205158, 205193, 205214, 205215, 205217, 205233, 205235, 205236, 205237, 205238, 205303, 205310, 205311, 205339, 205344, 205515, 205519, 205526, 205595, 205598, 205599, 205600, 205604, 205605, 205611, 205614, 205617, 205618, 205620, 205627, 205666, 205667, 205683, 205690, 205694, 205718, 205774, 205777, 205781, 205826, 205833, 205834, 205908, 205921, 205924, 205937, 205941, 205943, 205945, 205948, 205950, 205956, 205994, 206003, 206004, 206005, 206007, 206008, 206027, 206029, 206030, 206066, 206088, 206098, 206102, 206187, 206188, 206199, 206207, 206208, 206210, 206243, 206245, 206246, 206251, 206257, 206258, 206297, 206301, 206302, 206303, 206304, 206331, 206389, 206390, 206391, 206401, 206446, 206448, 206466, 206476, 206477, 206478, 206484, 206512, 206513, 206539, 206540, 206541, 206542, 206550, 206572, 206573, 206574, 206575, 206594, 206595, 206596, 206598, 206605, 206744, 206745, 206859, 206866, 206867, 206868, 206869, 206897, 206901, 206906, 206939, 206940, 207099, 207100, 207214, 207217, 207219, 207220, 207221, 207222, 207231, 207233, 207269, 207273, 207279, 207299, 207316, 207320, 207328, 207371, 207372, 207397, 207398, 207454, 207468, 207469, 207477, 207487, 207488, 207490, 207491, 207492, 207515, 207517, 207518, 207714, 207779, 207788, 207789, 207790, 207813, 207871, 207875, 207882, 207883, 207884, 207885, 207886, 207887, 207889, 207897, 207898, 207905, 207920, 207921, 207922, 207924, 208296, 208297, 208298, 208299, 208300, 208302, 208303, 208304, 208307, 208339, 208341, 208351, 208352, 208353, 208357, 208390, 208391, 208392, 208393, 208394, 208395, 208397, 208402, 208406, 208407, 208427, 208428, 208429, 208487, 208509, 208535, 208538, 208540, 208541, 208551, 208552, 208553, 208686, 209089, 209106, 209109, 209146, 209148, 209151};
	std::vector<int> runs (buffer, buffer + sizeof(buffer) / sizeof(int) );
	
	int buffer2 [] = {988082, 648533, 1072183, 7173545, 7814188, 1839521, 7989455, 233700, 2425388, 455779, 787162, 14600760, 20860552, 3610680, 6935753, 112682186, 489263, 6818781, 5832523, 55654802, 63680179, 60919736, 16, 2276835, 14969873, 19845916, 41722080, 44899668, 6994462, 29103065, 39951884, 6913464, 43088754, 13189767, 3455849, 5953748, 5014845, 8578147, 28953094, 42376235, 1465884, 68819515, 9429, 43032575, 56807043, 8756409, 32370741, 22274891, 48882706, 82176891, 78314382, 557799, 13298217, 34680, 24078880, 27443544, 33702684, 31315448, 23652828, 15687626, 41159465, 34236091, 34309591, 11168685, 20530096, 6, 6, 6, 2002926, 53416453, 3538606, 0, 19473117, 12322350, 12552934, 155771789, 2438635, 4887212, 61056991, 16883100, 1199902, 49732788, 27570803, 35142789, 76154, 5318712, 2676296, 58129992, 8721160, 1770849, 54650956, 38566618, 7781022, 29215033, 207866, 31318728, 16229285, 48851607, 40426615, 110020465, 117194342, 17814048, 12382, 348659, 20738856, 11968080, 119370495, 91592049, 13397948, 25376, 10, 4, 11, 28, 4, 7737645, 65321190, 0, 33723400, 3476644, 157267582, 1396490, 7607643, 7643266, 2199649, 5227552, 29005678, 47387379, 36704980, 24788986, 8711402, 5872, 0, 2, 17460803, 24513304, 2025729, 66296, 109441775, 14124116, 40603042, 3258265, 27976318, 1002975, 95675071, 18254979, 329107, 5142992, 1236148, 1093379, 983392, 2620083, 137283, 2195628, 38356678, 1, 21334058, 7011845, 69406146, 113328426, 1480022, 3253527, 4198264, 4546946, 22646797, 5107300, 9208182, 6001591, 8316217, 1, 2, 2870909, 2522939, 939165, 2463461, 3818591, 47535372, 2811896, 18311093, 3792046, 64744989, 51428527, 5700924, 2, 8511620, 8915933, 91525519, 80866773, 6810917, 53505016, 4, 3842748, 40271350, 28986617, 3315, 37813, 6338, 18477393, 416098, 186170, 6, 9188078, 3378624, 98156453, 717, 62827186, 3572488, 30082019, 19208898, 84352023, 115036913, 114642583, 266010, 21722691, 6884694, 3556239, 103435, 7085068, 49989510, 0, 0, 0, 14148, 0, 5139, 548, 0, 909347, 884407, 0, 1752774, 1519936, 7967270, 10, 29, 1976483, 1778803, 4640639, 8140311, 7161831, 4439008, 6746150, 12934103, 11990022, 132, 25222808, 3959510, 21, 446408, 400814, 101024290, 409918, 1860631, 25377639, 4, 75751340, 41669025, 1608429, 6973293, 0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 35, 0, 0, 8, 0, 81, 108, 39, 31, 58, 15, 23364444, 45, 129204731, 18778070, 22, 18506, 992, 1931, 13963, 32617, 1565, 491, 2137, 3156, 31865, 17396, 28429124, 27402305, 120515620, 115472862, 65861433, 2211004, 131034778, 5240, 762, 37665, 79109, 25968, 12085859, 101053036, 63224493, 19455160, 114282268, 65173289, 12982955, 131328899, 37498134, 13, 458192, 57, 3917893, 748, 14152533, 36632848, 1465909, 43718176, 20440959, 1313828, 14463518, 174369618, 10046228, 69921523, 2, 34125406, 9620600, 996634, 4561927, 33665100, 5485919, 71591412, 61590376, 77173827, 28798222, 104456810, 48073861, 12207602, 2, 52494384, 27470817, 1578866, 35100941, 37451970, 63206624, 49854477, 26580817, 18534648, 4132021, 112259280, 32428935, 19089092, 60203449, 143806824, 6645638, 2846639, 13102838, 1617310, 80766, 7012146, 4679493, 2176756, 795050, 2649493, 24, 35486129, 64436775, 10433598, 50315582, 11509811, 67867411, 30744426, 149297, 11308775, 53427520, 25669357, 12762071, 103080693, 44996412, 12751788, 1, 76884232, 99094371, 2897322, 9660945, 111909818, 654617, 54, 7015876, 3624480, 70567716, 31609126, 8982351, 251473, 831617, 4480086, 11092322, 98908314, 35945233, 1415072, 27777457, 53284202, 0, 1953512, 10172363, 20463968, 48925293, 61398882, 35513236, 21581984, 0, 139207588, 1288933, 1, 48441951, 1060860, 197652, 8214119, 17381921, 2096764, 10926024, 174008323, 4573249, 1878921, 897853, 27133, 9695, 2381785, 12725201, 14091735, 29441705, 380119, 1176934, 0, 0, 1685, 10179, 80061, 5091143, 3037582, 2286649, 6462224, 4540769, 5611585, 4920831, 4, 5179918, 13711, 3, 9690681, 1498410, 8397854, 2172553, 948389, 8441246, 36465, 137594, 9987892, 69143898, 15263980, 20, 75812997, 30970843, 100042610, 7854622, 38554398, 13869394, 22600349, 95453858, 1, 8274905, 29012226, 16554005, 18596289, 8073308, 93745494, 5712953, 8242956, 24887842, 30670741, 6542541, 33459464, 1484230, 1052, 179, 71, 105, 4951367, 228, 5509252, 536, 2076, 9300, 15789118, 24137258, 80224, 18350013, 32487730, 1329158, 8292737, 6464822, 20099546, 78908093, 52005341, 70908079, 3146, 223, 1, 1, 37746551, 91320454, 17318894, 68318595, 21838716, 2, 99525589, 17750108, 1, 39, 17, 18768033, 1496608, 131919477, 4365724, 38, 90, 17630533, 118786959, 121977808, 63219424, 3337993, 6564082, 41985456, 22783557, 32650261, 73245452, 2, 2, 23036067, 18730411, 40064138, 2, 13319963, 3, 15976240, 95820360, 145969451, 1, 0, 0, 0, 0, 0, 10, 2, 4, 62599524, 88260805, 143967161, 820092, 1384180, 5175933, 639086, 2871922, 11383537, 2610604, 8532227, 602306, 7196197, 2104512, 16918489, 10684, 16330632, 104525673, 44347308, 105710606, 12038970, 14762828, 419286, 2087534, 83594842, 113369, 817814, 25598851, 9079058, 9467220, 82325260, 33496253, 7617969, 84287352, 3, 58755733, 5860231, 28780959, 47696514, 40711768, 59, 198081, 577342, 6794571, 30427811, 16931642, 64217873, 105253540, 2839336, 663639, 2435636, 37643303, 117695774, 41099524, 1, 107595543, 0, 0, 0, 0, 0, 0, 0, 2, 9, 116424, 696763, 50332702, 80409124, 96248130, 1, 5, 24363398, 11829662, 5, 40485213, 877, 50731943, 19247126, 62188757, 94745070, 45591992, 120821477, 21220361, 44455740, 23185639, 12376, 3590, 1636, 39812, 1, 7917661, 322, 4158361, 45602522, 758353, 12144912, 9450985, 88509173, 85901170, 32613172, 4312017, 62799739, 89224822, 10677811, 914475, 54204410, 54745621, 44919115, 23041229, 23797776, 95401068, 0, 1, 0, 0, 0, 0, 52, 8, 0, 0, 0, 0, 0, 0, 28, 15, 13, 11817139, 31684846, 3596891, 40380412, 84001748, 5730080, 14898004, 81409016, 48102905, 31859050, 104916369, 5279419, 80926104, 4828519, 1175985, 9711505, 3082510, 2, 35024984, 29651460, 5770099, 37045886, 44479312, 21238684, 8934132, 51101733, 122226948, 78509566, 84846962, 24393544, 23128080, 16539441, 78721948, 119819707, 22876677, 373942, 34127, 46429, 97152248, 13328110, 3576523, 1951875, 11855329, 8510377, 33198875, 23259267, 70911271, 43255021, 3879495, 55209315, 138561555, 99594055, 29485902, 1060720, 1307243, 52962911, 17164240, 12872744, 22024053, 28034440, 98067257, 125169145, 7404747, 98174086, 2904109, 9733258, 13028940, 7715629, 20283683, 152157310, 9815690, 63242381, 74499107, 120694823, 9558097, 956035, 21603984, 340043, 11161987, 57523539, 24244043, 4253193, 183292174, 4015227, 7039573, 72724431, 60083868, 45312883, 12502572, 34628438, 18930267, 136335472, 4801356, 6789108, 588, 17723, 0, 8058, 23322, 2680, 16159, 33454, 5504465, 9531126, 22352467, 9359243, 16690798, 2702592, 68906323, 11841281, 35017130, 167526527, 95281987, 12196819, 24395041, 31681294, 7696, 14465, 7817, 28347, 3376073, 12884, 21700, 3651944, 126737002, 40650659, 73906051, 42362093, 1773374, 35502253, 40853365, 26564781, 90190051, 3722298, 14986665, 957518, 4886024, 16509113, 2481920, 2104045, 1291461, 78009766, 21519439, 67417457, 75013796, 1466450, 1092701, 6318051, 13658630, 46768045, 69258796, 1, 2874408, 57247948, 983004, 2007165, 224647, 2782718, 2315719, 7067192};
	std::vector<int> luminosityVector (buffer2, buffer2 + sizeof(buffer2) / sizeof(float));	
	

	
	//get the run number. This is some magic function defined in one of the libraries we're using, set to some number to run
	int runNumber;
	runNumber = iEvent.id().run();

	
	// Set the cuttoff Luminosity for the run
	int lumiCutoff;
	lumiCutoff = 50000000;
	
	//find the interator corresponding to the run number
	int runtag;
  	for(unsigned int i = 0; i < runs.size(); ++i){
  		if(runNumber==runs[i])
  		{
  			runtag = i;
	  	}
	}
			
		
	float luminosity;
	luminosity = luminosityVector.at(runtag);
	
	if(luminosity>lumiCutoff)
	{
		return;
	}
	// Way to call CALOJETS
	edm::Handle<std::vector<reco::CaloJet>> jets;
	iEvent.getByLabel( src_ , jets );
	const std::vector<reco::CaloJet> & jet = *jets;

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
