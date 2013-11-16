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
	void Fill();
	void Write();

	inline void SetFileName(G4String name) { fFileName = name; }
	inline G4String GetFileName() { return fFileName; }
	inline void SetRunID(G4int val) { fRunID = val; }
	inline G4int GetRunID() { return fRunID; }
	inline void SetEventID(G4int val) { fEventID = val; }
	inline G4int GetEventID() const { return fEventID; }
	inline void SetNuKineticEnergy(G4double val) { fNuKineticEnergy = val; }
	inline void SetNuMomentumUnitVectorX(G4double val) { fNuMomentumUnitVectorX = val; }
	inline void SetNuMomentumUnitVectorY(G4double val) { fNuMomentumUnitVectorY = val; }
	inline void SetNuMomentumUnitVectorZ(G4double val) { fNuMomentumUnitVectorZ = val; }
	inline void SetStepID(G4int val) { fStepID = val; }
	inline void SetParticleName(G4String/*char**/ val)
		{ strcpy(fParticleName, val.c_str()); }
	inline void SetPdgEncoding(G4int val) { fPdgEncoding = val; }
	inline void SetTrackID(G4int val) { fTrackID = val; }
	inline void SetParentID(G4int val) { fParentID = val; }
	inline void SetPreStepPositionX(G4double val) { fPreStepPositionX = val; }
	inline void SetPreStepPositionY(G4double val) { fPreStepPositionY = val; }
	inline void SetPreStepPositionZ(G4double val) { fPreStepPositionZ = val; }
	inline void SetPostStepPositionX(G4double val) { fPostStepPositionX = val; }
	inline void SetPostStepPositionY(G4double val) { fPostStepPositionY = val; }
	inline void SetPostStepPositionZ(G4double val) { fPostStepPositionZ = val; }
	inline void SetPreStepGlobalTime(G4double val) { fPreStepGlobalTime = val; }
	inline void SetPostStepGlobalTime(G4double val) { fPostStepGlobalTime = val; }
	inline void SetPreStepKineticEnergy(G4double val) { fPreStepKineticEnergy = val; }
	inline void SetPostStepKineticEnergy(G4double val) { fPostStepKineticEnergy = val; }
	inline void SetTotalEnergyDeposit(G4double val) { fTotalEnergyDeposit = val; }
	inline void SetStepLength(G4double val) { fStepLength = val; }
	inline void SetTrackLength(G4double val) { fTrackLength = val; }
	inline void SetCreatorProcessName(G4String/*char**/ val)
		{ strcpy(fCreatorProcessName, val.c_str()); }
	inline void SetProcessName(G4String/*char**/ val)
		{ strcpy(fProcessName, val.c_str()); }
	inline void SetProcessType(G4int val) { fProcessType = val; }
	inline void SetProcessSubType(G4int val) { fProcessSubType = val; }
	inline void SetTrackStatus(G4int val) { fTrackStatus = val; }
	inline void SetPostStepPointInDetector(G4bool val) { fPostStepPointInDetector = val; }

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
	int		fRunID;
	int		fEventID;
	double	fNuKineticEnergy;
	double	fNuMomentumUnitVectorX;
	double	fNuMomentumUnitVectorY;
	double	fNuMomentumUnitVectorZ;
	int		fStepID;
	char	fParticleName[80];
	int		fPdgEncoding;
	int		fTrackID;
	int		fParentID;
	double	fPreStepPositionX;
	double	fPreStepPositionY;
	double	fPreStepPositionZ;
	double	fPostStepPositionX;
	double	fPostStepPositionY;
	double	fPostStepPositionZ;
	double	fPreStepGlobalTime;
	double	fPostStepGlobalTime;
	double	fPreStepKineticEnergy;
	double	fPostStepKineticEnergy;
	double	fTotalEnergyDeposit;
	double	fStepLength;
	double	fTrackLength;
	char	fCreatorProcessName[80];
	char	fProcessName[80];
	int		fProcessType;
	int		fProcessSubType;
	int		fTrackStatus;
	bool	fPostStepPointInDetector;
};
#endif // INCLUDE_STEP_ROOT_IO_HH
