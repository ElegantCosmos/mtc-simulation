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
//class StackingMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MTCG4StackingAction : public G4UserStackingAction
{
  public:
    MTCG4StackingAction(/*MTCG4RunAction*,MTCG4EventAction*,HistoManager**/ );
   ~MTCG4StackingAction();
   
    void SetKillStatus(G4int value) {killSecondary = value;};
    MTCG4Trajectory* GetTrajectory(G4int);
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
	virtual void NewStage();
	virtual void PrepareNewEvent();
    
  private:
    MTCG4RunAction*			runaction;
    MTCG4EventAction*		eventaction;    
    //HistoManager*			histoManager;
    
    G4int					killSecondary;
    //StackingMessenger*	stackMessenger;
        
	G4int					stage;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

