//
// MTCG4PhysicsList.hh by Mich 100223
//
#ifndef MTCG4_PHYSICS_LIST_HH
#define MTCG4_PHYSICS_LIST_HH 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"

class MTCG4PhysicsMessenger;

class MTCG4PhysicsList: public G4VModularPhysicsList
{
public:
	MTCG4PhysicsList();
	virtual ~MTCG4PhysicsList();

public:
	virtual void ConstructParticle();
	virtual void ConstructProcess();
	virtual void SetCuts();

	void AddParameterisation();

private:
	void ConstructHadronPhysics(G4bool flag = false);
	void ConstructDecayPhysics();
	void ConstructEMPhysics();
	void ConstructOpticalPhysics();
	void ConstructMuonNuclearProcess();

private:
	std::vector<G4VPhysicsConstructor*> fEMPhysics;
	std::vector<G4VPhysicsConstructor*> fHadronPhysics;
	std::vector<G4VPhysicsConstructor*> fDecayPhysics;
	std::vector<G4VPhysicsConstructor*> fOpticalPhysics;

	MTCG4PhysicsMessenger *fMessenger;   

private:
	G4int fVerbose;
	G4double fCutForGamma;
	G4double fCutForElectron;
	G4double fCutForPositron;
	G4double fCutForProton;
};

#endif
