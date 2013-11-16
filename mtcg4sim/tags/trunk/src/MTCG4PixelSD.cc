//////////////////////////////////////////////////////////
// MTCG4PixelSD.cc by Mich 110502
//////////////////////////////////////////////////////////

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "MTCG4PixelSD.hh"
#include "MTCG4PixelHit.hh"
#include "MTCG4DetectorConstruction.hh"
//#include "MTCG4UserTrackInformation.hh"

MTCG4PixelSD::MTCG4PixelSD(G4String name)
  :G4VSensitiveDetector(name),pixelHitCollection(0),pixelPositionsX(0)
  ,pixelPositionsY(0),pixelPositionsZ(0)
{
  collectionName.insert("pixelHitCollection");
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
MTCG4PixelSD::~MTCG4PixelSD()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelSD::Initialize(G4HCofThisEvent* HCE){
  pixelHitCollection = new MTCG4PixelHitsCollection
                      (SensitiveDetectorName,collectionName[0]); 
  // Store collection with event and keep ID.
  static G4int HCID = -1;
  if(HCID<0){ 
    HCID = GetCollectionID(0); 
  }
  HCE->AddHitsCollection( HCID, pixelHitCollection );
	nEntries = 0;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4bool MTCG4PixelSD::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}

// Generates a hit and uses the postStepPoint's mother volume replica number recursively.
// i.e. 
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photo cathode
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4bool MTCG4PixelSD::ProcessHits_constStep(const G4Step* theStep,
				       G4TouchableHistory* ){
	G4ParticleDefinition* particleDefinition = theStep->GetTrack()->GetDefinition();
	// Need to know if this is an optical photon or a gamma.
	if(particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition() && particleDefinition != G4Gamma::GammaDefinition()) return false;
	const G4Event* evt = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4int eventNum = evt->GetEventID();
//	G4int trackID = theStep->GetTrack()->GetTrackID();
	G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	G4VPhysicalVolume* preStepPointPhysicalVolume;
	G4VPhysicalVolume* postStepPointPhysicalVolume[8];
	// Initialize physical volumes to NULL.
	preStepPointPhysicalVolume = NULL;
	for(G4int depth = 0; depth < 8; depth++){
		postStepPointPhysicalVolume[depth] = NULL;
	}
	// Get mother volume of pre-step point with a depth of 0.
	// Actually there is only two layers of mother volumes for the pre-step point.
	// The world volume and the scintillator volume.
	preStepPointPhysicalVolume = preStepPoint->GetTouchable()->GetVolume(0);
	// Get mother volume of post-step point with various depths from 0 to 7
	// This is because the photo-sensitive pixel is made up of 8 layers of geometries.
	// Each layer serves as a mother volume to the daughter volume that it contains.
	for(G4int depth = 0; depth < 8; depth++){
		postStepPointPhysicalVolume[depth] = postStepPoint->GetTouchable()->GetVolume(depth);
	}
	// The translation vector of a given volume layer is added recursively so that in the end,
	// the total translation vector of a given pixel is stored in the translation vector.
	// A rotation matrix is multiplied to the translation vector for the last volume layer,
	// because this is the only layer in which I needed to include rotation as well as translation
	// to get a pixel into its proper place along the scintillator cube face.
//	G4TouchableHistory* aHist = (G4TouchableHistory*)(theStep->GetPostStepPoint()->GetTouchable());
//	G4AffineTransform aTrans = aHist->GetHistory()->GetTopTransform();
//	G4RotationMatrix pixelRotationMatrix = aTrans.NetRotation();
//	G4ThreeVector pixelPositionVector = aTrans.NetTranslation();
	G4RotationMatrix pixelRotationMatrix = *(postStepPointPhysicalVolume[7]->GetObjectRotation());
	G4ThreeVector pixelPositionVector(0., 0., 0.);
	for(G4int depth = 0; depth < 8; depth++){
		if(depth == 7) pixelPositionVector *= pixelRotationMatrix;
		pixelPositionVector += postStepPointPhysicalVolume[depth]->GetObjectTranslation();
	}

	// This is a sort of hack to get the photon hit pixel position to be along the face of the scintillator cube.
	G4ThreeVector photonHitPositionVector = pixelPositionVector;
	G4double scintillatorBufferThickness = 
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorBufferThickness();
	G4double scintHalfX =
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthX();
	G4double scintHalfY =
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthY();
	G4double scintHalfZ=
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetScintillatorHalfLengthZ();
	if(photonHitPositionVector.x() > scintHalfX + scintillatorBufferThickness)
		photonHitPositionVector.setX(scintHalfX + scintillatorBufferThickness*mm);
	else if(photonHitPositionVector.x() < -scintHalfX - scintillatorBufferThickness)
		photonHitPositionVector.setX(-scintHalfX - scintillatorBufferThickness*mm);
	else if(photonHitPositionVector.y() > scintHalfY + scintillatorBufferThickness)
		photonHitPositionVector.setY(scintHalfY + scintillatorBufferThickness*mm);
	else if(photonHitPositionVector.y() < -scintHalfY - scintillatorBufferThickness)
		photonHitPositionVector.setY(-scintHalfY - scintillatorBufferThickness*mm);
	else if(photonHitPositionVector.z() > scintHalfZ + scintillatorBufferThickness)
		photonHitPositionVector.setZ(scintHalfZ + scintillatorBufferThickness*mm);
	else if(photonHitPositionVector.z() < -scintHalfZ - scintillatorBufferThickness)
		photonHitPositionVector.setZ(-scintHalfZ - scintillatorBufferThickness*mm);
	else{
		G4cerr << "Error in processing pixel photon hit. Photon hit place is not on MTC scintillator wall!" << G4endl;
		return false;
	}

	// Get various info about post-step point.
	G4String preStepPointPhysicalVolumeName = preStepPointPhysicalVolume->GetName();
	G4String postStepPointPhysicalVolumeName0 = postStepPointPhysicalVolume[0]->GetName();
	G4String postStepPointPhysicalVolumeName1 = postStepPointPhysicalVolume[1]->GetName();
	G4String postStepPointPhysicalVolumeName2 = postStepPointPhysicalVolume[2]->GetName();
	G4String postStepPointPhysicalVolumeName3 = postStepPointPhysicalVolume[3]->GetName();
	G4String postStepPointPhysicalVolumeName4 = postStepPointPhysicalVolume[4]->GetName();
	G4String postStepPointPhysicalVolumeName5 = postStepPointPhysicalVolume[5]->GetName();
	G4String postStepPointPhysicalVolumeName6 = postStepPointPhysicalVolume[6]->GetName();
	G4String postStepPointPhysicalVolumeName7 = postStepPointPhysicalVolume[7]->GetName();
	G4double hitTime = postStepPoint->GetGlobalTime();
	G4int pixelVolumeCopyNumber = postStepPoint->GetTouchable()->GetReplicaNumber(1);
	G4int pixelVolumeRowCopyNumber = postStepPoint->GetTouchable()->GetReplicaNumber(2);
	G4int MCPCopyNumber = postStepPoint->GetTouchable()->GetReplicaNumber(5);
	G4int MCPRowCopyNumber = postStepPoint->GetTouchable()->GetReplicaNumber(6);
	G4int MCPQuadSetCopyNumber = postStepPoint->GetTouchable()->GetCopyNumber(7);

	// Output for all photons that hit and were detected by a photo-sensitive pixel.	
	std::ofstream photonHitOutput;
	photonHitOutput.open("/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/allPhotonPixelHit.dat", std::ofstream::out | std::ofstream::app);
	photonHitOutput
		//<< preStepPointPhysicalVolumeName << " "
		//<< postStepPointPhysicalVolumeName0 << " "
		<< eventNum << " "
		<< MCPQuadSetCopyNumber << " "
		<< MCPRowCopyNumber << " "
		<< MCPCopyNumber << " "
		<< pixelVolumeRowCopyNumber << " "
		<< pixelVolumeCopyNumber << " "
		//<< eventNum << " "
		//<< trackID << " "
		<< hitTime/ns << " "
		<< photonHitPositionVector.x()/mm << " "
		<< photonHitPositionVector.y()/mm << " "
		<< photonHitPositionVector.z()/mm << G4endl;
	photonHitOutput.close();
  
	// Find the correct hit collection if it exists.
  nEntries=pixelHitCollection->entries();
  MTCG4PixelHit* hit=NULL;
  for(G4int i=0;i<nEntries;i++){
	  if((*pixelHitCollection)[i]->GetMCPQuadSetCopyNumber() == MCPQuadSetCopyNumber
			  && (*pixelHitCollection)[i]->GetMCPRowCopyNumber() == MCPRowCopyNumber
			  && (*pixelHitCollection)[i]->GetMCPCopyNumber() == MCPCopyNumber
			  && (*pixelHitCollection)[i]->GetPixelRowCopyNumber() == pixelVolumeRowCopyNumber
			  && (*pixelHitCollection)[i]->GetPixelVolumeCopyNumber() == pixelVolumeCopyNumber){
		  hit=(*pixelHitCollection)[i];
		  break;
	  }
  }
  if(hit==NULL){// This pixel was never previously hit in this event.
    hit = new MTCG4PixelHit(); // So create a new hit.
    hit->SetMCPQuadSetCopyNumber(MCPQuadSetCopyNumber); // And set some initialization parameters.
    hit->SetMCPRowCopyNumber(MCPRowCopyNumber);
	hit->SetMCPCopyNumber(MCPCopyNumber);
	hit->SetPixelRowCopyNumber(pixelVolumeRowCopyNumber);
	hit->SetPixelVolumeCopyNumber(pixelVolumeCopyNumber);
	hit->SetPixelVolumePhysical(postStepPointPhysicalVolume[0]);
	hit->SetPixelRotation(pixelRotationMatrix.xx(), pixelRotationMatrix.xy(), pixelRotationMatrix.xz(), pixelRotationMatrix.yx(), pixelRotationMatrix.yy(), pixelRotationMatrix.yz(), pixelRotationMatrix.zx(), pixelRotationMatrix.zy(), pixelRotationMatrix.zz());
    hit->SetPixelPosition(pixelPositionVector.x(), pixelPositionVector.y(), pixelPositionVector.z());
//	hit->SetPixelPosition(pixelPositionVector);
	hit->SetPhotonHitPosition(photonHitPositionVector.x(), photonHitPositionVector.y(), photonHitPositionVector.z());
	hit->SetHitTime(hitTime); // Record pixel hit time of photon if the pixel was never hit before.
//    G4cout << "Initial photon hit time was initialized as " << hitTime << G4endl; // For debugging.
	hit->SetDrawit(true);
	pixelHitCollection->insert(hit);
	nEntries++;
  }
  else{
	  hit->SetHitTime(hitTime); // Record pixel hit time of photon if the hit time is earlier than any other previous hit time.
//	  G4cout << "First photon hit time was corrected as " << hitTime << G4endl; // For debugging.
  }

  ///////////////////// Left over from original code. ///////////////////
  //if(!MTCG4DetectorConstruction::GetSphereOn()){
  //  hit->SetDrawit(true);
  //  //If the sphere is disabled then this hit is automaticaly drawn
  //}
  //else{//sphere enabled
  //  MTCG4UserTrackInformation* trackInfo=
  //    (MTCG4UserTrackInformation*)theStep->GetTrack()->GetUserInformation();
  //  if(trackInfo->GetTrackStatus()&hitSphere) 
  //    //only draw this hit if the photon has hit the sphere first
  //    hit->SetDrawit(true);
  //}
  ///////////////////////////////////////////////////////////////////////

  return true;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelSD::EndOfEvent(G4HCofThisEvent* ){
	DrawAll();
	PrintFirstPhotonPixelHit();
	//PrintMCPOutputSignal();
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelSD::clear(){
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelSD::DrawAll(/*const G4int nEntries*/){
	G4double hitTimeMax, hitTimeMin;
	G4double hitTimeAverage = 0.;
	G4double hitTimeStandardDeviation = 0.;;
	for(G4int i=0; i<nEntries; i++){
		hitTimeAverage += (*pixelHitCollection)[i]->GetHitTime()/((G4double)nEntries);
//		G4cout << "hitTime: " << ((*pixelHitCollection)[i]->GetHitTime())/ns << "ns" << G4endl;
	}
//	G4cout << "hitTimeAverage: " << hitTimeAverage/ns << "ns" << G4endl;
	for(G4int i=0; i<nEntries; i++){
		hitTimeStandardDeviation += pow((*pixelHitCollection)[i]->GetHitTime() - hitTimeAverage, 2)/((G4double)nEntries);
	}
	hitTimeStandardDeviation = sqrt(hitTimeStandardDeviation);
//	G4cout << "hitTimeSD: " << hitTimeStandardDeviation << G4endl;
	for(G4int i=0; i<nEntries; i++){
		if(i == 0){
			hitTimeMin = (*pixelHitCollection)[i]->GetHitTime();
		}
		else{
			if(hitTimeMin > (*pixelHitCollection)[i]->GetHitTime()) hitTimeMin = (*pixelHitCollection)[i]->GetHitTime();
		}
	}
	if(hitTimeMin < 0.) hitTimeMin = 0.;
	hitTimeMax = hitTimeAverage + .02*hitTimeStandardDeviation;

	const G4double colorScalingFactor = 1./(hitTimeMax - hitTimeMin);
	for(G4int i=0; i<nEntries; i++){
		G4VPhysicalVolume* pixelVolumePhysical = (*pixelHitCollection)[i]->GetPixelVolumePhysical();
		if(pixelVolumePhysical){
			G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
			if(pVVisManager){
				G4double red, green, blue;
				G4double color = colorScalingFactor*((*pixelHitCollection)[i]->GetHitTime() - hitTimeMin);
//				if(color >= 0.) color = pow(color, 1./16.);
//				G4cout << "color: " << color << G4endl;
				if(color < .5){
					red = 0.;
					green = 2.*(color);
					blue = 2.*(.5 - color);
				}
				else{
					red = 2.*(color - .5);
					green = 2.*(1. - color);
					blue = 0.;
				}
				G4VisAttributes attribs(G4Colour(red, green, blue, 1.));
				attribs.SetForceSolid(true);
				G4RotationMatrix pixelRotationMatrix = (*pixelHitCollection)[i]->GetPixelRotation();
				G4ThreeVector pixelPositionVector = (*pixelHitCollection)[i]->GetPixelPosition();
				G4Transform3D trans(pixelRotationMatrix, pixelPositionVector);
				pVVisManager->Draw(*pixelVolumePhysical, attribs, trans);
			}
		}
	}
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelSD::PrintFirstPhotonPixelHit(){
	const G4Event* evt = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4int eventNum = evt->GetEventID();

	// Print out info for pixel hit times of first photon arrival.
	// Header for First Photon Detection Output Files.
	std::ofstream photonFirstHitOutput;
	photonFirstHitOutput.open("/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/firstPhotonPixelHit.dat", std::ofstream::out | std::ofstream::app);
	photonFirstHitOutput << "# Total number of recorded first photon hits for event " << eventNum << ": " << nEntries << ".\n";
	G4int MCPQuadSetCopyNumber;
	G4int MCPRowCopyNumber;
	G4int MCPCopyNumber;
	G4int pixelVolumeRowCopyNumber;
	G4int pixelVolumeCopyNumber;
	G4double hitTime;
	G4ThreeVector photonHitPositionVector;
	for(G4int i=0; i<nEntries; i++){
		MCPQuadSetCopyNumber = (*pixelHitCollection)[i]->GetMCPQuadSetCopyNumber();
		MCPRowCopyNumber = (*pixelHitCollection)[i]->GetMCPRowCopyNumber();
		MCPCopyNumber = (*pixelHitCollection)[i]->GetMCPCopyNumber();
		pixelVolumeRowCopyNumber = (*pixelHitCollection)[i]->GetPixelRowCopyNumber();
		pixelVolumeCopyNumber = (*pixelHitCollection)[i]->GetPixelVolumeCopyNumber();
		hitTime = (*pixelHitCollection)[i]->GetHitTime();
		photonHitPositionVector = (*pixelHitCollection)[i]->GetPhotonHitPosition();
		photonFirstHitOutput
			<< eventNum << " "
			<< MCPQuadSetCopyNumber << " "
			<< MCPRowCopyNumber << " "
			<< MCPCopyNumber << " "
			<< pixelVolumeRowCopyNumber << " "
			<< pixelVolumeCopyNumber << " "
			<< hitTime/ns << " "
			<< photonHitPositionVector.x()/mm << " "
			<< photonHitPositionVector.y()/mm << " "
			<< photonHitPositionVector.z()/mm << " "
			<< G4endl;
	}
	photonFirstHitOutput.close();
}

void MTCG4PixelSD::PrintMCPOutputSignal(){
	const G4Event* evt = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
	G4int eventNum = evt->GetEventID();
	// MCP Pixel Signal Output //
	// Header for Signal Output of MCP Pixels.
	G4double hitTimeMax;
	G4double hitTimeMin;
	for(G4int i = 0; i < nEntries ; i++){
		if(i == 0 || hitTimeMax < (*pixelHitCollection)[i]->GetHitTime()) hitTimeMax = (*pixelHitCollection)[i]->GetHitTime();
		if(i == 0 || hitTimeMin > (*pixelHitCollection)[i]->GetHitTime()) hitTimeMin = (*pixelHitCollection)[i]->GetHitTime();
	}
	
	std::ofstream MCPSignalOutput;
	G4String MCPOutputSignalOutputPathTemplate = "/mnt/datadisk/school/research/MTC/MTCG4Sim/output/now/%dMCPOutputSignal.dat";
	char MCPOutputSignalOutputPathBuffer[100];
	sprintf(MCPOutputSignalOutputPathBuffer, MCPOutputSignalOutputPathTemplate.data(), eventNum);
	G4String* MCPOutputSignalOutputPath = new G4String(MCPOutputSignalOutputPathBuffer);
	MCPSignalOutput.open(MCPOutputSignalOutputPath->data(), std::ofstream::out | std::ofstream::app);
	MCPSignalOutput << "# An MCP pixel ID number in this file is given as a string of 5 digits such as XXXXX.\n";
	MCPSignalOutput << "# The 1st digit ranges from 0 through 5. It signifies the face of the scintillator cube that the MCP is attached to when the center of the scintillating cube is placed at the origin x = y = z = 0. 0 = +x face, 1 = -x face, 2 = +y face, 3 = -y face, 4 = +z face, 5 = -z face.\n";
	MCPSignalOutput << "# The 2nd digit ranges from 0 through 1. It represents one of the two rows of MCP's on a single side of the scintillating cube. The 2 X 2 MCP's on one cube face are arranged in matrix form when viewed from inside the scintillating cube. Think of the +z direction as the top of this matrix for the faces 0, 1, 2, 3. Think of the +x direction to be the top of the matrix for face 4. The -x direction is the top of the matrix for face 5. 0 = 1st row, 1 = 2nd row.\n";
	MCPSignalOutput << "# The 3rd digit ranges from 0 through 1. It specifies one of the two columns of MCP's on a single side of the scintillating cube. 0 = 1st column, 1 = 2nd column.\n";
	MCPSignalOutput << "# The 4th digit ranges from 0 through 7. It specifies one of the eight rows of pixels on a single MCP module face. When looking from inside of the scintillating cube. i.e. looking into the active pixel area of an MCP module. 0 = 1st row, 1 = 2nd row, . . . , 7 = 8th row.\n";
	MCPSignalOutput << "# The 5th digit ranges from 0 through 7. It specifies one of the eight columns of pixels on a single MCP module face. 0 = 1st column, 1 = 2nd column, . . . , 7 = 8th column.\n";
	MCPSignalOutput << "# The output below is in the format of global time in nanoseconds after the neutrino interaction in the scintillating cube. And current generated by each pixel readout in milliamperes.\n";
	// Header
	G4String header = "# time(ns)";
	for(G4int a = 0; a < 6; a++){
		for(G4int b = 0; b < 2; b++){
			for(G4int c = 0; c < 2; c++){
				for(G4int d = 0; d < 8; d++){
					for(G4int e = 0; e < 8; e++){
						char pixelID[5];
						G4String pixelIDTemplate = "%d%d%d%d%d";
						sprintf(pixelID, pixelIDTemplate.data(), a, b, c, d, e);
						header += " ";
						header += pixelID;
					}
				}
			}
		}
	}
	MCPSignalOutput << header << G4endl;
	// Output
	const G4double deltaTime = .1*ns;
	const G4double pulseFWHM = 1.8*ns;
	const G4double pixelGain = 1.*pow(10,6);
	const G4double electronCharge = e_SI*coulomb;
	const G4double peakCurrent = pixelGain*electronCharge/pulseFWHM;
	const G4double offset = .5*pulseFWHM; // Pulse offset is half of FWHM.
	const G4double width = pulseFWHM/(2.*sqrt(2.*log(2.))); // Gaussian pulse shape.
	for(G4double time = 0.*ns; time < hitTimeMax+2.*pulseFWHM; time+=deltaTime){
		MCPSignalOutput << time;
		for(G4int a = 0; a < 6; a++){
			for(G4int b = 0; b < 2; b++){
				for(G4int c = 0; c < 2; c++){
					for(G4int d = 0; d < 8; d++){
						for(G4int e = 0; e < 8; e++){
							G4double currentTotal;
							G4double currentBase = 0.*nanoampere;
							G4double currentSignal = 0.*milliampere;;
							for(G4int i = 0; i < nEntries; i++){
								if(a == (*pixelHitCollection)[i]->GetMCPQuadSetCopyNumber() && b == (*pixelHitCollection)[i]->GetMCPRowCopyNumber() && c == (*pixelHitCollection)[i]->GetMCPCopyNumber() && d == (*pixelHitCollection)[i]->GetPixelRowCopyNumber() && e == (*pixelHitCollection)[i]->GetPixelVolumeCopyNumber() && (time - pulseFWHM <= (*pixelHitCollection)[i]->GetHitTime() && (*pixelHitCollection)[i]->GetHitTime() < time)){
									G4double pulseTime = time - ((*pixelHitCollection)[i]->GetHitTime());
									currentSignal = peakCurrent*exp(-pow(pulseTime - offset, 2)/(2.*pow(width, 2)));
									break;
								}
							}
							currentTotal = currentSignal + currentBase;
							MCPSignalOutput << " ";
							MCPSignalOutput << currentTotal/milliampere;
							//G4cout << "currentTotal = " << currentTotal/milliampere << G4endl;
							//G4cout << "j = " << j << G4endl;
						}
					}
				}
			}
		}
		MCPSignalOutput << G4endl;
	}
	MCPSignalOutput.close();
}
