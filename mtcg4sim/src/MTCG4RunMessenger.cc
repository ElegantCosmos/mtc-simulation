#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

#include "MTCG4RunMessenger.hh"
#include "MTCG4RunAction.hh"

MTCG4RunMessenger::MTCG4RunMessenger(MTCG4RunAction* runAction)
: fRunAction(runAction)
{
	fSetRunIDCmd = new G4UIcmdWithAnInteger("/mtcg4/setRunID", this);
	fSetRunIDCmd->SetGuidance("Set run ID number.");
	fSetRunIDCmd->SetParameterName("runNo", false);
	fSetRunIDCmd->
		AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

	fSetOutputPathCmd = new
		G4UIcmdWithAString("/mtcg4/io/setOutputPath", this);
	fSetOutputPathCmd->
	 	SetGuidance("Set path for where all output files will be produced.");
 	fSetOutputPathCmd->
		SetParameterName("ouputPath", false);

	fSetStepOutputFileCmd = new
		G4UIcmdWithAString("/mtcg4/io/setStepOutputFile", this);
	fSetStepOutputFileCmd->
	 	SetGuidance("Set the file name for common run output containing all "
				"particle steps.");
 	fSetStepOutputFileCmd->
		SetParameterName("filename", false);

	fSetPEOutputFileCmd = new
		G4UIcmdWithAString("/mtcg4/io/setPEOutputFile", this);
	fSetPEOutputFileCmd->
		SetGuidance("Set the file name for photoelectron output.");
	fSetPEOutputFileCmd->SetParameterName("filename", false);
}

MTCG4RunMessenger::~MTCG4RunMessenger()
{
	delete fSetRunIDCmd;
	delete fSetOutputPathCmd;
	delete fSetStepOutputFileCmd;
	delete fSetPEOutputFileCmd;
}

void MTCG4RunMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue)
{
	G4String cmdName = cmd->GetCommandName();
	if(cmd == fSetRunIDCmd)
		fRunAction->SetRunID(fSetRunIDCmd->GetNewIntValue(newValue));
	else if(cmd == fSetOutputPathCmd)
		fRunAction->SetOutputPathName(newValue);
	else if(cmd == fSetStepOutputFileCmd)
		fRunAction->SetStepOutputFileName(newValue);
	else if(cmd == fSetPEOutputFileCmd)
		fRunAction->SetPEOutputFileName(newValue);
	else {
		G4Exception( "MTCG4RunMessenger.cc", "1", FatalErrorInArgument,
				(cmdName + ": " + newValue + " option not found.").c_str() );
	}
}
