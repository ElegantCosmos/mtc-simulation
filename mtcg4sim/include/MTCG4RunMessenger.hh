#ifndef MTCG4_RUN_MESSENGER_HH
#define MTCG4_RUN_MESSENGER_HH 1

#include "G4UImessenger.hh"

class MTCG4RunAction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

class MTCG4RunMessenger: public G4UImessenger
{
public:
	MTCG4RunMessenger(MTCG4RunAction* myAction);
	~MTCG4RunMessenger();

	void SetNewValue(G4UIcommand* command, G4String newValue);

private:
	MTCG4RunAction			*fRunAction;

	G4UIcmdWithAnInteger	*fSetRunIDCommand;
	G4UIcmdWithAString		*fSetOutputPathCommand;
	G4UIcmdWithAString		*fSetStepOutputFileCommand;
	G4UIcmdWithAString		*fSetPEOutputFileCommand;
};

#endif
