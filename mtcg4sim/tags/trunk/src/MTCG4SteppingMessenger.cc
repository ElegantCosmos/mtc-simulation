//
// MTCG4SteppingMessenger.cc Mich 111225
//
#include "MTCG4SteppingMessenger.hh"
#include "MTCG4SteppingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

MTCG4SteppingMessenger::MTCG4SteppingMessenger(MTCG4SteppingAction*
		step) : steppingAction(step)
{
	steppingDirectory = new G4UIdirectory("/MTCG4/steppingAction/");
	steppingDirectory->SetGuidance("Stepping action control.");

	commandForOutputtingOnlyFirst30Events = new
		G4UIcmdWithABool("/MTCG4/steppingAction/outputOnlyFirst30Events", this);
	commandForOutputtingOnlyFirst30Events->
		SetGuidance("Set only the first 30 events of a run to be outputted to file.");
	commandForOutputtingOnlyFirst30Events->SetGuidance("true: output only first 30 events, false: output all events.");
	commandForOutputtingOnlyFirst30Events->SetParameterName("onlyFirst30EventsAreOutputted", true);
	commandForOutputtingOnlyFirst30Events->AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4SteppingMessenger::~MTCG4SteppingMessenger()
{
	delete commandForOutputtingOnlyFirst30Events;
}

void MTCG4SteppingMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == commandForOutputtingOnlyFirst30Events){
		steppingAction->SetOutputOnlyFirst30Events(commandForOutputtingOnlyFirst30Events->GetNewBoolValue(newValue));
	}
}

G4String MTCG4SteppingMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == commandForOutputtingOnlyFirst30Events){
		answer =
			commandForOutputtingOnlyFirst30Events->ConvertToString(steppingAction->GetOutputOnlyFirst30Events());
	}
	return answer;
}
