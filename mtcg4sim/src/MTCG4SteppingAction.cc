///////////////////////////////////////
// MTCG4SteppingAction.cc by Mich 110502
///////////////////////////////////////

#include "MTCG4PmtSD.hh"
#include "MTCG4SteppingAction.hh"
#include "MTCG4SteppingMessenger.hh"
#include "MTCG4DetectorConstruction.hh"

#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "Randomize.hh"
#include "G4OpBoundaryProcess.hh"

#include "TFile.h"
#include "TTree.h"

#include "StepRootIO.hh"
#include "StepTextIO.hh"

//
// Define a constant quantum efficiency (0. ~ 1.) to be used by photo sensitive
// pixels when optical surface properties are not used.
//
#define CONSTANT_QUANTUM_EFFICIENCY 1.

MTCG4SteppingAction::MTCG4SteppingAction()
{
	// Flag to limit output data amount for testing purposes.
	fOutputOnlyFirstNEvents = -1; // All events outputted by default.

	fMessenger = new MTCG4SteppingMessenger(this);
}

MTCG4SteppingAction::~MTCG4SteppingAction()
{
	delete fMessenger;
}

// Method Called During a Step //
void MTCG4SteppingAction::UserSteppingAction(const G4Step *theStep)
{
	//const G4Event *theEvent = G4EventManager::GetEventManager()->
	//	GetConstCurrentEvent();
	//G4Track *theTrack = theStep->GetTrack();
	//G4ParticleDefinition *particleDef = theTrack->GetDefinition();
	G4VPhysicalVolume *postStepPointPhysicalVolume =
		theStep->GetPostStepPoint()->GetPhysicalVolume();
	if (!postStepPointPhysicalVolume) {
		// If the post-step point is not available,
		// the particle is out of the world volume.
		// Do not do anything with it and just return.
		//G4cout << particleDef->GetParticleName()
		//	<< " post step point is out of world!\n";
		return;
	}
	
	if (fOutputOnlyFirstNEvents >= 0) {// Do output for only first n events.
		if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->
				GetEventID() >= fOutputOnlyFirstNEvents)
			return;
	}

	DoStepOutputToRootFile(theStep);
	DoStepOutputToTextFile(theStep);

	fPhotonDetectedAtEndOfStep = false; // Reset photon detection flag.
}

//
// Do output for all events.
//
void MTCG4SteppingAction::DoStepOutputToTextFile(const G4Step *theStep) {
	const G4Track *theTrack = theStep->GetTrack();
	fStepOutToText = StepTextIO::GetInstance();
	fStepOutToText->Fill(theTrack, theStep);
	fStepOutToText->Write();
}

void MTCG4SteppingAction::DoStepOutputToRootFile(const G4Step *theStep) {
	const G4Track *theTrack = theStep->GetTrack();
	fStepOutToRoot = StepRootIO::GetInstance();
	fStepOutToRoot->Fill(theTrack, theStep);
}
