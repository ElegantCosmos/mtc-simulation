//////////////////////////////////////////////////////////
// MTCG4EventAction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <ctime>

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"

#include "MTCG4Trajectory.hh"
#include "MTCG4EventAction.hh"

#include "PESdsIO.hh"
#include "StepRootIO.hh"

//KLHitPhotonCollection MTCG4EventAction :: theHitPhotons = MTCG4HitPhotonCollection();
MTCG4HitPmtCollection MTCG4EventAction :: fTheHitPmtCollection = MTCG4HitPmtCollection();

MTCG4EventAction::MTCG4EventAction()
{}

MTCG4EventAction::~MTCG4EventAction()
{}

// Method Called At Beginning Of Action //
void MTCG4EventAction::BeginOfEventAction(const G4Event*)
{
	fTheHitPmtCollection.Clear(); // Clear all data stored in PMT collection.
}

// Method Called At End Of Action //
void MTCG4EventAction::EndOfEventAction(const G4Event* theEvent)
{
	fTheHitPmtCollection.Print(G4cout);
	G4int eventNo = theEvent->GetEventID();
	G4cout << "Event " << eventNo << " done!" << G4endl << G4endl;

	//this->DoStepOutToRoot(); // don't use for now
	//this->WritePESds(theEvent);
	//this->PrintTrajectories(theEvent);
}

void MTCG4EventAction::DoStepOutToRoot()
{
	// Writing step info to TTree at end of every event is probably time
	// consuming b/c we have to read out pre-existing tree and append new tree
	// for current event step info and resave. This is okay when runs take a
	// long time to process and we want to save relatively few events such as
	// those that only include long-lived isotopes of muon spallation products.
	// But in general we should do this at the end of the run.
	fStepOutToRoot = StepRootIO::GetInstance();
	fStepOutToRoot->WriteAtEndOfEvent();
}

void MTCG4EventAction::PrintTrajectories(const G4Event* theEvent)
{
	// Print stored trajectories of this event.
	G4TrajectoryContainer* trajectoryContainer = theEvent->GetTrajectoryContainer();
	G4int nTrajectories = 0;
	if(trajectoryContainer) {
		nTrajectories = trajectoryContainer->entries();
		for(G4int i = 0; i < nTrajectories; i++){
			MTCG4Trajectory* traj = (MTCG4Trajectory*)((*trajectoryContainer)[i]);
			G4cout
				<< "Trajectory "
				<< i
				<< ": "
				<< traj->GetParticleName()
				<< " with trackID "
				<< traj->GetTrackID()
				<< ", and parentID "
				<< traj->GetParentID()
				<< G4endl;
		}
	}
}

void MTCG4EventAction::WritePESds(const G4Event* theEvent)
{
	// Routine for filling data at end of event.
	fPEOut = PESdsIO::GetInstance();
	fPEOut->Write(theEvent);
}
