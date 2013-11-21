#include "G4TrackingManager.hh"
#include "G4Track.hh"

#include "MTCG4TrackingAction.hh"
#include "MTCG4Trajectory.hh"

MTCG4TrackingAction::MTCG4TrackingAction()
{;}

MTCG4TrackingAction::~MTCG4TrackingAction()
{;}

void MTCG4TrackingAction::PreUserTrackingAction(const G4Track* /*aTrack*/)
{
	//fpTrackingManager->SetStoreTrajectory(1);
	//fpTrackingManager->SetTrajectory(new MTCG4Trajectory(aTrack));
}

void MTCG4TrackingAction::PostUserTrackingAction(const G4Track*)
{;}
