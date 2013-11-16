//
// MTCG4PrimaryGeneratorAction.cc by Mich 110502
//

#include "MTCG4PrimaryGeneratorAction.hh"
#include "MTCG4PrimaryGeneratorMessenger.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "globals.hh"
#include "Randomize.hh"

MTCG4PrimaryGeneratorAction::MTCG4PrimaryGeneratorAction()
{
	//
	// Parameter default values.
	//
	incomingNeutrinoDirection = "random";
	inverseBetaDecayInteractionVertex = "center";
	flagForReadingInPrimaryParticleDataFile = true;
	inverseBetaDecayDaughterParticleKineticEnergy = 1000.*eV;
	primaryPositronIsShot = true;
	primaryNeutronIsShot = true;

	primaryGeneratorMessenger = new MTCG4PrimaryGeneratorMessenger(this);

	GenerateFilesAndHeadersForRun();
	G4int numParticle = 1;
	primaryNeutronGun = new G4ParticleGun(numParticle);
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* primaryNeutron = particleTable->FindParticle(G4Neutron::NeutronDefinition());
	primaryNeutronGun -> SetParticleDefinition(primaryNeutron);
	primaryPositronGun = new G4ParticleGun(numParticle);
	G4ParticleDefinition* primaryPositron = particleTable->FindParticle(G4Positron::PositronDefinition());
	primaryPositronGun->SetParticleDefinition(primaryPositron);

	//
	// Set primary particle to be a neutrino.
	// (This feature in Geant4 is not yet mature.)
	//
	//primaryElectronAntiNeutrinoGun = new G4ParticleGun(numParticle);
	//G4ParticleDefinition* primaryElectronAntiNeutrino = particleTable->FindParticle(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
	//primaryElectronAntiNeutrinoGun->SetParticleDefinition(primaryElectronAntiNeutrino);

	//
	// Settings for primary particles are read in from file.
	//
	if(flagForReadingInPrimaryParticleDataFile){
		primaryParticleDataInput = new std::ifstream("/mnt/datadisk/school/research/MTC/MTCG4Sim/trunk/primaryParticleData.dat", std::ifstream::in);
		if(!(primaryParticleDataInput->good()) || primaryParticleDataInput->bad()){
			G4cout << "primaryParticleData.dat was not successfully opened!\n";
			return;
		}
	}
}

MTCG4PrimaryGeneratorAction::~MTCG4PrimaryGeneratorAction()
{
	delete primaryNeutronGun;
	delete primaryPositronGun;
	if(flagForReadingInPrimaryParticleDataFile){
		primaryParticleDataInput->close();
	}
	delete primaryGeneratorMessenger;
}

void MTCG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* theEvent)
{
	G4double phi_neutrino, theta_neutrino;
	G4ThreeVector primaryVertexVector;
	G4ThreeVector primaryNeutronMomentumDirectionVector;
	G4ThreeVector primaryPositronMomentumDirectionVector;
	//G4ThreeVector primaryElectronAntiNeutrinoMomentumDirectionVector;
	//G4double primaryElectronAntiNeutrinoKineticEnergy;
	G4double primaryNeutronKineticEnergy;
	G4double primaryPositronKineticEnergy;
	if(flagForReadingInPrimaryParticleDataFile == true){
		G4double neutrinoKineticEnergy;
		G4int neutrinoEventNumber;
		G4double theta_neutron;
		G4double theta_positron;
		G4double cos_theta_neutron_wrt_neutrino_momentum_direction;
		G4double cos_theta_positron_wrt_neutrino_momentum_direction;
		G4double phi_neutron;
		G4double phi_positron;
		phi_neutron = 2.*pi*G4UniformRand();
		phi_positron = phi_neutron + pi;
		char bufferChar[256];
		G4String bufferString;
		while(primaryParticleDataInput->getline(bufferChar, 256)){
			bufferString.assign(bufferChar);
			// Ignore all comment lines of which the first non-space or
			// non-tab character of the line is not a digit.
			if(isdigit(bufferString[bufferString.find_first_not_of("		")])) break;
		}
		std::istringstream inputLine(bufferString.data());
		if(!(inputLine >> neutrinoEventNumber >> neutrinoKineticEnergy >> cos_theta_positron_wrt_neutrino_momentum_direction >> primaryPositronKineticEnergy >> cos_theta_neutron_wrt_neutrino_momentum_direction >> primaryNeutronKineticEnergy)){
			G4cout << bufferString << "\ncannot be read.\nINVALID FILE FORMAT FOR NEUTRON AND POSITRON EVENT.\nNO DATA WILL BE USED!\nSEE MTCG4PrimaryGeneratorAction.cc!\n";
			return;
		}
		else{
			// Multiply correct units before using these parameters.
			neutrinoKineticEnergy *= MeV;
			primaryPositronKineticEnergy *= MeV;
			primaryNeutronKineticEnergy *= MeV;

			//G4cout << "primaryNeutronKineticEnergy = " << primaryNeutronKineticEnergy/MeV << " MeV." << G4endl; // Test output to see if parameters were read correctly.
			//G4cout << "primaryPositronKineticEnergy = " << primaryPositronKineticEnergy/MeV << " MeV." << G4endl;
			//G4cout << "cos_theta_neutron_wrt_neutrino_momentum_direction = " << cos_theta_neutron_wrt_neutrino_momentum_direction << G4endl;
			//G4cout << "cos_theta_positron_wrt_neutrino_momentum_direction = " << cos_theta_positron_wrt_neutrino_momentum_direction << G4endl;
			theta_neutron = acos(cos_theta_neutron_wrt_neutrino_momentum_direction);
			theta_positron = acos(cos_theta_positron_wrt_neutrino_momentum_direction);
			((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetNeutrinoKineticEnergyOfEventAction(neutrinoKineticEnergy);
			((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetCosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino(cos_theta_neutron_wrt_neutrino_momentum_direction);
			((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetCosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino(cos_theta_neutron_wrt_neutrino_momentum_direction);

			//
			// Set primary particle vertex.
			//
			G4double x_vertex, y_vertex, z_vertex;
			if(inverseBetaDecayInteractionVertex == "center"){
				x_vertex = 0.*mm;
				y_vertex = 0.*mm;
				z_vertex = 0.*mm;
			}
			else if(inverseBetaDecayInteractionVertex == "random"){
				x_vertex =
					2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthX())*(G4UniformRand() - .5)*mm;
				y_vertex =
					2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthY())*(G4UniformRand() - .5)*mm;
				z_vertex =
					2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthZ())*(G4UniformRand() - .5)*mm;
			}
			primaryVertexVector = G4ThreeVector(x_vertex, y_vertex, z_vertex);

			//
			// Set primary particle momentum direction.
			//
			G4double px_neutron, py_neutron, pz_neutron;
			px_neutron = cos(phi_neutron)*sin(theta_neutron);
			py_neutron = sin(phi_neutron)*sin(theta_neutron);
			pz_neutron = cos(theta_neutron);
			primaryNeutronMomentumDirectionVector = G4ThreeVector(px_neutron, py_neutron, pz_neutron);

			G4double px_positron, py_positron, pz_positron;
			px_positron = cos(phi_positron)*sin(theta_positron);
			py_positron = sin(phi_positron)*sin(theta_positron);
			pz_positron = cos(theta_positron);
			primaryPositronMomentumDirectionVector = G4ThreeVector(px_positron, py_positron, pz_positron);

			if(incomingNeutrinoDirection == "random"){
				phi_neutrino = 2.*pi*G4UniformRand();
				theta_neutrino = acos(2.*G4UniformRand()-1.);
				primaryNeutronMomentumDirectionVector = primaryNeutronMomentumDirectionVector.rotateY(theta_neutrino);
				primaryNeutronMomentumDirectionVector = primaryNeutronMomentumDirectionVector.rotateZ(phi_neutrino);
				primaryPositronMomentumDirectionVector = primaryPositronMomentumDirectionVector.rotateY(theta_neutrino);
				primaryPositronMomentumDirectionVector = primaryPositronMomentumDirectionVector.rotateZ(phi_neutrino);
			}
			else if(incomingNeutrinoDirection == "+z"){
				phi_neutrino = theta_neutrino = 0.;
			}
		}
	}
	else{ // Initial settings for primary particles are set as the following default values when no neutrino info is read in from data file.
		// Set default values of primary particle properties.
		((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetNeutrinoKineticEnergyOfEventAction(0.*MeV); // Default value of neutrino kinetic energy is 0. MeV when no neutrino info is given.
		//((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetCosineOfNormalAngleOfNeutronInitialMomentum(1.);
		
		//
		// Set vertex of neutrino interaction.
		//
		G4double x_vertex, y_vertex, z_vertex;
		if(inverseBetaDecayInteractionVertex == "center"){
			x_vertex = 0.*mm;
			y_vertex = 0.*mm;
			z_vertex = 0.*mm;
		}
		else if(inverseBetaDecayInteractionVertex == "random"){
			x_vertex =
				2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthX())*(G4UniformRand() - .5)*mm;
			y_vertex =
				2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthY())*(G4UniformRand() - .5)*mm;
			z_vertex =
				2.*(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthZ())*(G4UniformRand() - .5)*mm;
		}
		primaryVertexVector = G4ThreeVector(x_vertex, y_vertex, z_vertex);
		
		G4double px_neutron, py_neutron, pz_neutron;
		px_neutron = 0.;
		py_neutron = 0.;
		pz_neutron = 1.;
		primaryNeutronMomentumDirectionVector = G4ThreeVector(px_neutron, py_neutron, pz_neutron); // Set default momentum direction for primary neutron.
		
		G4double px_positron, py_positron, pz_positron;
		px_positron = 0.;
		py_positron = 0.;
		pz_positron = 1.;
		primaryPositronMomentumDirectionVector = G4ThreeVector(px_positron, py_positron, pz_positron); // Set default momentum direction for primary positron.
		
		//G4double px_electronAntiNeutrino, py_electronAntiNeutrino, pz_electronAntiNeutrino;
		//px_electronAntiNeutrino = 0.;
		//py_electronAntiNeutrino = 0.;
		//pz_electronAntiNeutrino = 1.;
		//primaryElectronAntiNeutrinoMomentumDirectionVector = G4ThreeVector(px_electronAntiNeutrino, py_electronAntiNeutrino, pz_electronAntiNeutrino); // Set default momentum direction for primary electron antineutrino.
		
		// Set default kinetic energy of particles to be all identical for now.
		primaryNeutronKineticEnergy = inverseBetaDecayDaughterParticleKineticEnergy;
		primaryPositronKineticEnergy = inverseBetaDecayDaughterParticleKineticEnergy;
		//primaryElectronAntiNeutrinoKineticEnergy = inverseBetaDecayDaughterParticleKineticEnergy;
		if(incomingNeutrinoDirection == "random"){
			phi_neutrino = 2.*pi*G4UniformRand();
			theta_neutrino = acos(2.*G4UniformRand()-1.);
			primaryNeutronMomentumDirectionVector = primaryNeutronMomentumDirectionVector.rotateY(theta_neutrino);
			primaryNeutronMomentumDirectionVector = primaryNeutronMomentumDirectionVector.rotateZ(phi_neutrino);
			primaryPositronMomentumDirectionVector = primaryPositronMomentumDirectionVector.rotateY(theta_neutrino);
			primaryPositronMomentumDirectionVector = primaryPositronMomentumDirectionVector.rotateZ(phi_neutrino);
		}
		else if(incomingNeutrinoDirection == "+z"){
		phi_neutrino = theta_neutrino = 0.;
		}
	}
	((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->SetNeutrinoMomentumUnitVector(G4ThreeVector(cos(phi_neutrino)*sin(theta_neutrino), sin(phi_neutrino)*sin(theta_neutrino), cos(theta_neutrino)));
	primaryNeutronGun -> SetParticlePosition(primaryVertexVector);
	primaryNeutronGun -> SetParticleMomentumDirection(primaryNeutronMomentumDirectionVector);
	primaryNeutronGun -> SetParticleEnergy(primaryNeutronKineticEnergy); // Set kinetic energy of particle.

	primaryPositronGun -> SetParticlePosition(primaryVertexVector);
	primaryPositronGun -> SetParticleMomentumDirection(primaryPositronMomentumDirectionVector);
	primaryPositronGun -> SetParticleEnergy(primaryPositronKineticEnergy); // Set kinetic energy of particle.

//	primaryElectronAntiNeutrinoGun -> SetParticlePosition(primaryVertexVector);
//	primaryElectronAntiNeutrinoGun -> SetParticleMomentumDirection(primaryElectronAntiNeutrinoMomentumDirectionVector);
//	primaryElectronAntiNeutrinoGun -> SetParticleEnergy(primaryElectronAntiNeutrinoKineticEnergy);

if(primaryPositronIsShot == true)
	primaryPositronGun->GeneratePrimaryVertex(theEvent);
if(primaryNeutronIsShot == true)
	primaryNeutronGun->GeneratePrimaryVertex(theEvent);
//if(primaryElectronAntiNeutrinoIsShot == true)
//	primaryElectronAntiNeutrinoGun->GeneratePrimaryVertex(theEvent);
}

//
// Doing headers that are required once a run.
//
void MTCG4PrimaryGeneratorAction::GenerateFilesAndHeadersForRun(){
	std::ofstream headerOut;
	
	//
	// Headers for photon detection output files.
	//
	const G4String photonHitHeaderString = "# EventID MCPQuadSetCopyNumber MCPRowCopyNumber MCPCopyNumber pixelVolumeRowCopyNumber pixelVolumeCopyNumber hitTime(ns) photonHitPositionVector.x(mm) photonHitPositionVector.y(mm) photonHitPositionVector.z(mm)";
	headerOut.open("/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/allPhotonPixelHit.dat", std::ofstream::out);
	headerOut << photonHitHeaderString << G4endl;
	headerOut.close();
	headerOut.open("/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/firstPhotonPixelHit.dat", std::ofstream::out);
	headerOut << photonHitHeaderString << G4endl;
	headerOut.close();
}
