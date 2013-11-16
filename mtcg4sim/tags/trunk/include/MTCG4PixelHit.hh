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

#ifndef MTCG4_PIXEL_HIT_HH
#define MTCG4_PIXEL_HIT_HH 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

class G4VTouchable;

class MTCG4PixelHit : public G4VHit
{
public:
  
  MTCG4PixelHit();
  ~MTCG4PixelHit();
  MTCG4PixelHit(const MTCG4PixelHit &right);

  const MTCG4PixelHit& operator=(const MTCG4PixelHit &right);
  G4int operator==(const MTCG4PixelHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);
  
  void Draw();
  void Print();

  inline void SetDrawit(G4bool b){drawit=b;}
  inline G4bool GetDrawit(){return drawit;}

  inline void SetHitTime(G4double newHitTime){ if(newHitTime < hitTime || hitTime < 0.) hitTime = newHitTime; }
  inline G4double GetHitTime(){ /*G4cout << "GetHitTime() = " << hitTime << G4endl;*/ return hitTime; }

  inline void SetPhotonHitPosition(G4double x, G4double y, G4double z){ photonHitPositionVector = G4ThreeVector(x, y, z);}
  inline G4ThreeVector GetPhotonHitPosition(){ return photonHitPositionVector;}

  inline void SetMCPQuadSetCopyNumber(G4int newMCPQuadSetCopyNumber) { MCPQuadSetCopyNumber = newMCPQuadSetCopyNumber; }
  inline G4int GetMCPQuadSetCopyNumber() { return MCPQuadSetCopyNumber; }

  inline void SetMCPRowCopyNumber(G4int newMCPRowCopyNumber) { MCPRowCopyNumber = newMCPRowCopyNumber; }
  inline G4int GetMCPRowCopyNumber() { return MCPRowCopyNumber; }

  inline void SetMCPCopyNumber(G4int newMCPCopyNumber) { MCPCopyNumber = newMCPCopyNumber; }
  inline G4int GetMCPCopyNumber() { return MCPCopyNumber; }

  inline void SetPixelRowCopyNumber(G4int newPixelRowCopyNumber) { pixelVolumeRowCopyNumber = newPixelRowCopyNumber; }
  inline G4int GetPixelRowCopyNumber() { return pixelVolumeRowCopyNumber; }

  inline void SetPixelVolumeCopyNumber(G4int newPixelVolumeCopyNumber) { pixelVolumeCopyNumber = newPixelVolumeCopyNumber; }
  inline G4int GetPixelVolumeCopyNumber() { return pixelVolumeCopyNumber; }

  inline void SetPixelVolumePhysical(G4VPhysicalVolume* pixelVolumePhysical){this->pixelVolumePhysical=pixelVolumePhysical;}
  inline G4VPhysicalVolume* GetPixelVolumePhysical(){return pixelVolumePhysical;}

  inline void SetPixelRotation(G4double xx, G4double xy, G4double xz, G4double yx, G4double yy, G4double yz, G4double zx, G4double zy, G4double zz){
	  G4ThreeVector colX(xx, yx, zx);
	  G4ThreeVector colY(xy, yy, zy);
	  G4ThreeVector colZ(xz, yz, zz);
	  pixelRotationMatrix = G4RotationMatrix(colX, colY, colZ);
  }
  inline G4RotationMatrix GetPixelRotation(){ return pixelRotationMatrix;}

  inline void SetPixelPosition(G4double x, G4double y, G4double z) {pixelPositionVector = G4ThreeVector(x, y, z);}
  inline G4ThreeVector GetPixelPosition(){return pixelPositionVector;}
  
private:
  G4int MCPQuadSetCopyNumber;
  G4int MCPRowCopyNumber;
  G4int MCPCopyNumber;
  G4int pixelVolumeRowCopyNumber;
  G4int pixelVolumeCopyNumber;
  G4double hitTime;
  G4ThreeVector photonHitPositionVector;
  G4RotationMatrix pixelRotationMatrix;
  G4ThreeVector pixelPositionVector;
  G4VPhysicalVolume* pixelVolumePhysical;
  G4bool drawit;
};

typedef G4THitsCollection<MTCG4PixelHit> MTCG4PixelHitsCollection;

extern G4Allocator<MTCG4PixelHit> MTCG4PixelHitAllocator;

inline void* MTCG4PixelHit::operator new(size_t){
  void *aHit;
  aHit = (void *) MTCG4PixelHitAllocator.MallocSingle();
  return aHit;
}

inline void MTCG4PixelHit::operator delete(void *aHit){
  MTCG4PixelHitAllocator.FreeSingle((MTCG4PixelHit*) aHit);
}

#endif


