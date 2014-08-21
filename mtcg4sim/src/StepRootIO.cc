//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file persistency/P01/src/StepRootIO.cc
/// \brief Implementation of the StepRootIO class
//

#include <sys/stat.h>

#include <cassert>
#include <sstream>

#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4SteppingAction.hh"
#include "StepRootIO.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

static StepRootIO* StepRootIOManager = NULL;

TFile *StepRootIO::fRootFile = NULL;
TTree *StepRootIO::fStepTree = NULL;
G4String StepRootIO::fFileName = "";

StepRootIO::StepRootIO() :
	fRunID(-1),
	fEventID(-1),
	fNuKineticEnergy(-1),
	fNuMomUnitVectorX(0),
	fNuMomUnitVectorY(0),
	fNuMomUnitVectorZ(0),
	fStepID(-1),
	fPdgEncoding(0),
	fTrackID(-1),
	fParentID(-1),
	fPostStepPosX(0),
	fPostStepPosY(0),
	fPostStepPosZ(0),
	fPostStepMomX(0),
	fPostStepMomY(0),
	fPostStepMomZ(0),
	fPostStepGlobalTime(-1),
	fPostStepKineticEnergy(-1),
	fTotalEnergyDeposit(-1),
	fStepLength(-1),
	fTrackLength(-1),
	fProcessType(-1),
	fProcessSubType(-1),
	fTrackStatus(-1),
	fPhotonDetectedAtEndOfStep(false)
{
	fHe8WasFound = false;
	fLi9WasFound = false;

	std::cout << "unlink(" << fFileName.c_str() << ")" << std::endl;
	unlink(fFileName.c_str()); // Remove old ROOT file.

	// Create TFile object.
	fRootFile = new TFile(fFileName.c_str(), "RECREATE");
	assert(fRootFile); // Make sure ROOT file exits.

	// Create TTree object.
	fStepTree = new TTree("stepTree", "stepTree");
	fStepTree->SetDirectory(fRootFile);
	SetTreeBranches();
}

void StepRootIO::CreateInstance(G4String fileName)
{
	fFileName = fileName;
	StepRootIOManager = new StepRootIO();
}

void StepRootIO::ResetInstance()
{
	//if (fStepTree) {
	//	fStepTree->SetDirectory(0); // Make memory-resident.
	//	delete fStepTree; // Is memory-resident so needs to be deleted.
	//	fStepTree = NULL;
	//}
	if (fRootFile) {
		if (fRootFile->IsOpen()) fRootFile->Close();
		delete fRootFile;
		fRootFile = NULL;
	}
	delete StepRootIOManager;
	StepRootIOManager = NULL;
}

StepRootIO* StepRootIO::GetInstance()
{
	if (!StepRootIOManager)
	{
		StepRootIOManager = new StepRootIO();
	}
	return StepRootIOManager;
}

void StepRootIO::SetTreeBranches()
{
	//fStepTree->SetDirectory(0); // Make tree memory-resident.

	// Create branches.
	fStepTree->Branch("runID", &fRunID, "runID/I");
	fStepTree->Branch("eventID", &fEventID, "eventID/I");
	fStepTree->Branch("nuKineticEnergy", &fNuKineticEnergy,
			"nuKineticEnergy/D");
	fStepTree->Branch("nuMomentumUnitVectorX",
			&fNuMomUnitVectorX, "nuMomentumUnitVectorX/D");
	fStepTree->Branch("nuMomentumUnitVectorY",
			&fNuMomUnitVectorY, "nuMomentumUnitVectorY/D");
	fStepTree->Branch("nuMomentumUnitVectorZ",
			&fNuMomUnitVectorZ, "nuMomentumUnitVectorZ/D");
	fStepTree->Branch("stepID", &fStepID, "stepID/I");
	fStepTree->Branch("particleName", fParticleName, "particleName[256]/C");
	fStepTree->Branch("pdgEncoding", &fPdgEncoding, "pdgEncoding/I");
	fStepTree->Branch("trackID", &fTrackID, "trackID/I");
	fStepTree->Branch("parentID", &fParentID, "parentID/I");
	fStepTree->Branch("postStepPosX", &fPostStepPosX, "postStepPosX/D");
	fStepTree->Branch("postStepPosY", &fPostStepPosY, "postStepPosY/D");
	fStepTree->Branch("postStepPosZ", &fPostStepPosZ, "postStepPosZ/D");
	fStepTree->Branch("postStepMomX", &fPostStepMomX, "postStepMomX/D");
	fStepTree->Branch("postStepMomY", &fPostStepMomY, "postStepMomY/D");
	fStepTree->Branch("postStepMomZ", &fPostStepMomZ, "postStepMomZ/D");
	fStepTree->Branch("postStepGlobalTime", &fPostStepGlobalTime,
			"postStepGlobalTime/D");
	fStepTree->Branch("postStepKineticEnergy", &fPostStepKineticEnergy,
			"postStepKineticEnergy/D");
	fStepTree->Branch("totalEnergyDeposit", &fTotalEnergyDeposit,
			"totalEnergyDeposit/D");
	fStepTree->Branch("stepLength", &fStepLength, "stepLength/D");
	fStepTree->Branch("trackLength", &fTrackLength, "trackLength/D");
	fStepTree->Branch("processName", fProcessName, "processName[256]/C");
	fStepTree->Branch("processType", &fProcessType, "processType/I");
	fStepTree->Branch("processSubType", &fProcessSubType, "processSubType/I");
	fStepTree->Branch("trackStatus", &fTrackStatus, "trackStatus/I");
	fStepTree->Branch("postStepPhysVolumeName", fPostStepPhysVolumeName,
			"postStepPhysVolumeName[256]/C");
	fStepTree->Branch("photonDetectedAtEndOfStep", &fPhotonDetectedAtEndOfStep,
			"photonDetectedAtEndOfStep/O");
}

void StepRootIO::Fill(const G4Track *theTrack, const G4Step *theStep)
{
	//const G4Event *theEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
	////G4ThreeVector postStepPos;
	////G4ThreeVector postStepMom;
	////if (theStep) {// If step exists, tracking is underway. Get info from step.
	////	G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	////	postStepPos = postStepPoint->GetPosition();
	////	postStepMom = postStepPoint->GetMomentum();
	////	fPostStepGlobalTime = postStepPoint->GetGlobalTime()/ns;
	////	fPostStepKineticEnergy = postStepPoint->GetKineticEnergy()/MeV;
	////	strcpy(fPostStepPhysVolumeName,
	////			postStepPoint->GetPhysicalVolume()->GetName());
	////	const G4VProcess *postStepProcess =
	////	   	postStepPoint->GetProcessDefinedStep();
	////	if (postStepProcess != NULL) {
	////		strcpy(fProcessName, postStepProcess->GetProcessName());
	////		fProcessType = postStepProcess->GetProcessType();
	////		fProcessSubType = postStepProcess->GetProcessSubType();
	////	}
	////	else {
	////		strcpy(fProcessName, "nullProcess");
	////		fProcessType = -100;
	////		fProcessSubType = -100;
	////	}
	////	fTotalEnergyDeposit = theStep->GetTotalEnergyDeposit()/MeV;
	////	fStepLength = theStep->GetStepLength()/mm;
	////}
	////else { // If step doesn't exist, particle is not yet tracked, ie 0th step.
	////	postStepPos = theTrack->GetPosition(); // Use track position info.
	////	postStepMom = theTrack->GetMomentum();
	////	fPostStepGlobalTime = theTrack->GetGlobalTime()/ns;
	////	fPostStepKineticEnergy = theTrack->GetKineticEnergy()/MeV;
	////	strcpy(fPostStepPhysVolumeName, theTrack->GetVolume()->GetName());
	////	const G4VProcess *postStepProcess = theTrack->GetCreatorProcess();
	////	if (postStepProcess != NULL) {
	////		strcpy(fProcessName, postStepProcess->GetProcessName());
	////		fProcessType = postStepProcess->GetProcessType();
	////		fProcessSubType = postStepProcess->GetProcessSubType();
	////	}
	////	else {//If step nor creator process exists, it is 0th step of primary.
	////		strcpy(fProcessName, "primaryParticle");
	////		fProcessType = -100;
	////		fProcessSubType = -100;
	////	}
	////	fTotalEnergyDeposit = 0;
	////	fStepLength = theTrack->GetStepLength()/mm;
	////}

	//// Added as test to see if we can extract step info from G4Track object.
	//// Step info from G4Track object gets post-step info except for GetVolume()
	//// which gets the pre-step point volume. Lets get the post-step volume name
	//// from the G4Step object if it exists. If it doesn't exist, this means that
	//// the post-step volume is out of the world volume.
	//G4ThreeVector postStepPos = theTrack->GetPosition(); // Use track position info.
	//G4ThreeVector postStepMom = theTrack->GetMomentum();
	//fPostStepGlobalTime = theTrack->GetGlobalTime()/ns;
	//fPostStepKineticEnergy = theTrack->GetKineticEnergy()/MeV;
	//G4VPhysicalVolume* nextVolume = theTrack->GetNextVolume();
	//if (nextVolume) {
	//	strcpy(fPostStepPhysVolumeName, nextVolume->GetName());
	//}
	//else {
	//	strcpy(fPostStepPhysVolumeName, "outOfWorld");
	//}
	//if (!theStep) { // If step doesn't exist, it is still 0th step.
	//	const G4VProcess *creatorProcess = theTrack->GetCreatorProcess();
	//	if (creatorProcess != NULL) {
	//		strcpy(fProcessName, creatorProcess->GetProcessName());
	//		fProcessType = creatorProcess->GetProcessType();
	//		fProcessSubType = creatorProcess->GetProcessSubType();
	//	}
	//	else {//If step nor creator process exists, it is 0th step of primary.
	//		strcpy(fProcessName, "primaryParticle");
	//		fProcessType = -100;
	//		fProcessSubType = -100;
	//	}
	//	fTotalEnergyDeposit = 0;
	//}
	//else { // G4Step object exists. Tracking is underway. stepID > 0.
	//	const G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
	//	assert(postStepPoint);
	//	const G4VProcess *postStepProcess = postStepPoint->GetProcessDefinedStep();
	//	assert(postStepProcess);
	//	strcpy(fProcessName, postStepProcess->GetProcessName());
	//	fProcessType = postStepProcess->GetProcessType();
	//	fProcessSubType = postStepProcess->GetProcessSubType();
	//	fTotalEnergyDeposit = theStep->GetTotalEnergyDeposit()/MeV;
	//}
	//fStepLength = theTrack->GetStepLength()/mm;
	//// End of test. 20140429 -- Mich.

	//G4ParticleDefinition *particleDef = theTrack->GetDefinition();
	//G4ThreeVector nuMomUnitVector =
	//	dynamic_cast<MTCG4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction())->GetNeutrinoMomentumUnitVector();

	//// Variables to output.
	//const MTCG4RunAction *runAction = dynamic_cast<const MTCG4RunAction*>(
	//			G4RunManager::GetRunManager()->GetUserRunAction());
	//assert(runAction);
	//fRunID = runAction->GetRunID();
	//fEventID = theEvent->GetEventID();
	//fNuKineticEnergy = (
	//		(MTCG4EventAction*)
	//		G4EventManager::GetEventManager()->GetUserEventAction()
	//		)->GetNeutrinoKineticEnergyOfEventAction()/MeV;
	//fNuMomUnitVectorX = nuMomUnitVector.x();
	//fNuMomUnitVectorY = nuMomUnitVector.y();
	//fNuMomUnitVectorZ = nuMomUnitVector.z();
	//fStepID = theTrack->GetCurrentStepNumber();
	//strcpy(fParticleName, particleDef->GetParticleName().c_str());
	//fPdgEncoding = particleDef->GetPDGEncoding(); 
	//fTrackID = theTrack->GetTrackID();
	//fParentID = theTrack->GetParentID();
	//fPostStepPosX = postStepPos.x()/mm;
	//fPostStepPosY = postStepPos.y()/mm;
	//fPostStepPosZ = postStepPos.z()/mm;
	//fPostStepMomX = postStepMom.x()/MeV;
	//fPostStepMomY = postStepMom.y()/MeV;
	//fPostStepMomZ = postStepMom.z()/MeV;
	//fTrackLength = theTrack->GetTrackLength()/mm;
	//fTrackStatus = theTrack->GetTrackStatus();
	//fPhotonDetectedAtEndOfStep = (
	//		(MTCG4SteppingAction*)
	//		G4RunManager::GetRunManager()->GetUserSteppingAction()
	//		)->GetPhotonDetectedAtEndOfStep();

	//if (fPdgEncoding == 1000030090) fLi9WasFound = true; // 9Li found in step.
	//if (fPdgEncoding == 1000020080) fHe8WasFound = true; // 8He found in step.

	//fStepTree->Fill();
}

//// Write at end of every event.
//// This was used for saving long lived isotopes from muon spallation.
//void StepRootIO::Write()
//{
//	if (true) { // Write event always.
//	//if (fLi9WasFound || fHe8WasFound) { // Write event if 8He/9Li was found.
//		struct stat buffer;
//		std::cout << "fFileName: " << fFileName << std::endl;
//		if ( stat(fFileName.c_str(), &buffer) ) // If output file doesn't exist.
//			fRootFile = new TFile(fFileName.c_str(), "RECREATE");
//		else // Update file if it already exists.
//			fRootFile = new TFile(fFileName.c_str(), "UPDATE");
//		assert(fRootFile); // Make sure ROOT file exits.
//		TTree *previousTree = dynamic_cast<TTree*>(fRootFile->Get("stepTree"));
//		TList *list = new TList();
//		if (previousTree) list->Add(previousTree); // If previous tree is found.
//		list->Add(fStepTree); // Add new tree addition.
//		TTree *newTree = TTree::MergeTrees(list); // New tree from merging list.
//		assert(newTree);
//		//newTree->SetName("stepTree");
//		//newTree->SetDirectory(fRootFile); // No need to set dir to root file?
//		fRootFile->cd(); // Make sure current dir is the file to be saved to.
//		newTree->Write("", TObject::kOverwrite);
//		fRootFile->Close();
//		delete fRootFile;
//		fRootFile = NULL; // Reinitialize dangling pointer.
//	}
//	delete fStepTree; // Delete old step tree, it doesn't belong to any file.
//	fStepTree = new TTree("stepTree", "stepTree"); // New empty step tree.
//	SetTreeBranches(); // Need to recreate branches for new tree.
//	fLi9WasFound = false; // Reinitialize flags.
//	fHe8WasFound = false; // Reinitialize flags.
//}

void StepRootIO::Write() // Write at end of run.
{
	//std::cout << "fFileName: " << fFileName << std::endl;
	//fRootFile->cd(); // Make sure current dir is the file to be saved to.
	//fStepTree->Write("", TObject::kOverwrite);
	//fRootFile->Close();
}
