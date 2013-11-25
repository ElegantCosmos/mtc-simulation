//
// MTCG4SteppingMessenger.hh Mich 111225
//

#ifndef MTCG4SteppingMessenger_h
#define MTCG4SteppingMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4SteppingAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

class MTCG4SteppingMessenger: public G4UImessenger
{
	public:
		MTCG4SteppingMessenger(MTCG4SteppingAction*);
		~MTCG4SteppingMessenger();

	public:
		void SetNewValue(G4UIcommand* cmd, G4String newValue);
		G4String GetCurrentValue(G4UIcommand* cmd);

	private:
		MTCG4SteppingAction*	fStepAction;

		G4UIdirectory*			fStepDir;
		G4UIcmdWithAnInteger*	fOutputOnlyFirstNEventsCmd;
};

#endif
