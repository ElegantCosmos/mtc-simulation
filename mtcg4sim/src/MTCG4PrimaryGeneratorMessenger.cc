//
// MTCG4PrimaryGeneratorMessenger.cc Mich 111222
//

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

	fSetSeedCmd = new G4UIcmdWithAnInteger("/generator/seed", this);
	fSetSeedCmd->SetGuidance("Set the random seed of the generator.");
	fSetSeedCmd->SetParameterName("seed", false);
	fSetSeedCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetInputPathCmd = new
		G4UIcmdWithAString("/mtcg4/primary/setInputPath", this);
	fSetInputPathCmd->
	 	SetGuidance("Set path for where all input files are located.");
 	fSetInputPathCmd->
		SetParameterName("inputPath", false);

	fSetPrimaryDataFileCmd = new
		G4UIcmdWithAString("/mtcg4/primary/setPrimaryParticleDataFile", this);
	fSetPrimaryDataFileCmd->SetGuidance("Specify the file that contains "
			"parimary particle data.");      
	fSetPrimaryDataFileCmd->SetParameterName("filename", false);

	fSetIncomingNeutrinoDirectionCmd = new
		G4UIcmdWithAString("/mtcg4/primary/setIncomingNeutrinoDirection", this);
	fSetIncomingNeutrinoDirectionCmd->SetGuidance("Set incoming neutrino direction.");
	fSetIncomingNeutrinoDirectionCmd->SetGuidance("Options: +z, random.");
	fSetIncomingNeutrinoDirectionCmd->SetCandidates("+z random");
	fSetIncomingNeutrinoDirectionCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fSetInverseBetaDecayInteractionVertexCmd = new
		G4UIcmdWithAString("/mtcg4/primary/setInverseBetaDecayInteractionVertex",
				this);
	fSetInverseBetaDecayInteractionVertexCmd->
		SetGuidance("Set the neutrino inverse-beta decay interaction vertex in x-y-z coordinate space.");
	fSetInverseBetaDecayInteractionVertexCmd->
		SetGuidance("Options: center, random, -z.");
	fSetInverseBetaDecayInteractionVertexCmd->
		SetCandidates("center random -z");
	fSetInverseBetaDecayInteractionVertexCmd->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fReadInPrimaryParticleDataFileCmd = new
		G4UIcmdWithABool("/mtcg4/primary/readInPrimaryParticleDataFile", this);
	fReadInPrimaryParticleDataFileCmd->
  SetGuidance("Flag to read in data to set the initial momentum directions of the primary particles with respect to incoming neutrino direction from file primaryParticleData.dat.");
	fReadInPrimaryParticleDataFileCmd->
		SetGuidance("true: Initial momentum directions of positron and neutron are that of what is read in from file, false: initial momentum directions of positron and neutron are that of incoming neutrino.");
	fReadInPrimaryParticleDataFileCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fSetIBDDaughterParticleEnergyCmd = new
		G4UIcmdWithADoubleAndUnit("/mtcg4/primary/setInverseBetaDecayDaughterParticleKineticEnergy", this);
	fSetIBDDaughterParticleEnergyCmd->
		SetGuidance("Set the initial kinetic energies (same value for both particles) of positron and neutron.");
	fSetIBDDaughterParticleEnergyCmd->
		SetGuidance("This command is overridden by the command readInPrimaryParticleDataFile being set to true.");
	fSetIBDDaughterParticleEnergyCmd->
		SetDefaultUnit("MeV");
	fSetIBDDaughterParticleEnergyCmd->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fShootPrimaryPositronCmd = new
		G4UIcmdWithABool("/mtcg4/primary/shootPrimaryPositron", this);
	fShootPrimaryPositronCmd->
		SetGuidance("Flag to shoot primary positron from IBD.");
	fShootPrimaryPositronCmd->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootPrimaryPositronCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootPrimaryNeutronCmd = new
		G4UIcmdWithABool("/mtcg4/primary/shootPrimaryNeutron", this);
	fShootPrimaryNeutronCmd->
		SetGuidance("Flag to shoot primary neutron from IBD.");
	fShootPrimaryNeutronCmd->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootPrimaryNeutronCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootParticleGunCmd = new
		G4UIcmdWithABool("/gun/shootParticleGun", this);
	fShootParticleGunCmd->
		SetGuidance("Flag to shoot particle produced by G4ParticleGun.");
	fShootParticleGunCmd->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootParticleGunCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootGeneralParticleSourceCmd = new
		G4UIcmdWithABool("/gps/shootGeneralParticleSource", this);
	fShootGeneralParticleSourceCmd->
		SetGuidance("Flag to shoot particle produced by G4GeneralParticleSource.");
	fShootGeneralParticleSourceCmd->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootGeneralParticleSourceCmd->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	fShootLaserCmd = new
		G4UIcmdWithABool("/gun/shootLaser", this);
	fShootLaserCmd->
		SetGuidance("Flag to shoot laser pulse of photons.");
	fShootLaserCmd->
		SetGuidance("True: shoot, false: do not shoot.");
	fShootLaserCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fShootCosmicRayMuonCmd = new
		G4UIcmdWithABool("/gun/shootCosmicRayMuon", this);
	fShootCosmicRayMuonCmd->
		SetGuidance("Flag to shoot cosmic ray muons.");
	fShootCosmicRayMuonCmd->
		SetGuidance("A best guess sea level spectrum and angle distribution "
				"will be used.");
	fShootCosmicRayMuonCmd->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetCosmicRayMuonEnergyCmd = new
		G4UIcmdWithADoubleAndUnit("/gun/setCosmicRayMuonEnergy", this);
	fSetCosmicRayMuonEnergyCmd->
		SetGuidance("Sets kinetic energy of cosmic ray muons.");
	fSetCosmicRayMuonEnergyCmd->
		SetGuidance("Set energy with units, e.g. 1000 MeV.");
	fSetCosmicRayMuonEnergyCmd->
		SetGuidance("Defaut units in MeV.");
	fSetCosmicRayMuonEnergyCmd->
		SetGuidance("Set <= 0 to assign sea level muon spectrum.");
	fSetCosmicRayMuonEnergyCmd->
		SetDefaultUnit("MeV");
	fSetCosmicRayMuonEnergyCmd->
		AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetCosmicRayMuonDirectionCmd = new
		G4UIcmdWithAString("/gun/setCosmicRayMuonDirection", this);
	fSetCosmicRayMuonDirectionCmd->
		SetGuidance("Set direction of cosmic ray muon.");
	fSetCosmicRayMuonDirectionCmd->
		SetGuidance("Options: sea-level, -z.");
	fSetCosmicRayMuonDirectionCmd->
		SetCandidates("sea-level -z");
	fSetCosmicRayMuonDirectionCmd->
		AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4PrimaryGeneratorMessenger::~MTCG4PrimaryGeneratorMessenger()
{
	delete fPrimaryGeneratorDirectory;

	delete fSetSeedCmd;
	delete fSetInputPathCmd;
	delete fSetPrimaryDataFileCmd;
	delete fSetIncomingNeutrinoDirectionCmd;
	delete fSetInverseBetaDecayInteractionVertexCmd;
	delete fReadInPrimaryParticleDataFileCmd;
	delete fSetIBDDaughterParticleEnergyCmd;
	delete fShootPrimaryPositronCmd;
	delete fShootPrimaryNeutronCmd;
	delete fShootParticleGunCmd;
	delete fShootGeneralParticleSourceCmd;
	delete fShootLaserCmd;
	delete fShootCosmicRayMuonCmd;
	delete fSetCosmicRayMuonEnergyCmd;
	delete fSetCosmicRayMuonDirectionCmd;
}

void MTCG4PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue)
{
	if (cmd == fSetSeedCmd) {
		std::istringstream is((const char *)newValue);
		G4int new_seed = 0;
		is >> new_seed;
		CLHEP::HepRandom::setTheSeed(new_seed);
	}
	else if (cmd == fSetInputPathCmd) {
		fPrimaryGeneratorAction->SetInputPath(newValue);
	}
	else if (cmd == fSetPrimaryDataFileCmd) {
		fPrimaryGeneratorAction->SetPrimaryDataFileName(newValue);
	}
	else if (cmd == fSetIncomingNeutrinoDirectionCmd) {
		fPrimaryGeneratorAction->SetIncomingNeutrinoDirection(newValue);
	}
	else if (cmd == fSetInverseBetaDecayInteractionVertexCmd) {
		fPrimaryGeneratorAction->SetInverseBetaDecayInteractionVertex(newValue);
	}
	else if (cmd == fReadInPrimaryParticleDataFileCmd) {
		fPrimaryGeneratorAction->SetFlagForReadingInPrimaryParticleDataFile(
					fReadInPrimaryParticleDataFileCmd->
					GetNewBoolValue(newValue) );
	}
	else if (cmd == fSetIBDDaughterParticleEnergyCmd) {
		fPrimaryGeneratorAction->
			SetInverseBetaDecayDaughterParticleKineticEnergy(
				fSetIBDDaughterParticleEnergyCmd->
				GetNewDoubleValue(newValue) );
	}
	else if (cmd == fShootPrimaryPositronCmd) {
		fPrimaryGeneratorAction->
			SetFlagForShootingPrimaryPositron( fShootPrimaryPositronCmd->
					GetNewBoolValue(newValue) );
	}
	else if (cmd == fShootPrimaryNeutronCmd) {
		fPrimaryGeneratorAction->SetFlagForShootingPrimaryNeutron(
				fShootPrimaryNeutronCmd->GetNewBoolValue(newValue) );
	}
	else if (cmd == fShootParticleGunCmd) {
		fPrimaryGeneratorAction->SetFlagForShootingParticleGun(
				fShootParticleGunCmd->GetNewBoolValue(newValue) );
	}
	else if (cmd == fShootGeneralParticleSourceCmd) {
		fPrimaryGeneratorAction->SetFlagForShootingGeneralParticleSource(
				fShootGeneralParticleSourceCmd->GetNewBoolValue(newValue) );
	}
	else if (cmd == fShootLaserCmd) {
		fPrimaryGeneratorAction->SetFlagForShootingLaser(
				fShootLaserCmd->GetNewBoolValue(newValue) );
	}
	else if (cmd == fShootCosmicRayMuonCmd) {
		fPrimaryGeneratorAction->SetFlagForShootingCosmicRayMuon(
				fShootCosmicRayMuonCmd->GetNewBoolValue(newValue) );
	}
	else if (cmd == fSetCosmicRayMuonEnergyCmd) {
		fPrimaryGeneratorAction->SetCosmicRayMuonEnergy(
				fSetCosmicRayMuonEnergyCmd->GetNewDoubleValue(newValue) );
	}
	else if (cmd == fSetCosmicRayMuonDirectionCmd) {
		fPrimaryGeneratorAction->SetCosmicRayMuonDirection(newValue);
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

G4String MTCG4PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* cmd)
{
	G4String answer;
	if (cmd == fSetIncomingNeutrinoDirectionCmd) {
		answer = fPrimaryGeneratorAction->GetIncomingNeutrinoDirection();
	}
	else if (cmd == fSetInverseBetaDecayInteractionVertexCmd) {
		answer = fPrimaryGeneratorAction->GetInverseBetaDecayInteractionVertex();
	}
	else if (cmd == fReadInPrimaryParticleDataFileCmd) {
		answer = fReadInPrimaryParticleDataFileCmd->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForReadingInPrimaryParticleDataFile());
	}	
	else if (cmd == fSetIBDDaughterParticleEnergyCmd) {
		answer = fSetIBDDaughterParticleEnergyCmd->
			ConvertToString(fPrimaryGeneratorAction->
					GetInverseBetaDecayDaughterParticleKineticEnergy(), "eV");
	}	
	else if (cmd == fShootPrimaryPositronCmd) {
		answer = fShootPrimaryPositronCmd->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForShootingPrimaryPositron());
	}
	else if (cmd == fShootPrimaryNeutronCmd) {
		answer = fShootPrimaryNeutronCmd->
			ConvertToString(fPrimaryGeneratorAction->
					GetFlagForShootingPrimaryNeutron());
	}
	else if (cmd == fShootParticleGunCmd) {
		answer = fShootParticleGunCmd->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingParticleGun() );
	}
	else if (cmd == fShootGeneralParticleSourceCmd) {
		answer = fShootGeneralParticleSourceCmd->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingGeneralParticleSource() );
	}
	else if (cmd == fShootLaserCmd) {
		answer = fShootLaserCmd->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingLaser() );
	}
	else if (cmd == fShootCosmicRayMuonCmd) {
		answer = fShootCosmicRayMuonCmd->
			ConvertToString( fPrimaryGeneratorAction->
					GetFlagForShootingCosmicRayMuon() );
	}
	return answer;
}
