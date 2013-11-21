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
	void SetBuilderList1(G4bool flag = false);

private:
	G4VPhysicsConstructor *fEmPhysicsList;
	G4VPhysicsConstructor *fParticleList;
	std::vector<G4VPhysicsConstructor*> fHadronPhysics;

	MTCG4PhysicsMessenger *physicsMessenger;   
};

#endif
