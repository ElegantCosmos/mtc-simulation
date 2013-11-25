///////////////////////////////////////
// MTCG4SteppingAction.cc by Mich 110502
///////////////////////////////////////

#include "MTCG4RunAction.hh"
#include "MTCG4PmtSD.hh"
#include "MTCG4SteppingAction.hh"
#include "MTCG4SteppingMessenger.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "G4OpBoundaryProcess.hh"

#include "TFile.h"
#include "TTree.h"

#include "StepRootIO.hh"
#include "StepTextIO.hh"

//
// Define a constant quantum efficiency (0. ~ 1.) to be used by photo sensitive
// pixels when optical surface properties are not used.
//
#define CONSTANT_QUANTUM_EFFICIENCY 1.

MTCG4SteppingAction::MTCG4SteppingAction()
{
	// Flag to limit output data amount for testing purposes.
	fOutputOnlyFirstNEvents = -1; // All events outputted by default.

	fMessenger = new MTCG4SteppingMessenger(this);
}

MTCG4SteppingAction::~MTCG4SteppingAction()
{
	delete fMessenger;
}

// Method Called During a Step //
void MTCG4SteppingAction::UserSteppingAction(const G4Step* theStep)
{
	//const G4Event* theEvent = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	//G4Track* theTrack = theStep->GetTrack();
	//G4ParticleDefinition* particleDefinition = theTrack->GetDefinition();
	G4VPhysicalVolume* postStepPointPhysicalVolume =
		theStep->GetPostStepPoint()->GetPhysicalVolume();
	if (!postStepPointPhysicalVolume) {
		// If the post-step point is not available,
		// the particle is out of the world volume.
		// Do not do anything with it and just return.
		//G4cout << particleDefinition->GetParticleName() << " post step point is out of world!\n";
		return;
	}
	
	if (fOutputOnlyFirstNEvents >= 0) // Do output for only first n events.
		if (G4EventManager::GetEventManager()-> GetConstCurrentEvent() ->
				GetEventID() >= fOutputOnlyFirstNEvents)
			return;

	DoStepOutputToRootFile(theStep);
	//DoStepOutputToTextFile(theStep);

	fPhotonDetectedAtEndOfStep = false; // Reset photon detection flag.
}

// Optical Photon and Gamma Detection postStepProcess at Sensitive Pixel //
//
//void MTCG4SteppingAction::DetectPhotonOfSteppingAction(const G4Step* theStep, G4String& _boundary_status_name, G4String& isBetweenScintillatorAndSensitivePixel) {
//	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
//	G4VPhysicalVolume* preStepPointPhysicalVolume = preStepPoint->GetPhysicalVolume();
//	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
//	G4VPhysicalVolume* postStepPointPhysicalVolume = postStepPoint->GetPhysicalVolume();
//	G4ParticleDefinition* particleDefinition = theStep->GetTrack()->GetDefinition();
//	if (particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition() || particleDefinition == G4Gamma::GammaDefinition()) { // If the particle is an optical photon or a gamma, we want it to go through the detection postStepProcess at the surface of the sensitive pixel geometry.
//		G4String preStepPointPhysicalVolumeName = preStepPointPhysicalVolume->GetName(); // Get the names of the volumes.
//		G4String postStepPointPhysicalVolumeName = postStepPointPhysicalVolume->GetName();
//		// If the particle is at a boundary between two geometries, the pre-step
//		// volume is the scintillator or the world, and the post-step volume is the
//		// pixel.
//			G4cout << "step status = " << postStepPoint->GetStepStatus() << G4endl;
//			G4cout << "prestep point physical volume = " <<
//				preStepPointPhysicalVolumeName << G4endl;
//			G4cout << "postStepPointPhysicalVolumeName = " <<
//				postStepPointPhysicalVolumeName << G4endl;
//		if (postStepPoint->GetStepStatus() == fGeomBoundary &&
//				postStepPointPhysicalVolumeName == "photocathode_physical") {
//			G4String particleName = particleDefinition->GetParticleName();
//			//G4cout << particleName << " is at the boundary between scintillator and pixel.\n";
//			isBetweenScintillatorAndSensitivePixel = "betweenScintillatorAndPixel";
//			if ((dynamic_cast<MTCG4DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()))->GetOpticalSurfaceProperties()
//					== true) {
//				G4OpBoundaryProcessStatus boundaryStatus = Undefined;
//				G4OpBoundaryProcess* boundary = NULL;
//				if (!boundary) {
//					G4ProcessManager* pm 
//						= theStep->GetTrack()->GetDefinition()->GetProcessManager();
//					G4int nprocesses = pm->GetProcessListLength();
//					G4ProcessVector* pv = pm->GetProcessList(); // Get a pointer to the list of processes available for this particle.
//					//G4cout << "postStepProcess Manager instantiated.\n"; // Of debugging.
//					G4int i;
//					for(i = 0; i < nprocesses; i++) {
//						if ((*pv)[i]->GetProcessName() == "OpBoundary") { // If the optical boundary postStepProcess is found in the list of available processes, set it to boundaryStatus.
//							//G4cout << "boundary postStepProcess found!\n";
//							boundary = (G4OpBoundaryProcess*)(*pv)[i];
//							boundaryStatus = boundary->GetStatus();
//							//G4cout << "boundary status set\n"; 
//							break;
//						}
//					}
//				}
//				switch(boundaryStatus) { // Depending on the status of the particle at the geometrical boundary, the particle will be handled differently.
//					case Absorption:
//						//G4cout << particleName << " is absorbed by sensitive pixel!\n";
//						_boundary_status_name = "absorbedByPixel";
//						break;
//					case Detection: // Note, this assumes that the volume causing detection is the photocathode because it is the only one with non-zero efficiency.
//						// Triger sensitive detector manually since photon is absorbed but status was "Detection".
//						{
//							//G4cout << particleName << " is detected by sensitive pixel!\n";
//							_boundary_status_name = "detectedByPixel";
//							//G4cout << "This is a test to see if QE is working.\n";
//							G4SDManager* SDman = G4SDManager::GetSDMpointer();
//							G4String sdName = "/MTCG4Photocathode/MTCG4PmtSD";
//							MTCG4PmtSD* pmtSD = (MTCG4PmtSD*)SDman
//								->FindSensitiveDetector(sdName);
//							if (pmtSD)
//								pmtSD->ProcessHits_constStep(theStep, NULL);
//							//			MTCG4PmtSD* pmtSD = new MTCG4PmtSD(sdName);
//							//			SDman->AddNewDetector(pmtSD);
//							//			pmtLogical
//							//MTCG4UserTrackInformation->AddTrackStatusFlag(hitPMT);
//						}
//						break;
//					case FresnelReflection:
//						//G4cout << "FresnelReflection!" << G4endl;
//						_boundary_status_name = "FresnelReflection";
//						break;
//					case TotalInternalReflection:
//						//G4cout << "TotalInternalReflection!" << G4endl;
//						_boundary_status_name = "TotalInternalReflection";
//						break;
//					case SpikeReflection: // What is this?
//						//G4cout << "SpikeReflection!" << G4endl;
//						_boundary_status_name = "SpikeReflection";				
//						break;
//					case NoRINDEX: // This seems to occur as an error message when I did the optical surface method incorrectly in MTCG4DetectorConstruction.cc.
//						//G4cout << "NoRINDEX!" << G4endl;
//						_boundary_status_name = "NoRINDEX!";
//						break;
//					default:
//						//G4cout << "default" << G4endl;
//						_boundary_status_name = "default";
//						break;
//				}
//			}
//			else {
//				G4Track* theTrack = theStep->GetTrack();
//				// Manual Photon Detection postStepProcess Not Using Quantum Efficiency Feature Provided By Geant //
//				//G4double photonEnergy = postStepPoint->GetTotalEnergy();
//				const G4double quantumEfficiency = CONSTANT_QUANTUM_EFFICIENCY;
//				G4double randomFlat = G4UniformRand();
//				if (randomFlat > quantumEfficiency) {
//					//G4cout << particleName << " is not detected by sensitive pixel due to quantum efficiency.\n";
//					theTrack->SetTrackStatus(fStopAndKill);
//					//G4cout << "Killed " << particleName << ".\n";
//					return;
//				}
//				//G4cout << particleName << " is detected by sensitive pixel!\n";
//				G4SDManager* SDman = G4SDManager::GetSDMpointer();
//				G4String sdName="/MTCG4Photocathode/MTCG4PmtSD";
//				MTCG4PmtSD* pmtSD = (MTCG4PmtSD*)SDman
//					->FindSensitiveDetector(sdName);
//				if (pmtSD)
//					pmtSD->ProcessHits_constStep(theStep, NULL);
//				theTrack->SetTrackStatus(fStopAndKill);
//				//G4cout << "Killed " << particleName << ".\n";
//				//			MTCG4PmtSD* pmtSD = new MTCG4PmtSD(sdName);
//				//			SDman->AddNewDetector(pmtSD);
//				//			pmtLogical                                                                                                     		
//				//MTCG4UserTrackInformation->AddTrackStatusFlag(hitPMT);                                                           		
//			}
//		}
//	}
//}

//
// Do output for all events.
//
void MTCG4SteppingAction::DoStepOutputToTextFile(const G4Step* theStep) {
	const G4Event* theEvent = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4Track* theTrack = theStep->GetTrack();
	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
	G4ThreeVector preStepPosition = preStepPoint->GetPosition();
	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	G4ThreeVector postStepPosition = postStepPoint->GetPosition();
	G4ParticleDefinition* particleDefinition = theTrack->GetDefinition();
	G4String postStepPointPhysicalVolumeName = postStepPoint->GetPhysicalVolume()->GetName();
	G4ThreeVector nuMomentumUnitVector =
		(dynamic_cast<MTCG4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction()))->GetNeutrinoMomentumUnitVector();
	const G4VProcess *postStepProcess = postStepPoint->GetProcessDefinedStep();
	//fParticleName[0] = '\0';
	//fCreatorProcessName[0] = '\0';
	//fProcessName[0] = '\0';

	// Variables to output.
	const MTCG4RunAction *runAction = dynamic_cast<const MTCG4RunAction*>(
				G4RunManager::GetRunManager()->GetUserRunAction());
	assert(runAction);
	G4int		runID = runAction->GetRunID();
	G4int		eventID = theEvent->GetEventID();
	G4double	nuKineticEnergy =
		((MTCG4EventAction*)G4EventManager::GetEventManager() ->
		 GetUserEventAction()) -> GetNeutrinoKineticEnergyOfEventAction()/MeV;
	G4double	nuMomentumUnitVectorX = nuMomentumUnitVector.x();
	G4double	nuMomentumUnitVectorY = nuMomentumUnitVector.y();
	G4double	nuMomentumUnitVectorZ = nuMomentumUnitVector.z();
	G4int		stepID = theTrack->GetCurrentStepNumber();
	G4String	particleName =
		/*const_cast<char*>(*/particleDefinition->GetParticleName()/*.c_str())*/;
	G4int		pdgEncoding = particleDefinition->GetPDGEncoding(); 
	G4int		trackID = theTrack->GetTrackID();
	G4int		parentID = theTrack->GetParentID();
	G4double	preStepPositionX = preStepPosition.x()/mm;
	G4double	preStepPositionY = preStepPosition.y()/mm;
	G4double	preStepPositionZ = preStepPosition.z()/mm;
	G4double	postStepPositionX = postStepPosition.x()/mm;
	G4double	postStepPositionY = postStepPosition.y()/mm;
	G4double	postStepPositionZ = postStepPosition.z()/mm;
	G4double	preStepGlobalTime = preStepPoint->GetGlobalTime()/ns;
	G4double	postStepGlobalTime = postStepPoint->GetGlobalTime()/ns;
	G4double	preStepKineticEnergy = preStepPoint->GetKineticEnergy()/MeV;
	G4double	postStepKineticEnergy = postStepPoint->GetKineticEnergy()/MeV;
	G4double	totalEnergyDeposit = theStep->GetTotalEnergyDeposit()/MeV;
	G4double	stepLength = theStep->GetStepLength()/mm;
	G4double	trackLength = theTrack->GetTrackLength()/mm;
	G4String creatorProcessName;
	if (theTrack->GetParentID() > 0) {
		creatorProcessName =
			/*const_cast<char*>(*/theTrack->GetCreatorProcess()->GetProcessName()/*.c_str())*/;
	}
	else {
		//strcpy(creatorProcessName, "primaryParticle");
		creatorProcessName = "primaryParticle";
	}
	G4String processName;
	G4int processType;
	G4int processSubType;
	if (postStepProcess != NULL) {
		processName =
			/*const_cast<char*>(*/postStepProcess->GetProcessName()/*.c_str())*/;
		processType = postStepProcess->GetProcessType();
		processSubType = postStepProcess->GetProcessSubType();
	}
	else {
		//strcpy(processName, "nullProcess");
		processName = "nullProcess";
		processType = -100;
		processSubType = -100;
	}
	G4int trackStatus = theTrack->GetTrackStatus();
	G4bool postStepPointInDetector;
	// Find if the post step point ended in the scintillator volume.
	if (postStepPointPhysicalVolumeName == "scint_physical")
		postStepPointInDetector = true;
	else
		postStepPointInDetector = false;

	//// Fill text file.
	StepTextIO* fText = StepTextIO::GetInstance();
	fText->SetRunID(runID);
	fText->SetEventID(eventID);
	fText->SetNuKineticEnergy(nuKineticEnergy);
	fText->SetNuMomentumUnitVectorX(nuMomentumUnitVectorX);
	fText->SetNuMomentumUnitVectorY(nuMomentumUnitVectorY);
	fText->SetNuMomentumUnitVectorZ(nuMomentumUnitVectorZ);
	fText->SetStepID(stepID);
	fText->SetParticleName(particleName);
	fText->SetPdgEncoding(pdgEncoding);
	fText->SetTrackID(trackID);
	fText->SetParentID(parentID);
	fText->SetPreStepPositionX(preStepPositionX);
	fText->SetPreStepPositionY(preStepPositionY);
	fText->SetPreStepPositionZ(preStepPositionZ);
	fText->SetPostStepPositionX(postStepPositionX);
	fText->SetPostStepPositionY(postStepPositionY);
	fText->SetPostStepPositionZ(postStepPositionZ);
	fText->SetPreStepGlobalTime(preStepGlobalTime);
	fText->SetPostStepGlobalTime(postStepGlobalTime);
	fText->SetPreStepKineticEnergy(preStepKineticEnergy);
	fText->SetPostStepKineticEnergy(postStepKineticEnergy);
	fText->SetTotalEnergyDeposit(totalEnergyDeposit);
	fText->SetStepLength(stepLength);
	fText->SetTrackLength(trackLength);
	fText->SetCreatorProcessName(creatorProcessName);
	fText->SetProcessName(processName);
	fText->SetProcessType(processType);
	fText->SetProcessSubType(processSubType);
	fText->SetTrackStatus(trackStatus);
	fText->SetPostStepPointInDetector(postStepPointInDetector);
	fText->SetPhotonDetectedAtEndOfStep(fPhotonDetectedAtEndOfStep);
	fText->Write();
}

void MTCG4SteppingAction::DoStepOutputToRootFile(const G4Step* theStep) {
	const G4Event* theEvent = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4Track* theTrack = theStep->GetTrack();
	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
	G4ThreeVector preStepPosition = preStepPoint->GetPosition();
	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	G4ThreeVector postStepPosition = postStepPoint->GetPosition();
	G4ParticleDefinition* particleDefinition = theTrack->GetDefinition();
	G4String postStepPointPhysicalVolumeName = postStepPoint->GetPhysicalVolume()->GetName();
	G4ThreeVector nuMomentumUnitVector =
		dynamic_cast<MTCG4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoMomentumUnitVector();
	const G4VProcess *postStepProcess = postStepPoint->GetProcessDefinedStep();
	//fParticleName[0] = '\0';
	//fCreatorProcessName[0] = '\0';
	//fProcessName[0] = '\0';

	// Variables to output.
	const MTCG4RunAction *runAction = dynamic_cast<const MTCG4RunAction*>(
				G4RunManager::GetRunManager()->GetUserRunAction());
	assert(runAction);
	G4int		runID = runAction->GetRunID();
	G4int		eventID = theEvent->GetEventID();
	G4double	nuKineticEnergy =
		((MTCG4EventAction*)G4EventManager::GetEventManager() ->
		 GetUserEventAction()) -> GetNeutrinoKineticEnergyOfEventAction()/MeV;
	G4double	nuMomentumUnitVectorX = nuMomentumUnitVector.x();
	G4double	nuMomentumUnitVectorY = nuMomentumUnitVector.y();
	G4double	nuMomentumUnitVectorZ = nuMomentumUnitVector.z();
	G4int		stepID = theTrack->GetCurrentStepNumber();
	G4String	particleName =
		/*const_cast<char*>(*/particleDefinition->GetParticleName()/*.c_str())*/;
	G4int		pdgEncoding = particleDefinition->GetPDGEncoding(); 
	G4int		trackID = theTrack->GetTrackID();
	G4int		parentID = theTrack->GetParentID();
	G4double	preStepPositionX = preStepPosition.x()/mm;
	G4double	preStepPositionY = preStepPosition.y()/mm;
	G4double	preStepPositionZ = preStepPosition.z()/mm;
	G4double	postStepPositionX = postStepPosition.x()/mm;
	G4double	postStepPositionY = postStepPosition.y()/mm;
	G4double	postStepPositionZ = postStepPosition.z()/mm;
	G4double	preStepGlobalTime = preStepPoint->GetGlobalTime()/ns;
	G4double	postStepGlobalTime = postStepPoint->GetGlobalTime()/ns;
	G4double	preStepKineticEnergy = preStepPoint->GetKineticEnergy()/MeV;
	G4double	postStepKineticEnergy = postStepPoint->GetKineticEnergy()/MeV;
	G4double	totalEnergyDeposit = theStep->GetTotalEnergyDeposit()/MeV;
	G4double	stepLength = theStep->GetStepLength()/mm;
	G4double	trackLength = theTrack->GetTrackLength()/mm;
	G4String creatorProcessName;
	if (theTrack->GetParentID() > 0) {
		creatorProcessName =
			/*const_cast<char*>(*/theTrack->GetCreatorProcess()->GetProcessName()/*.c_str())*/;
	}
	else {
		//strcpy(creatorProcessName, "primaryParticle");
		creatorProcessName = "primaryParticle";
	}
	G4String processName;
	G4int processType;
	G4int processSubType;
	if (postStepProcess != NULL) {
		processName =
			/*const_cast<char*>(*/postStepProcess->GetProcessName()/*.c_str())*/;
		processType = postStepProcess->GetProcessType();
		processSubType = postStepProcess->GetProcessSubType();
	}
	else {
		//strcpy(processName, "nullProcess");
		processName = "nullProcess";
		processType = -100;
		processSubType = -100;
	}
	G4int trackStatus = theTrack->GetTrackStatus();
	G4bool postStepPointInDetector;
	// Find if the post step point ended in the scintillator volume.
	if (postStepPointPhysicalVolumeName == "scint_physical") {
		postStepPointInDetector = true;
	}
	else {
		postStepPointInDetector = false;
	}

	//// Fill TTree.
	StepRootIO* fRoot = StepRootIO::GetInstance();
	fRoot->SetRunID(runID);
	fRoot->SetEventID(eventID);
	fRoot->SetNuKineticEnergy(nuKineticEnergy);
	fRoot->SetNuMomentumUnitVectorX(nuMomentumUnitVectorX);
	fRoot->SetNuMomentumUnitVectorY(nuMomentumUnitVectorY);
	fRoot->SetNuMomentumUnitVectorZ(nuMomentumUnitVectorZ);
	fRoot->SetStepID(stepID);
	fRoot->SetParticleName(particleName);
	fRoot->SetPdgEncoding(pdgEncoding);
	fRoot->SetTrackID(trackID);
	fRoot->SetParentID(parentID);
	fRoot->SetPreStepPositionX(preStepPositionX);
	fRoot->SetPreStepPositionY(preStepPositionY);
	fRoot->SetPreStepPositionZ(preStepPositionZ);
	fRoot->SetPostStepPositionX(postStepPositionX);
	fRoot->SetPostStepPositionY(postStepPositionY);
	fRoot->SetPostStepPositionZ(postStepPositionZ);
	fRoot->SetPreStepGlobalTime(preStepGlobalTime);
	fRoot->SetPostStepGlobalTime(postStepGlobalTime);
	fRoot->SetPreStepKineticEnergy(preStepKineticEnergy);
	fRoot->SetPostStepKineticEnergy(postStepKineticEnergy);
	fRoot->SetTotalEnergyDeposit(totalEnergyDeposit);
	fRoot->SetStepLength(stepLength);
	fRoot->SetTrackLength(trackLength);
	fRoot->SetCreatorProcessName(creatorProcessName);
	fRoot->SetProcessName(processName);
	fRoot->SetProcessType(processType);
	fRoot->SetProcessSubType(processSubType);
	fRoot->SetTrackStatus(trackStatus);
	fRoot->SetPostStepPointInDetector(postStepPointInDetector);
	fRoot->SetPhotonDetectedAtEndOfStep(fPhotonDetectedAtEndOfStep);
	fRoot->Fill();
}
