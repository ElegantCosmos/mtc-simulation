//
// MTCG4PrimaryGeneratorAction.hh
//

#ifndef MTCG4_PRIMARY_GENERATOR_ACTION_HH
#define MTCG4_PRIMARY_GENERATOR_ACTION_HH 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class MTCG4PrimaryGeneratorMessenger;
class G4ParticleGun;
class G4Event;

class MTCG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	MTCG4PrimaryGeneratorAction();
	~MTCG4PrimaryGeneratorAction();

public:
	void SetIncomingNeutrinoDirection(G4String neutrinoDirection)
	{ incomingNeutrinoDirection = neutrinoDirection; }
	inline G4String GetIncomingNeutrinoDirection()
	{ return incomingNeutrinoDirection; }
	void SetInverseBetaDecayInteractionVertex(G4String vertex)
	{ inverseBetaDecayInteractionVertex = vertex; }
	inline G4String GetInverseBetaDecayInteractionVertex()
	{ return inverseBetaDecayInteractionVertex; }
	void SetFlagForReadingInPrimaryParticleDataFile(G4bool flag)
	{ flagForReadingInPrimaryParticleDataFile = flag; }
	inline G4bool GetFlagForReadingInPrimaryParticleDataFile()
	{ return flagForReadingInPrimaryParticleDataFile; }
	void SetInverseBetaDecayDaughterParticleKineticEnergy(G4double energy)
	{ inverseBetaDecayDaughterParticleKineticEnergy = energy; }
	inline G4double GetInverseBetaDecayDaughterParticleKineticEnergy()
	{ return inverseBetaDecayDaughterParticleKineticEnergy; }
	void SetFlagForShootingPrimaryPositron(G4bool flag)
	{ primaryPositronIsShot = flag; }
	inline G4bool GetFlagForShootingPrimaryPositron()
	{ return primaryPositronIsShot; }
	void SetFlagForShootingPrimaryNeutron(G4bool flag)
	{ primaryNeutronIsShot = flag; }
	inline G4bool GetFlagForShootingPrimaryNeutron()
	{ return primaryNeutronIsShot; }

	void GeneratePrimaries(G4Event*);
	void GenerateFilesAndHeadersForRun();
	void GenerateFilesAndHeadersForEvent(G4Event*);
	std::ifstream* primaryParticleDataInput;

private:
	//
	// Parameters.
	//
	G4String incomingNeutrinoDirection;
	G4String inverseBetaDecayInteractionVertex;
	G4bool flagForReadingInPrimaryParticleDataFile;
	G4double inverseBetaDecayDaughterParticleKineticEnergy;
	G4bool primaryPositronIsShot;
	G4bool primaryNeutronIsShot;
	//G4bool primaryElectronAntiNeutrinoIsShot;

private:
	MTCG4PrimaryGeneratorMessenger* primaryGeneratorMessenger;

	G4ParticleGun* primaryNeutronGun;
	G4ParticleGun* primaryPositronGun;
	G4ParticleGun* primaryElectronAntiNeutrinoGun;
};

#endif
