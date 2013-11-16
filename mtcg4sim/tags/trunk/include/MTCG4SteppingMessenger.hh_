//
// MTCG4SteppingMessenger.hh Mich 111225
//

#ifndef MTCG4SteppingMessenger_h
#define MTCG4SteppingMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4SteppingAction;
class G4UIdirectory;
class G4UIcmdWithABool;

class MTCG4SteppingMessenger: public G4UImessenger
{
	public:
		MTCG4SteppingMessenger(MTCG4SteppingAction*);
		~MTCG4SteppingMessenger();

	public:
		void SetNewValue(G4UIcommand*, G4String);
		G4String GetCurrentValue(G4UIcommand*);

	private:
		MTCG4SteppingAction* steppingAction;
		G4UIdirectory* steppingDirectory;
		G4UIcmdWithABool* commandForOutputtingOnlyFirst30Events;
};

#endif
