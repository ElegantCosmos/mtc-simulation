//
// MTCG4StackingMessenger.hh Mich 2013/11/25
//

#ifndef MTCG4_STACKING_MESSENGER_HH
#define MTCG4_STACKING_MESSENGER_HH

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4StackingAction;
class G4UIdirectory;
class G4UIcmdWithAString;

class MTCG4StackingMessenger: public G4UImessenger
{
public:
	MTCG4StackingMessenger(MTCG4StackingAction*);
	~MTCG4StackingMessenger();

public:
	void SetNewValue(G4UIcommand*, G4String);
	//G4String GetCurrentValue(G4UIcommand*);

private:
	MTCG4StackingAction*	fStackingAction;

	G4UIdirectory*			fStackingDir;

	G4UIcmdWithAString*		fCmdForTrackingOpticalPhotons;
};

#endif
