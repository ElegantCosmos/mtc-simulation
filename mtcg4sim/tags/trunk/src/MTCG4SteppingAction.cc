///////////////////////////////////////
// MTCG4SteppingAction.cc by Mich 110502
///////////////////////////////////////

#include "MTCG4PixelSD.hh"
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

//
// Define a constant quantum efficiency (0. ~ 1.) to be used by photo sensitive
// pixels when optical surface properties are not used.
//
#define CONSTANT_QUANTUM_EFFICIENCY 1.

MTCG4SteppingAction::MTCG4SteppingAction()
{
	// Flag to limit output data amount for
	// testing purposes.
	onlyFirst30EventsAreOutputted = false;
	headerFlag = false;
	boundaryStatusName = "default";
	isBetweenScintillatorAndSensitivePixel = "default";
	steppingMessenger = new MTCG4SteppingMessenger(this);
}

MTCG4SteppingAction::~MTCG4SteppingAction()
{
	delete steppingMessenger;
}

// Method Called During a Step //
void MTCG4SteppingAction::UserSteppingAction(const G4Step* theStep)
{
	//const G4Event* theEvent = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	//G4Track* theTrack = theStep->GetTrack();
	//G4ParticleDefinition* particleDefinition = theTrack->GetDefinition();
	G4VPhysicalVolume* postStepPointPhysicalVolume = theStep->GetPostStepPoint()->GetPhysicalVolume();
	if(!postStepPointPhysicalVolume){
		// If the post-step point is not available,
		// the particle is out of the world volume.
		// Do not do anything with it and just return.
		//G4cout << particleDefinition->GetParticleName() << " post step point is out of world!\n";
		return;
	}
	
	if(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
				GetUserDetectorConstruction())->GetMCPPlacement() == true){
		// Optical photon and gamma detection process at sensitive pixel.
		DetectPhotonOfSteppingAction(theStep, boundaryStatusName, isBetweenScintillatorAndSensitivePixel);
	}

	if(onlyFirst30EventsAreOutputted){
		G4int eventNumber =
			G4EventManager::GetEventManager()->
			GetConstCurrentEvent()->GetEventID();
		if(eventNumber < 30) // For large runs, we don't want too much data.
			OutputAllParticleOfSteppingAction(theStep, boundaryStatusName, isBetweenScintillatorAndSensitivePixel);
	}
	else{ // Do output for all events.
		OutputAllParticleOfSteppingAction(theStep, boundaryStatusName, isBetweenScintillatorAndSensitivePixel);
	}
}

// Optical Photon and Gamma Detection Process at Sensitive Pixel //
//
void MTCG4SteppingAction::DetectPhotonOfSteppingAction(const G4Step* theStep, G4String& boundaryStatusName, G4String& isBetweenScintillatorAndSensitivePixel){
	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
	G4VPhysicalVolume* preStepPointPhysicalVolume = preStepPoint->GetPhysicalVolume();
	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	G4VPhysicalVolume* postStepPointPhysicalVolume = postStepPoint->GetPhysicalVolume();
	G4ParticleDefinition* particleDefinition = theStep->GetTrack()->GetDefinition();
	if(particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition() || particleDefinition == G4Gamma::GammaDefinition()){ // If the particle is an optical photon or a gamma, we want it to go through the detection process at the surface of the sensitive pixel geometry.
		G4String preStepPointPhysicalVolumeName = preStepPointPhysicalVolume->GetName(); // Get the names of the volumes.
		G4String postStepPointPhysicalVolumeName = postStepPointPhysicalVolume->GetName();
		if(postStepPoint->GetStepStatus()==fGeomBoundary && (preStepPointPhysicalVolumeName == "scintillator_physical" || preStepPointPhysicalVolumeName == "world_physical") && postStepPointPhysicalVolumeName == "pixel_physical"){ // If the particle is at a boundary between two geometries, the pre-step volume is the scintillator or the world, and the post-step volume is the pixel.
			G4String particleName = particleDefinition->GetParticleName();
			//G4cout << particleName << " is at the boundary between scintillator and pixel.\n";
			isBetweenScintillatorAndSensitivePixel = "betweenScintillatorAndPixel";
			if(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetOpticalSurfaceProperties()
					== true){
				G4OpBoundaryProcessStatus boundaryStatus = Undefined;
				G4OpBoundaryProcess* boundary = NULL;
				if(!boundary){
					G4ProcessManager* pm 
						= theStep->GetTrack()->GetDefinition()->GetProcessManager();
					G4int nprocesses = pm->GetProcessListLength();
					G4ProcessVector* pv = pm->GetProcessList(); // Get a pointer to the list of processes available for this particle.
					//G4cout << "Process Manager instantiated.\n"; // Of debugging.
					G4int i;
					for(i = 0; i < nprocesses; i++){
						if((*pv)[i]->GetProcessName() == "OpBoundary"){ // If the optical boundary process is found in the list of available processes, set it to boundaryStatus.
							//G4cout << "boundary process found!\n";
							boundary = (G4OpBoundaryProcess*)(*pv)[i];
							boundaryStatus = boundary->GetStatus();
							//G4cout << "boundary status set\n"; 
							break;
						}
					}
				}
				switch(boundaryStatus){ // Depending on the status of the particle at the geometrical boundary, the particle will be handled differently.
					case Absorption:
						//G4cout << particleName << " is absorbed by sensitive pixel!\n";
						boundaryStatusName = "absorbedByPixel";
						break;
					case Detection: // Note, this assumes that the volume causing detection is the photocathode because it is the only one with non-zero efficiency.
						// Triger sensitive detector manually since photon is absorbed but status was "Detection".
						{
							//G4cout << particleName << " is detected by sensitive pixel!\n";
							boundaryStatusName = "detectedByPixel";
							//G4cout << "This is a test to see if QE is working.\n";
							G4SDManager* SDman = G4SDManager::GetSDMpointer();
							G4String sdName = "/MTCG4Pixel/MTCG4PixelSD";
							MTCG4PixelSD* pmtSD = (MTCG4PixelSD*)SDman
								->FindSensitiveDetector(sdName);
							if(pmtSD)
								pmtSD->ProcessHits_constStep(theStep, NULL);
							//			MTCG4PixelSD* pmtSD = new MTCG4PixelSD(sdName);
							//			SDman->AddNewDetector(pmtSD);
							//			pmtLogical
							//MTCG4UserTrackInformation->AddTrackStatusFlag(hitPMT);
						}
						break;
					case FresnelReflection:
						//G4cout << "FresnelReflection!" << G4endl;
						boundaryStatusName = "FresnelReflection";
						break;
					case TotalInternalReflection:
						//G4cout << "TotalInternalReflection!" << G4endl;
						boundaryStatusName = "TotalInternalReflection";
						break;
					case SpikeReflection: // What is this?
						//G4cout << "SpikeReflection!" << G4endl;
						boundaryStatusName = "SpikeReflection";				
						break;
					case NoRINDEX: // This seems to occur as an error message when I did the optical surface method incorrectly in MTCG4DetectorConstruction.cc.
						//G4cout << "NoRINDEX!" << G4endl;
						boundaryStatusName = "NoRINDEX!";
						break;
					default:
						//G4cout << "default" << G4endl;
						boundaryStatusName = "default";
						break;
				}
			}
			else{
				G4Track* theTrack = theStep->GetTrack();
				// Manual Photon Detection Process Not Using Quantum Efficiency Feature Provided By Geant //
				//G4double photonEnergy = postStepPoint->GetTotalEnergy();
				const G4double quantumEfficiency = CONSTANT_QUANTUM_EFFICIENCY;
				G4double randomFlat = G4UniformRand();
				if(randomFlat > quantumEfficiency){
					//G4cout << particleName << " is not detected by sensitive pixel due to quantum efficiency.\n";
					theTrack->SetTrackStatus(fStopAndKill);
					//G4cout << "Killed " << particleName << ".\n";
					return;
				}
				//G4cout << particleName << " is detected by sensitive pixel!\n";
				G4SDManager* SDman = G4SDManager::GetSDMpointer();
				G4String sdName="/MTCG4Pixel/MTCG4PixelSD";
				MTCG4PixelSD* pmtSD = (MTCG4PixelSD*)SDman
					->FindSensitiveDetector(sdName);
				if(pmtSD)
					pmtSD->ProcessHits_constStep(theStep, NULL);
				theTrack->SetTrackStatus(fStopAndKill);
				//G4cout << "Killed " << particleName << ".\n";
				//			MTCG4PixelSD* pmtSD = new MTCG4PixelSD(sdName);
				//			SDman->AddNewDetector(pmtSD);
				//			pmtLogical                                                                                                     		
				//MTCG4UserTrackInformation->AddTrackStatusFlag(hitPMT);                                                           		
			}
		}
	}
}

//
// Do output for all events.
//
void MTCG4SteppingAction::OutputAllParticleOfSteppingAction(const G4Step* theStep, G4String boundaryStatusName, G4String isBetweenScintillatorAndSensitivePixel){
	const G4Event* theEvent = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4Track* theTrack = theStep->GetTrack();
	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	G4int eventNumber = theEvent->GetEventID();
	G4int stepNumber = theTrack->GetCurrentStepNumber();
	G4ParticleDefinition* particleDefinition = theTrack->GetDefinition();
	G4String postStepPointPhysicalVolumeName = postStepPoint->GetPhysicalVolume()->GetName();
	G4int particlePostStepPointInScintillator;
	if(postStepPointPhysicalVolumeName == "scintillator_physical") particlePostStepPointInScintillator = 1;
	else particlePostStepPointInScintillator = 0;
	// File names for the primary particles. I put in PrimaryOtherParticle.dat as a precaution but it really serves no purpose.
	//	G4String primaryPositronOutputPath = "/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/PrimaryPositron.dat",
	//			 primaryNeutronOutputPath = "/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/PrimaryNeutron.dat",
	//			 primaryOtherParticleOutputPath = "/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/PrimaryOtherParticle.dat";
	//	if(theTrack->GetParentID() == 0) 
	//	{
	//		if(particleDefinition == G4Neutron::NeutronDefinition() )
	//			currentFileTemplate = primaryNeutronOutputPath;
	//		else if(particleDefinition == G4Positron::PositronDefinition() )
	//			currentFileTemplate = primaryPositronOutputPath;
	//		else
	//			currentFileTemplate = primaryOtherParticleOutputPath;
	//	}
	std::ofstream stepOutput;
	//	char fname[100];
	//	sprintf(fname, currentFileTemplate.data(), eventNumber);
	//	G4String* fileNameString = new G4String(fname);
	// Do main output for particle tracking.
	//	stepOutput.open(fileNameString->data(), std::ofstream::out | std::ofstream::app);
	//	G4String dopingAgentSettingString, enrichmentSettingString, primaryParticleSettingString;
	//#if READ_IN_PRIMARY_PARTICLE_DATA_FLAG
	//	G4String initialKineticEnergyString = "Reactor";
	//#else
	//	G4String initialKineticEnergyString;
	//	G4double initialKineticEnergy = PRIMARY_PARTICLE_KINETIC_ENERGY*eV;
	//	if(initialKineticEnergy == 1000.*eV) initialKineticEnergyString = "1000eV";
	//	else if(initialKineticEnergy ==10000.*eV) initialKineticEnergyString = "10000eV";
	//	else if(initialKineticEnergy == 100000.*eV) initialKineticEnergyString = "100000eV";
	//	else initialKineticEnergyString = "Error";
	//#endif
	//
	//#if (SHOOT_PRIMARY_POSITRON_FLAG && SHOOT_PRIMARY_NEUTRON_FLAG)
	//	G4String primaryParticleSettingString = "PositronAndNeutron";
	//#elif (SHOOT_PRIMARY_POSITRON_FLAG && (!SHOOT_PRIMARY_NEUTRON_FLAG))
	//	G4String primaryParticleSettingString = "Positron";
	//#elif (!(SHOOT_PRIMARY_POSITRON_FLAG) && SHOOT_PRIMARY_NEUTRON_FLAG)
	//	G4String primaryParticleSettingString = "Neutron";
	//#else
	//	G4String primaryParticleSettingString = "MiscParticle";
	//#endif
	//
	//#if BORON_DOPANT_FLAG
	//	G4String dopingAgentSettingString = "Boron";
	//#if BORON_ENRICHMENT_FLAG
	//	G4String enrichmentSettingString = "Enriched";
	//#else
	//	G4String enrichmentSettingString = "Natural";
	//#endif
	//
	//#else
	//	G4String dopingAgentSettingString = "Lithium";
	//#if LITHIUM_ENRICHMENT_FLAG
	//	G4String enrichmentSettingString = "Enriched";
	//#else
	//	G4String enrichmentSettingString = "Natural";
	//#endif
	//#endif
	//
	//	G4String dopingFractionString;
	//	G4double dopingFraction = DOPING_FRACTION;
	//	if(dopingFraction == 0.010) dopingFractionString = "1Percent";
	//	else if(dopingFraction == 0.025) dopingFractionString = "2.5Percent";
	//	else if(dopingFraction == 0.050) dopingFractionString = "5Percent";
	//	else dopingFractionString = "Error";

	std::ostringstream fileNameStream;
	//	fileNameStream << "/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/MTCG4SimOutput_" << initialKineticEnergyString << primaryParticleSettingString << "In" << enrichmentSettingString << dopingAgentSettingString << "DopedAt" << dopingFractionString << ".dat";
	fileNameStream <<
		"/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/MTCG4SimOutput.dat";
	G4String* fileNameString = new G4String(fileNameStream.str());
	stepOutput.open(fileNameString->data(), std::ofstream::out | std::ofstream::app);
	stepOutput.setf(std::ios::fixed, std::ios::floatfield);
	stepOutput.precision(7);
	G4int s=5;
	G4int m=10;
	G4int l=15;
	G4String allParticleOutputHeaderString = "# ";
	allParticleOutputHeaderString += "EventNumber ";
	allParticleOutputHeaderString += "NeutrinoKineticEnergy(MeV) ";
	allParticleOutputHeaderString += "NeutrinoMomentumUnitVectorX ";
	allParticleOutputHeaderString += "NeutrinoMomentumUnitVectorY ";
	allParticleOutputHeaderString += "NeutrinoMomentumUnitVectorZ ";
	allParticleOutputHeaderString += "StepNumber ";
	allParticleOutputHeaderString += "ParticleName ";
	allParticleOutputHeaderString += "PDGEncoding ";
	allParticleOutputHeaderString += "TrackID ";
	allParticleOutputHeaderString += "ParentID ";
	allParticleOutputHeaderString += "PreStepPositionX(mm) ";
	allParticleOutputHeaderString += "PreStepPositionY(mm) ";
	allParticleOutputHeaderString += "PreStepPositionZ(mm) ";
	allParticleOutputHeaderString += "PostStepPositionX(mm) ";
	allParticleOutputHeaderString += "PostStepPositionY(mm) ";
	allParticleOutputHeaderString += "PostStepPositionZ(mm) ";
	allParticleOutputHeaderString += "PreStepGlobalTime(ns) ";
	allParticleOutputHeaderString += "PostStepGlobalTime(ns) ";
	allParticleOutputHeaderString += "PreStepKineticEnergy(MeV) ";
	allParticleOutputHeaderString += "PostStepKineticEnergy(MeV) ";
	allParticleOutputHeaderString += "TotalEnergyDeposit(MeV) ";
	allParticleOutputHeaderString += "StepLength(mm) ";
	allParticleOutputHeaderString += "TrackLength(mm) ";
	allParticleOutputHeaderString += "ProcessName ";
	allParticleOutputHeaderString += "ProcessType ";
	allParticleOutputHeaderString += "ProcessSubType ";
	allParticleOutputHeaderString += "TrackStatus ";
	allParticleOutputHeaderString += "postStepPointInDetector ";
	if(eventNumber == 0 && theTrack->GetParentID() == 0 && stepNumber == 1 && headerFlag != true){
		stepOutput << allParticleOutputHeaderString << G4endl;
		headerFlag = true;
	}
	stepOutput << std::setw(m) << std::left << eventNumber << " ";
	stepOutput << std::setw(m) << ((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoKineticEnergyOfEventAction()/MeV << " ";
	stepOutput << std::setw(m) << ((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoMomentumUnitVector().x() << " ";
	stepOutput << std::setw(m) << ((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoMomentumUnitVector().y() << " ";
	stepOutput << std::setw(m) << ((MTCG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoMomentumUnitVector().z() << " ";
	stepOutput << std::setw(s) << stepNumber << " ";
	stepOutput << std::setw(l) << particleDefinition->GetParticleName() << " ";
	stepOutput << std::setw(m) << particleDefinition->GetPDGEncoding() << " ";
	stepOutput << std::setw(s) << std::right << theTrack->GetTrackID() << " ";
	stepOutput << std::setw(s) <<	theTrack->GetParentID() << " ";
	stepOutput << std::setw(m) <<	preStepPoint->GetPosition().x()/mm << " ";
	stepOutput << std::setw(m) <<	preStepPoint->GetPosition().y()/mm << " ";
	stepOutput << std::setw(m) <<	preStepPoint->GetPosition().z()/mm << " ";
	stepOutput << std::setw(m) <<	postStepPoint->GetPosition().x()/mm << " ";
	stepOutput << std::setw(m) <<	postStepPoint->GetPosition().y()/mm << " ";
	stepOutput << std::setw(m) <<	postStepPoint->GetPosition().z()/mm << " ";
	stepOutput << std::setw(m) <<	preStepPoint->GetGlobalTime()/ns << " ";
	stepOutput << std::setw(m) <<	postStepPoint->GetGlobalTime()/ns << " ";
	stepOutput << std::setw(l) <<	preStepPoint->GetKineticEnergy()/MeV << " ";
	stepOutput << std::setw(l) <<	postStepPoint->GetKineticEnergy()/MeV << " ";
	stepOutput << std::setw(l) <<	theStep->GetTotalEnergyDeposit()/MeV << " ";
	stepOutput << std::setw(l) <<	theStep->GetStepLength()/mm << " ";
	stepOutput << std::setw(l) <<	theTrack->GetTrackLength()/mm << " ";
	stepOutput << std::setw(l) << std::left <<	postStepPoint->GetProcessDefinedStep()->GetProcessName() << " ";
	stepOutput << postStepPoint->GetProcessDefinedStep()->GetProcessType() << " ";
	stepOutput << std::setw(s) << postStepPoint->GetProcessDefinedStep()->GetProcessSubType() << " ";
	stepOutput << std::setw(s) << std::right << theTrack->GetTrackStatus() << " ";
//#if MCP_FLAG
  if(((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetMCPPlacement()
== true){
	stepOutput << std::setw(s) << std::right << boundaryStatusName << " "; // This was placed here only for testing purposes.
	stepOutput << std::setw(s) << std::right << isBetweenScintillatorAndSensitivePixel << " ";
	}
//#endif
	stepOutput << particlePostStepPointInScintillator << " ";
	stepOutput << G4endl;
	stepOutput.close();

//
// For testing.
//
//	G4cout << "doping_fraction = " <<
//		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetDopingFraction() << G4endl;
//
}
