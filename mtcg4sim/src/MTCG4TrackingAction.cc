#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "MTCG4TrackingAction.hh"
#include "MTCG4Trajectory.hh"
#include "StepRootIO.hh"
#include "StepTextIO.hh"

MTCG4TrackingAction::MTCG4TrackingAction()
{;}

MTCG4TrackingAction::~MTCG4TrackingAction()
{;}

void MTCG4TrackingAction::PreUserTrackingAction(const G4Track* theTrack)
{
	//fpTrackingManager->SetStoreTrajectory(1);
	//fpTrackingManager->SetTrajectory(new MTCG4Trajectory(theTrack));

	// Output 0th step info to various output files.
	DoStepOutputToRootFile(theTrack);
	DoStepOutputToTextFile(theTrack);
}

void MTCG4TrackingAction::PostUserTrackingAction(const G4Track*)
{;}

// Do output for all events.
void MTCG4TrackingAction::DoStepOutputToTextFile(const G4Track *theTrack) {
	const G4Step *theStep;
	fStepOutToText = StepTextIO::GetInstance();
	// At PreUserTrackingAction no steps are tracked yet and only the G4Track
	// object exists. So we will only pass in a NULL pointer for the step. This
	// is recognized correctly to be the 0th step of the track in the
	// fStepOutToText instance.
	fStepOutToText->Fill(theTrack, theStep = NULL);
	fStepOutToText->Write();
}

void MTCG4TrackingAction::DoStepOutputToRootFile(const G4Track *theTrack) {
	const G4Step *theStep;
	fStepOutToRoot = StepRootIO::GetInstance();
	fStepOutToRoot->Fill(theTrack, theStep = NULL);
}
