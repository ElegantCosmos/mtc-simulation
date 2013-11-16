//////////////////////////////////////////////////////////
// MTCG4PixelHit.cc by Mich 110502
//////////////////////////////////////////////////////////

#include "MTCG4PixelHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

G4Allocator<MTCG4PixelHit> MTCG4PixelHitAllocator;

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
MTCG4PixelHit::MTCG4PixelHit()
  :MCPQuadSetCopyNumber(-1),MCPRowCopyNumber(-1),MCPCopyNumber(-1),
	pixelVolumeRowCopyNumber(-1),pixelVolumeCopyNumber(-1),
	hitTime(-1.),photonHitPositionVector(0),/*pixelRotationMatrix(0),*/pixelPositionVector(0),pixelVolumePhysical(0),drawit(false)
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
MTCG4PixelHit::~MTCG4PixelHit()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
MTCG4PixelHit::MTCG4PixelHit(const MTCG4PixelHit &right)
  : G4VHit()
{
	MCPQuadSetCopyNumber = right.MCPQuadSetCopyNumber;
	MCPRowCopyNumber = right.MCPRowCopyNumber;
	MCPCopyNumber = right.MCPCopyNumber;
	pixelVolumeRowCopyNumber = right.pixelVolumeRowCopyNumber;
	pixelVolumeCopyNumber = right.pixelVolumeCopyNumber;
	pixelRotationMatrix = right.pixelRotationMatrix;
	pixelPositionVector = right.pixelPositionVector;
	photonHitPositionVector = right.photonHitPositionVector;
	hitTime=right.hitTime;
	pixelVolumePhysical=right.pixelVolumePhysical;
	drawit=right.drawit;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
const MTCG4PixelHit& MTCG4PixelHit::operator=(const MTCG4PixelHit &right){
	MCPQuadSetCopyNumber = right.MCPQuadSetCopyNumber;
	MCPRowCopyNumber = right.MCPRowCopyNumber;
	MCPCopyNumber = right.MCPCopyNumber;
	pixelVolumeRowCopyNumber = right.pixelVolumeRowCopyNumber;
	pixelVolumeCopyNumber = right.pixelVolumeCopyNumber;
	pixelRotationMatrix = right.pixelRotationMatrix;
	pixelPositionVector = right.pixelPositionVector;
	photonHitPositionVector = right.photonHitPositionVector;
	hitTime=right.hitTime;
	pixelVolumePhysical=right.pixelVolumePhysical;
	drawit=right.drawit;
	return *this;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4int MTCG4PixelHit::operator==(const MTCG4PixelHit &right) const{
  return (MCPQuadSetCopyNumber == right.MCPQuadSetCopyNumber
		  && MCPRowCopyNumber == right.MCPRowCopyNumber
		  && MCPCopyNumber == right.MCPCopyNumber
		  && pixelVolumeRowCopyNumber == right.pixelVolumeRowCopyNumber
		  && pixelVolumeCopyNumber == right.pixelVolumeCopyNumber
		  && pixelRotationMatrix == right.pixelRotationMatrix
		  && pixelPositionVector == right.pixelPositionVector);
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelHit::Draw(){/*
  if(drawit && pixelVolumePhysical){ //ReDraw only the PMTs that have hit counts > 0
    //Also need a physical volume to be able to draw anything
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager){//Make sure that the VisManager exists
      G4VisAttributes attribs(G4Colour(0., 1., 0., 1.));
      attribs.SetForceSolid(true);
      G4RotationMatrix pixelRotationMatrix = this->GetPixelRotation();
	  G4ThreeVector pixelPositionVector = this->GetPixelPosition();
	  G4Transform3D trans(pixelRotationMatrix, pixelPositionVector);//Create transform
      pVVisManager->Draw(*pixelVolumePhysical,attribs,trans);//Draw it
    }
  }
*/}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void MTCG4PixelHit::Print(){
}









