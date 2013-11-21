//
// MTCG4SteppingMessenger.cc Mich 111225
//
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

#include "MTCG4SteppingMessenger.hh"
#include "MTCG4SteppingAction.hh"

MTCG4SteppingMessenger::MTCG4SteppingMessenger(MTCG4SteppingAction* step)
:	_steppingAction(step)
{
	_steppingDirectory = new G4UIdirectory("/mtcg4/io/");
	_steppingDirectory->SetGuidance("Stepping action control.");

	_commandForOutputtingOnlyFirstNEvents = new
		G4UIcmdWithAnInteger("/mtcg4/io/outputOnlyFirstNEvents", this);
	_commandForOutputtingOnlyFirstNEvents ->
		SetGuidance("Set only the first n events of a run to be outputted to file.");
	_commandForOutputtingOnlyFirstNEvents ->
		SetGuidance("n < 0: output all events, n >= 0: output only first n events");
	_commandForOutputtingOnlyFirstNEvents ->
		SetParameterName("onlyFirstNEventsAreOutputted", true);
	_commandForOutputtingOnlyFirstNEvents->AvailableForStates(G4State_PreInit,
			G4State_Idle);
}

MTCG4SteppingMessenger::~MTCG4SteppingMessenger()
{
	delete _commandForOutputtingOnlyFirstNEvents;
	delete _steppingDirectory;
}

void MTCG4SteppingMessenger::SetNewValue(G4UIcommand* command, G4String
		newValue)
{
	if(command == _commandForOutputtingOnlyFirstNEvents)
		_steppingAction -> SetOutputOnlyFirstNEvents(
				_commandForOutputtingOnlyFirstNEvents -> GetNewIntValue(newValue) );
}

G4String MTCG4SteppingMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == _commandForOutputtingOnlyFirstNEvents)
		answer = _commandForOutputtingOnlyFirstNEvents ->
			ConvertToString(_steppingAction -> GetOutputOnlyFirstNEvents());
	return answer;
}
