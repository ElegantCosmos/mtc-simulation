//
// MTCG4PhysicsList.cc version to use QGSP_BIC_HP physics list. Mich 20130715.
//
// This class is based on examples/extended/hadronic/Hadr01/src/PhysicsList.cc.
// Although it seems a bit complicated, this method seems to give me the most
// control over what kind of physics processes can be included. eg. A predefined
// Geant4 physics list such as HadronPhysicsQGSP_BERT_HP can be used along with
// any other physics constructor or processes as wished.

#include "MTCG4PhysicsList.hh"
//#include "MTCG4PhysicsMessenger.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysicsLHEP.hh"
#include "G4HadronQElasticPhysics.hh"
#include "G4ChargeExchangePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4NeutronCrossSectionXS.hh"
#include "G4QStoppingPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4LHEPStoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonLHEPPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "HadronPhysicsFTFP_BERT.hh"
#include "HadronPhysicsFTF_BIC.hh"
#include "HadronPhysicsLHEP.hh"
#include "HadronPhysicsLHEP_EMV.hh"
#include "G4HadronInelasticQBBC.hh"
#include "HadronPhysicsQGSC_BERT.hh"
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#include "HadronPhysicsQGSP_BIC.hh"
#include "HadronPhysicsQGSP_BIC_HP.hh"
#include "HadronPhysicsQGSP_FTFP_BERT.hh"
#include "HadronPhysicsQGS_BIC.hh"

#include "G4LossTableManager.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"

#include "G4SystemOfUnits.hh"

MTCG4PhysicsList::MTCG4PhysicsList() : G4VModularPhysicsList(),
	fMessenger(NULL)
{
	fVerbose = 0;
	//defaultCutValue = 1.*mm; // Taken from original KLG4sim code
	defaultCutValue = 0.001*mm; // Taken from KLG4sim modifications by D. Motoki
	fCutForGamma     = defaultCutValue;
	fCutForElectron  = defaultCutValue;
	fCutForPositron  = defaultCutValue;
	fCutForProton    = defaultCutValue;

	//fMessenger = new MTCG4PhysicsMessenger(this);

	// Particles fPar
}

MTCG4PhysicsList::~MTCG4PhysicsList()
{
	for (size_t i = 0; i < fHadronPhysics.size(); ++i) {
		delete fHadronPhysics[i];
	}
	for (size_t i = 0; i < fEMPhysics.size(); ++i) {
		delete fEMPhysics[i];
	}
	for (size_t i = 0; i < fOpticalPhysics.size(); ++i) {
		delete fOpticalPhysics[i];
	}
	for (size_t i = 0; i < fDecayPhysics.size(); ++i) {
		delete fDecayPhysics[i];
	}
	//delete fMessenger;
}

#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void MTCG4PhysicsList::ConstructParticle()
{
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

void MTCG4PhysicsList::ConstructProcess()
{
	// Decay physics.
	ConstructDecayPhysics();

	// EM physics.
	ConstructEMPhysics();

	// Hadronic physics.
	G4bool useHighPrecision;
	ConstructHadronPhysics(useHighPrecision = true);

	// Optical physics.
	ConstructOpticalPhysics();

	// Parameterisation.
	AddParameterisation();

	// Particle transportation.
	AddTransportation();

	// Construct individual physics processes.
	ConstructMuonNuclearProcess();
}

void MTCG4PhysicsList::ConstructEMPhysics()
{
	fEMPhysics.push_back(new G4EmStandardPhysics(fVerbose));
	fEMPhysics.push_back(new G4EmExtraPhysics(fVerbose));
	for (size_t i = 0; i < fEMPhysics.size(); ++i) {
		fEMPhysics[i]->ConstructProcess();
	}
}

void MTCG4PhysicsList::ConstructHadronPhysics(G4bool flagHP)
{
	//fHadronPhysics.push_back(new HadronPhysicsQGSP_BIC_HP());
	fHadronPhysics.push_back(new HadronPhysicsQGSP_BERT_HP(fVerbose));
	if (flagHP) {
		fHadronPhysics.push_back( new G4HadronElasticPhysicsHP(fVerbose) );
	} else {
		fHadronPhysics.push_back( new G4HadronElasticPhysics(fVerbose) );
	}
	fHadronPhysics.push_back( new G4StoppingPhysics(fVerbose));
	fHadronPhysics.push_back( new G4IonBinaryCascadePhysics(fVerbose));
	fHadronPhysics.push_back( new G4IonLHEPPhysics(fVerbose));
	fHadronPhysics.push_back( new G4NeutronTrackingCut(fVerbose));
	for (size_t i = 0; i < fHadronPhysics.size(); ++i) {
		fHadronPhysics[i]->ConstructProcess();
	}
}

void MTCG4PhysicsList::ConstructOpticalPhysics()
{
	G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics(fVerbose);
	opticalPhysics->SetMaxNumPhotonsPerStep(100); // #of photons per step.
	//opticalPhysics->SetMaxBetaChangePerStep(0.10); //%change in beta per step.
	opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
	opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);

	fOpticalPhysics.push_back(opticalPhysics);
	for (size_t i = 0; i < fOpticalPhysics.size(); ++i) {
		fOpticalPhysics[i]->ConstructProcess();
	}
}

void MTCG4PhysicsList::ConstructDecayPhysics()
{
	fDecayPhysics.push_back(new G4DecayPhysics(fVerbose));
	fDecayPhysics.push_back(new G4RadioactiveDecayPhysics(fVerbose));
	for (size_t i = 0; i < fDecayPhysics.size(); ++i) {
		fDecayPhysics[i]->ConstructProcess();
	}
}

void MTCG4PhysicsList::ConstructMuonNuclearProcess()
{
	G4MuonNuclearProcess *muNucProcess = new G4MuonNuclearProcess();
	muNucProcess->RegisterMe(new G4MuonVDNuclearModel());
	theParticleIterator->reset();
	while ((*theParticleIterator)()) {
		G4ParticleDefinition *particle = theParticleIterator->value();
		G4ProcessManager *pManager = particle->GetProcessManager();
		if (muNucProcess->IsApplicable(*particle)) {
			pManager->AddProcess(muNucProcess);
			pManager->SetProcessOrdering(muNucProcess, idxPostStep);
			//pManager->SetProcessOrdering(muNucProcess, idxAtRest);
		}
	}
}

#include "G4FastSimulationManagerProcess.hh"

void MTCG4PhysicsList::AddParameterisation()
{
	G4FastSimulationManagerProcess*
		theFastSimulationManagerProcess =
		new G4FastSimulationManagerProcess();
	theParticleIterator->reset();
	while ( (*theParticleIterator)() ) {
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

void MTCG4PhysicsList::SetCuts()
{
	//  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
	//   the default cut value for all particle types
	// 
	SetCutsWithDefault();
	if (fVerbose > 0) DumpCutValuesTable();   
}
