//////////////////////////////////////////////////////////
// MTCG4DetectorConstruction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include <cmath>
#include <cstring>

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RegionStore.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4SurfaceProperty.hh"

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4DetectorMessenger.hh"
//#include "MTCG4PhysicsList.hh"
#include "MTCG4PmtSD.hh"

MTCG4PmtSD* MTCG4DetectorConstruction::fAPmtSD;

MTCG4DetectorConstruction::MTCG4DetectorConstruction()
	// Intialize some pointers. I don't know if this is really necessary.
	: fWorldSolid(0), fWorldLogical(0), fWorldPhysical(0), fScintSolid(0),
	fScintLogical(0), fScintPhysical(0), fPmtBoundingVolumeSolid(0),
	fPmtBoundingVolumeLogical(0),
	fPmtBoundingVolume1Physical(0), fPmtBoundingVolume2Physical(0),
	fPmtBoundingVolume3Physical(0), fPmtBoundingVolume4Physical(0),
	fPmtBoundingVolume5Physical(0), fPmtBoundingVolume6Physical(0),
	fPmtBoundingVolume7Physical(0), fPmtBoundingVolume8Physical(0),
	fPmtBoundingVolume9Physical(0), fPmtBoundingVolume10Physical(0),
	fPmtBoundingVolume11Physical(0), fPmtBoundingVolume12Physical(0),
	fPmtBoundingVolume13Physical(0), fPmtBoundingVolume14Physical(0),
	fPmtBoundingVolume15Physical(0), fPmtBoundingVolume16Physical(0),
	fPmtBoundingVolume17Physical(0), fPmtBoundingVolume18Physical(0),
	fPmtBoundingVolume19Physical(0), fPmtBoundingVolume20Physical(0),
	fPmtBoundingVolume21Physical(0), fPmtBoundingVolume22Physical(0),
	fPmtBoundingVolume23Physical(0), fPmtBoundingVolume24Physical(0),
   	fPmtGlassHousingSolid(0),
	fPmtGlassHousingLogical(0), fPmtGlassHousingPhysical(0),
	fPmtGlassWindowSolid(0),
	fPmtGlassWindowLogical(0), fPmtGlassWindowPhysical(0)
{
	// Detector variables.
	fDopant = boron;
	fDopantIsEnriched = false;
	fDopingFraction = 1.*perCent;
	fCadFilesPath = "mtc_cad/";
	//fPmtsArePlaced = true; // Flag to place PMTs on scintillator surfaces.
	fPmtsArePlaced = false; // Flag to place PMTs on scintillator surfaces.
	//fFrameIsPlaced = true;
	fFrameIsPlaced = false;
	fNeutronHpThermalScatteringUsed = true;
	//fNeutronHpThermalScatteringUsed =
	//	dynamic_cast<MTCG4PhysicsList*>(G4RunManager::GetRunManager()->GetUserPhysicsList())
	//	->GetNeutronHPThermalScattering();

	// Store some geometry values.
	fScintDimensionX = fScintDimensionY = fScintDimensionZ =
		SCINT_DIMENSION;
	fScintBufferThickness = SCINT_BUFFER_THICKNESS;
	fPhotocathodeFaceDimension = PHOTOCATHODE_FACE_DIMENSION;
	fPixelPitch = PMT_PIXEL_FACE_PITCH;
	fTopOfPmtInnerVacuumExtentInZ =
		.5*PMT_GLASS_HOUSING_DEPTH - PMT_GLASS_THICKNESS_AT_FACE_AND_REAR;
	fBottomOfPmtInnerVacuumExtentInZ =
		.5*PMT_GLASS_HOUSING_DEPTH - PMT_GLASS_THICKNESS_AT_FACE_AND_REAR -
		PMT_INNER_VACUUM_DEPTH;

	// Visualization flags.
	fScintVis = scintVisGray;
	fPmtVis = pmtVisGray;
	fPeripheralGeometryVis = peripheralVisOff;
	//fPeripheralGeometryVis = peripheralVisGray;

 	// Construct detector messenger.
	fMessenger = new MTCG4DetectorMessenger(this);
	//G4cout << "fNeutronHpThermalScatteringUsed = " << fNeutronHpThermalScatteringUsed << G4endl;

	// Material definitions.
	DefineMaterials();
	SetScintMaterial(); // Default material used for scintillator.
}

MTCG4DetectorConstruction::~MTCG4DetectorConstruction()
{
	delete fMessenger;
}

G4VPhysicalVolume* MTCG4DetectorConstruction::Construct()
{
	G4cout << "fDopantIsEnriched = " << fDopantIsEnriched << G4endl;
	G4cout << "fDopingFraction = " << fDopingFraction << G4endl;
	//G4cout << "fNeutronHpThermalScatteringUsed = " << fNeutronHpThermalScatteringUsed << G4endl;
	
	// Geometry definitions.
	SetupGeometry();

	// Geometry surface properties.
	SetMaterialProperties();

	// Visualization attributes.
	SetVisAttributes();

	return fWorldPhysical;
} 

//
// Detector geometry.
//
void MTCG4DetectorConstruction::SetupGeometry()
{
	// Create world.
	SetupWorld();

	// Create scintillator.
	SetupScintillator();

	// Create PMTs.
	if (fPmtsArePlaced) SetupPmtGeometries();

	// Create support frame/clams stand rods, etc.
	if (fFrameIsPlaced) SetupPeripheralGeometries();
}// SetupGeometry().

//
// Material properties such as refractive index and photocathode optical
// properties.
// Important note: this needs to be done after the definitions of materials, as
// I have done in this code, in order to work.
// All optical properties are handled within the vacuum photon wavelengths of
// 200~800 nm. I think this range should be kept consistent throughout the code
// for all materials because we don't want weird things going on when a photon
// travels inside a medium where the optical properties for the photon's
// energy is not defined. Extrapolation using best guesses are used in the case
// no optical properties data can be found in literature.
// Look inside each implementation file for details.
void MTCG4DetectorConstruction::SetMaterialProperties() {
	SetAirMaterialProperties();
	if (fPmtsArePlaced) SetPmtMaterialProperties();
	SetScintMaterialProperties();
}

// Currently gives seg-fault. Fix later.
void MTCG4DetectorConstruction::UpdateGeometry()
{
	//G4cout << "fWorldPhysical = " << fWorldPhysical << G4endl;
	//if (!fWorldPhysical) return;

	////
	//// Clean old geometry, if any.
	////
	G4GeometryManager::GetInstance()->OpenGeometry();
	//G4cout << "geometry is closed = " << G4GeometryManager::GetInstance()->
	//	IsGeometryClosed() << G4endl;

	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	////G4RegionStore::GetInstance()->Clean();
	G4LogicalSkinSurface::CleanSurfaceTable();
	G4LogicalBorderSurface::CleanSurfaceTable();
	//G4SurfaceProperty::CleanSurfacePropertyTable();
	//G4cout << "fAPmtSD: " << fAPmtSD << G4endl;
	//if (fAPmtSD) delete fAPmtSD;
	//if (fSDManager) delete fSDManager;

	//if (fWorldSolid) delete fWorldSolid;
	//if (fWorldLogical) delete fWorldLogical;
	//if (fWorldPhysical) delete fWorldPhysical;

	//if (fWorldSolid) {
	//	G4cout << "fWorldSolid exists.\n";
	// 	delete fWorldSolid;
	//}
	//if (fWorldLogical) {
	//	G4cout << "fWorldLogical exists.\n";
	// 	delete fWorldLogical;
	//}
	//if (fWorldPhysical) {
	//	G4cout << "fWorldPhysical exists.\n";
	// 	delete fWorldPhysical;
	//}

	// Delete left-over pointers? Is this really needed?
	//if (fScintSolid) delete fScintSolid;
	//if (fScintLogical) delete fScintLogical;
	//if (fScintPhysical) delete fScintPhysical;

	//if (fPmtBoundingVolumeSolid) delete fPmtBoundingVolumeSolid;
	//if (fPmtBoundingVolumeLogical) delete fPmtBoundingVolumeLogical;
	//if (fPmtBoundingVolume1Physical) delete fPmtBoundingVolume1Physical;
	//if (fPmtBoundingVolume2Physical) delete fPmtBoundingVolume2Physical;
	//if (fPmtBoundingVolume3Physical) delete fPmtBoundingVolume3Physical;
	//if (fPmtBoundingVolume4Physical) delete fPmtBoundingVolume4Physical;
	//if (fPmtBoundingVolume5Physical) delete fPmtBoundingVolume5Physical;
	//if (fPmtBoundingVolume6Physical) delete fPmtBoundingVolume6Physical;
	//if (fPmtBoundingVolume7Physical) delete fPmtBoundingVolume7Physical;
	//if (fPmtBoundingVolume8Physical) delete fPmtBoundingVolume8Physical;
	//if (fPmtBoundingVolume9Physical) delete fPmtBoundingVolume9Physical;
	//if (fPmtBoundingVolume10Physical) delete fPmtBoundingVolume10Physical;
	//if (fPmtBoundingVolume11Physical) delete fPmtBoundingVolume11Physical;
	//if (fPmtBoundingVolume12Physical) delete fPmtBoundingVolume12Physical;
	//if (fPmtBoundingVolume13Physical) delete fPmtBoundingVolume13Physical;
	//if (fPmtBoundingVolume14Physical) delete fPmtBoundingVolume14Physical;
	//if (fPmtBoundingVolume15Physical) delete fPmtBoundingVolume15Physical;
	//if (fPmtBoundingVolume16Physical) delete fPmtBoundingVolume16Physical;
	//if (fPmtBoundingVolume17Physical) delete fPmtBoundingVolume17Physical;
	//if (fPmtBoundingVolume18Physical) delete fPmtBoundingVolume18Physical;
	//if (fPmtBoundingVolume19Physical) delete fPmtBoundingVolume19Physical;
	//if (fPmtBoundingVolume20Physical) delete fPmtBoundingVolume20Physical;
	//if (fPmtBoundingVolume21Physical) delete fPmtBoundingVolume21Physical;
	//if (fPmtBoundingVolume22Physical) delete fPmtBoundingVolume22Physical;
	//if (fPmtBoundingVolume23Physical) delete fPmtBoundingVolume23Physical;
	//if (fPmtBoundingVolume24Physical) delete fPmtBoundingVolume24Physical;

	//if (fPmtGlassHousingSolid) delete fPmtGlassHousingSolid;
	//if (fPmtGlassHousingLogical) delete fPmtGlassHousingLogical;
	//if (fPmtGlassHousingPhysical) delete fPmtGlassHousingPhysical;

	//if (fPmtGlassWindowSolid) delete fPmtGlassWindowSolid;
	//if (fPmtGlassWindowLogical) delete fPmtGlassWindowLogical;
	//if (fPmtGlassWindowPhysical) delete fPmtGlassWindowPhysical;

	//if (fPmtRearBoardSolid) delete fPmtRearBoardSolid;
	//if (fPmtRearBoardLogical) delete fPmtRearBoardLogical;
	//if (fPmtRearBoardPhysical) delete fPmtRearBoardPhysical;

	//if (fPmtBlackWrappingSolid) delete fPmtBlackWrappingSolid;
	//if (fPmtBlackWrappingLogical) delete fPmtBlackWrappingLogical;
	//if (fPmtBlackWrappingPhysical) delete fPmtBlackWrappingPhysical;

	//if (fPmtInnerVacuumSolid) delete fPmtInnerVacuumSolid;
	//if (fPmtInnerVacuumLogical) delete fPmtInnerVacuumLogical;
	//if (fPmtInnerVacuumPhysical) delete fPmtInnerVacuumPhysical;

	//if (fDynodeSolid) delete fDynodeSolid;
	//if (fDynodeLogical) delete fDynodeLogical;
	//if (fDynodePhysical) delete fDynodePhysical;

	//if (fPmtOuterVacuumSolid) delete fPmtOuterVacuumSolid;
	//if (fPmtOuterVacuumLogical) delete fPmtOuterVacuumLogical;
	//if (fPmtOuterVacuumPhysical) delete fPmtOuterVacuumPhysical;

	// Define new ones.
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	G4RegionStore::GetInstance()->UpdateMaterialList(fWorldPhysical);
}
