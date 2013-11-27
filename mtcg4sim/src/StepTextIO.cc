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
/// \file persistency/P01/src/StepTextIO.cc
/// \brief Implementation of the StepTextIO class
//
#include <iostream>
#include <iomanip>
#include <fstream>

#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4SteppingAction.hh"
#include "StepTextIO.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

static StepTextIO* StepTextIOManager = 0;

G4String StepTextIO::fFileName;
std::ofstream StepTextIO::fOut;

StepTextIO::StepTextIO()
{
	fFirstLineOfOutput = true;
	unlink(fFileName.c_str());
	fOut.open(fFileName, std::ofstream::out);

	// Create output header.
	fAllParticleStepHeader = "# ";
	fAllParticleStepHeader += "RunID ";
	fAllParticleStepHeader += "EventID ";
	fAllParticleStepHeader += "NeutrinoKineticEnergy(MeV) ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorX ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorY ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorZ ";
	fAllParticleStepHeader += "StepID ";
	fAllParticleStepHeader += "ParticleName ";
	fAllParticleStepHeader += "PdgEncoding ";
	fAllParticleStepHeader += "TrackID ";
	fAllParticleStepHeader += "ParentID ";
	fAllParticleStepHeader += "PostStepPositionX(mm) ";
	fAllParticleStepHeader += "PostStepPositionY(mm) ";
	fAllParticleStepHeader += "PostStepPositionZ(mm) ";
	fAllParticleStepHeader += "PostStepMomentumX ";
	fAllParticleStepHeader += "PostStepMomentumY ";
	fAllParticleStepHeader += "PostStepMomentumZ ";
	fAllParticleStepHeader += "PostStepGlobalTime(ns) ";
	fAllParticleStepHeader += "PostStepKineticEnergy(MeV) ";
	fAllParticleStepHeader += "TotalEnergyDeposit(MeV) ";
	fAllParticleStepHeader += "StepLength(mm) ";
	fAllParticleStepHeader += "TrackLength(mm) ";
	fAllParticleStepHeader += "ProcessName ";
	fAllParticleStepHeader += "ProcessType ";
	fAllParticleStepHeader += "ProcessSubType ";
	fAllParticleStepHeader += "TrackStatus ";
	fAllParticleStepHeader += "postStepPointPhysicalVolumeName ";
	fAllParticleStepHeader += "photonDetectedAtEndOfStep";
}

StepTextIO::~StepTextIO()
{}

void StepTextIO::CreateInstance(G4String fileName)
{
	fFileName = fileName;
	StepTextIOManager = new StepTextIO();
}

StepTextIO* StepTextIO::GetInstance()
{
	if (StepTextIOManager == 0 )
	{
		StepTextIOManager = new StepTextIO();
	}
	return StepTextIOManager;
}

void StepTextIO::ResetInstance()
{
	if (fOut.is_open()) fOut.close();
	delete StepTextIOManager;
	StepTextIOManager = NULL;
}

void StepTextIO::Fill(const G4Track *theTrack, const G4Step *theStep)
{
	const G4Event* theEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
	G4ThreeVector postStepPos;
	G4ThreeVector postStepMom;
	if (theStep) {// If step exists, tracking is underway. Get info from step.
		G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
		postStepPos = postStepPoint->GetPosition();
		postStepMom = postStepPoint->GetMomentum();
		fPostStepGlobalTime = postStepPoint->GetGlobalTime()/ns;
		fPostStepKineticEnergy = postStepPoint->GetKineticEnergy()/MeV;
		fPostStepPhysVolumeName = postStepPoint->GetPhysicalVolume()->GetName();
		const G4VProcess *postStepProcess =
		   	postStepPoint->GetProcessDefinedStep();
		if (postStepProcess != NULL) {
			fProcessName = postStepProcess->GetProcessName();
			fProcessType = postStepProcess->GetProcessType();
			fProcessSubType = postStepProcess->GetProcessSubType();
		}
		else {
			fProcessName = "nullProcess";
			fProcessType = -100;
			fProcessSubType = -100;
		}
		fTotalEnergyDeposit = theStep->GetTotalEnergyDeposit()/MeV;
		fStepLength = theStep->GetStepLength()/mm;
	}
	else { // If step doesn't exist, particle is not yet tracked, ie 0th step.
		postStepPos = theTrack->GetPosition(); // Use track position info.
		postStepMom = theTrack->GetMomentum();
		fPostStepGlobalTime = theTrack->GetGlobalTime()/ns;
		fPostStepKineticEnergy = theTrack->GetKineticEnergy()/MeV;
		fPostStepPhysVolumeName = theTrack->GetVolume()->GetName();
		const G4VProcess *postStepProcess = theTrack->GetCreatorProcess();
		if (postStepProcess != NULL) {
			fProcessName = postStepProcess->GetProcessName();
			fProcessType = postStepProcess->GetProcessType();
			fProcessSubType = postStepProcess->GetProcessSubType();
		}
		else {//If step nor creator process exists, it is 0th step of primary.
			fProcessName = "primaryParticle";
			fProcessType = -100;
			fProcessSubType = -100;
		}
		fTotalEnergyDeposit = 0;
		fStepLength = theTrack->GetStepLength()/mm;
	}
	G4ParticleDefinition* particleDef = theTrack->GetDefinition();
	G4ThreeVector nuMomUnitVector =
		(dynamic_cast<MTCG4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction()))->GetNeutrinoMomentumUnitVector();

	// Variables to output.
	const MTCG4RunAction *runAction = dynamic_cast<const MTCG4RunAction*>(
				G4RunManager::GetRunManager()->GetUserRunAction());
	assert(runAction);
	fRunID = runAction->GetRunID();
	fEventID = theEvent->GetEventID();
	fNuKineticEnergy = (
			(MTCG4EventAction*)
			G4EventManager::GetEventManager()->GetUserEventAction()
			)->GetNeutrinoKineticEnergyOfEventAction()/MeV;
	fNuMomUnitVectorX = nuMomUnitVector.x();
	fNuMomUnitVectorY = nuMomUnitVector.y();
	fNuMomUnitVectorZ = nuMomUnitVector.z();
	fStepID = theTrack->GetCurrentStepNumber();
	fParticleName = particleDef->GetParticleName();
	fPdgEncoding = particleDef->GetPDGEncoding(); 
	fTrackID = theTrack->GetTrackID();
	fParentID = theTrack->GetParentID();
	fPostStepPosX = postStepPos.x()/mm;
	fPostStepPosY = postStepPos.y()/mm;
	fPostStepPosZ = postStepPos.z()/mm;
	fPostStepMomX = postStepMom.x();
	fPostStepMomY = postStepMom.y();
	fPostStepMomZ = postStepMom.z();
	fTrackLength = theTrack->GetTrackLength()/mm;
	fTrackStatus = theTrack->GetTrackStatus();
	fPhotonDetectedAtEndOfStep = (
			(MTCG4SteppingAction*)
			G4RunManager::GetRunManager()->GetUserSteppingAction()
			)->GetPhotonDetectedAtEndOfStep();
}

void StepTextIO::Write()
{
	// Text file output.
	fOut.setf(std::ios::fixed, std::ios::floatfield);
	fOut.precision(7);
	const G4int s=5;
	const G4int m=10;
	const G4int l=15;
	if(fFirstLineOfOutput) fOut << fAllParticleStepHeader << G4endl;
	fFirstLineOfOutput = false;
	fOut << std::setw(m) << fRunID << " ";
	fOut << std::setw(m) << fEventID << " ";
	fOut << std::setw(m) << fNuKineticEnergy << " ";
	fOut << std::setw(m) << fNuMomUnitVectorX << " ";
	fOut << std::setw(m) << fNuMomUnitVectorY << " ";
	fOut << std::setw(m) << fNuMomUnitVectorZ << " ";
	fOut << std::setw(s) << fStepID << " ";
	fOut << std::setw(l) << fParticleName << " ";
	fOut << std::setw(m) << fPdgEncoding << " ";
	fOut << std::setw(s) << std::right << fTrackID << " ";
	fOut << std::setw(s) <<	fParentID << " ";
	fOut << std::setw(m) <<	fPostStepPosX << " ";
	fOut << std::setw(m) <<	fPostStepPosY << " ";
	fOut << std::setw(m) <<	fPostStepPosZ << " ";
	fOut << std::setw(m) <<	fPostStepMomX << " ";
	fOut << std::setw(m) <<	fPostStepMomY << " ";
	fOut << std::setw(m) <<	fPostStepMomZ << " ";
	fOut << std::setw(m) <<	fPostStepGlobalTime << " ";
	fOut << std::setw(l) <<	fPostStepKineticEnergy << " ";
	fOut << std::setw(l) <<	fTotalEnergyDeposit << " ";
	fOut << std::setw(l) <<	fStepLength << " ";
	fOut << std::setw(l) <<	fTrackLength << " ";
	fOut << std::setw(l) << fProcessName << " ";
	fOut << std::setw(m) << fProcessType << " ";
	fOut << std::setw(m) << fProcessSubType << " ";
	fOut << std::setw(s) << std::right << fTrackStatus << " ";
	fOut << fPostStepPhysVolumeName << " ";
	fOut << fPhotonDetectedAtEndOfStep;
	fOut << G4endl;
}

void StepTextIO::Close()
{
	fOut.close();
}
