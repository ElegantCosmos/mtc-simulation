#include "MTCG4TrackingAction.hh"
//#include "MTCG4TrackInformation.hh"
#include "MTCG4Trajectory.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"

MTCG4TrackingAction::MTCG4TrackingAction()
{;}

MTCG4TrackingAction::~MTCG4TrackingAction()
{;}

void MTCG4TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
	fpTrackingManager->SetStoreTrajectory(true);
	fpTrackingManager->SetTrajectory(new MTCG4Trajectory(aTrack));

//	if(aTrack->GetParentID()==0 && aTrack->GetUserInformation()==0)
//	{
//		MTCG4TrackInformation* aTrackInformation = new MTCG4TrackInformation(aTrack);
//		G4Track* theTrack = (G4Track*)aTrack;
//		theTrack -> SetUserInformation(aTrackInformation);
//	}
}

//void MTCG4TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
//{
//}
