#ifndef MTCG4TrackingAction_h
#define MTCG4TrackingAction_h 1

#include "G4UserTrackingAction.hh"

class G4Track;
class StepTextIO;
class StepRootIO;

class MTCG4TrackingAction : public G4UserTrackingAction {
public:
    MTCG4TrackingAction();
    virtual ~MTCG4TrackingAction();
   
    virtual void PreUserTrackingAction(const G4Track *theTrack);
	virtual void PostUserTrackingAction(const G4Track *theTrack);

private:
	void DoStepOutputToRootFile(const G4Track *theTrack);
	void DoStepOutputToTextFile(const G4Track *theTrack);

private:
	StepTextIO *fStepOutToText;
	StepRootIO *fStepOutToRoot;
};

#endif
