//
// MTCG4SteppingMessenger.cc Mich 111225
//
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

#include "MTCG4SteppingMessenger.hh"
#include "MTCG4SteppingAction.hh"

MTCG4SteppingMessenger::MTCG4SteppingMessenger(MTCG4SteppingAction* step)
: fStepAction(step)
{
	fStepDir = new G4UIdirectory("/mtcg4/io/");
	fStepDir->SetGuidance("Stepping action control.");

	fOutputOnlyFirstNEventsCmd = new
		G4UIcmdWithAnInteger("/mtcg4/io/outputOnlyFirstNEvents", this);
	fOutputOnlyFirstNEventsCmd ->
		SetGuidance("Set only the first n events of a run to be outputted to file.");
	fOutputOnlyFirstNEventsCmd ->
		SetGuidance("n < 0: output all events, n >= 0: output only first n events");
	fOutputOnlyFirstNEventsCmd ->
		SetParameterName("onlyFirstNEventsAreOutputted", true);
	fOutputOnlyFirstNEventsCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);
}

MTCG4SteppingMessenger::~MTCG4SteppingMessenger()
{
	delete fOutputOnlyFirstNEventsCmd;
	delete fStepDir;
}

void MTCG4SteppingMessenger::SetNewValue(G4UIcommand* cmd, G4String
		newValue)
{
	if(cmd == fOutputOnlyFirstNEventsCmd)
		fStepAction -> SetOutputOnlyFirstNEvents(
				fOutputOnlyFirstNEventsCmd -> GetNewIntValue(newValue) );
}

G4String MTCG4SteppingMessenger::GetCurrentValue(G4UIcommand* cmd)
{
	G4String answer;
	if(cmd == fOutputOnlyFirstNEventsCmd)
		answer = fOutputOnlyFirstNEventsCmd ->
			ConvertToString(fStepAction -> GetOutputOnlyFirstNEvents());
	return answer;
}
