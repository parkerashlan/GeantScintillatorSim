///////////////////////////////////////////
//
// Oct/2015  Fabio -> RunAction.cc
//
///////////////////////////////////////////

#include "RunAction.hh"
#include "Analysis.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4Run.hh"
#include "G4ios.hh"

#include <iomanip>

RunAction::RunAction() : G4UserRunAction()
{
	  // Create analysis manager
	  // The choice of analysis technology is done via selecting of a namespace
	  // in B4Analysis.hh
	  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	  G4cout << "Using " << analysisManager->GetType() << G4endl;

	  // Create directories
	  //analysisManager->SetHistoDirectoryName("histograms");
	  //analysisManager->SetNtupleDirectoryName("ntuple");
	  analysisManager->SetVerboseLevel(1);
	  analysisManager->SetFirstHistoId(0); // default is 0
	  analysisManager->SetFirstNtupleId(0); // default is 0

	  // Book histograms, ntuple
	  //

	  // Creating histograms

	  //  G4int CreateH1(const G4String& name, const G4String& title,
	  //                 G4int nbins, G4double xmin, G4double xmax,
	  //                 const G4String& unitName = "none",
	  //                 const G4String& fcnName = "none",
	  //                 const G4String& binSchemeName = "linear");


	  xmin = 0; // in MeV
	  xmax = 12.;// in MeV
	  binsize = .1; // in MeV
	  nbins= (double)((xmax-xmin)/binsize);
	  analysisManager->CreateH1("H1","Edep in Crystal", nbins, xmin*MeV, xmax*MeV);

	  xmin = 0; //
	  xmax = 2e3; //
	  binsize = 2; //
	  nbins= (int)(xmax-xmin)/binsize;
      analysisManager->CreateH1("H2","nAbsorbed Photons", nbins, xmin, xmax);

      // Here we need some units!
      xmin = 0; // in ns
	  xmax = 500; // in ns
	  binsize = 2; // in ns
	  nbins= (int)(xmax-xmin)/binsize;
          analysisManager->CreateH1("H3","Time of Absorption", nbins, xmin*ns, xmax*ns);

      xmin = 0; // in keV
	  xmax = 12e3; // in keV
	  binsize = 2; // in keV
	  nbins= (int)((xmax-xmin)/binsize);
	  analysisManager->CreateH1("H4","Primary Energy of Particles", nbins, xmin*MeV, xmax*MeV);

	  xmin = 0; //
	  xmax = 18; //
	  binsize = 1; //
	  nbins= (int)(xmax-xmin)/binsize;
      analysisManager->CreateH1("H4","SiPM Absorbed Photons", nbins, xmin, xmax);

	  // Creating ntuple
	  //
	  analysisManager->CreateNtuple("B4", "Edep and TrackL...");
	  analysisManager->CreateNtupleDColumn("Edep");
	  analysisManager->CreateNtupleDColumn("nAbsPhotons");
	  analysisManager->CreateNtupleDColumn("absTime");
	  analysisManager->CreateNtupleDColumn("EPrimaries");
	  analysisManager->CreateNtupleDColumn("nSiPMPhotons");
	  analysisManager->FinishNtuple();

}

RunAction::~RunAction()
{
	delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
	  //inform the runManager to save random number seed
	  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	  // Get analysis manager
	  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	  // Open an output file
	  //
	  G4String fileName = "../data/Edep.root";
	  analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*)
{
	  // print histogram statistics
	  //
	  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	  if ( analysisManager->GetH1(0) ) {
	    G4cout << G4endl << " ----> print histograms statistic ";
	    if(isMaster) {
	      G4cout << "for the entire run " << G4endl << G4endl;
	    }
	    else {
	      G4cout << "for the local thread " << G4endl << G4endl;
	    }

	    G4cout << G4endl << " EDep : mean = "
	       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
	       << " rms = "
	       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;



	  }
//	  G4cout << G4endl << "Total Photon Count: " << EventAction::nAbsPhotons <<
//	    	 << "SiPM Photon Count: " << EventAction::nSiPMPhotons << G4endl;

	  // save histograms & ntuple
	  //
	  analysisManager->Write();
	  analysisManager->CloseFile();

}


