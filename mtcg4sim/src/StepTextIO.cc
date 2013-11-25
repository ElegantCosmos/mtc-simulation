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
	fAllParticleStepHeader += "EventNumber ";
	fAllParticleStepHeader += "NeutrinoKineticEnergy(MeV) ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorX ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorY ";
	fAllParticleStepHeader += "NeutrinoMomentumUnitVectorZ ";
	fAllParticleStepHeader += "StepNumber ";
	fAllParticleStepHeader += "ParticleName ";
	fAllParticleStepHeader += "PDGEncoding ";
	fAllParticleStepHeader += "TrackID ";
	fAllParticleStepHeader += "ParentID ";
	fAllParticleStepHeader += "PreStepPositionX(mm) ";
	fAllParticleStepHeader += "PreStepPositionY(mm) ";
	fAllParticleStepHeader += "PreStepPositionZ(mm) ";
	fAllParticleStepHeader += "PostStepPositionX(mm) ";
	fAllParticleStepHeader += "PostStepPositionY(mm) ";
	fAllParticleStepHeader += "PostStepPositionZ(mm) ";
	fAllParticleStepHeader += "PreStepGlobalTime(ns) ";
	fAllParticleStepHeader += "PostStepGlobalTime(ns) ";
	fAllParticleStepHeader += "PreStepKineticEnergy(MeV) ";
	fAllParticleStepHeader += "PostStepKineticEnergy(MeV) ";
	fAllParticleStepHeader += "TotalEnergyDeposit(MeV) ";
	fAllParticleStepHeader += "StepLength(mm) ";
	fAllParticleStepHeader += "TrackLength(mm) ";
	fAllParticleStepHeader += "CreatorProcessName ";
	fAllParticleStepHeader += "ProcessName ";
	fAllParticleStepHeader += "ProcessType ";
	fAllParticleStepHeader += "ProcessSubType ";
	fAllParticleStepHeader += "TrackStatus ";
	fAllParticleStepHeader += "postStepPointInDetector ";
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
	fOut << std::setw(m) << std::left << fEventID << " ";
	fOut << std::setw(m) << fNuKineticEnergy << " ";
	fOut << std::setw(m) << fNuMomentumUnitVectorX << " ";
	fOut << std::setw(m) << fNuMomentumUnitVectorY << " ";
	fOut << std::setw(m) << fNuMomentumUnitVectorZ << " ";
	fOut << std::setw(s) << fStepID << " ";
	fOut << std::setw(l) << fParticleName << " ";
	fOut << std::setw(m) << fPdgEncoding << " ";
	fOut << std::setw(s) << std::right << fTrackID << " ";
	fOut << std::setw(s) <<	fParentID << " ";
	fOut << std::setw(m) <<	fPreStepPositionX << " ";
	fOut << std::setw(m) <<	fPreStepPositionY << " ";
	fOut << std::setw(m) <<	fPreStepPositionZ << " ";
	fOut << std::setw(m) <<	fPostStepPositionX << " ";
	fOut << std::setw(m) <<	fPostStepPositionY << " ";
	fOut << std::setw(m) <<	fPostStepPositionZ << " ";
	fOut << std::setw(m) <<	fPreStepGlobalTime << " ";
	fOut << std::setw(m) <<	fPostStepGlobalTime << " ";
	fOut << std::setw(l) <<	fPreStepKineticEnergy << " ";
	fOut << std::setw(l) <<	fPostStepKineticEnergy << " ";
	fOut << std::setw(l) <<	fTotalEnergyDeposit << " ";
	fOut << std::setw(l) <<	fStepLength << " ";
	fOut << std::setw(l) <<	fTrackLength << " ";
	fOut << std::setw(l) << fCreatorProcessName << " ";
	fOut << std::setw(l) << fProcessName << " ";
	fOut << std::setw(m) << fProcessType << " ";
	fOut << std::setw(m) << fProcessSubType << " ";
	fOut << std::setw(s) << std::right << fTrackStatus << " ";
	fOut << fPostStepPointInDetector << " ";
	fOut << fPhotonDetectedAtEndOfStep;
	fOut << G4endl;
}

void StepTextIO::Close()
{
	fOut.close();
}
