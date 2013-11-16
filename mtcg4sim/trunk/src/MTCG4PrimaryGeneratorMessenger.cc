//
// MTCG4PrimaryGeneratorMessenger.cc Mich 111222
//

#include "CLHEP/Random/Random.h"

#include "Randomize.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "MTCG4PrimaryGeneratorMessenger.hh"
#include "MTCG4PrimaryGeneratorAction.hh"

MTCG4PrimaryGeneratorMessenger::MTCG4PrimaryGeneratorMessenger(MTCG4PrimaryGeneratorAction* primGen)
: fPrimaryGeneratorAction(primGen)
{
	fPrimaryGeneratorDirectory = new G4UIdirectory("/mtcg4/primary/");
	fPrimaryGeneratorDirectory->SetGuidance("Primary particle generator control.");

	fSetSeedCommand = new G4UIcmdWithAnInteger("/generator/seed", this);
	fSetSeedCommand->SetGuidance("Set the random seed of the generator.");
	fSetSeedCommand->SetParameterName("seed", false);
	fSetSeedCommand->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetInputPathCommand = new
		G4UIcmdWithAString("/mtcg4/primary/setInputPath", this);
	fSetInputPathCommand->
	 	SetGuidance("Set path for where all input files are located.");
 	fSetInputPathCommand->
		SetParameterName("inputPath", false);

	fSetPrimaryDataFileCommand = new
		G4UIcmdWithAString("/mtcg4/primary/setPrimaryParticleDataFile", this);
	fSetPrimaryDataFileCommand->SetGuidance("Specify the file that contains "
			"parimary particle data.");      
	fSetPrimaryDataFileCommand->SetParameterName("filename", false);

	fSetIncomingNeutrinoDirectionCommand = new
		G4UIcmdWithAString("/mtcg4/primary/setIncomingNeutrinoDirection", this);
	fSetIncomingNeutrinoDirectionCommand->SetGuidance("Set incoming neutrino direction.");
	fSetIncomingNeutrinoDirectionCommand->SetGuidance("Options: +z, random.");
	fSetIncomingNeutrinoDirectionCommand->SetCandidates("+z random");
	fSetIncomingNeutrinoDirectionCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fSetInverseBetaDecayInteractionVertexCommand = new
		G4UIcmdWithAString("/mtcg4/primary/setInverseBetaDecayInteractionVertex",
				this);
	fSetInverseBetaDecayInteractionVertexCommand->
		SetGuidance("Set the neutrino inverse-beta decay interaction vertex in x-y-z coordinate space.");
	fSetInverseBetaDecayInteractionVertexCommand->
		SetGuidance("Options: center, random, -z.");
	fSetInverseBetaDecayInteractionVertexCommand->
		SetCandidates("center random -z");
	fSetInverseBetaDecayInteractionVertexCommand->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fReadInPrimaryParticleDataFileCommand = new
		G4UIcmdWithABool("/mtcg4/primary/readInPrimaryParticleDataFile", this);
	fReadInPrimaryParticleDataFileCommand->
  SetGuidance("Flag to read in data to set the initial momentum directions of the primary particles with respect to incoming neutrino direction from file primaryParticleData.dat.");
	fReadInPrimaryParticleDataFileCommand->
		SetGuidance("true: Initial momentum directions of positron and neutron are that of what is read in from file, false: initial momentum directions of positron and neutron are that of incoming neutrino.");
	fReadInPrimaryParticleDataFileCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fSetInverseBetaDecayDaughterParticleKineticEnergyCommand = new
		G4UIcmdWithADoubleAndUnit("/mtcg4/primary/setInverseBetaDecayDaughterParticleKineticEnergy", this);
	fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->
		SetGuidance("Set the initial kinetic energies (same value for both particles) of positron and neutron.");
	fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->
		SetGuidance("This command is overridden by the command readInPrimaryParticleDataFile being set to true.");
	fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->SetDefaultUnit("eV");
	fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fShootPrimaryPositronCommand = new
		G4UIcmdWithABool("/mtcg4/primary/shootPrimaryPositron", this);
	fShootPrimaryPositronCommand->
		SetGuidance("Flag to shoot primary positron from IBD.");
	fShootPrimaryPositronCommand->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootPrimaryPositronCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootPrimaryNeutronCommand = new
		G4UIcmdWithABool("/mtcg4/primary/shootPrimaryNeutron", this);
	fShootPrimaryNeutronCommand->
		SetGuidance("Flag to shoot primary neutron from IBD.");
	fShootPrimaryNeutronCommand->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootPrimaryNeutronCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootParticleGunCommand = new
		G4UIcmdWithABool("/gun/shootParticleGun", this);
	fShootParticleGunCommand->
		SetGuidance("Flag to shoot particle produced by G4ParticleGun.");
	fShootParticleGunCommand->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootParticleGunCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootGeneralParticleSourceCommand = new
		G4UIcmdWithABool("/gps/shootGeneralParticleSource", this);
	fShootGeneralParticleSourceCommand->
		SetGuidance("Flag to shoot particle produced by G4GeneralParticleSource.");
	fShootGeneralParticleSourceCommand->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootGeneralParticleSourceCommand->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootLaserCommand = new
		G4UIcmdWithABool("/gun/shootLaser", this);
	fShootLaserCommand->
		SetGuidance("Flag to shoot laser pulse of photons.");
	fShootLaserCommand->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootLaserCommand->AvailableForStates(G4State_PreInit, G4State_Idle);

	fShootCosmicRayMuonCommand = new
		G4UIcmdWithABool("/gun/shootCosmicRayMuon", this);
	fShootCosmicRayMuonCommand->
		SetGuidance("Flag to shoot cosmic ray muons.");
	fShootCosmicRayMuonCommand->
		SetGuidance("A best guess sea level spectrum and angle distribution "
				"will be used.");
	fShootCosmicRayMuonCommand->
		AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4PrimaryGeneratorMessenger::~MTCG4PrimaryGeneratorMessenger()
{
	delete fSetSeedCommand;
	delete fSetInputPathCommand;
	delete fSetPrimaryDataFileCommand;
	delete fSetIncomingNeutrinoDirectionCommand;
	delete fSetInverseBetaDecayInteractionVertexCommand;
	delete fReadInPrimaryParticleDataFileCommand;
	delete fSetInverseBetaDecayDaughterParticleKineticEnergyCommand;
	delete fShootPrimaryPositronCommand;
	delete fShootPrimaryNeutronCommand;
	delete fPrimaryGeneratorDirectory;
	delete fShootParticleGunCommand;
	delete fShootGeneralParticleSourceCommand;
	delete fShootLaserCommand;
	delete fShootCosmicRayMuonCommand;
}

void MTCG4PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == fSetSeedCommand) {
		std::istringstream is((const char *)newValue);
		G4int new_seed = 0;
		is >> new_seed;
		CLHEP::HepRandom::setTheSeed(new_seed);
	}

	else if (command == fSetInputPathCommand)
		fPrimaryGeneratorAction->SetInputPath(newValue);

	else if (command == fSetPrimaryDataFileCommand)
		fPrimaryGeneratorAction->SetPrimaryDataFileName(newValue);

	else if (command == fSetIncomingNeutrinoDirectionCommand)
		fPrimaryGeneratorAction->SetIncomingNeutrinoDirection(newValue);

	else if (command == fSetInverseBetaDecayInteractionVertexCommand)
		fPrimaryGeneratorAction->SetInverseBetaDecayInteractionVertex(newValue);

	else if (command == fReadInPrimaryParticleDataFileCommand)
		fPrimaryGeneratorAction->SetFlagForReadingInPrimaryParticleDataFile(
					fReadInPrimaryParticleDataFileCommand->
					GetNewBoolValue(newValue) );

	else if (command == fSetInverseBetaDecayDaughterParticleKineticEnergyCommand)
		fPrimaryGeneratorAction->
			SetInverseBetaDecayDaughterParticleKineticEnergy(
				fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->
				GetNewDoubleValue(newValue) );

	else if (command == fShootPrimaryPositronCommand)
		fPrimaryGeneratorAction->
			SetFlagForShootingPrimaryPositron( fShootPrimaryPositronCommand->
					GetNewBoolValue(newValue) );

	else if (command == fShootPrimaryNeutronCommand)
		fPrimaryGeneratorAction->SetFlagForShootingPrimaryNeutron(
				fShootPrimaryNeutronCommand->GetNewBoolValue(newValue) );

	else if (command == fShootParticleGunCommand)
		fPrimaryGeneratorAction->SetFlagForShootingParticleGun(
				fShootParticleGunCommand->GetNewBoolValue(newValue) );

	else if (command == fShootGeneralParticleSourceCommand)
		fPrimaryGeneratorAction->SetFlagForShootingGeneralParticleSource(
				fShootGeneralParticleSourceCommand->GetNewBoolValue(newValue) );

	else if (command == fShootLaserCommand)
		fPrimaryGeneratorAction->SetFlagForShootingLaser(
				fShootLaserCommand->GetNewBoolValue(newValue) );

	else if (command == fShootCosmicRayMuonCommand)
		fPrimaryGeneratorAction->SetFlagForShootingCosmicRayMuon(
				fShootCosmicRayMuonCommand->GetNewBoolValue(newValue) );
}

G4String MTCG4PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;

	if (command == fSetIncomingNeutrinoDirectionCommand)
		answer = fPrimaryGeneratorAction->GetIncomingNeutrinoDirection();
	
	else if (command == fSetInverseBetaDecayInteractionVertexCommand)
		answer = fPrimaryGeneratorAction->GetInverseBetaDecayInteractionVertex();
	
	else if (command == fReadInPrimaryParticleDataFileCommand)
		answer = fReadInPrimaryParticleDataFileCommand->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForReadingInPrimaryParticleDataFile());
	
	else if (command == fSetInverseBetaDecayDaughterParticleKineticEnergyCommand)
		answer = fSetInverseBetaDecayDaughterParticleKineticEnergyCommand->
			ConvertToString(fPrimaryGeneratorAction->
					GetInverseBetaDecayDaughterParticleKineticEnergy(), "eV");
	
	else if (command == fShootPrimaryPositronCommand)
		answer = fShootPrimaryPositronCommand->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForShootingPrimaryPositron());

	else if (command == fShootPrimaryNeutronCommand)
		answer = fShootPrimaryNeutronCommand->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForShootingPrimaryNeutron());

	else if (command == fShootParticleGunCommand)
		answer = fShootParticleGunCommand->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingParticleGun() );

	else if (command == fShootGeneralParticleSourceCommand)
		answer = fShootGeneralParticleSourceCommand->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingGeneralParticleSource() );

	else if (command == fShootLaserCommand)
		answer = fShootLaserCommand->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingLaser() );

	else if (command == fShootCosmicRayMuonCommand)
		answer = fShootCosmicRayMuonCommand->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingCosmicRayMuon() );

	return answer;
}
