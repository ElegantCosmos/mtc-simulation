//
// MTCG4PrimaryGeneratorMessenger.hh by Mich 111222
//

#ifndef MTCG4PrimaryGeneratorMessenger_h
#define MTCG4PrimaryGeneratorMessenger_h 1

#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UImessenger.hh"

class MTCG4PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class MTCG4PrimaryGeneratorMessenger: public G4UImessenger
{
public:
	MTCG4PrimaryGeneratorMessenger(MTCG4PrimaryGeneratorAction*);
	~MTCG4PrimaryGeneratorMessenger();

public:
	void SetNewValue(G4UIcommand*, G4String);
	G4String GetCurrentValue(G4UIcommand*);

private:
	MTCG4PrimaryGeneratorAction* fPrimaryGeneratorAction;

	G4UIdirectory			*fPrimaryGeneratorDirectory;
	G4UIcmdWithAnInteger	*fSetSeedCommand;
	G4UIcmdWithAString		*fSetInputPathCommand;
	G4UIcmdWithAString		*fSetPrimaryDataFileCommand;
	G4UIcmdWithAString		*fSetIncomingNeutrinoDirectionCommand;
	G4UIcmdWithAString		*fSetInverseBetaDecayInteractionVertexCommand;
	G4UIcmdWithABool		*fReadInPrimaryParticleDataFileCommand;
	G4UIcmdWithADoubleAndUnit
		*fSetInverseBetaDecayDaughterParticleKineticEnergyCommand;
	G4UIcmdWithABool		*fShootPrimaryPositronCommand;
	G4UIcmdWithABool		*fShootPrimaryNeutronCommand;
	G4UIcmdWithABool		*fShootParticleGunCommand;
	G4UIcmdWithABool		*fShootGeneralParticleSourceCommand;
	G4UIcmdWithABool		*fShootLaserCommand;
	G4UIcmdWithABool		*fShootCosmicRayMuonCommand;
};

#endif
