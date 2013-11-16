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
#ifndef MTCG4_PIXEL_SD_HH
#define MTCG4_PIXEL_SD_HH 1

#include "G4DataVector.hh"
#include "G4VSensitiveDetector.hh"
#include "MTCG4PixelHit.hh"
class G4Step;
class G4HCofThisEvent;

class MTCG4PixelSD : public G4VSensitiveDetector
{

public:
  MTCG4PixelSD(G4String name);
  ~MTCG4PixelSD();
  
  void Initialize(G4HCofThisEvent* HCE);
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  
  // A version of processHits that keeps aStep constant.
  G4bool ProcessHits_constStep(const G4Step* aStep,
			       G4TouchableHistory* ROhist);
  void EndOfEvent(G4HCofThisEvent* HCE);
  void clear();
  void DrawAll(/*const G4int nEntries*/);
  void PrintFirstPhotonPixelHit(/*const G4int nEntries*/);
  void PrintMCPOutputSignal(/*const G4int nEntries*/);

  // Initialize the arrays to store PMT positions.
  inline void InitializePixels(G4int nPMTs){
    if(pixelPositionsX)delete pixelPositionsX;
    if(pixelPositionsY)delete pixelPositionsY;
    if(pixelPositionsZ)delete pixelPositionsZ;
    pixelPositionsX=new G4DataVector(nPMTs);
    pixelPositionsY=new G4DataVector(nPMTs);
    pixelPositionsZ=new G4DataVector(nPMTs);
  }

  // Store a PMT position.
  inline void SetPixelPosition(G4int n,G4double x,G4double y,G4double z){
    if(pixelPositionsX)pixelPositionsX->insertAt(n,x);
    if(pixelPositionsY)pixelPositionsY->insertAt(n,y);
    if(pixelPositionsZ)pixelPositionsZ->insertAt(n,z);
  }
  
private:
  MTCG4PixelHitsCollection* pixelHitCollection;
  G4int nEntries;  
  G4DataVector* pixelPositionsX;
  G4DataVector* pixelPositionsY;
  G4DataVector* pixelPositionsZ;
};

#endif

