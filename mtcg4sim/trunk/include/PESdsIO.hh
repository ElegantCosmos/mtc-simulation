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
/// \file persistency/P01/include/PESdsIO.hh
/// \brief Definition of the PESdsIO class
//
// $Id$
#ifndef INCLUDE_PE_SDS_IO_HH 
#define INCLUDE_PE_SDS_IO_HH

// Include files
#include "globals.hh"

#include "libmtc-sds.hpp"

/** @class PESdsIO
 *   
 *
 *  @author Witold POKORSKI
 *  @date   2005-10-27
 */

/// Root IO implementation for the persistency example
class TFile;
class TTree;
class G4Event;

class PESdsIO 
{
public: 
	virtual ~PESdsIO();

	static void CreateInstance(
		G4String fileName,
		G4String runName,
		G4String runType,
		G4String initials,
		G4int timeSec,
		G4int timeNanoSec,
		G4int timePicoSec,
		G4String location,
		G4String temperature,
		G4String comments);
	static PESdsIO* GetInstance();
	static void ResetInstance();
	inline void SetFileName(G4String name)
	{ fFileName = name; }
	void InitializeRunInfo();
	void Write(const G4Event* theEvent);
	void Close();

protected:
	PESdsIO(); 
	// Dont forget to declare these two. You want to make sure they
	// are unaccessable otherwise you may accidently get copies of
	// your singleton appearing.
	PESdsIO(PESdsIO const&);			// Do not implement.
	void operator=(PESdsIO const&);		// Do not implement.

private:
	static mtc::sds::SDS* fSdsFile;
	static struct mtc::sds::RunDescriptorStruct fRun;
	static struct mtc::sds::EventDescriptorStruct fEvent;
	static G4String	fFileName;
};
#endif // INCLUDE_PE_SDS_IO_HH
