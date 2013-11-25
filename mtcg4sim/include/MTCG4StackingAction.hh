#ifndef MTCG4StackingAction_h
#define MTCG4StackingAction_h 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"

#include "MTCG4Trajectory.hh"

class G4TouchableHistory;
class G4Track;
//class MTCG4Trajectory;
class MTCG4RunAction;
class MTCG4EventAction;
//enum G4ClassificationOfNewTrack;

//class HistoManager;
class MTCG4StackingMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MTCG4StackingAction : public G4UserStackingAction
{
public:
	MTCG4StackingAction(/*MTCG4RunAction*,MTCG4EventAction*,HistoManager**/ );
	~MTCG4StackingAction();

	//void SetKillStatus(G4int value) { fKillSecondary = value; };
	void SetTrackOpticalPhotonsFlag(G4String value)
	{
		if (value == "killImmediately")
			fOpticalPhotonTrackingStatus = killImmediately;
		else if (value == "trackImmediately")
		   	fOpticalPhotonTrackingStatus = trackImmediately;
		else if (value == "saveAndTrack")
		   	fOpticalPhotonTrackingStatus = saveAndTrack;
		else if (value == "saveAndKill")
			fOpticalPhotonTrackingStatus = saveAndKill;
		else {
			G4Exception("MTCG4StackingAction.hh", "1", FatalErrorInArgument,
					"Error: wrong optical photon tracking option provided by user.");
		}
   	}
	MTCG4Trajectory* GetTrajectory(G4int);
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
	virtual void NewStage();
	virtual void PrepareNewEvent();

private:
	MTCG4StackingMessenger*	fStackMessenger;

	MTCG4RunAction*			fRunAction;
	MTCG4EventAction*		fEventAction;    
	//HistoManager*			fHistoManager;

	//G4int					fKillSecondary;
	G4int					fStage;

	enum HOW_TO_TRACK_OPTICAL_PHOTON {
		killImmediately,
		trackImmediately,
		saveAndTrack,
		saveAndKill
	} fOpticalPhotonTrackingStatus;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

