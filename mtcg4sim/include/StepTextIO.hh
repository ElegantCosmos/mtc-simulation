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
/// \file persistency/P01/include/StepTextIO.hh
/// \brief Definition of the StepTextIO class
//
// $Id$
#ifndef INCLUDE_STEP_TEXT_IO_HH 
#define INCLUDE_STEP_TEXT_IO_HH

// Include files
#include "globals.hh"

/** @class StepTextIO
 *   
 *
 *  @author Witold POKORSKI
 *  @date   2005-10-27
 */

/// Root IO implementation for the persistency example
class TTree;
class G4Track;
class G4Step;

class StepTextIO 
{
public: 
	virtual ~StepTextIO();

	static void CreateInstance(G4String fileName);
	static void ResetInstance();
	static StepTextIO *GetInstance();
	// We include both track and step pointers here as input so that we can tell
	// if current step number is 0 or not, i.e. if the particle is being tracked
	// yet or not.
	void Fill(const G4Track *theTrack, const G4Step *theStep);
	void Write();
	void Close();
	void inline SetFirstLineOfEvent(bool flag = true) { fFirstLineOfEvent = flag; }

protected:
	StepTextIO(); 
	// Dont forget to declare these two. You want to make sure they
	// are unaccessable otherwise you may accidently get copies of
	// your singleton appearing.
	StepTextIO(StepTextIO const&);			// Do not implement.
	void operator=(StepTextIO const&);		// Do not implement.

private:
	static std::ofstream	fOut;
	static G4String			fFileName;
	G4String				fAllParticleStepHeader;
	G4bool					fFirstLineOfOutput;
	G4bool					fFirstLineOfEvent;

	// Variables to output to user.
	G4int		fRunID;
	G4int       fEventID;
	G4double    fNuKineticEnergy;
	G4double    fNuMomUnitVectorX;
	G4double    fNuMomUnitVectorY;
	G4double    fNuMomUnitVectorZ;
	G4int       fStepID;
	G4String	fParticleName;
	G4int       fPdgEncoding;
	G4int       fTrackID;
	G4int       fParentID;
	G4double    fPostStepPosX;
	G4double    fPostStepPosY;
	G4double    fPostStepPosZ;
	G4double	fPostStepMomX;
	G4double	fPostStepMomY;
	G4double	fPostStepMomZ;
	G4double    fPostStepGlobalTime;
	G4double    fPostStepKineticEnergy;
	G4double    fTotalEnergyDeposit;
	G4double    fStepLength;
	G4double    fTrackLength;
	G4String	fProcessName;
	G4int       fProcessType;
	G4int       fProcessSubType;
	G4int       fTrackStatus;
	G4String	fPostStepPhysVolumeName;
	G4bool		fPhotonDetectedAtEndOfStep;
};
#endif // INCLUDE_ROOTIO_HH
