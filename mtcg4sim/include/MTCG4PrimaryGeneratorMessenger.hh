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
	G4UIcmdWithAnInteger	*fSetSeedCmd;
	G4UIcmdWithAString		*fSetInputPathCmd;
	G4UIcmdWithAString		*fSetPrimaryDataFileCmd;
	G4UIcmdWithAString		*fSetIncomingNeutrinoDirectionCmd;
	G4UIcmdWithAString		*fSetInverseBetaDecayInteractionVertexCmd;
	G4UIcmdWithABool		*fReadInPrimaryParticleDataFileCmd;
	G4UIcmdWithADoubleAndUnit *fSetIBDDaughterParticleEnergyCmd;
	G4UIcmdWithABool		*fShootPrimaryPositronCmd;
	G4UIcmdWithABool		*fShootPrimaryNeutronCmd;
	G4UIcmdWithABool		*fShootParticleGunCmd;
	G4UIcmdWithABool		*fShootGeneralParticleSourceCmd;
	G4UIcmdWithABool		*fShootLaserCmd;
	G4UIcmdWithABool		*fShootCosmicRayMuonCmd;
	G4UIcmdWithADoubleAndUnit *fSetCosmicRayMuonEnergyCmd;
	G4UIcmdWithAString		*fSetCosmicRayMuonDirectionCmd;
};

#endif
