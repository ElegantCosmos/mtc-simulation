//
// MTCG4PhysicsList.cc version to use QGSP_BIC_HP physics list. Mich 20130715.
//

#include "MTCG4PhysicsList.hh"
//#include "MTCG4PhysicsMessenger.hh"

#include "HadronPhysicsQGSP_BIC_HP.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonLHEPPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4RadioactiveDecayPhysics.hh"

MTCG4PhysicsList::MTCG4PhysicsList() : G4VModularPhysicsList()
{
	verboseLevel = 0;
	defaultCutValue = 1.*mm; // Taken from original KLG4sim code
	//defaultCutValue = 0.001*mm; // Taken from KLG4sim modifications by D. Motoki

	// Particles
	fParticleList = new G4DecayPhysics("decays");

	// EM physics
	fEmPhysicsList = new G4EmStandardPhysics();
	//fHadronPhysics.push_back(new HadronPhysicsQGSP_BIC_HP());
	fHadronPhysics.push_back(new HadronPhysicsQGSP_BERT_HP());
	SetBuilderList1(true);

	//physicsMessenger = new MTCG4PhysicsMessenger(this);
}

MTCG4PhysicsList::~MTCG4PhysicsList()
{
	delete fParticleList;
	delete fEmPhysicsList;
	for(size_t i = 0; i < fHadronPhysics.size(); ++i)
		delete fHadronPhysics[i];
	//delete physicsMessenger;
}

#include "G4LeptonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void MTCG4PhysicsList::ConstructParticle()
{
	//fParticleList->ConstructParticle();

	G4BosonConstructor *bosonConstructor;
	bosonConstructor->ConstructParticle();

	G4LeptonConstructor *leptonConstructor;
	leptonConstructor->ConstructParticle();

	G4MesonConstructor *mesonConstructor;
	mesonConstructor->ConstructParticle();

	G4BaryonConstructor *baryonConstructor;
	baryonConstructor->ConstructParticle();

	G4IonConstructor *ionConstructor;
	ionConstructor->ConstructParticle();

	G4ShortLivedConstructor *shortLivedConstructor;
	shortLivedConstructor->ConstructParticle();

}

#include "G4ProcessManager.hh"
#include "G4MuonMinusCapture.hh"
#include "G4OpticalPhysics.hh"

void MTCG4PhysicsList::ConstructProcess()
{
	AddTransportation();
	// Muon.
	G4ProcessManager *pmanager = G4MuonMinus::MuonMinus()->GetProcessManager();
	//pmanager->AddRestProcess(new G4MuonMinusCapture());
	G4MuonNuclearProcess *muNucProcess = new G4MuonNuclearProcess();
	muNucProcess->RegisterMe(new G4MuonVDNuclearModel());
	pmanager->AddProcess(muNucProcess, -1,-1,2);
	fEmPhysicsList->ConstructProcess();
	fParticleList->ConstructProcess();
	for(size_t i = 0; i < fHadronPhysics.size(); ++i)
		fHadronPhysics[i]->ConstructProcess();

	// Parameterisation.
	AddParameterisation();
}

#include "G4FastSimulationManagerProcess.hh"

void MTCG4PhysicsList::AddParameterisation()
{
	G4FastSimulationManagerProcess*
    theFastSimulationManagerProcess =
      new G4FastSimulationManagerProcess();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    // both postStep and alongStep action are required if the detector
    // makes use of ghost volumes. If no ghost, the postStep
    // is sufficient (and faster?).
#define MTCG4_USES_GHOST_VOLUMES 0
#if MTCG4_USES_GHOST_VOLUMES
    pmanager->AddProcess(theFastSimulationManagerProcess, -1, 1, 1);
#else
    pmanager->AddProcess(theFastSimulationManagerProcess, -1, -1, 1);
#endif
  }
}

void MTCG4PhysicsList::SetBuilderList1(G4bool flagHP)
{
  fHadronPhysics.push_back( new G4EmExtraPhysics(verboseLevel));
  if(flagHP) {
    fHadronPhysics.push_back( new G4HadronElasticPhysicsHP(verboseLevel) );
  } else {
    fHadronPhysics.push_back( new G4HadronElasticPhysics(verboseLevel) );
  }
  fHadronPhysics.push_back( new G4StoppingPhysics(verboseLevel));
  fHadronPhysics.push_back( new G4IonBinaryCascadePhysics(verboseLevel));
  fHadronPhysics.push_back( new G4IonLHEPPhysics(verboseLevel));
  fHadronPhysics.push_back( new G4NeutronTrackingCut(verboseLevel));
  fHadronPhysics.push_back( new G4OpticalPhysics(verboseLevel));
  fHadronPhysics.push_back( new G4DecayPhysics(verboseLevel));
  fHadronPhysics.push_back( new G4RadioactiveDecayPhysics(verboseLevel));
}

void MTCG4PhysicsList::SetCuts()
{
	//  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
	//   the default cut value for all particle types
	// 
	SetCutsWithDefault();
	if (verboseLevel>0) DumpCutValuesTable();   
}
