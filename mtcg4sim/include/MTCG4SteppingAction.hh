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

#ifndef MTCG4_STEPPING_ACTION_HH
#define MTCG4_STEPPING_ACTION_HH 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"

class TFile;
class TTree;
class MTCG4SteppingMessenger;

class MTCG4SteppingAction : public G4UserSteppingAction
{
public:
	MTCG4SteppingAction();
    ~MTCG4SteppingAction();

public:
	void UserSteppingAction(const G4Step*);
	//void DetectPhotonOfSteppingAction(const G4Step*, G4String&, G4String&);
	void DoStepOutputToTextFile(const G4Step*);
	void DoStepOutputToRootFile(const G4Step*);
	void SetOutputOnlyFirstNEvents(G4int n)
		{ fOutputOnlyFirstNEvents = n; }
	inline G4int GetOutputOnlyFirstNEvents() const
		{ return fOutputOnlyFirstNEvents; }
	inline void SetPhotonDetectedAtEndOfStep(G4bool flag)
		{ fPhotonDetectedAtEndOfStep = flag; }

private:
	G4int	fOutputOnlyFirstNEvents;
	G4bool	fPhotonDetectedAtEndOfStep;

private:
	MTCG4SteppingMessenger* fMessenger;
};

#endif
