/////////////////////////////////////////////////////////////////////////
//
//
//
// This class collects information at three different levels: 
// 1 - Energy deposited in the sensitive volume
// 2 - Number of photons absorbed by the photocathode (or just generated!)
// 3 - Time at which each photon is absorbed
//
// This information is passed to the Event Action Class via 
// the eventAction pointer
//
/////////////////////////////////////////////////////////////////////////

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4SteppingManager.hh"

#include "G4RunManager.hh"

#include "fstream"
#include "iomanip"

using namespace std;	 

SteppingAction::SteppingAction(EventAction* EvAct)
:eventAction(EvAct)
{ }

SteppingAction::~SteppingAction()
{ }

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
	const G4String currentPhysicalName 
    = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();

	const G4String particleName
	= aStep->GetTrack()->GetDefinition()->GetParticleName();

	if (currentPhysicalName == "BackSiPM") {

		aStep->GetTrack()->SetTrackStatus(fStopAndKill);

		}

	if (currentPhysicalName == "Scintillator"){

		G4double EdepStep = aStep->GetTotalEnergyDeposit();
		
		if (EdepStep > 0.) eventAction->EdepInCrystal = eventAction->EdepInCrystal + EdepStep;


		if (particleName == "opticalphoton"){
		 	eventAction->nAbsPhotons++;

			const G4String nextvol
			= aStep->GetTrack()->GetNextVolume()->GetName();

		 	//G4cout<< "Next Volume: " << nextvol << G4endl;




		 	if (nextvol == "SiPM") {
		 		eventAction->nSiPMPhotons++;

		 		eventAction->absTime = aStep -> GetPreStepPoint() -> GetGlobalTime();

		 		aStep->GetTrack()->SetTrackStatus(fStopAndKill);

		 	}
		 	//G4cout << "nAbsPhotons : " << eventAction->nAbsPhotons << G4endl;
		 	//eventAction->absTime = aStep -> GetPreStepPoint() -> GetGlobalTime();
		 	//aStep->GetTrack()->SetTrackStatus(fStopAndKill);
		 }


	}
	
	// check if the photon hits front of SiPM (the face in contact with the scintillator)

	//if (currentPhysicalName == "SiPM"){

	//	eventAction->nSiPMPhotons++;
			
	//	eventAction->absTime = aStep -> GetPreStepPoint() -> GetGlobalTime();


	//	aStep->GetTrack()->SetTrackStatus(fStopAndKill);



}

