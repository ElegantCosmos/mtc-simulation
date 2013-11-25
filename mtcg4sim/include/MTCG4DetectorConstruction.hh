//
//    DetectorConstruction.hh by Mich 120417
//

#ifndef MTCG4_DETECTOR_CONSTRUCTION_HH
#define MTCG4_DETECTOR_CONSTRUCTION_HH 1

#include "MTCG4PmtSD.hh"

#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"
//
// Class definitions.
//
class MTCG4DetectorMessenger;
class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4UnionSolid;
class G4SubtractionSolid;

enum ScintVis {scintVisOff, scintVisGray, scintVisColor};
enum PmtVis {pmtVisOff, pmtVisGray, pmtVisColor};
enum PeripheralGeomeotryVis {peripheralVisOff, peripheralVisGray,
	peripheralVisColor};

class MTCG4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
	MTCG4DetectorConstruction();
	~MTCG4DetectorConstruction();

public:
	inline G4double GetWorldHight() const
	{ return fWorldHight; }
	inline G4double GetWorldWidthX() const
	{ return fWorldWidthX; }
	inline G4double GetWorldWidthY() const
	{ return fWorldWidthY; }
	void SetNeutronCaptureDopantMaterial(G4String material)
	{ fNeutronCaptureMaterial = material; }
	inline G4String GetNeutronCaptureDopantMaterial() const
	{ return fNeutronCaptureMaterial; }
	void SetDopantEnrichment(G4bool value)
	{ fDopantIsEnriched = value; }
	inline G4bool GetDopantEnrichment() const
	{ return fDopantIsEnriched; }
	void SetDopingFraction(G4double value)
	{ fDopingFraction = value; }
	inline G4double GetDopingFraction() const
	{	return fDopingFraction; }
	inline G4double GetScintDimensionX() const
	{ return fScintDimensionX; }
	inline G4double GetScintDimensionY() const
	{ return fScintDimensionY; }
	inline G4double GetScintDimensionZ() const
	{ return fScintDimensionZ; }
	inline G4double GetScintBufferThickness() const
	{ return fScintBufferThickness; }
	void SetScintVis(G4int value)
	{ fScintVis = static_cast<ScintVis>(value); }
	inline ScintVis GetScintVis() const
	{ return fScintVis; }
	void SetPMTModuleVis(G4int value)
	{ fPmtVis = static_cast<PmtVis>(value); }
	inline PmtVis GetPMTModuleVis() const
	{ return fPmtVis; }
	void SetPMTPlacement(G4bool value)
	{ fPmtsArePlaced = value; }
	inline G4bool GetPMTPlacement() const
	{ return fPmtsArePlaced; }
	inline G4double GetPixelPitch() const
	{ return fPixelPitch; }
	inline G4double GetPhotocathodeFaceDimension() const
	{ return fPhotocathodeFaceDimension; }
	inline G4double GetTopOfPmtInnerVacuumExtentInZ() const
	{ return fTopOfPmtInnerVacuumExtentInZ; }
	inline G4double GetBottomOfPmtInnerVacuumExtentInZ() const
	{ return fBottomOfPmtInnerVacuumExtentInZ; }

public:
	G4VPhysicalVolume* Construct();
	void DefineMaterials();
	void SetScintMaterial();
	void SetupGeometry();
	void SetupPMTGeometries();
	void SetupPeripheralGeometries();
	void CreatePMTs();
	void SetVisAttributes();
	void SetMaterialProperties();
	void SetAirMaterialProperties();
	void SetScintMaterialProperties();
	void SetPmtMaterialProperties();
	void UpdateGeometry();
	G4double GetEJ254Density(G4double dopingFraction) const;

private:
	// World dimensions.
	G4double fWorldHight;
	G4double fWorldWidthX;
	G4double fWorldWidthY;

	// Detector dimensions.
	G4double fPmtGlassThicknessAtFaceAndRear;
	G4double fPmtInnerVacuumDepth;
	G4double fPhotocathodeFaceDimension;
	G4double fPixelPitch;
	G4double fScintDimensionX, fScintDimensionY, fScintDimensionZ;
	G4double fScintBufferThickness;

	// Z coordinate positions of the top and bottom of the vacuum 1 volum.
	G4double fTopOfPmtInnerVacuumExtentInZ;
	G4double fBottomOfPmtInnerVacuumExtentInZ;

private:

	//
	// Parameters.
	//
	G4String	fCadFilesPath;	// directory path where CAD files are located.
	G4bool		fPmtsArePlaced; // Flag to place PMTs on scintillator surfaces.
	G4bool		fFrameIsPlaced; // Flag to place support frame/clamps.
	G4String	fNeutronCaptureMaterial;
	G4bool		fDopantIsEnriched;
	G4double	fDopingFraction;
	ScintVis	fScintVis;
	PmtVis		fPmtVis;
	PeripheralGeomeotryVis fPeripheralGeometryVis;
	G4bool		fNeutronHpThermalScatteringUsed;		

	//
	// Choices of scint types.
	//
	G4Material* fEJ254_OnePercentNaturalBoronDoped;
	G4Material* fEJ254_TwoAndHalfPercentNaturalBoronDoped;
	G4Material* fEJ254_FivePercentNaturalBoronDoped;
	G4Material* fEJ254_OnePercentEnrichedBoronDoped;
	G4Material* fEJ254_TwoAndHalfPercentEnrichedBoronDoped;
	G4Material* fEJ254_FivePercentEnrichedBoronDoped;
	G4Material* fEJ254_OnePercentNaturalLithiumDoped;
	G4Material* fEJ254_TwoAndHalfPercentNaturalLithiumDoped;
	G4Material* fEJ254_FivePercentNaturalLithiumDoped;
	G4Material* fEJ254_OnePercentEnrichedLithiumDoped;
	G4Material* fEJ254_TwoAndHalfPercentEnrichedLithiumDoped;
	G4Material* fEJ254_FivePercentEnrichedLithiumDoped;

	//
	// Materials comprising detector.
	//
	G4Material* fScintMaterial;
	G4Material* fAir;
	G4Material* fPmtVacuum;
	G4Material* fSchott8337B;
	G4Material* fPmtCircuitBoard;
	G4Material* fPmtBlackWrapping;
	G4Material* fBialkaliPhotocathode;
	G4Material* fDelrin;
	G4Material* fAluminum;

	//
	// Logical and physical volumes.
	//
	G4Box* fWorldSolid;
	G4LogicalVolume* fWorldLogical;
	G4VPhysicalVolume* fWorldPhysical;

	G4Box* fScintSolid;
	G4LogicalVolume* fScintLogical;
	G4VPhysicalVolume* fScintPhysical;

	G4Box* fPmtBoundingVolumeSolid;
	G4LogicalVolume* fPmtBoundingVolumeLogical;
	G4VPhysicalVolume* fPmtBoundingVolume1Physical;
	G4VPhysicalVolume* fPmtBoundingVolume2Physical;
	G4VPhysicalVolume* fPmtBoundingVolume3Physical;
	G4VPhysicalVolume* fPmtBoundingVolume4Physical;
	G4VPhysicalVolume* fPmtBoundingVolume5Physical;
	G4VPhysicalVolume* fPmtBoundingVolume6Physical;
	G4VPhysicalVolume* fPmtBoundingVolume7Physical;
	G4VPhysicalVolume* fPmtBoundingVolume8Physical;
	G4VPhysicalVolume* fPmtBoundingVolume9Physical;
	G4VPhysicalVolume* fPmtBoundingVolume10Physical;
	G4VPhysicalVolume* fPmtBoundingVolume11Physical;
	G4VPhysicalVolume* fPmtBoundingVolume12Physical;
	G4VPhysicalVolume* fPmtBoundingVolume13Physical;
	G4VPhysicalVolume* fPmtBoundingVolume14Physical;
	G4VPhysicalVolume* fPmtBoundingVolume15Physical;
	G4VPhysicalVolume* fPmtBoundingVolume16Physical;
	G4VPhysicalVolume* fPmtBoundingVolume17Physical;
	G4VPhysicalVolume* fPmtBoundingVolume18Physical;
	G4VPhysicalVolume* fPmtBoundingVolume19Physical;
	G4VPhysicalVolume* fPmtBoundingVolume20Physical;
	G4VPhysicalVolume* fPmtBoundingVolume21Physical;
	G4VPhysicalVolume* fPmtBoundingVolume22Physical;
	G4VPhysicalVolume* fPmtBoundingVolume23Physical;
	G4VPhysicalVolume* fPmtBoundingVolume24Physical;

	G4Box* fPmtGlassHousingSolid;
	G4LogicalVolume* fPmtGlassHousingLogical;
	G4VPhysicalVolume* fPmtGlassHousingPhysical;

	G4Box* fPmtGlassWindowSolid;
	G4LogicalVolume* fPmtGlassWindowLogical;
	G4VPhysicalVolume* fPmtGlassWindowPhysical;

	G4Box* fPmtRearBoardSolid;
	G4LogicalVolume* fPmtRearBoardLogical;
	G4VPhysicalVolume* fPmtRearBoardPhysical;

	G4SubtractionSolid* fPmtBlackWrappingSolid;
	G4LogicalVolume* fPmtBlackWrappingLogical;
	G4VPhysicalVolume* fPmtBlackWrappingPhysical;

	G4Box* fPmtInnerVacuumSolid;
	G4LogicalVolume* fPmtInnerVacuumLogical;
	G4VPhysicalVolume* fPmtInnerVacuumPhysical;

	G4Box* fPmtOuterVacuumSolid;
	G4LogicalVolume* fPmtOuterVacuumLogical;
	G4VPhysicalVolume* fPmtOuterVacuumPhysical;

	G4Box* fDynodeSolid;
	G4LogicalVolume* fDynodeLogical;
	G4VPhysicalVolume* fDynodePhysical;

	//
	// Detector peripheral geometries.
	//

	// End plates.
	G4VSolid* fFrameEndPlateSolid0;
	G4VSolid* fFrameEndPlateSolid1;
	G4VSolid* fFrameEndPlateSolid[2];
	G4LogicalVolume* fFrameEndPlateLogical0;
	G4LogicalVolume* fFrameEndPlateLogical1;
	G4LogicalVolume* fFrameEndPlateLogical[2];
	G4VPhysicalVolume* fFrameEndPlatePhysical0;
	G4VPhysicalVolume* fFrameEndPlatePhysical1;
	G4VPhysicalVolume* fFrameEndPlatePhysical[2];

	// Side plates.
	G4VSolid* fFrameSidePlateSolid[4];
	G4LogicalVolume* fFrameSidePlateLogical[4];
	G4VPhysicalVolume* fFrameSidePlatePhysical[4];

	// End clamps.
	G4VSolid* fFrameEndClampSolid[8];
	G4LogicalVolume* fFrameEndClampLogical[8];
	G4VPhysicalVolume* fFrameEndClampPhysical[8];

	// Side clamps.
	G4VSolid* fFrameSideClampSolid[16];
	G4LogicalVolume* fFrameSideClampLogical[16];
	G4VPhysicalVolume* fFrameSideClampPhysical[16];

	// Legs.
	G4VSolid* fLegSolid[4];
	G4LogicalVolume* fLegLogical[4];
	G4VPhysicalVolume* fLegPhysical[4];

	//
	// This is used to confirm PMT orientation.
	// Only used for testing purposes.
	//
	//G4Box* testMarkerSolid;
	//G4LogicalVolume* testMarkerLogical;
	//G4VPhysicalVolume* testMarkerPhysical;

	//
	// Sensitive detector.
	//
	G4SDManager* fSDManager;
	static MTCG4PmtSD* fAPmtSD;

	//
	// Messenger.
	//
	MTCG4DetectorMessenger* fMessenger;
};

#endif
