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
/// \file persistency/P01/include/StepRootIO.hh
/// \brief Definition of the StepRootIO class
//
// $Id$
#ifndef INCLUDE_STEP_ROOT_IO_HH 
#define INCLUDE_STEP_ROOT_IO_HH

// Include files
#include "globals.hh"

/** @class StepRootIO
 *   
 *
 *  @author Witold POKORSKI
 *  @date   2005-10-27
 */

/// Root IO implementation for the persistency example
class TFile;
class TTree;

class StepRootIO 
{
public: 
	static void CreateInstance(G4String fileName);
	static void ResetInstance();
	static StepRootIO* GetInstance();
	void SetTreeBranches();
	// We include both track and step pointers here as input so that we can tell
	// if current step number is 0 or not, i.e. if the particle is being tracked
	// yet or not.
	void Fill(const G4Track *theTrack, const G4Step *theStep);
	void Write();

protected:
	StepRootIO(); 
	virtual ~StepRootIO() {;}

	// Dont forget to declare these two. You want to make sure they
	// are unaccessable otherwise you may accidently get copies of
	// your singleton appearing.
	StepRootIO(StepRootIO const&);			// Do not implement.
	void operator=(StepRootIO const&);		// Do not implement.

private:
	static G4String	fFileName;
	static TFile	*fRootFile;
	static TTree	*fStepTree;

	G4bool	fHe8WasFound;
	G4bool	fLi9WasFound;

	// TTree variable. Normal c++ basic types for input into TTree.
	G4int		fRunID;
	G4int		fEventID;
	G4double	fNuKineticEnergy;
	G4double	fNuMomUnitVectorX;
	G4double	fNuMomUnitVectorY;
	G4double	fNuMomUnitVectorZ;
	G4int		fStepID;
	char		fParticleName[256];
	G4int		fPdgEncoding;
	G4int		fTrackID;
	G4int		fParentID;
	G4double	fPostStepPosX;
	G4double	fPostStepPosY;
	G4double	fPostStepPosZ;
	G4double	fPostStepMomX;
	G4double	fPostStepMomY;
	G4double	fPostStepMomZ;
	G4double	fPostStepGlobalTime;
	G4double	fPostStepKineticEnergy;
	G4double	fTotalEnergyDeposit;
	G4double	fStepLength;
	G4double	fTrackLength;
	char		fProcessName[256];
	G4int		fProcessType;
	G4int		fProcessSubType;
	G4int		fTrackStatus;
	char		fPostStepPhysVolumeName[256];
	G4bool		fPhotonDetectedAtEndOfStep;
};
#endif // INCLUDE_STEP_ROOT_IO_HH
