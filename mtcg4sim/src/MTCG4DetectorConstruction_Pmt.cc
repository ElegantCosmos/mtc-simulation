#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4PmtOpticalModel.hh"

//
// Some detector geometry constants and other constants.
//

// (mm) Length of one side of PMT glass housing face.
const G4double PMT_GLASS_HOUSING_FACE_DIMENSION = 59.*mm;

// (mm) Thickness of PMT.
const G4double PMT_DEPTH = 21.3*mm;
//const G4double PMT_DEPTH = 22.*mm;

// (mm) Radius of curvature for rounded off edges of PMT body.
const G4double PMT_EDGE_RADIUS = 2.*mm; // Not used for now.

// (mm) Glass thickness around edge of PMT face that is not the active area.
const G4double PMT_GLASS_THICKNESS_AT_PMT_EDGE = 3.*mm;

// (mm) Thickness of glass at PMT front and rear faces.
const G4double PMT_GLASS_THICKNESS_AT_FACE_AND_REAR = 2.032*mm;

// (mm) Thickness of black wrapping around edge of PMT module.
const G4double PMT_BLACK_WRAPPING_THICKNESS = 0.2*mm;

// (mm) Dimensions of PMT bounding volume (box).
const G4double PMT_BOUNDING_VOLUME_FACE_DIMENSION =
PMT_GLASS_HOUSING_FACE_DIMENSION + 2.*PMT_BLACK_WRAPPING_THICKNESS;
const G4double PMT_BOUNDING_VOLUME_DEPTH = PMT_DEPTH + 1.*mm;

// (mm) Dimensions of rear circuit board of PMT module.
const G4double PMT_REARBOARD_FACE_DIMENSION = PMT_GLASS_HOUSING_FACE_DIMENSION;
const G4double PMT_REARBOARD_THICKNESS = 2.*mm;

// (mm) PMT pixel face dimension.
const G4double PMT_PIXEL_FACE_DIMENSION = 5.9*mm;

// (mm) PMT pixel pitch (space between pixel centers).
const G4double PMT_PIXEL_FACE_PITCH = 6.5*mm;

// (mm) PMT glass housing.
const G4double PMT_GLASS_HOUSING_DEPTH = PMT_DEPTH - PMT_REARBOARD_THICKNESS;

// (mm) Photocathode dimensions.
const G4double PHOTOCATHODE_FACE_DIMENSION = 8*PMT_PIXEL_FACE_PITCH;

// (mm) Dimensions of the vacuum volume inside PMT module that touches
// photocathode surface.
const G4double PMT_INNER_VACUUM_FACE_DIMENSION = PHOTOCATHODE_FACE_DIMENSION;
const G4double PMT_INNER_VACUUM_DEPTH = 3.*mm;

// (mm) Dimensions of the total vacuum volume inside PMT module glass housing.
const G4double PMT_OUTER_VACUUM_FACE_DIMENSION =
PMT_GLASS_HOUSING_FACE_DIMENSION - 2*PMT_GLASS_THICKNESS_AT_PMT_EDGE;
const G4double PMT_OUTER_VACUUM_DEPTH = PMT_DEPTH - PMT_REARBOARD_THICKNESS -
2*PMT_GLASS_THICKNESS_AT_FACE_AND_REAR;

// (mm) Dynode dimensions.
const G4double DYNODE_FACE_DIMENSION = PHOTOCATHODE_FACE_DIMENSION;
const G4double DYNODE_THICKNESS = 4.*mm;

// Some constants for PMT placement.
const G4double PMT_PLACEMENT_NORMAL_TO_CUBEFACE =
.5*SCINT_DIMENSION + .5*PMT_BOUNDING_VOLUME_DEPTH + SCINT_BUFFER_THICKNESS;
// 0.15 millimeter of PMT placement tolerance will be added to simulate the
// "real world" error of actually placing a physical PMT.
const G4double PMT_PLACEMENT_PARALLEL_TO_CUBEFACE =
.5*PMT_BOUNDING_VOLUME_FACE_DIMENSION + .15*mm;

//
// SetupPmtGeometries()
// Used by MTCG4DetectorConstruction::SetupGeometries()
//
void MTCG4DetectorConstruction::SetupPmtGeometries()
{
	G4NistManager* nist = G4NistManager::Instance();
	G4Material* worldMaterial =
		nist->FindOrBuildMaterial("G4_AIR");
	G4Material* vacuumMaterial =
		nist->FindOrBuildMaterial("PMT_Vacuum");
	// Photocathode material not used since replacing photoelectric process with
	// MTCG4PmtOpticalModel.
	//G4Material* photocathodeMaterial =
	//	nist->FindOrBuildMaterial("Bialkali_Photocathode");
	G4Material* pmtGlassMaterial =
		nist->FindOrBuildMaterial("SCHOTT_8337B");
	// The dynode material is set to be the same "glass" as the PMT bulk glass.
	G4Material* dynodeMaterial =
		nist->FindOrBuildMaterial("SCHOTT_8337B");
	G4Material* pmtPcbMaterial =
		nist->FindOrBuildMaterial("PMT_PCB"); // PMT Printed Circuit Board.
	G4Material* pmtBlackWrappingMaterial =
		nist->FindOrBuildMaterial("PMT_Black_Wrapping");

	//// Test marker volume. This is used to confirm PMT orientation.
	//testMarkerSolid = new G4Box(
	//		"testSolid",
	//	 	PHOTOCATHODE_HALF_THICKNESS,
	//		PHOTOCATHODE_HALF_THICKNESS,
	//	 	PHOTOCATHODE_HALF_THICKNESS);
	//testMarkerLogical = new G4LogicalVolume(
	//		testMarkerSolid,
	//	 	PMTMaterial,
	//		"testLogical");
	//testMarkerPhysical = new G4PVPlacement(
	//		NULL,
	//	 	G4ThreeVector(
	//			.5*PMT_GLASS_HOUSING_FACE_DIMENSION-PHOTOCATHODE_HALF_THICKNESS,
	//		 	.5*PMT_GLASS_HOUSING_FACE_DIMENSION-PHOTOCATHODE_HALF_THICKNESS,
	//		 	.5*PMT_DEPTH-PHOTOCATHODE_HALF_THICKNESS),
	//	 	testMarkerLogical,
	//	 	"testPhysical",
	//	 	fPmtBoundingVolumeLogical,
	//	 	false,
	//	 	0);

	// Here we create a template logical volume for the PMT bounding volume that
	// we can "copy-and-paste" to replicate all the PMTs deployed in the detector.
	// "pmt" means one Planacon multi-channel plate photomultiplier tube XP85012
	// by Photonis.
	fPmtBoundingVolumeSolid = new G4Box(
			"pmtBoundingVolumeSolid",
			.5*PMT_BOUNDING_VOLUME_FACE_DIMENSION,
			.5*PMT_BOUNDING_VOLUME_FACE_DIMENSION,
			.5*PMT_BOUNDING_VOLUME_DEPTH);
 	fPmtBoundingVolumeLogical = new G4LogicalVolume(
			fPmtBoundingVolumeSolid,
			worldMaterial,
			"pmtBoundingVolumeLogical");

	// Actual physical volumes of PMT bounding volumes replicated here. The PMT
	// numbering and their order for placement follows Shige's proposed
	// convention. Please refer to his diagram for details. Names given to each
	// PMT follow the format "pmtNPhysical_S_A_B", where N is the PMT ID number
	// ranging from 1 through 24, S is an integer ranging from 1 through 6
	// identifying the surface number of the scintillating cube, and A and B stand
	// for two coordinate numbers each being either a 1 or a 2. A and B are the
	// two meaningful coordinate numbers for the x, y, or z coordinates in this
	// order. For more details please see file PMTsDatabase.ods in
	// hanohano@hanohano-pc2:~/mTC/ directory.

	// PMT 1.
	G4RotationMatrix pmt1RotationMatrix;
	pmt1RotationMatrix.rotateZ(pi*rad);
	G4ThreeVector pmt1TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume1Physical = new G4PVPlacement(
			G4Transform3D(pmt1RotationMatrix,	pmt1TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt1Physical_1_1_1",
			fWorldLogical,
			false,
			0
			);

	// PMT 2.
	G4RotationMatrix pmt2RotationMatrix;
	pmt2RotationMatrix.rotateZ(pi*rad);
	G4ThreeVector pmt2TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume2Physical = new G4PVPlacement(
			G4Transform3D(pmt2RotationMatrix,	pmt2TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt2Physical_1_2_1",
			fWorldLogical,
			false,
			1
			);

	// PMT 3.
	G4RotationMatrix pmt3RotationMatrix; // No rotation.
	G4ThreeVector pmt3TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume3Physical = new G4PVPlacement(
			G4Transform3D(pmt3RotationMatrix,	pmt3TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt3Physical_1_1_2",
			fWorldLogical,
			false,
			2
			);

	// PMT 4.
	G4RotationMatrix pmt4RotationMatrix; // No rotation.
	G4ThreeVector pmt4TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume4Physical = new G4PVPlacement(
			G4Transform3D(pmt4RotationMatrix,	pmt4TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt4Physical_1_2_2",
			fWorldLogical,
			false,
			3
			);

	// PMT 5.
	G4RotationMatrix pmt5RotationMatrix;
	pmt5RotationMatrix.rotateX(-.5*pi*rad);
	G4ThreeVector pmt5TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume5Physical = new G4PVPlacement(
			G4Transform3D(pmt5RotationMatrix,	pmt5TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt5Physical_2_1_1",
			fWorldLogical,
			false,
			4
			);

	// PMT 6.
	G4RotationMatrix pmt6RotationMatrix;
	pmt6RotationMatrix.rotateX(-.5*pi*rad);
	G4ThreeVector pmt6TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume6Physical = new G4PVPlacement(
			G4Transform3D(pmt6RotationMatrix,	pmt6TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt6Physical_2_2_1",
			fWorldLogical,
			false,
			5
			);

	// PMT 7.
	G4RotationMatrix pmt7RotationMatrix;
	pmt7RotationMatrix.rotateZ(pi*rad);
	pmt7RotationMatrix.rotateX(-.5*pi*rad);
	G4ThreeVector pmt7TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume7Physical = new G4PVPlacement(
			G4Transform3D(pmt7RotationMatrix,	pmt7TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt7Physical_2_1_2",
			fWorldLogical,
			false,
			6
			);

	// PMT 8.
	G4RotationMatrix pmt8RotationMatrix;
	pmt8RotationMatrix.rotateZ(pi*rad);
	pmt8RotationMatrix.rotateX(-.5*pi*rad);
	G4ThreeVector pmt8TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume8Physical = new G4PVPlacement(
			G4Transform3D(pmt8RotationMatrix,	pmt8TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt8Physical_2_2_2",
			fWorldLogical,
			false,
			7
			);

	// PMT 9.
	G4RotationMatrix pmt9RotationMatrix;
	pmt9RotationMatrix.rotateZ(-.5*pi*rad);
	pmt9RotationMatrix.rotateY(.5*pi*rad);
	G4ThreeVector pmt9TranslationVector(
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume9Physical = new G4PVPlacement(
			G4Transform3D(pmt9RotationMatrix,	pmt9TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt9Physical_3_1_1",
			fWorldLogical,
			false,
			8
			);

	// PMT 10.
	G4RotationMatrix pmt10RotationMatrix;
	pmt10RotationMatrix.rotateZ(-.5*pi*rad);
	pmt10RotationMatrix.rotateY(.5*pi*rad);
	G4ThreeVector pmt10TranslationVector(
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume10Physical = new G4PVPlacement(
			G4Transform3D(pmt10RotationMatrix,	pmt10TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt10Physical_3_2_1",
			fWorldLogical,
			false,
			9
			);

	// PMT 11.
	G4RotationMatrix pmt11RotationMatrix;
	pmt11RotationMatrix.rotateZ(.5*pi*rad);
	pmt11RotationMatrix.rotateY(.5*pi*rad);
	G4ThreeVector pmt11TranslationVector(
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume11Physical = new G4PVPlacement(
			G4Transform3D(pmt11RotationMatrix,	pmt11TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt11Physical_3_1_2",
			fWorldLogical,
			false,
			10
			);

	// PMT 12.
	G4RotationMatrix pmt12RotationMatrix;
	pmt12RotationMatrix.rotateZ(.5*pi*rad);
	pmt12RotationMatrix.rotateY(.5*pi*rad);
	G4ThreeVector pmt12TranslationVector(
			-PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume12Physical = new G4PVPlacement(
			G4Transform3D(pmt12RotationMatrix,	pmt12TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt12Physical_3_2_2",
			fWorldLogical,
			false,
			11
			);

	// PMT 13.
	G4RotationMatrix pmt13RotationMatrix;
	pmt13RotationMatrix.rotateZ(.5*pi*rad);
	pmt13RotationMatrix.rotateY(-.5*pi*rad);
	G4ThreeVector pmt13TranslationVector(
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume13Physical = new G4PVPlacement(
			G4Transform3D(pmt13RotationMatrix,	pmt13TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt13Physical_4_1_1",
			fWorldLogical,
			false,
			12
			);

	// PMT 14.
	G4RotationMatrix pmt14RotationMatrix;
	pmt14RotationMatrix.rotateZ(.5*pi*rad);
	pmt14RotationMatrix.rotateY(-.5*pi*rad);
	G4ThreeVector pmt14TranslationVector(
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume14Physical = new G4PVPlacement(
			G4Transform3D(pmt14RotationMatrix,	pmt14TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt14Physical_4_2_1",
			fWorldLogical,
			false,
			13
			);

	// PMT 15.
	G4RotationMatrix pmt15RotationMatrix;
	pmt15RotationMatrix.rotateZ(-.5*pi*rad);
	pmt15RotationMatrix.rotateY(-.5*pi*rad);
	G4ThreeVector pmt15TranslationVector(
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume15Physical = new G4PVPlacement(
			G4Transform3D(pmt15RotationMatrix,	pmt15TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt15Physical_4_1_2",
			fWorldLogical,
			false,
			14
			);

	// PMT 16.
	G4RotationMatrix pmt16RotationMatrix;
	pmt16RotationMatrix.rotateZ(-.5*pi*rad);
	pmt16RotationMatrix.rotateY(-.5*pi*rad);
	G4ThreeVector pmt16TranslationVector(
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume16Physical = new G4PVPlacement(
			G4Transform3D(pmt16RotationMatrix,	pmt16TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt16Physical_4_2_2",
			fWorldLogical,
			false,
			15
			);

	// PMT 17.
	G4RotationMatrix pmt17RotationMatrix;
	pmt17RotationMatrix.rotateZ(pi*rad);
	pmt17RotationMatrix.rotateX(.5*pi*rad);
	G4ThreeVector pmt17TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume17Physical = new G4PVPlacement(
			G4Transform3D(pmt17RotationMatrix,	pmt17TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt17Physical_5_1_1",
			fWorldLogical,
			false,
			16
			);

	// PMT 18.
	G4RotationMatrix pmt18RotationMatrix;
	pmt18RotationMatrix.rotateZ(pi*rad);
	pmt18RotationMatrix.rotateX(.5*pi*rad);
	G4ThreeVector pmt18TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume18Physical = new G4PVPlacement(
			G4Transform3D(pmt18RotationMatrix,	pmt18TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt18Physical_5_2_1",
			fWorldLogical,
			false,
			17
			);

	// PMT 19.
	G4RotationMatrix pmt19RotationMatrix;
	pmt19RotationMatrix.rotateX(.5*pi*rad);
	G4ThreeVector pmt19TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume19Physical = new G4PVPlacement(
			G4Transform3D(pmt19RotationMatrix,	pmt19TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt19Physical_5_1_2",
			fWorldLogical,
			false,
			18
			);

	// PMT 20.
	G4RotationMatrix pmt20RotationMatrix;
	pmt20RotationMatrix.rotateX(.5*pi*rad);
	G4ThreeVector pmt20TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE
			);
	fPmtBoundingVolume20Physical = new G4PVPlacement(
			G4Transform3D(pmt20RotationMatrix,	pmt20TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt20Physical_5_2_2",
			fWorldLogical,
			false,
			19
			);

	// PMT 21.
	G4RotationMatrix pmt21RotationMatrix;
	pmt21RotationMatrix.rotateX(pi*rad);
	G4ThreeVector pmt21TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume21Physical = new G4PVPlacement(
			G4Transform3D(pmt21RotationMatrix,	pmt21TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt21Physical_6_1_1",
			fWorldLogical,
			false,
			20
			);

	// PMT 22.
	G4RotationMatrix pmt22RotationMatrix;
	pmt22RotationMatrix.rotateX(pi*rad);
	G4ThreeVector pmt22TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume22Physical = new G4PVPlacement(
			G4Transform3D(pmt22RotationMatrix,	pmt22TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt22Physical_6_2_1",
			fWorldLogical,
			false,
			21
			);

	// PMT 23.
	G4RotationMatrix pmt23RotationMatrix;
	pmt23RotationMatrix.rotateZ(pi*rad);
	pmt23RotationMatrix.rotateX(pi*rad);
	G4ThreeVector pmt23TranslationVector(
			-PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume23Physical = new G4PVPlacement(
			G4Transform3D(pmt23RotationMatrix,	pmt23TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt23Physical_6_1_2",
			fWorldLogical,
			false,
			22
			);

	// PMT 24.
	G4RotationMatrix pmt24RotationMatrix;
	pmt24RotationMatrix.rotateZ(pi*rad);
	pmt24RotationMatrix.rotateX(pi*rad);
	G4ThreeVector pmt24TranslationVector(
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_PARALLEL_TO_CUBEFACE,
			+PMT_PLACEMENT_NORMAL_TO_CUBEFACE
			);
	fPmtBoundingVolume24Physical = new G4PVPlacement(
			G4Transform3D(pmt24RotationMatrix,	pmt24TranslationVector),
			fPmtBoundingVolumeLogical,
			"pmt24Physical_6_2_2",
			fWorldLogical,
			false,
			23
			);

	// PMT glass housing.
	fPmtGlassHousingSolid = new G4Box(
			"pmtGlassHousingSolid",
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION,
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION,
			.5*PMT_GLASS_HOUSING_DEPTH);
	fPmtGlassHousingLogical = new G4LogicalVolume(
			fPmtGlassHousingSolid,
			pmtGlassMaterial,
			"pmtGlassHousingLogical");
	fPmtGlassHousingPhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,
				.5*PMT_BOUNDING_VOLUME_DEPTH-.5*PMT_GLASS_HOUSING_DEPTH),
			fPmtGlassHousingLogical,
			"pmtGlassHousingPhysical",
			fPmtBoundingVolumeLogical,
			false,
			0);

	// PMT glass window on which the photocathode material is attached to.
	// This glass window is actually a part of the glass housing but it is
	// defined seperately like this to enable the photocathode surface to exist
	// between two volumes: "pmtGlassWindowPhysical" and
	// "pmtInnerVacuumPhysical".
	fPmtGlassWindowSolid = new G4Box(
		"pmtGlassWindowSolid",
		.5*PMT_INNER_VACUUM_FACE_DIMENSION,
		.5*PMT_INNER_VACUUM_FACE_DIMENSION,
		.5*PMT_GLASS_THICKNESS_AT_FACE_AND_REAR);
	fPmtGlassWindowLogical = new G4LogicalVolume(
		fPmtGlassWindowSolid,
		pmtGlassMaterial,
	 	"pmtGlassWindowLogical");
	fPmtGlassWindowPhysical = new G4PVPlacement(
		NULL,
		G4ThreeVector(0,0,
			.5*PMT_GLASS_HOUSING_DEPTH-.5*PMT_GLASS_THICKNESS_AT_FACE_AND_REAR),
		fPmtGlassWindowLogical,
		"pmtGlassWindowPhysical",
		fPmtGlassHousingLogical,
		false,
		0);

	//// Test marker volume. This is used to confirm PMT orientation.
	//G4VSolid *testMarkerSolid = new G4Box(
	//		"test_solid",
	//	 	0.5*PMT_PIXEL_FACE_DIMENSION,
	//	 	0.5*PMT_PIXEL_FACE_DIMENSION,
	//	 	0.5*PMT_GLASS_THICKNESS_AT_FACE_AND_REAR);
	//G4LogicalVolume *testMarkerLogical = new G4LogicalVolume(
	//		testMarkerSolid,
	//	 	pmtGlassMaterial,
	//		"test_logical");
	//G4VPhysicalVolume *testMarkerPhysical = new G4PVPlacement(
	//		NULL,
	//	 	G4ThreeVector(
	//			.5*PHOTOCATHODE_FACE_DIMENSION,
	//		 	.5*PHOTOCATHODE_FACE_DIMENSION,
	//		 	0),
	//	 	testMarkerLogical,
	//	 	"test_physical",
	//	 	fPmtGlassWindowLogical,
	//	 	false,
	//	 	0);
	//testMarkerLogical->
	//	SetVisAttributes(new G4VisAttributes(G4Colour(1., 0., 0., 1.)));

	// The vacuum volume inside the PMT glass housing that is the mother volume of
	// the inner vacume volume defined below. The vacuum inside a single PMT is
	// divided into two volumes, inner and outer, in order to simulate a very thin
	// layer of zero thickness photocathode material just on the inside of the PMT
	// front glass.  Geant needs to have this layer sandwiched between two
	// physical volumes in order to create it so that is the reason why we divided
	// the vacuum volume into two regions. One to just be the main bulk volume and
	// another specifically to sandwich the photocathode layer with the front
	// glass. Physically they are of the same material and are indistinguishable.
	// It is assumed to be a simple box shape for simplicity.
	fPmtOuterVacuumSolid = new G4Box(
			"pmtOuterVacuumSolid",
			.5*PMT_OUTER_VACUUM_FACE_DIMENSION,
			.5*PMT_OUTER_VACUUM_FACE_DIMENSION,
			.5*PMT_OUTER_VACUUM_DEPTH);
	fPmtOuterVacuumLogical = new G4LogicalVolume(fPmtOuterVacuumSolid,
			vacuumMaterial, "pmtOuterVacuum");
	fPmtOuterVacuumPhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,0),
			fPmtOuterVacuumLogical,
			"pmtOuterVacuumPhysical",
			fPmtGlassHousingLogical,
			false,
			0);

	////// Black wrapping material around glass housing of PMT.
	////// This is assumed to be a uniform film of constant thickness around the
	////// sides of the PMT.
	G4Box* fPmtBlackWrappingOuterSolid = new G4Box(
			"pmtBlackWrappingOuterSolid",
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION + PMT_BLACK_WRAPPING_THICKNESS,
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION + PMT_BLACK_WRAPPING_THICKNESS,
			.5*PMT_DEPTH);
	// 1.*mm extra length needed for subtractor volume to prevent precision
	// loss, or errors in visualization.
	G4Box* fPmtBlackWrappingInnerSolid = new G4Box(
			"pmtBlackWrappingInnerSolid",
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION,
			.5*PMT_GLASS_HOUSING_FACE_DIMENSION,
			.5*PMT_DEPTH + 1.*mm);
	fPmtBlackWrappingSolid = new G4SubtractionSolid(
			"pmtBlackWrappingSolid",
			fPmtBlackWrappingOuterSolid,
			fPmtBlackWrappingInnerSolid);
	fPmtBlackWrappingLogical = new G4LogicalVolume(
			fPmtBlackWrappingSolid,
			pmtBlackWrappingMaterial,
		 	"pmtBlackWrappingLogical");
	fPmtBlackWrappingPhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,.5*PMT_BOUNDING_VOLUME_DEPTH-.5*PMT_DEPTH),
			fPmtBlackWrappingLogical,
			"pmtBlackWrappingPhysical",
			fPmtBoundingVolumeLogical,
			false,
			0);

	//// Circuit board material on the back side of the PMT.
	//// This is assumed to be a uniform thickness of a single material with no
	//// actual circuit path materials nor electrical components whatsoever.
	fPmtRearBoardSolid = new G4Box(
			"pmtRearBoardSolid",
			.5*PMT_REARBOARD_FACE_DIMENSION,
		 	.5*PMT_REARBOARD_FACE_DIMENSION,
			.5*PMT_REARBOARD_THICKNESS);
	fPmtRearBoardLogical = new G4LogicalVolume(
			fPmtRearBoardSolid,
			pmtPcbMaterial,
		 	"pmtRearBoard");
	fPmtRearBoardPhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,
				.5*PMT_BOUNDING_VOLUME_DEPTH-PMT_GLASS_HOUSING_DEPTH-
				.5*PMT_REARBOARD_THICKNESS),
			fPmtRearBoardLogical,
			"pmtRearBoardPhysical",
			fPmtBoundingVolumeLogical,
			false,
			0);

	// Vacuum volume inside PMT glass housing that touches photocathode surface.
	// It is assumed to be a simple box shape for simplicity.
	fPmtInnerVacuumSolid = new G4Box("pmtInnerVacuumSolid",
			.5*PMT_INNER_VACUUM_FACE_DIMENSION,
			.5*PMT_INNER_VACUUM_FACE_DIMENSION,
		 	.5*PMT_INNER_VACUUM_DEPTH);
	fPmtInnerVacuumLogical = new G4LogicalVolume(fPmtInnerVacuumSolid,
			vacuumMaterial, "pmtInnerVacuumLogical");
	fPmtInnerVacuumPhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,.5*PMT_OUTER_VACUUM_DEPTH-.5*PMT_INNER_VACUUM_DEPTH),
			fPmtInnerVacuumLogical,
			"pmtInnerVacuumPhysical",
			fPmtOuterVacuumLogical,
			false,
			0);

	// Dynode multiplier structure inside PMT vacuum.
	// This is a crude representation assummed to be a simple flat box.
	fDynodeSolid = new G4Box("fDynodeSolid", .5*DYNODE_FACE_DIMENSION,
			.5*DYNODE_FACE_DIMENSION, .5*DYNODE_THICKNESS);
	fDynodeLogical = new G4LogicalVolume(fDynodeSolid, dynodeMaterial,
			"fDynodeLogical");
	fDynodePhysical = new G4PVPlacement(
			NULL,
			G4ThreeVector(0,0,0),
			fDynodeLogical,
			"fDynodePhysical",
			fPmtOuterVacuumLogical,
			false,
			0);

	// Sensitive detectors.
	// Get the sensitive detector manager and define fAPmtSD if it is not yet
	// defined. Assign the photocathode to the sensitive detector manager.
	fSDManager = G4SDManager::GetSDMpointer();
	if (!fAPmtSD) {
		G4String MTCG4PmtSDName =
			"/MTCG4PMT/MTCG4PmtSD";
		fAPmtSD = new MTCG4PmtSD( MTCG4PmtSDName );
		fSDManager->AddNewDetector( fAPmtSD );
		fPmtGlassWindowLogical->SetSensitiveDetector( fAPmtSD );
		fPmtInnerVacuumLogical->SetSensitiveDetector( fAPmtSD );
	}
} // SetupPmtGeometries()

//
// Material properties for PMT materials.
//
void MTCG4DetectorConstruction::SetPmtMaterialProperties()
{
	const int kNSpecPointsVacuum = 8;
	// Optical properties of PMT vacuum.
	const G4double wavelengthsForVacuum[kNSpecPointsVacuum] = {
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		300*nm,
		200*nm,
	};
	G4double photonEnergiesForVacuum[kNSpecPointsVacuum];
	// Set order of photon energies for PMT vacuum in increasing order.
	for (G4int i = 0; i < kNSpecPointsVacuum; ++i) {
		photonEnergiesForVacuum[i] =
			h_Planck*c_light/wavelengthsForVacuum[i];
	}
	G4double vacuumRindex[kNSpecPointsVacuum] = {
		1., // 800*nm
		1., // 700*nm
		1., // 600*nm
		1., // 500*nm
		1., // 400*nm
		1., // 300*nm
		1. // 200*nm
	};
	G4double vacuumAbslength[kNSpecPointsVacuum] = {
		1.e99*km, // 800*nm
		1.e99*km, // 700*nm
		1.e99*km, // 600*nm
		1.e99*km, // 500*nm
		1.e99*km, // 400*nm
		1.e99*km, // 300*nm
		1.e99*km // 200*nm
	};
	G4double vacuumRayleighScatteringLength[kNSpecPointsVacuum] = {
		1.e99*km, // 800*nm
		1.e99*km, // 700*nm
		1.e99*km, // 600*nm
		1.e99*km, // 500*nm
		1.e99*km, // 400*nm
		1.e99*km, // 300*nm
		1.e99*km // 200*nm
	};
	G4MaterialPropertiesTable* pmtVacuum_MPT = new G4MaterialPropertiesTable();
	pmtVacuum_MPT->AddProperty("RINDEX", photonEnergiesForVacuum,
			vacuumRindex, kNSpecPointsVacuum);
	pmtVacuum_MPT->AddProperty("ABSLENGTH", photonEnergiesForVacuum,
			vacuumAbslength, kNSpecPointsVacuum);
	pmtVacuum_MPT->AddProperty("RAYLEIGH", photonEnergiesForVacuum,
			vacuumRayleighScatteringLength, kNSpecPointsVacuum);
	fPmtVacuum->SetMaterialPropertiesTable(pmtVacuum_MPT);

	const int kNSpecPointsSchott8337B = 9;
	// Optical properties of PMT glass SCHOTT 8337B.
	const G4double wavelengthsForSchott8337B[kNSpecPointsSchott8337B] = {
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		350*nm,
		300*nm,
		250*nm,
		200*nm
	};
	// Set order of photon energies for SCHOTT 8337B in increasing order.
	G4double photonEnergiesForSchott8337B[kNSpecPointsSchott8337B];
	for (G4int i = 0; i < kNSpecPointsSchott8337B; ++i) {
		photonEnergiesForSchott8337B[i] =
			h_Planck*c_light/wavelengthsForSchott8337B[i];
	}
	// Following numbers were obtained from formula for Borosilicate crown glass
	// (N-BK10) at http://refractiveindex.info/?group=SCHOTT&material=N-BK10:
	// n^2 − 1 = (0.888308131*lambda^2)/(lambda^2 − 0.00516900822) +
	// (0.328964475*lambda^2)/(lambda^2 − 0.0161190045) +
	// (0.984610769*lambda^2)/(lambda^2 − 99.7575331)
	// N-BK10 is different from SCHOTT 8337B but it was the closest I could find
	// the dispersion curve for among the Borosilicate Crown glasses. It has an
	// refractive index of n = 1.49782 at 587.6nm where as SCHOTT 8337B should
	// have n = 1.476 at 587.6nm. Fix later.
	G4double schott8337BRindex[kNSpecPointsSchott8337B] = {
		1.49220, // 800*nm
		1.49435, // 700*nm
		1.49735, // 600*nm
		1.50207, // 500*nm
		1.51070, // 400*nm
		1.51824, // 350*nm
		1.53045, // 300*nm
		1.53382, // 250*nm constant extrapolation from 290nm.
		1.53382 // 200*nm constant extrapolation from 290nm.
	};
	// Following Transmission coefficient values for SCHOTT 8337B were obtained
	// from
	// http://www.us.schott.com/english/download/us_2010TechnglasWebcopy.pdf.
	// "SCHOTT Technical Glasses, Physical and technical properties. Page 20, Fig.
	// 28. The formula: absorption length = -(length)/ln(T) was used where length
	// is the distance of light travel in mm and T is the transmission probability
	// it %.
	G4double schott8337BAbslength[kNSpecPointsSchott8337B] = {
		5.99653*mm, // 800*nm 92% constant extrapolation from 400nm.
		5.99653*mm, // 700*nm 92% constant extrapolation from 400nm.
		5.99653*mm, // 600*nm 92% constant extrapolation from 400nm.
		5.99653*mm, // 500*nm 92% constant extrapolation from 400nm.
		5.99653*mm, // 400*nm 92% transmission per 0.5mm thickness
		5.99653*mm, // 350*nm 92% transmission per 0.5mm thickness
		4.74561*mm, // 300*nm 90% transmission per 0.5mm thickness
		3.91134*mm, // 250*nm 88% transmission per 0.5mm thickness
		1.73803*mm // 200*nm 75% transmission per 0.5mm thickness
	};
	G4MaterialPropertiesTable* schott_8337B_MPT = new G4MaterialPropertiesTable();
	schott_8337B_MPT->AddProperty("RINDEX", photonEnergiesForSchott8337B,
			schott8337BRindex, kNSpecPointsSchott8337B);
	schott_8337B_MPT->AddProperty("ABSLENGTH", photonEnergiesForSchott8337B,
			schott8337BAbslength, kNSpecPointsSchott8337B);
	fSchott8337B->SetMaterialPropertiesTable(schott_8337B_MPT);

	const int kNSpecPointsBialkali = 11;
	const G4double wavelengthForBialkali[kNSpecPointsBialkali] =
	{
		800*nm,
		650*nm,
		600*nm,
		550*nm,
		500*nm,
		450*nm,
		400*nm,
		350*nm,
		300*nm,
		250*nm,
		200*nm
	};
	G4double photonEnergiesForBialkali[kNSpecPointsBialkali];
	for (G4int i=0; i<kNSpecPointsBialkali; ++i) {
		photonEnergiesForBialkali[i] =
			h_Planck*c_light/wavelengthForBialkali[i];
	}
	//
	// There are two types of bialkali photocathodes simulated below; standard
	// blue sensitive bialkali KCsSb, and green-enhanced bialkali and RbCsSb.
	// We will pick K2CsSb bialkali material as our photocathode material
	// because this is what was specified when Jeffrey DeFazio Senior
	// Member of Technical Staff POWER TUBES Photonis was contacted. 
	//
	//
	// Index of refraction (real part) and extinction coefficient (imaginary
	// part of index of refraction) for blue-sensitive bialkali (KCsSb)
	// photocathode.  Reference: Nuclear Instruments and Methods in Physics
	// Research Section A: Accelerators, Spectrometers, Detectors and
	// Associated Equipment Volume 539, Issues 1-2, 21 February 2005. Optical
	// properties of bialkali photocathodes Original Research
	// Article Pages 217-235. D. Motta, S. Schönert
	G4double KCsSbRindex[kNSpecPointsBialkali] =
	{
		2.96, // 800*nm, simple constant extrapolation from 680nm.
		2.95, // 650*nm
		2.99, // 600*nm
		3.17333, // 550*nm
		3.00, // 500*nm
		2.81333, // 450*nm
		2.24667, // 400*nm
		1.92, // 350*nm, simple constant extrapolation from 380nm.
		1.92, // 300*nm, simple constant extrapolation from 380nm.
		1.92, // 250*nm, simple constant extrapolation from 380nm.
		1.92 // 200*nm, simple constant extrapolation from 380nm.
	};
	G4double KCsSbKindex[kNSpecPointsBialkali] =
	{
		0.33, // 800*nm, simple constant extrapolation from 680nm.
		0.34, // 650*nm
		0.39333, // 600*nm
		0.59667, // 550*nm
		1.06, // 500*nm
		1.46, // 450*nm
		1.69667, // 400*nm
		1.69, // 350*nm, simple constant extrapolation from 380nm.
		1.69, // 300*nm, simple constant extrapolation from 380nm.
		1.69, // 250*nm, simple constant extrapolation from 380nm.
		1.69 // 200*nm, simple constant extrapolation from 380nm.
	};

	//
	// Index of refraction (real part) and extinction coefficient (imaginary
	// part of index of refraction) for green-enhanced bialkali (RbCsSb)
	// photocathode.  Reference: Nuclear Instruments and Methods in Physics
	// Research Section A: Accelerators, Spectrometers, Detectors and
	// Associated Equipment Volume 539, Issues 1-2, 21 February 2005, Pages
	// 217-235
	//

	//const G4double RbCsSbRindex[kNSpecPointsBialkali] =
	//{
	//	3.13, // 800*nm, simple constant extrapolation from 680nm.
	//	3.14, // 650*nm
	//	3.24, // 600*nm
	//	3.26, // 550*nm
	//	3.16, // 500*nm
	//	2.58667, // 450*nm
	//	2.24667, // 400*nm
	//	2.07, // 350*nm, simple constant extrapolation from 380nm.
	//	2.07, // 300*nm, simple constant extrapolation from 380nm.
	//	2.07, // 250*nm, simple constant extrapolation from 380nm.
	//	2.07 // 200*nm, simple constant extrapolation from 380nm.
	//};
	//const G4double RbCsSbKindex[kNSpecPointsBialkali] =
	//{
	//	0.35, // 800*nm, simple constant extrapolation from 680nm.
	//	0.37, // 650*nm
	//	0.44, // 600*nm
	//	0.82667, // 550*nm
	//	1.21, // 500*nm
	//	1.38333, // 450*nm
	//	1.18333, // 400*nm
	//	1.22, // 350*nm, simple constant extrapolation from 380nm.
	//	1.22, // 300*nm, simple constant extrapolation from 380nm.
	//	1.22, // 250*nm, simple constant extrapolation from 380nm.
	//	1.22 // 200*nm, simple constant extrapolation from 380nm.
	//};

	G4double* photocathodeRindex = KCsSbRindex;
	G4double* photocathodeKindex = KCsSbKindex;
	//const G4double* photocathodeRindex = RbCsSbRindex;
	//const G4double* photocathodeKindex = RbCsSbKindex;

	// Quantum efficiency obtained from Planacon PMT spec-sheet for XP85012.
	// Reference: http://www.photonis.com/en/ism/63-planacon.html. Rev10-Sept12.
	// Quantum efficiency of photon detection at photocathode surface.
	G4double photocathodeQuatumEff[kNSpecPointsBialkali] = 
	{
		0.00, // 800nm
		0.00, // 650nm
		0.01, // 600nm
		0.03, // 550nm
		0.11, // 500nm
		0.17, // 450nm
		0.21, // 400nm
		0.205, // 350nm
		0.19, // 300nm
		0.14, // 250nm
		0.06 // 200nm
	};
	G4double photonhitLateralDimensionAlongPhotocathodeSurface[2] =
	{
		0.0*mm,
		1.0*mm
	};
	G4double photocathodeThickness[2] =
	{
		20.*nm,
		20.*nm
	};
	G4MaterialPropertiesTable* photocathode_MPT = new G4MaterialPropertiesTable();
	photocathode_MPT->AddProperty("EFFICIENCY", photonEnergiesForBialkali,
			photocathodeQuatumEff, kNSpecPointsBialkali);
	photocathode_MPT->AddProperty("RINDEX", photonEnergiesForBialkali,
			photocathodeRindex, kNSpecPointsBialkali);
	photocathode_MPT->AddProperty("KINDEX", photonEnergiesForBialkali,
			photocathodeKindex, kNSpecPointsBialkali);
	photocathode_MPT->AddProperty("THICKNESS",
			photonhitLateralDimensionAlongPhotocathodeSurface,
			photocathodeThickness, 2);
	G4OpticalSurface* photocathodeOpticalSurface =
		new G4OpticalSurface("photocathodeOpticalSurface");
	photocathodeOpticalSurface->SetType(dielectric_metal);
	photocathodeOpticalSurface->SetMaterialPropertiesTable(photocathode_MPT);
	new G4LogicalBorderSurface("photocathodeLogicalSurface1",
			fPmtGlassWindowPhysical, fPmtInnerVacuumPhysical,
			photocathodeOpticalSurface);
	new G4LogicalBorderSurface("photocathodeLogicalSurface2",
			fPmtInnerVacuumPhysical, fPmtGlassWindowPhysical,
			photocathodeOpticalSurface);
	//new G4LogicalSkinSurface("photocathodeSurface", photocathodeLogical, photocathodeOpticalSurface);

	//
	// FastSimulationModel
	//
	new MTCG4PmtOpticalModel(
			"pmtGlassHousingOpticalModel",
			fPmtGlassHousingPhysical);
} // SetPmtMaterialProperties()
