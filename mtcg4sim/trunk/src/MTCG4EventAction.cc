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
	G4int eventNo = theEvent->GetEventID();
	G4cout << "Event " << eventNo << " done!" << G4endl << G4endl;

	// Routine for filling data at end of event.
	fPEOut = PESdsIO::GetInstance();
	fPEOut->Write(theEvent);

	fStepOutToRoot = StepRootIO::GetInstance();
	fStepOutToRoot->Write();

	//// Print stored trajectories of this event.
	//G4TrajectoryContainer* trajectoryContainer = theEvent->GetTrajectoryContainer();
	//G4int nTrajectories = 0;
	//if(trajectoryContainer) {
	//	nTrajectories = trajectoryContainer->entries();
	//	for(G4int i = 0; i < nTrajectories; i++){
	//		MTCG4Trajectory* traj = (MTCG4Trajectory*)((*trajectoryContainer)[i]);
	//		G4cout
	//			<< "Trajectory "
	//			<< i
	//			<< ": "
	//			<< traj->GetParticleName()
	//			<< " with trackID "
	//			<< traj->GetTrackID()
	//			<< ", and parentID "
	//			<< traj->GetParentID()
	//			<< G4endl;
	//	}
	//}
}
