//
// MTCG4PhysicsList.cc by Mich 110502
//

#include "globals.hh"
#include "MTCG4PhysicsMessenger.hh"
#include "MTCG4PhysicsList.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4ProcessManager.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhysics.hh"

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

// Added extra headers from KLPhysicsList.cc
#include "G4ParticleWithCuts.hh"
#include "G4ProcessVector.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include "G4IonConstructor.hh"
#include "G4IonTable.hh"
#include "G4Ions.hh"
#include "G4RadioactiveDecay.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4PhysicsList::MTCG4PhysicsList() :  G4VUserPhysicsList()
{
	cerenkovIsTurnedOn = true;
	scintillationIsTurnedOn = true;
	neutronHPThermalScatteringIsTurnedOn = true;
	theCerenkovProcess           = NULL;
	theScintillationProcess      = NULL;
	theAbsorptionProcess         = NULL;
	theRayleighScatteringProcess = NULL;
	theMieHGScatteringProcess    = NULL;
	theBoundaryProcess           = NULL;

	defaultCutValue = 1.*mm;

	physicsMessenger = new MTCG4PhysicsMessenger(this);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4PhysicsList::~MTCG4PhysicsList()
{
	delete physicsMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4LeptonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"

void MTCG4PhysicsList::ConstructParticle()
{
	//
	// In this method, static member functions should be called
	// for all particles which you want to use.
	// This ensures that objects of these particle types will be
	// created in the program.
	//

	G4BosonConstructor* bosonConstructor;
	bosonConstructor->ConstructParticle();
	G4LeptonConstructor* leptonConstructor;
	leptonConstructor->ConstructParticle();
	G4MesonConstructor* mesonConstructor;
	mesonConstructor->ConstructParticle();
	G4BaryonConstructor* baryonConstructor;
	baryonConstructor->ConstructParticle();
	G4IonConstructor* ionConstructor;
	ionConstructor->ConstructParticle();
}

void MTCG4PhysicsList::ConstructProcess()
{
	AddTransportation();
	ConstructGeneral();
	ConstructEM();
	ConstructOp();
	// Extra stuff added from KLPhysicsList.cc.
	AddParameterisation();
	ConstructHadronic();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Decay.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::ConstructGeneral()
{
	// Add Decay Processes.
	G4Decay* theDecayProcess = new G4Decay();
	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		if (theDecayProcess->IsApplicable(*particle)) {
			pmanager ->AddProcess(theDecayProcess);
			// Set ordering for PostStepDoIt and AtRestDoIt.
			pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
			pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//****************************** Added from KLPhysicsList

#include "G4FastSimulationManagerProcess.hh"

void MTCG4PhysicsList::AddParameterisation()
{}


#include "G4AntiNeutronAnnihilationAtRest.hh"
#include "G4AntiProtonAnnihilationAtRest.hh"
#include "G4KaonMinusAbsorptionAtRest.hh"
#include "G4PionMinusAbsorptionAtRest.hh"
#include "G4MuonMinusCaptureAtRest.hh"
// #include "G4NeutronCaptureAtRest.hh" // Wrong physics, don't use! [GAHS]

#include "G4HadronElasticProcess.hh"   
#include "G4HadronInelasticProcess.hh" 
#include "G4HadronFissionProcess.hh"   
#include "G4HadronCaptureProcess.hh"         

#include "G4NeutronInelasticProcess.hh" 

#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPFission.hh"
#include "G4NeutronHPCapture.hh"

// high precision < 20 MeV
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPFissionData.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPInelasticData.hh"

// generic models for 0 to infinite energy (used for E > 20 MeV)
// (these may actually be "translations" of the GEANT3.21/GHEISHA models....)
#include "G4LElastic.hh"
#include "G4LFission.hh"
#include "G4LCapture.hh"

// Low energy (used for 20 MeV < E < 50 GeV)
#include "G4LENeutronInelastic.hh"  // 0 to 55 GeV

// High energy (used for > 50 GeV)
#include "G4HENeutronInelastic.hh"  // 45 GeV to 10 TeV

// Ionisation
#include "G4hIonisation.hh"


#define G4std  std
inline void AddDataSet(class G4HadronicProcess*p, class G4VCrossSectionDataSet*d) { p->AddDataSet(d); }

#include "G4HadronicInteraction.hh"
#include "G4NeutronHPThermalScattering.hh"
#include "G4NeutronHPThermalScatteringData.hh"
#include "G4ProcessManager.hh"

void MTCG4PhysicsList::ConstructHadronic()
{
	G4ProcessManager * pmanager = 0;  

	// Kaon Minus.
	pmanager = G4KaonMinus::KaonMinus()->GetProcessManager();
	pmanager->AddProcess(new G4hIonisation(),-1,2,2);
	pmanager->AddRestProcess(new G4KaonMinusAbsorptionAtRest());

	// Pion Minus.
	pmanager = G4PionMinus::PionMinus()->GetProcessManager();
	pmanager->AddProcess(new G4hIonisation(),-1,2,2);
	pmanager->AddRestProcess(new G4PionMinusAbsorptionAtRest());

	// Muon.
	//if ( particle == G4MuonMinus::MuonMinus() ) {
	//  pmanager->AddRestProcess(new G4MuonMinusCaptureAtRest());
	//}

	// Proton.
	pmanager = G4Proton::Proton()->GetProcessManager();
	pmanager->AddProcess(new G4hIonisation(),-1,2,2);


	// Neutron.
	pmanager = G4Neutron::Neutron()->GetProcessManager();

	G4HadronElasticProcess*   theHadronElasticProcess
		= new   G4HadronElasticProcess();
	G4LElastic* theNeutronLElastic
		= new   G4LElastic();
	G4NeutronHPElastic* theNeutronHPElastic
		= new   G4NeutronHPElastic();
	if(neutronHPThermalScatteringIsTurnedOn == true){
		G4NeutronHPThermalScattering* theNeutronThermalElasticModel = new G4NeutronHPThermalScattering();  
		theNeutronThermalElasticModel->SetMaxEnergy( 4.*eV );
		theNeutronHPElastic->SetMinEnergy( 4.*eV );
		AddDataSet(theHadronElasticProcess, new G4NeutronHPThermalScatteringData());
		theHadronElasticProcess->RegisterMe( theNeutronThermalElasticModel );
	}
	theNeutronLElastic->SetMinEnergy( 20.*MeV );
	theNeutronHPElastic->SetMaxEnergy( 20.*MeV );
	AddDataSet(theHadronElasticProcess, new G4NeutronHPElasticData() );
	theHadronElasticProcess->RegisterMe( theNeutronHPElastic );
	theHadronElasticProcess->RegisterMe( theNeutronLElastic );
	pmanager->AddDiscreteProcess( theHadronElasticProcess );

	G4NeutronInelasticProcess*   theNeutronInelasticProcess
		= new   G4NeutronInelasticProcess();
	G4LENeutronInelastic* theNeutronLENeutronInelastic
		= new   G4LENeutronInelastic();
	G4HENeutronInelastic* theNeutronHENeutronInelastic
		= new   G4HENeutronInelastic();
	G4NeutronHPInelastic* theNeutronHPInelastic
		= new   G4NeutronHPInelastic();
	theNeutronHPInelastic->SetMaxEnergy( 20.*MeV );
	theNeutronLENeutronInelastic->SetMinEnergy( 20.*MeV );
	theNeutronInelasticProcess->RegisterMe( theNeutronHPInelastic );
	theNeutronInelasticProcess->RegisterMe( theNeutronLENeutronInelastic );
	theNeutronInelasticProcess->RegisterMe( theNeutronHENeutronInelastic );
	AddDataSet(theNeutronInelasticProcess, new G4NeutronHPInelasticData() );
	pmanager->AddDiscreteProcess( theNeutronInelasticProcess );

	G4HadronFissionProcess*   theHadronFissionProcess
		= new   G4HadronFissionProcess();
	G4LFission* theNeutronLFission
		= new   G4LFission();
	G4NeutronHPFission* theNeutronHPFission
		= new   G4NeutronHPFission();
	theNeutronHPFission->SetMaxEnergy( 20.*MeV );
	theNeutronLFission->SetMinEnergy( 20.*MeV );
	theHadronFissionProcess->RegisterMe( theNeutronHPFission );
	theHadronFissionProcess->RegisterMe( theNeutronLFission );
	AddDataSet(theHadronFissionProcess, new G4NeutronHPFissionData() );
	pmanager->AddDiscreteProcess( theHadronFissionProcess );

	G4HadronCaptureProcess*   theCaptureProcess
		= new   G4HadronCaptureProcess();
	G4LCapture* theNeutronLCapture
		= new   G4LCapture();
	G4NeutronHPCapture* theNeutronHPCapture
		= new   G4NeutronHPCapture();
	theNeutronHPCapture->SetMaxEnergy( 20.*MeV );
	theNeutronLCapture->SetMinEnergy( 20.*MeV );
	theCaptureProcess->RegisterMe( theNeutronHPCapture );
	theCaptureProcess->RegisterMe( theNeutronLCapture );
	AddDataSet(theCaptureProcess, new G4NeutronHPCaptureData() );
	pmanager->AddDiscreteProcess( theCaptureProcess );
	theCaptureProcess->DumpPhysicsTable(*(G4Neutron::NeutronDefinition()));
	// ( end neutron )
}
// ***************************End of Added from KLPhysicsList.


#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::ConstructEM()
{
	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		if (particle == G4Gamma::GammaDefinition()) {
			// Gamma.
			// Construct processes for gamma.
			pmanager->AddDiscreteProcess(new G4GammaConversion());
			pmanager->AddDiscreteProcess(new G4ComptonScattering());
			pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());

		} else if (particle == G4Electron::ElectronDefinition()) {
			// Electron.
			// Construct processes for electron.
			pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
			pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);

		} else if (particle == G4Positron::PositronDefinition()) {
			// Positron.
			// Construct processes for positron.
			pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
			pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);
			pmanager->AddProcess(new G4eplusAnnihilation(), 0, -1, 4);

		} else if(particle == G4MuonPlus::MuonPlusDefinition() ||
				particle == G4MuonMinus::MuonMinusDefinition()) {
			// Muon.
			// Construct processes for muon.
			pmanager->AddProcess(new G4MuMultipleScattering(),-1, 1, 1);
			pmanager->AddProcess(new G4MuIonisation(),      -1, 2, 2);
			pmanager->AddProcess(new G4MuBremsstrahlung(),  -1, 3, 3);
			pmanager->AddProcess(new G4MuPairProduction(),  -1, 4, 4);

		} else {
			if ((particle->GetPDGCharge() != 0.) &&
					(particle != G4ChargedGeantino::ChargedGeantinoDefinition())) {
				// All other charged particles except geantino.
				pmanager->AddProcess(new G4hMultipleScattering(),-1,1,1);
				pmanager->AddProcess(new G4hIonisation(),       -1,2,2);
			}
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::ConstructOp()
{
	theCerenkovProcess           = new G4Cerenkov("Cerenkov");
	theScintillationProcess      = new G4Scintillation("Scintillation");
	theAbsorptionProcess         = new G4OpAbsorption();
	theRayleighScatteringProcess = new G4OpRayleigh();
	theMieHGScatteringProcess    = new G4OpMieHG();
	theBoundaryProcess           = new G4OpBoundaryProcess();
	//theCerenkovProcess->DumpPhysicsTable();
	//theScintillationProcess->DumpPhysicsTable();
	//theRayleighScatteringProcess->DumpPhysicsTable();

	//SetVerbose(1);

	theCerenkovProcess->SetTrackSecondariesFirst(true);
	theCerenkovProcess->SetMaxNumPhotonsPerStep(300);
	theCerenkovProcess->SetMaxBetaChangePerStep(10.0);

	theScintillationProcess->SetTrackSecondariesFirst(true);
	theScintillationProcess->SetScintillationYieldFactor(1.);

	// Use Birks Correction in the Scintillation process.

	G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
	theScintillationProcess->AddSaturation(emSaturation);

	G4OpticalSurfaceModel themodel = unified;
	theBoundaryProcess->SetModel(themodel);

	theParticleIterator->reset();
	while( (*theParticleIterator)() ){
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		if(cerenkovIsTurnedOn){
			if (theCerenkovProcess->IsApplicable(*particle)) {
				pmanager->AddProcess(theCerenkovProcess);
				pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
			}
		}
		if(scintillationIsTurnedOn){ 
			if (theScintillationProcess->IsApplicable(*particle)) {
				pmanager->AddProcess(theScintillationProcess);
				pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
				pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
			}
		}
		if (particle == G4OpticalPhoton::OpticalPhotonDefinition() || particle == G4Gamma::GammaDefinition()) {
			pmanager->AddDiscreteProcess(theAbsorptionProcess);
			pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
			pmanager->AddDiscreteProcess(theMieHGScatteringProcess);
			pmanager->AddDiscreteProcess(theBoundaryProcess);
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::SetVerbose(G4int verbose)
{
	theCerenkovProcess->SetVerboseLevel(verbose);
	theScintillationProcess->SetVerboseLevel(verbose);
	theAbsorptionProcess->SetVerboseLevel(verbose);
	theRayleighScatteringProcess->SetVerboseLevel(verbose);
	theMieHGScatteringProcess->SetVerboseLevel(verbose);
	theBoundaryProcess->SetVerboseLevel(verbose);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::SetNbOfPhotonsCerenkov(G4int MaxNumber)
{  
	theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumber);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTCG4PhysicsList::SetCuts()
{
	//  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
	//   the default cut value for all particle types
	// 
	SetCutsWithDefault();

	if (verboseLevel>0) DumpCutValuesTable();   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


































































