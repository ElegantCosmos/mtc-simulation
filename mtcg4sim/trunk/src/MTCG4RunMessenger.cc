#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

#include "MTCG4RunMessenger.hh"
#include "MTCG4RunAction.hh"

MTCG4RunMessenger::MTCG4RunMessenger(MTCG4RunAction* runAction)
: fRunAction(runAction)
{
	fSetRunIDCommand = new G4UIcmdWithAnInteger("/mtcg4/setRunID", this);
	fSetRunIDCommand->SetGuidance("Set run ID number.");
	fSetRunIDCommand->SetParameterName("runNo", false);
	fSetRunIDCommand->
		AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

	fSetOutputPathCommand = new
		G4UIcmdWithAString("/mtcg4/io/setOutputPath", this);
	fSetOutputPathCommand->
	 	SetGuidance("Set path for where all output files will be produced.");
 	fSetOutputPathCommand->
		SetParameterName("ouputPath", false);

	fSetStepOutputFileCommand = new
		G4UIcmdWithAString("/mtcg4/io/setStepOutputFile", this);
	fSetStepOutputFileCommand->
	 	SetGuidance("Set the file name for common run output containing all "
				"particle steps.");
 	fSetStepOutputFileCommand->
		SetParameterName("filename", false);

	fSetPEOutputFileCommand = new
		G4UIcmdWithAString("/mtcg4/io/setPEOutputFile", this);
	fSetPEOutputFileCommand->
		SetGuidance("Set the file name for photoelectron output.");
	fSetPEOutputFileCommand->SetParameterName("filename", false);
}

MTCG4RunMessenger::~MTCG4RunMessenger()
{
	delete fSetRunIDCommand;
	delete fSetOutputPathCommand;
	delete fSetStepOutputFileCommand;
	delete fSetPEOutputFileCommand;
}

void MTCG4RunMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == fSetRunIDCommand)
		fRunAction->SetRunID(fSetRunIDCommand->GetNewIntValue(newValue));
	else if(command == fSetOutputPathCommand)
		fRunAction->SetOutputPathName(newValue);
	else if(command == fSetStepOutputFileCommand)
		fRunAction->SetStepOutputFileName(newValue);
	else if(command == fSetPEOutputFileCommand)
		fRunAction->SetPEOutputFileName(newValue);
}
