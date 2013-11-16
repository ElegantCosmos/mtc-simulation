//
// MTCG4PrimaryGeneratorMessenger.cc Mich 111222
//
#include "MTCG4PrimaryGeneratorMessenger.hh"
#include "MTCG4PrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

MTCG4PrimaryGeneratorMessenger::MTCG4PrimaryGeneratorMessenger(MTCG4PrimaryGeneratorAction* primGen)
: primaryGeneratorAction(primGen)
{
	primaryGeneratorDirectory = new G4UIdirectory("/MTCG4/primary/");
	primaryGeneratorDirectory->SetGuidance("Primary particle generator control.");

	commandForIncomingNeutrinoDirection = new
		G4UIcmdWithAString("/MTCG4/primary/setIncomingNeutrinoDirection", this);
	commandForIncomingNeutrinoDirection->SetGuidance("Set incoming neutrino direction.");
	commandForIncomingNeutrinoDirection->SetGuidance("Options: +z, random.");
	commandForIncomingNeutrinoDirection->SetCandidates("+z random");
	commandForIncomingNeutrinoDirection->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForInverseBetaDecayInteractionVertex = new
		G4UIcmdWithAString("/MTCG4/primary/setInverseBetaDecayInteractionVertex",
				this);
	commandForInverseBetaDecayInteractionVertex->
		SetGuidance("Set the neutrino inverse-beta decay interaction vertex in x-y-z coordinate space.");
	commandForInverseBetaDecayInteractionVertex->
		SetGuidance("Options: center, random.");
	commandForInverseBetaDecayInteractionVertex->SetCandidates("center random");
	commandForInverseBetaDecayInteractionVertex->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForReadingInPrimaryParticleDataFile = new
		G4UIcmdWithABool("/MTCG4/primary/readInPrimaryParticleDataFile", this);
	commandForReadingInPrimaryParticleDataFile->
  SetGuidance("Flag to read in data to set the initial momentum directions of the primary particles with respect to incoming neutrino direction from file primaryParticleData.dat.");
	commandForReadingInPrimaryParticleDataFile->
		SetGuidance("true: Initial momentum directions of positron and neutron are that of what is read in from file, false: initial momentum directions of positron and neutron are that of incoming neutrino.");
	commandForReadingInPrimaryParticleDataFile->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForInverseBetaDecayDaughterParticleKineticEnergy = new
		G4UIcmdWithADoubleAndUnit("/MTCG4/primary/setInverseBetaDecayDaughterParticleKineticEnergy", this);
	commandForInverseBetaDecayDaughterParticleKineticEnergy->
		SetGuidance("Set the initial kinetic energies (same value for both particles) of positron and neutron.");
	commandForInverseBetaDecayDaughterParticleKineticEnergy->
		SetGuidance("This command is overridden by the command readInPrimaryParticleDataFile being set to true.");
	commandForInverseBetaDecayDaughterParticleKineticEnergy->SetDefaultUnit("eV");
	commandForInverseBetaDecayDaughterParticleKineticEnergy->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForShootingPrimaryPositron = new
		G4UIcmdWithABool("/MTCG4/primary/shootPrimaryPositron", this);
	commandForShootingPrimaryPositron->
		SetGuidance("Flag to shoot primary positron.");
	commandForShootingPrimaryPositron->
		SetGuidance("True: shoot, false: do not shoot.");
	commandForShootingPrimaryPositron->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForShootingPrimaryNeutron = new
		G4UIcmdWithABool("/MTCG4/primary/shootPrimaryNeutron", this);
	commandForShootingPrimaryNeutron->
		SetGuidance("Flag to shoot primary neutron.");
	commandForShootingPrimaryNeutron->
		SetGuidance("True: shoot, false: do not shoot.");
	commandForShootingPrimaryNeutron->AvailableForStates(G4State_PreInit,
			G4State_Idle);

}

MTCG4PrimaryGeneratorMessenger::~MTCG4PrimaryGeneratorMessenger()
{
	delete commandForIncomingNeutrinoDirection;
	delete commandForInverseBetaDecayInteractionVertex;
	delete commandForReadingInPrimaryParticleDataFile;
	delete commandForInverseBetaDecayDaughterParticleKineticEnergy;
	delete commandForShootingPrimaryPositron;
	delete commandForShootingPrimaryNeutron;
}

void MTCG4PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == commandForIncomingNeutrinoDirection){
		primaryGeneratorAction->SetIncomingNeutrinoDirection(newValue);
	}
	else if(command == commandForInverseBetaDecayInteractionVertex){
		primaryGeneratorAction->SetInverseBetaDecayInteractionVertex(newValue);
	}
	else if(command == commandForReadingInPrimaryParticleDataFile){
		primaryGeneratorAction->
			SetFlagForReadingInPrimaryParticleDataFile(commandForReadingInPrimaryParticleDataFile->GetNewBoolValue(newValue));
	}
	else if(command == commandForInverseBetaDecayDaughterParticleKineticEnergy){
		primaryGeneratorAction->
			SetInverseBetaDecayDaughterParticleKineticEnergy(commandForInverseBetaDecayDaughterParticleKineticEnergy->GetNewDoubleValue(newValue));
	}
	else if(command == commandForShootingPrimaryPositron){
		primaryGeneratorAction->
			SetFlagForShootingPrimaryPositron(commandForShootingPrimaryPositron->GetNewBoolValue(newValue));
	}
	else if(command == commandForShootingPrimaryNeutron){
		primaryGeneratorAction->
			SetFlagForShootingPrimaryNeutron(commandForShootingPrimaryNeutron->GetNewBoolValue(newValue));
	}
}

G4String MTCG4PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == commandForIncomingNeutrinoDirection){
		answer = primaryGeneratorAction->GetIncomingNeutrinoDirection();
	}
	else if(command == commandForInverseBetaDecayInteractionVertex){
		answer = primaryGeneratorAction->GetInverseBetaDecayInteractionVertex();
	}
	else if(command == commandForReadingInPrimaryParticleDataFile){
		answer =
			commandForReadingInPrimaryParticleDataFile->ConvertToString(primaryGeneratorAction->GetFlagForReadingInPrimaryParticleDataFile());
	}
	else if(command == commandForInverseBetaDecayDaughterParticleKineticEnergy){
		answer =
			commandForInverseBetaDecayDaughterParticleKineticEnergy->ConvertToString(primaryGeneratorAction->GetInverseBetaDecayDaughterParticleKineticEnergy(), "eV");
	}
	else if(command == commandForShootingPrimaryPositron){
		answer = 
			commandForShootingPrimaryPositron->ConvertToString(primaryGeneratorAction->GetFlagForShootingPrimaryPositron());
	}
	else if(command == commandForShootingPrimaryNeutron){
		answer = 
			commandForShootingPrimaryNeutron->ConvertToString(primaryGeneratorAction->GetFlagForShootingPrimaryNeutron());
	}
	return answer;
}
