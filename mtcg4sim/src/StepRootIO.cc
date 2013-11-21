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
	fNuMomentumUnitVectorX(0),
	fNuMomentumUnitVectorY(0),
	fNuMomentumUnitVectorZ(0),
	fStepID(-1),
	fPdgEncoding(0),
	fTrackID(-1),
	fParentID(-1),
	fPreStepPositionX(0),
	fPreStepPositionY(0),
	fPreStepPositionZ(0),
	fPostStepPositionX(0),
	fPostStepPositionY(0),
	fPostStepPositionZ(0),
	fPreStepGlobalTime(-1),
	fPostStepGlobalTime(-1),
	fPreStepKineticEnergy(-1),
	fPostStepKineticEnergy(-1),
	fTotalEnergyDeposit(-1),
	fStepLength(-1),
	fTrackLength(-1),
	fProcessType(-1),
	fProcessSubType(-1),
	fTrackStatus(-1),
	fPostStepPointInDetector(false)
{
	fHe8WasFound = false;
	fLi9WasFound = false;

	std::cout << "unlink(" << fFileName.c_str() << ")" << std::endl;
	unlink(fFileName.c_str()); // Remove old ROOT file.

	// Create TTree.
	fStepTree = new TTree("stepTree", "stepTree");
	SetTreeBranches();
}

void StepRootIO::CreateInstance(G4String fileName)
{
	fFileName = fileName;
	StepRootIOManager = new StepRootIO();
}

void StepRootIO::ResetInstance()
{
	if (fStepTree) {
		fStepTree->SetDirectory(0); // Make memory-resident.
		delete fStepTree; // Is memory-resident so needs to be deleted.
		fStepTree = NULL;
	}
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
	fStepTree->SetDirectory(0); // Make tree memory-resident.

	// Create branches.
	fStepTree->Branch("runID", &fRunID, "fRunID/I");
	fStepTree->Branch("eventID", &fEventID, "fEventID/I");
	fStepTree->Branch("nuKineticEnergy", &fNuKineticEnergy,
			"fNuKineticEnergy/D");
	fStepTree->Branch("nuMomentumUnitVectorX",
			&fNuMomentumUnitVectorX, "fNuMomentumUnitVectorX/D");
	fStepTree->Branch("nuMomentumUnitVectorY",
			&fNuMomentumUnitVectorY, "fNuMomentumUnitVectorY/D");
	fStepTree->Branch("nuMomentumUnitVectorZ",
			&fNuMomentumUnitVectorZ, "fNuMomentumUnitVectorZ/D");
	fStepTree->Branch("stepID", &fStepID, "fStepID/I");
	fStepTree->Branch("particleName", fParticleName, "fParticleName[80]/C");
	fStepTree->Branch("pdgEncoding", &fPdgEncoding, "fPdgEncoding/I");
	fStepTree->Branch("trackID", &fTrackID, "fTrackID/I");
	fStepTree->Branch("parentID", &fParentID, "fParentID/I");
	fStepTree->Branch("preStepPositionX", &fPreStepPositionX,
			"fPreStepPositionX/D");
	fStepTree->Branch("preStepPositionY", &fPreStepPositionY,
			"fPreStepPositionY/D");
	fStepTree->Branch("preStepPositionZ", &fPreStepPositionZ,
			"fPreStepPositionZ/D");
	fStepTree->Branch("postStepPositionX", &fPostStepPositionX,
			"fPostStepPositionX/D");
	fStepTree->Branch("postStepPositionY", &fPostStepPositionY,
			"fPostStepPositionY/D");
	fStepTree->Branch("postStepPositionZ", &fPostStepPositionZ,
			"fPostStepPositionZ/D");
	fStepTree->Branch("preStepGlobalTime", &fPreStepGlobalTime,
			"fPreStepGlobalTime/D");
	fStepTree->Branch("postStepGlobalTime", &fPostStepGlobalTime,
			"fPostStepGlobalTime/D");
	fStepTree->Branch("preStepKineticEnergy", &fPreStepKineticEnergy,
			"fPreStepKineticEnergy/D");
	fStepTree->Branch("postStepKineticEnergy", &fPostStepKineticEnergy,
			"fPostStepKineticEnergy/D");
	fStepTree->Branch("totalEnergyDeposit", &fTotalEnergyDeposit,
			"fTotalEnergyDeposit/D");
	fStepTree->Branch("stepLength", &fStepLength, "fStepLength/D");
	fStepTree->Branch("trackLength", &fTrackLength, "fTrackLength/D");
	fStepTree->Branch("creatorProcessName", fCreatorProcessName,
	      "fCreatorProcessName[80]/C");
	fStepTree->Branch("processName", fProcessName, "fProcessName[80]/C");
	fStepTree->Branch("processType", &fProcessType, "fProcessType/I");
	fStepTree->Branch("processSubType", &fProcessSubType, "fProcessSubType/I");
	fStepTree->Branch("trackStatus", &fTrackStatus, "fTrackStatus/I");
	fStepTree->Branch("postStepPointInDetector", &fPostStepPointInDetector,
			"fPostStepPointInDetector/O");
}

void StepRootIO::Fill()
{
	if (fPdgEncoding == 1000030090) fLi9WasFound = true; // 9Li found in step.
	if (fPdgEncoding == 1000020080) fHe8WasFound = true; // 8He found in step.
	fStepTree->Fill();
}

void StepRootIO::Write()
{
	if (fLi9WasFound || fHe8WasFound) { // Write event if 8He/9Li was found.
		struct stat buffer;
		if ( stat(fFileName.c_str(), &buffer) ) // If output file doesn't exist.
			fRootFile = new TFile(fFileName.c_str(), "RECREATE");
		else // Update file if it already exists.
			fRootFile = new TFile(fFileName.c_str(), "UPDATE");
		assert(fRootFile); // Make sure ROOT file exits.
		TTree *previousTree = dynamic_cast<TTree*>(fRootFile->Get("stepTree"));
		TList *list = new TList();
		if (previousTree) list->Add(previousTree); // If previous tree is found.
		list->Add(fStepTree); // Add new tree addition.
		TTree *newTree = TTree::MergeTrees(list); // New tree from merging list.
		newTree->SetName("stepTree");
		newTree->SetDirectory(fRootFile);
		newTree->Write("", TObject::kOverwrite);
		fRootFile->Close();
		delete fRootFile;
		fRootFile = NULL; // Reinitialize dangling pointer.
	}
	delete fStepTree; // Delete old step tree, it doesn't belong to any file.
	fStepTree = new TTree("stepTree", "stepTree"); // New empty step tree.
	SetTreeBranches(); // Need to recreate branches for new tree.
	fLi9WasFound = false; // Reinitialize flags.
	fHe8WasFound = false; // Reinitialize flags.
}
