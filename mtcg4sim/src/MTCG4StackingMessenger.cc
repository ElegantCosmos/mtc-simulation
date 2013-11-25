//
// MTCG4StackingMessenger.cc Mich 2013/11/25
//

#include "MTCG4StackingMessenger.hh"
#include "MTCG4StackingAction.hh"

#include "G4UIcmdWithAString.hh"

MTCG4StackingMessenger::MTCG4StackingMessenger(MTCG4StackingAction* action)
	: fStackingAction(action)
{
	fStackingDir = new G4UIdirectory("/mtcg4/stackingAction/");
	fStackingDir->SetGuidance("Stacking action controls.");

	fCmdForTrackingOpticalPhotons = new
		G4UIcmdWithAString("/mtcg4/stackingAction/trackOpticalPhotons", this);
	fCmdForTrackingOpticalPhotons->
		SetGuidance("Set tracking option for optical photons.");
	fCmdForTrackingOpticalPhotons->
		SetGuidance("Options: trackImmediately, saveAndTrack, saveAndKill, "
				"killImmediately.");
	fCmdForTrackingOpticalPhotons->
		SetCandidates("trackImmediately killImmediately saveAndTrack saveAndKill.");
	fCmdForTrackingOpticalPhotons->
		SetParameterName("trackOpticalPhotons", true);
	fCmdForTrackingOpticalPhotons->SetDefaultValue("trackImmediately");
	fCmdForTrackingOpticalPhotons->
		AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4StackingMessenger::~MTCG4StackingMessenger()
{
	delete fStackingDir;
	
	delete fCmdForTrackingOpticalPhotons;
}

void MTCG4StackingMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
	G4String answer;
	if (cmd == fCmdForTrackingOpticalPhotons) {
		fStackingAction->SetTrackOpticalPhotonsFlag(newValue);
	}
	else {
		G4Exception(
				"MTCG4PrimaryGeneratorMessenger.cc",
				"0",
				FatalErrorInArgument,
				"Macro cmd not found. Please check macro parameters."
				);
	}
}
