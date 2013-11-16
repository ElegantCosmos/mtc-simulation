//
// MTCG4PrimaryGeneratorAction.cc by Mich 110502
//

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include "MTCG4PrimaryGeneratorAction.hh"
#include "MTCG4PrimaryGeneratorMessenger.hh"
#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4DetectorConstruction.hh"

G4double GetCosmicRayMuonDifferentialIntensity();
void GetMuonMomentumAngles(G4double &theta, G4double &phi);
G4double GetMuonEnergy();

MTCG4PrimaryGeneratorAction::MTCG4PrimaryGeneratorAction()
{
	//
	// Parameter default values.
	//
	fInputFileIsOpened = false;
	fNuDirectionDescription = "random";
	fInverseBetaDecayInteractionVertex = "random";
	fFlagForReadingInPrimaryParticleDataFile = false;
	fInverseBetaDecayDaughterParticleKineticEnergy = 1000.*eV;
	fPrimaryPositronIsShot = false;
	fPrimaryNeutronIsShot = false;

	fMessenger = new MTCG4PrimaryGeneratorMessenger(this);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	assert(particleTable);
	const G4int kNumOfParticles = 1;

	fParticleGun = new G4ParticleGun();
	fGeneralParticleSource = new G4GeneralParticleSource();

	fPrimaryNeutronGun = new G4ParticleGun(kNumOfParticles);
	fPrimaryNeutronGun->SetParticleDefinition(particleTable->
			FindParticle(G4Neutron::NeutronDefinition()));

	fPrimaryPositronGun = new G4ParticleGun(kNumOfParticles);
	fPrimaryPositronGun->SetParticleDefinition(particleTable->
			FindParticle(G4Positron::PositronDefinition()));

	fPrimaryLaserGun = new G4ParticleGun(kNumOfParticles);
	fPrimaryLaserGun->SetParticleDefinition(particleTable->
			FindParticle(G4Gamma::GammaDefinition()));

	fPrimaryCosmicRayMuonGun = new G4ParticleGun(kNumOfParticles);
	
	//
	// Set primary particle to be a neutrino.
	// (This feature in Geant4 is not yet mature.)
	//
	//fPrimaryElectronAntiNuGun = new G4ParticleGun(kNumOfParticles);
	//G4ParticleDefinition* primaryElectronAntiNeutrino =
	//	particleTable->
	//	FindParticle(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
	//fPrimaryElectronAntiNuGun->SetParticleDefinition(
	//			particleTable->
	//			FindParticle(G4ElectronAntiNeutrino::ElectronAntiNeutrinoDefinition())
	//			);
}

MTCG4PrimaryGeneratorAction::~MTCG4PrimaryGeneratorAction()
{
	//if (fFlagForReadingInPrimaryParticleDataFile) {
	//	fPrimaryParticleDataInput->close(); // Make sure file is closed.
	//	delete fPrimaryParticleDataInput;
	//}
	delete fParticleGun;
	delete fGeneralParticleSource;
	delete fPrimaryNeutronGun;
	delete fPrimaryPositronGun;
	delete fPrimaryLaserGun;
	delete fPrimaryCosmicRayMuonGun;
	delete fMessenger;
}

void MTCG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* theEvent)
{
	// Set pseudo-random number generator seed value before event is simulated
	// but after event is known (G4Event). According to Geant4 forums, placing
	// this code in BeginOfEventActio() is too late, so put it here.
	// This enables us to revisit a particularly interesting event and re-run
	// individual events using the appropriate seed after running a whole series
	// of runs.
	const G4int runID = (dynamic_cast<const MTCG4RunAction*>(
				G4RunManager::GetRunManager()->GetUserRunAction()))->GetRunID();
	const G4int eventID = theEvent->GetEventID();
	// Create unique event/run ID for specifying paricular event+run.
	// Max of 1e6 events possible.
	const G4int seed = abs(runID)*1000000 + abs(eventID); // abs() just in case.
	G4cout << "Seed: " << seed << G4endl;
	CLHEP::HepRandom::setTheSeed(seed);

	//G4ThreeVector primaryElectronAntiNuMomentumDirection;
	//G4double primaryElectronAntiNuKineticEnergy;
	if (fPrimaryPositronIsShot || fPrimaryNeutronIsShot) {
		GenerateIBDPrimaries(theEvent);
	} // If primary positron or neutron is shot.
	//if (_primaryElectronAntiNuIsShot) { // Fix later.
	//	fPrimaryElectronAntiNuGun->SetParticlePosition(primaryVertex);
	//	fPrimaryElectronAntiNuGun->
	//		SetParticleMomentumDirection(primaryElectronAntiNuMomentumDirection);
	//	fPrimaryElectronAntiNuGun->
	//		SetParticleEnergy(primaryElectronAntiNuKineticEnergy);

	//	// Generate primary vertex.
	//	fPrimaryElectronAntiNuGun->GeneratePrimaryVertex(theEvent);
	//}

	if (fCosmicRayMuonIsShot) {
		GenerateCosmicRayMuons(theEvent);
	}
	if (fLaserIsShot) {
		// Fix later.
	}
	if (fParticleGunIsShot)
		fParticleGun->GeneratePrimaryVertex(theEvent);
	if (fGeneralParticleSourceIsShot)
		fGeneralParticleSource->GeneratePrimaryVertex(theEvent);
}

void MTCG4PrimaryGeneratorAction::GenerateIBDPrimaries(G4Event *theEvent)
{
	G4ThreeVector primaryVertex;
	G4double phi_neutrino = 0, theta_neutrino = 0;
	G4ThreeVector primaryNeutronMomentumDirection;
	G4ThreeVector primaryPositronMomentumDirection;
	G4double primaryNeutronKineticEnergy;
	G4double primaryPositronKineticEnergy;
	if (fFlagForReadingInPrimaryParticleDataFile) {
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

		// Open primary data file if not yet opened.
		if (!fInputFileIsOpened) {
			fPrimaryParticleDataInput = new
				std::ifstream( (fInputPath + fPrimaryDataFileName ).c_str(),
						std::ifstream::in);
			// Check if file was opened	correctly.
			if (!(fPrimaryParticleDataInput->good()))
				G4Exception("MTCG4PrimaryGeneratorAction::fPrimaryParticleDataInput",
						"", FatalErrorInArgument, "File not opened correctly.");
			fInputFileIsOpened = true;
		}

		char bufferChar[256];
		G4String bufferString;
		assert(fPrimaryParticleDataInput->good());
		while(fPrimaryParticleDataInput->getline(bufferChar, 256)) {
			bufferString.assign(bufferChar);
			// Ignore all comment lines of which the first non-space or
			// non-tab character of the line is not a digit.
			if (isdigit(bufferString[bufferString.find_first_not_of("		")])) break;
		}
		std::istringstream inputLine(bufferString.data());
		if (!( // If primary data file is unsuccessfully read.
					inputLine
					>> neutrinoEventNumber
					>> neutrinoKineticEnergy
					>> cos_theta_positron_wrt_neutrino_momentum_direction
					>> primaryPositronKineticEnergy
					>> cos_theta_neutron_wrt_neutrino_momentum_direction
					>> primaryNeutronKineticEnergy))
			G4Exception("MTCG4PrimaryGeneratorAction::GeneratePrimaries()",
					"",
					FatalErrorInArgument,
					(bufferString + " cannot be read.\n" +
					 "INVALID FILE FORMAT FOR NEUTRON AND POSITRON EVENT.\n" +
					 "NO DATA WILL BE USED!\n" +
					 "SEE MTCG4PrimaryGeneratorAction.cc!\n").c_str());
		else { // If primary data file is read in correctly.
			// Multiply correct units before using these parameters.
			neutrinoKineticEnergy *= MeV;
			primaryPositronKineticEnergy *= MeV;
			primaryNeutronKineticEnergy *= MeV;

			//G4cout << "primaryNeutronKineticEnergy = " << primaryNeutronKineticEnergy/MeV << " MeV." << G4endl; // Test output to see if parameters were read correctly.
			//G4cout << "primaryPositronKineticEnergy = " << primaryPositronKineticEnergy/MeV << " MeV." << G4endl;
			//G4cout << "cos_theta_neutron_wrt_neutrino_momentum_direction = " << cos_theta_neutron_wrt_neutrino_momentum_direction << G4endl;
			//G4cout << "cos_theta_positron_wrt_neutrino_momentum_direction = " << cos_theta_positron_wrt_neutrino_momentum_direction << G4endl;

			// Get original angles.
			theta_neutron =
				acos(cos_theta_neutron_wrt_neutrino_momentum_direction);
			theta_positron =
				acos(cos_theta_positron_wrt_neutrino_momentum_direction);

			// Set some common values in MTCG4EventAction.
			((MTCG4EventAction*)G4EventManager::GetEventManager()->
			 GetUserEventAction())->
				SetNeutrinoKineticEnergyOfEventAction(neutrinoKineticEnergy);
			((MTCG4EventAction*)G4EventManager::GetEventManager()->
			 GetUserEventAction())->
				SetCosOfNormalAngleOfNeutronInitMomentumWrtNeutrino(
						cos_theta_neutron_wrt_neutrino_momentum_direction);
			((MTCG4EventAction*)G4EventManager::GetEventManager()->
			 GetUserEventAction())->
				SetCosOfNormalAngleOfPositronInitMomentumWrtNeutrino(
						cos_theta_neutron_wrt_neutrino_momentum_direction);

			// Set primary particle vertex.
			G4double x_vertex = 0*mm, y_vertex = 0*mm, z_vertex = 0*mm;
			if (fInverseBetaDecayInteractionVertex == "center") {
				// Do nothing.
			}
			else if (fInverseBetaDecayInteractionVertex == "random") {
				x_vertex =
					(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
					  GetUserDetectorConstruction())->
					 GetScintDimensionX())*(G4UniformRand() - .5);
				y_vertex =
					(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
					  GetUserDetectorConstruction())->
					 GetScintDimensionY())*(G4UniformRand() - .5);
				z_vertex =
					(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
					  GetUserDetectorConstruction())->
					 GetScintDimensionZ())*(G4UniformRand() - .5);
			}
			else if (fInverseBetaDecayInteractionVertex == "-z") {
				x_vertex = 0.*mm;	y_vertex = 0.*mm;	z_vertex = -15.*mm;
			}
			primaryVertex = G4ThreeVector(x_vertex, y_vertex, z_vertex);

			// Set primary particle momentum directions.
			primaryNeutronMomentumDirection =
				G4ThreeVector(
						cos(phi_neutron)*sin(theta_neutron),
						sin(phi_neutron)*sin(theta_neutron),
						cos(theta_neutron));
			primaryPositronMomentumDirection =
				G4ThreeVector(
						cos(phi_positron)*sin(theta_positron),
						sin(phi_positron)*sin(theta_positron),
						cos(theta_positron));

			// Set phi and theta angles for neutrino direction.
			if (fNuDirectionDescription == "random") {
				phi_neutrino = 2.*pi*G4UniformRand();
				theta_neutrino = acos(2.*G4UniformRand()-1.);
				primaryNeutronMomentumDirection =
					primaryNeutronMomentumDirection.rotateY(theta_neutrino);
				primaryNeutronMomentumDirection =
					primaryNeutronMomentumDirection.rotateZ(phi_neutrino);
				primaryPositronMomentumDirection =
					primaryPositronMomentumDirection.rotateY(theta_neutrino);
				primaryPositronMomentumDirection =
					primaryPositronMomentumDirection.rotateZ(phi_neutrino);
			}
			else if (fNuDirectionDescription == "+z")
				phi_neutrino = theta_neutrino = 0.;
		} // If primary data file is successfully read.
	} // If primary data file is decided to be read in.
	else { // Following default values used if no neutrino data info is read in.

		// Default value of neutrino kinetic energy = 0 MeV.
		((MTCG4EventAction*)G4EventManager::GetEventManager()->
		 GetUserEventAction())->SetNeutrinoKineticEnergyOfEventAction(0.*MeV);
		//((MTCG4EventAction*)G4EventManager::GetEventManager()->
		//GetUserEventAction())->
		//SetCosineOfNormalAngleOfNeutronInitialMomentum(1.);

		// Set vertex of neutrino interaction.
		G4double x_vertex = 0*mm, y_vertex = 0*mm, z_vertex = 0*mm;
		if (fInverseBetaDecayInteractionVertex == "center") {
			// Do nothing.
		}
		else if (fInverseBetaDecayInteractionVertex == "random") {
			x_vertex = (((MTCG4DetectorConstruction*)
						G4RunManager::GetRunManager()->
						GetUserDetectorConstruction())->
					GetScintDimensionX())*(G4UniformRand() - .5);
			y_vertex = (((MTCG4DetectorConstruction*)
						G4RunManager::GetRunManager()->
						GetUserDetectorConstruction())->
					GetScintDimensionY())*(G4UniformRand() - .5);
			z_vertex = (((MTCG4DetectorConstruction*)
						G4RunManager::GetRunManager()->
						GetUserDetectorConstruction())->
					GetScintDimensionZ())*(G4UniformRand() - .5);
		}
		else if (fInverseBetaDecayInteractionVertex == "-z") {
			x_vertex = 0.*mm;
			y_vertex = 0.*mm;
			z_vertex = -15.*mm;
		}
		primaryVertex = G4ThreeVector(x_vertex, y_vertex, z_vertex);

		// Set default momentum direction for primary neutron.
		G4double px_neutron, py_neutron, pz_neutron;
		px_neutron = 0.;
		py_neutron = 0.;
		pz_neutron = -1.;
		primaryNeutronMomentumDirection =
			G4ThreeVector(px_neutron, py_neutron, pz_neutron);

		// Set default momentum direction for primary positron.
		G4double px_positron, py_positron, pz_positron;
		px_positron = 0.;
		py_positron = 0.;
		pz_positron = -1.;
		primaryPositronMomentumDirection =
			G4ThreeVector(px_positron, py_positron, pz_positron);

		// Set default momentum direction for primary electron antineutrino.
		//G4double px_electronAntiNu, py_electronAntiNu, pz_electronAntiNu;
		//px_electronAntiNu = 0.;
		//py_electronAntiNu = 0.;
		//pz_electronAntiNu = 1.;
		//primaryElectronAntiNuMomentumDirection =
		//	G4ThreeVector(px_electronAntiNu, py_electronAntiNu, pz_electronAntiNu);

		// Set default kinetic energy of particles to be all identical for now.
		primaryNeutronKineticEnergy =
			fInverseBetaDecayDaughterParticleKineticEnergy;
		primaryPositronKineticEnergy =
			fInverseBetaDecayDaughterParticleKineticEnergy;
		//primaryElectronAntiNuKineticEnergy =
		//	fInverseBetaDecayDaughterParticleKineticEnergy;

		// Set phi and theta angles for neutrino direction.
		if (fNuDirectionDescription == "random") {
			phi_neutrino = 2.*pi*G4UniformRand();
			theta_neutrino = acos(2.*G4UniformRand()-1.);
			primaryNeutronMomentumDirection.setTheta(theta_neutrino);
			primaryNeutronMomentumDirection.setPhi(phi_neutrino);
			primaryPositronMomentumDirection.setTheta(theta_neutrino);
			primaryPositronMomentumDirection.setPhi(phi_neutrino);
		}
		else if (fNuDirectionDescription == "+z")
			phi_neutrino = theta_neutrino = 0.;
	}

	// Set neutrino momentum unit vector in MTCG4EventAction.
	((MTCG4EventAction*)G4EventManager::GetEventManager()->
	 GetUserEventAction())->SetNeutrinoMomentumUnitVector(
	 G4ThreeVector(cos(phi_neutrino)*sin(theta_neutrino),
		 sin(phi_neutrino)*sin(theta_neutrino), cos(theta_neutrino)));

	// Set inverse-beta decay vertex.
	fPrimaryNeutronGun->SetParticlePosition(primaryVertex);
	fPrimaryPositronGun->SetParticlePosition(primaryVertex);

	// Set momentum direction of neutron and positron.
	fPrimaryNeutronGun->
		SetParticleMomentumDirection(primaryNeutronMomentumDirection);
	fPrimaryPositronGun->
		SetParticleMomentumDirection(primaryPositronMomentumDirection);

	// Set kinetic energy of particle.
	fPrimaryNeutronGun->SetParticleEnergy(primaryNeutronKineticEnergy);
	fPrimaryPositronGun->SetParticleEnergy(primaryPositronKineticEnergy);

	// Finally, generate primary vertices if respective "shoot" flag is true.
	if (fPrimaryNeutronIsShot)
		fPrimaryNeutronGun->GeneratePrimaryVertex(theEvent);
	if (fPrimaryPositronIsShot)
		fPrimaryPositronGun->GeneratePrimaryVertex(theEvent);
}

void MTCG4PrimaryGeneratorAction::GenerateCosmicRayMuons(G4Event *theEvent)
{
	// Ratio of the number of mu+ over mu- taken to be 1.225 +/- 0.049 c.f.
	// Blackett (1937), Jones (1939), Hughes (1940) for momentum range
	// 0.4~20 GeV/c. "Cosmic Rays at Ground Level", Wolfendale, chap.2
	// "Energetic Muons", sec 6. "The muon charge ratio", page 44.
	const G4double muonRatio = 1.225;
	const G4double muonPlusRatioToAllMuons = muonRatio/(1.0+muonRatio);
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	assert(particleTable);
	const G4double scintDimensionX = // Length of scintillator cube in X.
		(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		  GetUserDetectorConstruction())->
		 GetScintDimensionX());
	const G4double scintDimensionY = // Length of scintillator cube in Y.
		(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		  GetUserDetectorConstruction())->
		 GetScintDimensionY());
	const G4double scintDimensionZ = // Length of scintillator cube in Z.
		(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		  GetUserDetectorConstruction())->
		 GetScintDimensionZ());
	// Get rho and phi of initial muon starting position vector. We want this
	// vector to point with uniform probability over a circular disk surface
	// parallel to the x-y plane with a radius such that its diameter will cover
	// the entire cube regardless of where the disk may be in terms of
	// orientation and position if the disk axis always goes through the center
	// of the cube.  The hight of this disk wrt the center of the
	// cube (0, 0, 0) is arbitrarily set to 65 mm + 130 mm (the hight of the
	// scintillator cube). The differential surface area for a disk in
	// cyllindrical coordinates is dS = rho*d(rho)*d(phi) = 0.5*d(rho^2)*d(phi).
	// So sampling uniformly and randomly over [0, 1] for rho^2 is what we want
	// to do. Then we can take the sqrt to find rho. Phi can be sampled as is
	// over [0, 2pi].
	const G4double muonInitPosRho =
		0.5*
		sqrt(scintDimensionX*scintDimensionX + scintDimensionY*scintDimensionY +
				scintDimensionZ*scintDimensionZ)*sqrt(G4UniformRand());
	const G4double muonInitPosPhi = twopi*G4UniformRand()*rad;
	const G4double muonInitPosZ = // Initial muon Z coordinate.
				scintDimensionZ + 65.*mm;
	//G4ThreeVector muonInitPos( // Muon initial vertex.
	//		muonInitPosRho*sin(muonInitPosPhi/rad),
	//		muonInitPosRho*cos(muonInitPosPhi/rad),
	//	   	muonInitPosZ);
	G4ThreeVector muonInitPos(0*mm, 0*mm, 5000.1*mm);
	G4cout << "muonInitPos before rotation: " << muonInitPos << G4endl;
	G4double muonMomAnglePhi = 0*rad, // Muon momentum angles.
			 muonMomAngleTheta = 0*rad;
	GetMuonMomentumAngles(muonMomAngleTheta, muonMomAnglePhi);
	//G4cout << "muonMomAnglePhi: " << muonMomAnglePhi << G4endl;
	//G4cout << "muonMomAngleTheta: " << muonMomAngleTheta << G4endl;

	// Rotate circular disk of muon initial positions such that it is
	// perpendicular to the muon momentum. This way we maximize the probability
	// that the muon will interact with the scintillating cube so we can save
	// computing time. The disk will be placed on the opposite side of the muon
	// momentum direction wrt the center of the cube.
	//muonInitPos.rotateY(pi - muonMomAngleTheta/rad);
	//muonInitPos.rotateZ(pi + muonMomAnglePhi/rad);
	//G4cout << "muonInitPos after rotation: " << muonInitPos << G4endl;

	// Get muon kinetic energy.
	const G4double muonEnergy = GetMuonEnergy();
	//G4cout << "muonEnergy: " << muonEnergy << G4endl;

	// Set primary particle properties.
	if (G4UniformRand() < muonPlusRatioToAllMuons) // Muon is mu+.
		fPrimaryCosmicRayMuonGun->SetParticleDefinition(particleTable->
				FindParticle(G4MuonPlus::MuonPlusDefinition()));
	else // Muon is mu-.
		fPrimaryCosmicRayMuonGun->SetParticleDefinition(particleTable->
				FindParticle(G4MuonMinus::MuonMinusDefinition()));
	fPrimaryCosmicRayMuonGun->SetParticlePosition(muonInitPos);
	//G4ThreeVector primaryCosmicRayMuonMomentumDirection(0, 0, 1);
	//primaryCosmicRayMuonMomentumDirection.setPhi(muonMomAnglePhi/rad);
	//primaryCosmicRayMuonMomentumDirection.setTheta(muonMomAngleTheta/rad);
	//primaryCosmicRayMuonMomentumDirection.setMag(1);
	G4ThreeVector primaryCosmicRayMuonMomentumDirection(0, 0, -1);
	fPrimaryCosmicRayMuonGun->SetParticleMomentumDirection(
			primaryCosmicRayMuonMomentumDirection);
	fPrimaryCosmicRayMuonGun->SetParticleEnergy(muonEnergy);
	fPrimaryCosmicRayMuonGun->GeneratePrimaryVertex(theEvent);
}

void GetMuonMomentumAngles(G4double &theta, G4double &phi)
{
	G4double cosTheta; // Cosine zenith angle of muon origin wrt (0,0,0).
	G4double probability; // Random probability between 0~1.
	G4double muonTheta, muonPhi;

	// Loop over random samples over [0, 1]. Compare this to the zenith angle
	// pdf which is proportional to (cos(theta))^2.
	do {
		cosTheta = G4UniformRand(); // Uniform distribution 0~1 in cos theta.
		probability = G4UniformRand();
	} while (probability > cosTheta*cosTheta); // Zenith angle pdf = costheta^2.
	assert(cosTheta >= 0 && cosTheta <= 1); 
	muonTheta = (pi - acos(cosTheta))*rad; // Convert to angle of muon dir.
	muonPhi = twopi*G4UniformRand()*rad; // Random phi angle 0~twopi.
	G4ThreeVector muonDir(0, 0, 1);
	muonDir.setTheta(muonTheta/rad);
	muonDir.setPhi(muonPhi/rad);
	muonDir.setMag(1);
	G4cout << "muonTheta: " << muonTheta/rad << G4endl;
	G4cout << "muonPhi: " <<muonPhi/rad << G4endl;
	theta = muonTheta;
	phi = muonPhi;
}

G4double GetCosmicRayMuonDifferentialIntensity(G4double momentum) 
{
	// Use momentum in GeV/c, c = 1.
	const G4double a = 0.147698;
	const G4double b = -3.15792;
	const G4double c = -2.88618;
	// Differential cosmic ray muon intensity (cm^-2*sr^-1*sec^-1*(GeV/c)^-1).
	return a*pow(momentum/GeV - b, c);
}

G4double GetMuonEnergy()
{
	G4double momentum; // Momentum of muon in GeV/c, c = 1.
	G4double randNo = 0; // Random sample 0~1.
	do {
		momentum = 1000*G4UniformRand()*GeV; // Momentum 0~1000 GeV/c, c = 1.
		// Ignore momentum below 0.1 GeV (minimum of fit spectrum of muons).
		if (momentum < 0.1*GeV) continue;
		randNo = G4UniformRand(); // 0~1.
	} while (randNo > 400*GetCosmicRayMuonDifferentialIntensity(momentum));
	const G4double mass = G4MuonMinus::MuonMinusDefinition()->GetPDGMass();
	G4double energy = sqrt(momentum*momentum + mass*mass) - mass;
	return energy;
}
