//////////////////////////////////////////////////////////
// MTCG4DetectorConstruction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include <cstring>

#include "globals.hh"
#include "cmath"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVParameterised.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RegionStore.hh"

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4DetectorMessenger.hh"
#include "MTCG4PhysicsList.hh"
#include "MTCG4PmtSD.hh"
#include "MTCG4PmtOpticalModel.hh"

#include "CADMesh.hh"

//
// Some detector geometry constants and other constants.
//

// (mm) Length of one side of World Volume cube.
//const G4double WORLD_DIMENSION_HIGHT = 1000.*mm;
//const G4double WORLD_DIMENSION_WIDTH_X = 1000.*mm;
//const G4double WORLD_DIMENSION_WIDTH_Y = 1000.*mm;
const G4double WORLD_DIMENSION_HIGHT = 100000.*mm;
const G4double WORLD_DIMENSION_WIDTH_X = 100000.*mm;
const G4double WORLD_DIMENSION_WIDTH_Y = 100000.*mm;

// (mm) Length of one side of sctintillator cube. Our current mTC scintillator
// cube dimension is 130mm on one edge.
//const G4double SCINT_DIMENSION = 130.*mm;
const G4double SCINT_DIMENSION = 10000.*mm;

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

// (mm) Thickness of buffer in between scintillator surface and PMT. This may be
// some sort of container that holds the scintillator cube inside, such as a
// glass container. This feature is not used at this moment so is set to 0.
const G4double SCINT_BUFFER_THICKNESS = 0.*mm;

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

MTCG4PmtSD* MTCG4DetectorConstruction::fAPmtSD;

MTCG4DetectorConstruction::MTCG4DetectorConstruction()
	// Intialize some pointers. I don't know if this is really necessary.
	:fWorldSolid(0), fWorldLogical(0), fWorldPhysical(0), fScintSolid(0),
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
	// World dimensions.
	fWorldHight = WORLD_DIMENSION_HIGHT;
	fWorldWidthX = WORLD_DIMENSION_WIDTH_X;
	fWorldWidthY = WORLD_DIMENSION_WIDTH_Y;

	// Detector variables.
	fNeutronCaptureMaterial = "boron";
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
	fPmtGlassThicknessAtFaceAndRear = PMT_GLASS_THICKNESS_AT_FACE_AND_REAR;
	fPmtInnerVacuumDepth = PMT_INNER_VACUUM_DEPTH;
	fPhotocathodeFaceDimension = PHOTOCATHODE_FACE_DIMENSION;
	fPixelPitch = PMT_PIXEL_FACE_PITCH;
	fTopOfPmtInnerVacuumExtentInZ =
		.5*PMT_GLASS_HOUSING_DEPTH - PMT_GLASS_THICKNESS_AT_FACE_AND_REAR;
	fBottomOfPmtInnerVacuumExtentInZ =
		.5*PMT_GLASS_HOUSING_DEPTH - PMT_GLASS_THICKNESS_AT_FACE_AND_REAR -
		PMT_INNER_VACUUM_DEPTH;

	// Visualization flags.
	fScintVisualization = scintVisGray;
	fPmtVisualization = pmtVisGray;
	fPeripheralGeometryVisualization = peripheralVisOff;
	//fPeripheralGeometryVisualization = peripheralVisGray;

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
	
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
	
	// Geometry definitions.
	SetupGeometry();

	// Geometry surface properties.
	SetMaterialProperties();

	// Visualization attributes.
	SetVisualizationAttributes();

	return fWorldPhysical;
} 

void MTCG4DetectorConstruction::DefineMaterials()
{
	G4NistManager* nistManager = G4NistManager::Instance();

	//
	// Elements.
 	//

	G4double a; // Atomic mass of element.
	G4double z; // Atomic number of element.
	G4String name, symbol;

	// Helium.
	G4Element* H =
	 	new G4Element("Hydrogen", symbol="H", z = 1, a = 1.01*g/mole);
	// Carbon.
	G4Element* C =
	 	new G4Element("Carbon", symbol="C", z = 6, a = 12.01*g/mole);
	// Oxygen.
	G4Element* O =
	 	new G4Element(name = "Oxygen", symbol = "O", z = 8, a = 16.00*g/mole);
	// Silicon.
	G4Element* Si =
		new G4Element(name = "Silicon", symbol = "Si", z = 14, a = 28.09*g/mole);
	// Clorine.
	G4Element* Cl =
		new G4Element(name = "Clorine", symbol = "Cl", z = 17, a = 35.45*g/mole);

	//
	// Materials definitions.
	//

	G4int n; // Number of nucleons in element atom.
	G4double density;
	G4int ncomponents, natoms;
	G4double fractionMass;
	G4double relativeAbundance;
	G4double anthraceneFraction;
	G4double enrichmentFraction;

	// Air.
	fAir = nistManager->FindOrBuildMaterial("G4_AIR");
	
	// PMT vacuum is very dilute air.
	G4State state;
	G4double temperature;
	G4double pressure;
	fPmtVacuum =
		new G4Material(
				name = "PMT_Vacuum",
			 	density = 1.e-3*kGasThreshold, // from PhysicalConstants.h
				ncomponents = 1,
			 	state = kStateGas,
			 	temperature = STP_Temperature, // from PhysicalConstants.h
				pressure = STP_Pressure*density/(1.29e-3*g/cm3)); // from klg4sim?
	fPmtVacuum->AddMaterial(fAir, fractionMass = 100.*perCent);
	// Following method gives incorrect pressure of 1 atm.
	//fPmtVacuum = nistManager ->
	// 	BuildMaterialWithNewDensity("PMT_Vacuum", "G4_AIR", density =
	//			1.e-3*kGasThreshold); // From G4PhysicalConstants.hh
	
	// PMT borosilicate glass SCHOTT 8337B.
	fSchott8337B =
		new G4Material(name = "SCHOTT_8337B", density = 2.22*g/cm3, ncomponents =
				2);
	fSchott8337B->AddElement(Si, natoms = 1);
	fSchott8337B->AddElement(O, natoms = 2);
	
	// PMT rear circuit board. Fiberglass (SiO2)?
	fPmtCircuitBoard =
		new G4Material(name = "PMT_PCB", density = 2.65*g/cm3, ncomponents
				= 2);
	fPmtCircuitBoard->AddElement(Si, natoms = 1);
	fPmtCircuitBoard->AddElement(O, natoms = 2);
	
	// PMT black wrapping material. Polyvinyl chloride?
	fPmtBlackWrapping =
		new G4Material(
				name = "PMT_Black_Wrapping",
			 	density = 1.2*g/cm3, // rough number from Wikipedia?
				ncomponents = 3);
	fPmtBlackWrapping->AddElement(C, natoms = 2);
	fPmtBlackWrapping->AddElement(H, natoms = 3);
	fPmtBlackWrapping->AddElement(Cl, natoms = 1);
	
	//// Bialkali photocathode material.
	//// Following numbers are made-up.
	//// Photocathode material not used in simulation anymore.
	//fBialkaliPhotocathode = new G4Material(
	//		name = "Bialkali_Photocathode",
	//	 	density = 0.100*g/cm3,
	//	 	ncomponents = 3);
	//fBialkaliPhotocathode->AddElement(K, fractionMass = 37.5*perCent);
	//fBialkaliPhotocathode->AddElement(Cs, fractionMass = 37.5*perCent);
	//fBialkaliPhotocathode->AddElement(Sb, fractionMass = 25.0*perCent);

	// Delrin.
	fDelrin = nistManager->FindOrBuildMaterial("G4_POLYOXYMETHYLENE");

	// Aluminum.
	fAluminum = nistManager->FindOrBuildMaterial("G4_Al");

	// Define Polyvinyltoluene (PVT).
	// PVT data taken from following paper:
	// "Radiation damage in polyvinyltoluene (PVT)"
	// Elsevier Volume 63, Issue 1, January 2002, Pages 89–92,
	// L. Torrisi, Dipartimento di Fisica, Università di Messina, Ctr. Papardo
	// Sperone, 31–98166 S. Agata, Messina, Italy
	G4Material* polyvinyltoluene =
	 	new G4Material("polyvinyltoluene", density=1.032*g/cm3, ncomponents=2);
	polyvinyltoluene->AddElement(C, natoms=9);
	polyvinyltoluene->AddElement(H, natoms=10);

	// Define Anthracene.	
	G4Material* anthracene =
	 	new G4Material("anthracene", density=1.25*g/cm3, ncomponents=2);
	anthracene->AddElement(C, natoms=14);
	anthracene->AddElement(H, natoms=10);

	// Define enriched and natural Boron.
	G4Isotope* B10 = new G4Isotope("Boron10", z=5, n=10, a=10.0129370*g/mole);
	G4Isotope* B11 = new G4Isotope("Boron11", z=5, n=11, a=11.0093054*g/mole);
	G4Element* BNatural =
	 	new G4Element("Natrual_Boron", symbol="B_natural", ncomponents=2);
	// Set the enrichment fraction of Boron as the
	// fractional ratio B10/(B10+B11).
	BNatural->AddIsotope(B10, enrichmentFraction = 0.1997);
	BNatural->AddIsotope(B11, relativeAbundance = 1. - enrichmentFraction);
	G4Element* BEnriched =
	 	new G4Element("Enriched_Boron", symbol="B_enriched", ncomponents=2);
	BEnriched->AddIsotope(B10, enrichmentFraction = 1.);
	BEnriched->AddIsotope(B11, relativeAbundance = 1. - enrichmentFraction);	
	
	// Define enriched and natural Lithium.
	G4Isotope* Li6 = new G4Isotope("Lithium6", z=3, n=6, a=6.015122795*g/mole);
	G4Isotope* Li7 = new G4Isotope("Lithium7", z=3, n=7, a=7.01600455*g/mole);
	G4Element* LiNatural =
	 	new G4Element("Enriched_Lithium", symbol="Li_natural", ncomponents=2);
	// Set the enrichment fraction of Lithium as the
	// fractional ratio Li6/(Li7+Li6).
	LiNatural->AddIsotope(Li6, enrichmentFraction = 0.075);
	LiNatural->AddIsotope(Li7, relativeAbundance = 1. - enrichmentFraction);
	G4Element* LiEnriched =
	 	new G4Element("Enriched_Lithium", symbol="Li_enriched", ncomponents=2);
	LiEnriched->AddIsotope(Li6, enrichmentFraction = 1.);
	LiEnriched->AddIsotope(Li7, relativeAbundance = 1. - enrichmentFraction);
	
	// Set doping fractions for each type of doping fraction.
	// The mass fraction of all other materials comprising the scintillator
	// are set to be a function of the doping fraction.

	//
	// Boron doped scintillator.
	//

	G4double dopingFraction;
	// scintillator doped with 0 percent natural Boron.
	dopingFraction = 0.*perCent;
	fEJ254_OnePercentNaturalBoronDoped =
		new G4Material("EJ254_ZeroPercentNaturalBoronDoped", density =
				GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentNaturalBoronDoped->AddElement(BNatural, fractionMass = dopingFraction);

	// scintillator doped with 1 percent natural Boron.
	dopingFraction = 1.*perCent;
	fEJ254_OnePercentNaturalBoronDoped =
		new G4Material("EJ254_OnePercentNaturalBoronDoped", density =
				GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentNaturalBoronDoped->AddElement(BNatural, fractionMass = dopingFraction);

	// scintillator doped with 2.5 percent natural Boron.
	dopingFraction = 2.5*perCent;
	fEJ254_TwoAndHalfPercentNaturalBoronDoped = new G4Material("EJ254_TwoAndHalfPercentNaturalBoronDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddElement(BNatural, fractionMass = dopingFraction);

	// scintillator doped with 5 percent natural Boron.
	dopingFraction = 5.*perCent;
	fEJ254_FivePercentNaturalBoronDoped = new G4Material("EJ254_FivePercentNaturalBoronDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_FivePercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_FivePercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_FivePercentNaturalBoronDoped->AddElement(BNatural, fractionMass = dopingFraction);
	
	// scintillator doped with 1 percent enriched Boron.
	dopingFraction = 1.*perCent;
	fEJ254_OnePercentEnrichedBoronDoped = new G4Material("EJ254_OnePercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentEnrichedBoronDoped->AddElement(BEnriched, fractionMass = dopingFraction);

	// scintillator doped with 2.5 percent enriched Boron.
	dopingFraction = 2.5*perCent;
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped = new G4Material("EJ254_TwoAndHalfPercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddElement(BEnriched, fractionMass = dopingFraction);
	
	// scintillator doped with 5 percent enriched Boron.
	dopingFraction = 5.*perCent;
	fEJ254_FivePercentEnrichedBoronDoped = new G4Material("EJ254_FivePercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_FivePercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_FivePercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_FivePercentEnrichedBoronDoped->AddElement(BEnriched, fractionMass = dopingFraction);

	//
	// Lithium doped scintillator.
	//
	
	// scintillator doped with 0 percent natural Lithium.
	dopingFraction = 0.*perCent;
	fEJ254_OnePercentNaturalLithiumDoped = new G4Material("EJ254_OnePercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentNaturalLithiumDoped->AddElement(LiNatural, fractionMass = dopingFraction);

	// scintillator doped with 1 percent natural Lithium.
	dopingFraction = 1.*perCent;
	fEJ254_OnePercentNaturalLithiumDoped = new G4Material("EJ254_OnePercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentNaturalLithiumDoped->AddElement(LiNatural, fractionMass = dopingFraction);

	// scintillator doped with 2.5 percent natural Lithium.
	dopingFraction = 2.5*perCent;
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped = new G4Material("EJ254_TwoAndHalfPercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddElement(LiNatural, fractionMass = dopingFraction);
	
	// scintillator doped with 5 percent natural Lithium.
	dopingFraction = 5.*perCent;
	fEJ254_FivePercentNaturalLithiumDoped = new G4Material("EJ254_FivePercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_FivePercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_FivePercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_FivePercentNaturalLithiumDoped->AddElement(LiNatural, fractionMass = dopingFraction);
	
	// scintillator doped with 1 percent enriched Lithium.
	dopingFraction = 1.*perCent;
	fEJ254_OnePercentEnrichedLithiumDoped = new G4Material("EJ254_OnePercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_OnePercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_OnePercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_OnePercentEnrichedLithiumDoped->AddElement(LiEnriched, fractionMass = dopingFraction);

	// scintillator doped with 2.5 percent enriched Lithium.
	dopingFraction = 2.5*perCent;
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped = new G4Material("EJ254_TwoAndHalfPercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddElement(LiEnriched, fractionMass = dopingFraction);
	
	// scintillator doped with 5 percent enriched Lithium.
	dopingFraction = 5.*perCent;
	fEJ254_FivePercentEnrichedLithiumDoped = new G4Material("EJ254_FivePercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), ncomponents = 3);
	fEJ254_FivePercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	fEJ254_FivePercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	fEJ254_FivePercentEnrichedLithiumDoped->AddElement(LiEnriched, fractionMass = dopingFraction);
}// DefineMaterial().

//
// Detector geometry.
//
void MTCG4DetectorConstruction::SetupGeometry()
{
	//
	// Set materials to be used for detector construction.
	//
	G4NistManager* nist = G4NistManager::Instance();
	G4Material* worldMaterial =
		nist->FindOrBuildMaterial("G4_AIR");
	
	//
	// World volume.
 	//
	fWorldSolid = new G4Box(
			"fWorldSolid",
		 	.5*WORLD_DIMENSION_WIDTH_X,
		 	.5*WORLD_DIMENSION_WIDTH_Y,
		 	.5*WORLD_DIMENSION_HIGHT);
	fWorldLogical = new G4LogicalVolume(
			fWorldSolid,
		 	worldMaterial,
		 	"fWorldLogical");
	fWorldPhysical = new G4PVPlacement(
			NULL,
		 	G4ThreeVector(0., 0., 0.),
		 	fWorldLogical,
		 	"fWorldPhysical",
		 	NULL, // It is not placed in anything.
		 	false,
		 	0); // This is the world volume. All that Geant knows.

	//
	// scintillator volume.
 	//
	fScintSolid = new G4Box(
			"fScintSolid",
		 	.5*SCINT_DIMENSION,
		 	.5*SCINT_DIMENSION,
		 	.5*SCINT_DIMENSION);
//#if THERMAL_SCATTERING_FLAG
//	fScintLogical = new G4LogicalVolume(fScintSolid, scintMaterial_TS, "scintillatorLogical"); // scintillator volume for thermal scattering.
//#else
	fScintLogical = new G4LogicalVolume(
			fScintSolid,
		 	fScintMaterial,
			"scintillatorLogical");// Scintillator volume for non-thermal scattering.
//#endif
	fScintPhysical = new G4PVPlacement(
			NULL, // The scintillator cube is placed without any rotation.
			G4ThreeVector(0., 0., 0.), // The scintillator cube is placed in the center of the world volume.
			fScintLogical,
			"fScintPhysical",
			fWorldLogical, // It is placed in the world logical volume.
			false,
			0);

	if(fPmtsArePlaced)
		SetupPMTGeometries(); // Create PMTs.
	if(fFrameIsPlaced)
		SetupPeripheralGeometries(); // Create support frame/clams stand rods, etc.
}// SetupGeometry().

// Used by MTCG4DetectorConstruction::SetupGeometries()
void MTCG4DetectorConstruction::SetupPMTGeometries()
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
	pmt1RotationMatrix.rotateZ(+CLHEP::pi*rad);
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
	pmt2RotationMatrix.rotateZ(+CLHEP::pi*rad);
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
	pmt5RotationMatrix.rotateX(-.5*CLHEP::pi*rad);
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
	pmt6RotationMatrix.rotateX(-.5*CLHEP::pi*rad);
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
	pmt7RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt7RotationMatrix.rotateX(-.5*CLHEP::pi*rad);
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
	pmt8RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt8RotationMatrix.rotateX(-.5*CLHEP::pi*rad);
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
	pmt9RotationMatrix.rotateZ(-.5*CLHEP::pi*rad);
	pmt9RotationMatrix.rotateY(+.5*CLHEP::pi*rad);
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
	pmt10RotationMatrix.rotateZ(-.5*CLHEP::pi*rad);
	pmt10RotationMatrix.rotateY(+.5*CLHEP::pi*rad);
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
	pmt11RotationMatrix.rotateZ(+.5*CLHEP::pi*rad);
	pmt11RotationMatrix.rotateY(+.5*CLHEP::pi*rad);
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
	pmt12RotationMatrix.rotateZ(+.5*CLHEP::pi*rad);
	pmt12RotationMatrix.rotateY(+.5*CLHEP::pi*rad);
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
	pmt13RotationMatrix.rotateZ(+.5*CLHEP::pi*rad);
	pmt13RotationMatrix.rotateY(-.5*CLHEP::pi*rad);
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
	pmt14RotationMatrix.rotateZ(+.5*CLHEP::pi*rad);
	pmt14RotationMatrix.rotateY(-.5*CLHEP::pi*rad);
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
	pmt15RotationMatrix.rotateZ(-.5*CLHEP::pi*rad);
	pmt15RotationMatrix.rotateY(-.5*CLHEP::pi*rad);
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
	pmt16RotationMatrix.rotateZ(-.5*CLHEP::pi*rad);
	pmt16RotationMatrix.rotateY(-.5*CLHEP::pi*rad);
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
	pmt17RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt17RotationMatrix.rotateX(+.5*CLHEP::pi*rad);
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
	pmt18RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt18RotationMatrix.rotateX(+.5*CLHEP::pi*rad);
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
	pmt19RotationMatrix.rotateX(+.5*CLHEP::pi*rad);
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
	pmt20RotationMatrix.rotateX(+.5*CLHEP::pi*rad);
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
	pmt21RotationMatrix.rotateX(+CLHEP::pi*rad);
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
	pmt22RotationMatrix.rotateX(+CLHEP::pi*rad);
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
	pmt23RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt23RotationMatrix.rotateX(+CLHEP::pi*rad);
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
	pmt24RotationMatrix.rotateZ(+CLHEP::pi*rad);
	pmt24RotationMatrix.rotateX(+CLHEP::pi*rad);
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
	if(!fAPmtSD) {
		G4String MTCG4PmtSDName =
			"/MTCG4PMT/MTCG4PmtSD";
		fAPmtSD = new MTCG4PmtSD( MTCG4PmtSDName );
		fSDManager->AddNewDetector( fAPmtSD );
		fPmtGlassWindowLogical->SetSensitiveDetector( fAPmtSD );
		fPmtInnerVacuumLogical->SetSensitiveDetector( fAPmtSD );
	}
} // SetupPMTGeometries()

// Used by MTCG4DetectorConstruction::SetupGeometry()
void MTCG4DetectorConstruction::SetupPeripheralGeometries()
{
	G4NistManager* nist = G4NistManager::Instance();
	G4Material* fFrameMaterial =
		nist->FindOrBuildMaterial("G4_POLYOXYMETHYLENE"); // Delrin.
	G4Material* fLegMaterial =
		nist->FindOrBuildMaterial("G4_Al"); // Aluminum.

	//
	// MTC CAD design import to create peripheral structures.
	// 

	// Offset of the origin of the MTCG4Sim coordinate system from the origin of
	// the CAD world coordinate system. As of now, the CAD world coordinate
	// system is also set to be the center of the cube. When importing
	// additional CAD components into MTCG4Sim, please first make sure that the
	// origin of the CAD world coordinate system coincides with the center of
	// the cube.
	const G4ThreeVector offset = G4ThreeVector(0.*mm, 0.*mm, 0.*mm);

	// File format of CAD files used.
	const G4String cadFormat = "STL";

	//
	// End plates of frame structure that holds the scintillating cube.
	//

	// 'const_cast<char*>' to a dynamically allocated non-const char array needs
	// to be used in the CADMesh() constructors below. This is because the
	// CADMesh constructor takes in a pointer to non-const char.
	// I think this was a mistake on the part of the programmer that wrote
	// CADMesh. He probably should have used (const char*) for the type accepted
	// by the constructor. If 'const_cast<char*>' is not used, warning messages
	// appear such as "warning: deprecated conversion from string constant to
	// ‘char*’", or the program will segmentation fault. I'm not sure about the
	// memory leaks associated with not deleting the dynamic non-const char
	// array. Will have to fix this later if it turns out to be a problem.

	for(unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate) {
		std::ostringstream oss;
		oss << iEndPlate+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameEndPlate" + oss.str() + ".stl";
		G4String logicalName = "frameEndPlateLogical" + oss.str();
		G4String physicalName = "frameEndPlatePhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh( buffer1, buffer2, mm, offset, false);
		fFrameEndPlateSolid[iEndPlate] = mesh->TessellatedMesh();
		fFrameEndPlateLogical[iEndPlate] = new G4LogicalVolume(
					fFrameEndPlateSolid[iEndPlate],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameEndPlatePhysical[iEndPlate] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameEndPlateLogical[iEndPlate],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Side plates of frame structure that holds the scintillating cube.
	//

	for(unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate) {
		std::ostringstream oss; oss << iSidePlate+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameSidePlate" + oss.str() + ".stl";
		G4String logicalName = "frameSidePlateLogical" + oss.str();
		G4String physicalName = "frameSidePlatePhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameSidePlateSolid[iSidePlate] = mesh->TessellatedMesh();
		fFrameSidePlateLogical[iSidePlate] = new G4LogicalVolume(
					fFrameSidePlateSolid[iSidePlate],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameSidePlatePhysical[iSidePlate] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameSidePlateLogical[iSidePlate],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// End clamps of frame structure that holds the scintillating cube.
	//

	for(unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp) {
		std::ostringstream oss; oss << iEndClamp+1;
		G4String stlFileName =
			fCadFilesPath + "frameEndClamp" + oss.str() + ".stl";
		G4String logicalName = "frameEndClampLogical" + oss.str();
		G4String physicalName = "frameEndClampPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameEndClampSolid[iEndClamp] = mesh->TessellatedMesh();
		fFrameEndClampLogical[iEndClamp] = new G4LogicalVolume(
					fFrameEndClampSolid[iEndClamp],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameEndClampPhysical[iEndClamp] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameEndClampLogical[iEndClamp],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Side clamps of frame structure that holds the scintillating cube.
	//

	for(unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp) {
		std::ostringstream oss; oss << iSideClamp+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameSideClamp" + oss.str() + ".stl";
		G4String logicalName = "frameSideClampLogical" + oss.str();
		G4String physicalName = "frameSideClampPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameSideClampSolid[iSideClamp] = mesh->TessellatedMesh();
		fFrameSideClampLogical[iSideClamp] = new G4LogicalVolume(
					fFrameSideClampSolid[iSideClamp],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameSideClampPhysical[iSideClamp] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameSideClampLogical[iSideClamp],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Legs holding frame structure that holds the scintillating cube.
	//

	for(unsigned int iLeg = 0; iLeg < 4; ++iLeg) {
		std::ostringstream oss; oss << iLeg+1;
		G4String stlFileName =
		   	fCadFilesPath + "leg" + oss.str() + ".stl";
		G4String logicalName = "legLogical" + oss.str();
		G4String physicalName = "legPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fLegSolid[iLeg] = mesh->TessellatedMesh();
		fLegLogical[iLeg] = new G4LogicalVolume(
					fLegSolid[iLeg],
				   	fLegMaterial,
					logicalName.c_str());
		fLegPhysical[iLeg] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fLegLogical[iLeg],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}
} // SetupPeripheralGeometries()

//
// Material properties such as refractive index and photocathode optical
// properties.
// Important note: this needs to be done after the definitions of materials, as
// I have done in this code, in order to work.
void MTCG4DetectorConstruction::SetMaterialProperties() {
	// Set different photon wavelengths that will be referenced in calculating
	// optical properties of air.
	const G4double wavelengthsForAir[8] = 
	{
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		300*nm,
		200*nm,
		100*nm
	};
	// Set order of photon energies for air in increasing order.
	static G4double photonEnergiesForAir[8];
	for(G4int i = 0; i < 8; i++)
		photonEnergiesForAir[i] =
			CLHEP::h_Planck*CLHEP::c_light/wavelengthsForAir[i];
	// Dispersion formula used for air: n-1 =
	// C1/(C2-lambda^-2)+C3/(C4-lambda^-2), where C1 = 5792105E-8; C2 = 238.0185;
	// C3 = 167917E-8; C4 = 57.362 and lambda is in units of um.  Reference:
	// Philip E. Ciddor. Refractive index of air: new equations for the visible
	// and near infrared, Appl. Optics 35, 1566-1573 (1996)
	static G4double airRindex[8] =
	{
		1.0002750, // 800*nm
		1.0002758, // 700*nm
		1.0002770, // 600*nm
		1.0002790, // 500*nm
		1.0002828, // 400*nm
		1.0002916, // 300*nm
		1.0003238, // 200*nm
		1.0003803 // 100*nm
	};
	static G4double airAbslength[8] = {
		1.0e6*mm, // 800*nm
		1.0e6*mm, // 700*nm
		1.0e6*mm, // 600*nm
		1.0e6*mm, // 500*nm
		1.0e6*mm, // 400*nm
		1.0e6*mm, // 300*nm
		1.0e6*mm, // 200*nm
		1.0e6*mm // 100*nm
	}; // Fix later.
	// Attenuation length formula used for Rayleigh scattering.
	// Attenuation length = 1/alpha where alpha =
	// 2*(2*CLHEP::pi/lambda)^4/(3*CLHEP::pi*N)*(n-1)^2.
	// lambda = wavelengthsForAir(cm), N(density of molecules) = 2.7e19(cm^-3),
	// n = refractive index.
	// Reference: T. Tsang, Classical electrodynamics, Blue Sky and Red Sunset,
	// p.202.
	static G4double
		airRayleighScatteringLength[8] = {
			4.42161e+05*m, // 800*nm
			2.57685e+05*m, // 700*nm
			1.37889e+05*m, // 600*nm
			6.55476e+04*m, // 500*nm
			2.61316e+04*m, // 400*nm
			7.77670e+03*m, // 300*nm
			1.24581e+03*m, // 200*nm
			5.64459e+01*m, // 100*nm
		};
	G4MaterialPropertiesTable* air_MPT = new G4MaterialPropertiesTable();
	air_MPT->AddProperty("RINDEX", photonEnergiesForAir, airRindex, 8);
	air_MPT->AddProperty("ABSLENGTH", photonEnergiesForAir, airAbslength, 8);
	air_MPT->AddProperty("RAYLEIGH", photonEnergiesForAir, airRayleighScatteringLength, 8);
	fAir->SetMaterialPropertiesTable(air_MPT);

	if(fPmtsArePlaced)
		MTCG4DetectorConstruction::SetPmtMaterialProperties();
	MTCG4DetectorConstruction::SetScintMaterialProperties();
} // SetMaterialProperties()

//
// Material properties for scintillator.
//
void MTCG4DetectorConstruction::SetScintMaterialProperties()
{
	// Set different photon wavelengths that will be referenced in calculating
	// scintillator properties, e.g. decay time constant.
	const G4double wavelengthForEJ254[13] =
	{
		600*nm,
		500*nm,
		490*nm,
		480*nm,
		470*nm,
		460*nm,
		450*nm,
		440*nm,
		430*nm,
		425*nm,
		420*nm,
		410*nm,
		400*nm
	};
	// Set order of photon energies for EJ254 in increasing order.
	static G4double photonEnergiesForEJ254[13];
	for(G4int i = 0; i < 13; i++)
		photonEnergiesForEJ254[i] =
			CLHEP::h_Planck*CLHEP::c_light/wavelengthForEJ254[i];
	static G4double fEJ254_ScintFast[13] =
	{
		0.00, // 600*nm, arbitrary cutoff
		0.05, // 500*nm
		0.10, // 490*nm
		0.15, // 480*nm
		0.30, // 470*nm
		0.45, // 460*nm
		0.55, // 450*nm
		0.75, // 440*nm
		0.95, // 430*nm
		1.00, // 425*nm, peak of distribution
		0.90, // 420*nm
		0.25, // 410*nm
		0.00 // 400*nm
	};
	static G4double fEJ254_ScintSlow[13] =
	{
		0.00, // 600*nm, arbitrary cutoff
		0.05, // 500*nm
		0.10, // 490*nm
		0.15, // 480*nm
		0.30, // 470*nm
		0.45, // 460*nm
		0.55, // 450*nm
		0.75, // 440*nm
		0.95, // 430*nm
		1.00, // 425*nm, peak of distribution
		0.90, // 420*nm
		0.25, // 410*nm
		0.00 // 400*nm
	};
	// Refractive index for EJ254 plastic scintillator.
	// Formula used for refractive index n where n^2 = (nI)^2 + A/(lambda^2 -
	// lambda_0^2).  nI = 1.53319, A = 20,690nm^2, lambda_0 = 196.6nm.
	// nI was fitted such that lambda = 425nm => n = 1.58 as is reported in the
	// EJ254 data sheet. Other constants were extracted as is from the following
	// reference.
	// Reference: Journal of Colloid and Interface Science, Volume 118, Issue 2,
	// August 1987, Pages 314–325, Measurement of the complex refractive index of
	// polyvinyltoluene in the UV, visible, and near IR: Application to the size
	// determination of PVT latices.
	// T. Depireux, F. Dumont, A. Watillon
	static G4double fEJ254_RefIndex[13] =
	{
		1.55405, // 600*nm
		1.56479, // 500*nm
		1.56633, // 490*nm
		1.56798, // 480*nm
		1.56978, // 470*nm
		1.57172, // 460*nm
		1.57383, // 450*nm
		1.57613, // 440*nm
		1.57865, // 430*nm
		1.58000, // 425*nm
		1.58141, // 420*nm
		1.58446, // 410*nm
		1.58782 // 400*nm
	}; // Fix later.
	// This seems to be a pretty constant number according to the above reference.
	static G4double fEJ254_AbsLength[13] = {
		250.*cm, // 600*nm
		250.*cm, // 500*nm
		250.*cm, // 490*nm
		250.*cm, // 480*nm
		250.*cm, // 470*nm
		250.*cm, // 460*nm
		250.*cm, // 450*nm
		250.*cm, // 440*nm
		250.*cm, // 430*nm
		250.*cm, // 425*nm
		250.*cm, // 420*nm
		250.*cm, // 410*nm
		250.*cm // 400*nm
	}; // Fix later.
	G4MaterialPropertiesTable* EJ254_MPT = new G4MaterialPropertiesTable();
	EJ254_MPT->AddProperty("FASTCOMPONENT", photonEnergiesForEJ254,
			fEJ254_ScintFast, 13);
	EJ254_MPT->AddProperty("SLOWCOMPONENT", photonEnergiesForEJ254,
			fEJ254_ScintSlow, 13);
	EJ254_MPT->AddProperty("RINDEX", photonEnergiesForEJ254, fEJ254_RefIndex, 13);
	EJ254_MPT->AddProperty("ABSLENGTH", photonEnergiesForEJ254, fEJ254_AbsLength,
			13);
	// The following scintillation yield numbers are taken from the
	// fEJ254_OnePercentNaturalBoronDoped data sheet for the case of Boron doping
	// only.  We will have to use these numbers for now until we can find those of
	// the case for Lithium.
	static G4double scintillationYield;
	if(fDopingFraction < 2.5*perCent)
		scintillationYield =
			( (8600. - 9200.)/(2.5*perCent - 1.*perCent) *
			 (fDopingFraction - 2.5*perCent) + 8600.) / MeV;
	else if(fDopingFraction >= 2.5*perCent)
		scintillationYield =
			( (7500. - 8600.)/(5.*perCent - 2.5*perCent) *
				(fDopingFraction - 2.5*perCent) + 8600.) / MeV;
	EJ254_MPT->AddConstProperty("SCINTILLATIONYIELD", scintillationYield);
	EJ254_MPT->AddConstProperty("RESOLUTIONSCALE", 1.0); // Fix later.
	EJ254_MPT->AddConstProperty("FASTTIMECONSTANT", 2.2*ns); // Fix later.
	EJ254_MPT->AddConstProperty("SLOWTIMECONSTANT", 2.2*ns);
	// Yield ratio is not needed for now when there is only one component.
	// Yield ratio is the relative strength of the fast component as a fraction of
	// total scintillation yield.
	EJ254_MPT->AddConstProperty("YIELDRATIO", 1.); // Fix later.
	// Set Birks constant for scintillation photon quenching.
	// This number was derived from simulation of the 7Li/alpha daughters
	// produced from neutron capture on 10B. The EJ254 data sheet explains that
	// the photon yield from 7Li/alpha is equivalent to 76keV visible photon
	// deposition of an e-. The data sheet also mentions that 1% natural Boron
	// doped EJ254 gives 9,200 photons/MeV of e-, so these two numbers where
	// used for the derivation of Birks constant.
	G4double birksConst = 0.110*mm/MeV;
	
	fEJ254_OnePercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_FivePercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_OnePercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_FivePercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_OnePercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_FivePercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_OnePercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	fEJ254_FivePercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
} // SetScintMaterialProperties()

//
// Material properties for PMT materials.
//
void MTCG4DetectorConstruction::SetPmtMaterialProperties()
{
	// Optical properties of PMT vacuum.
	static G4double wavelengthsForVacuum[8] = {
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		300*nm,
		200*nm,
		100*nm
	};
	static G4double photonEnergiesForVacuum[8];
	// Set order of photon energies for PMT vacuum in increasing order.
	for(G4int i = 0; i < 8; i++)
		photonEnergiesForVacuum[i] =
			CLHEP::h_Planck*CLHEP::c_light/wavelengthsForVacuum[i];
	static G4double vacuumRindex[8] = {
		1., // 800*nm
		1., // 700*nm
		1., // 600*nm
		1., // 500*nm
		1., // 400*nm
		1., // 300*nm
		1., // 200*nm
		1. // 100*nm
	};
	static G4double vacuumAbslength[8] = {
		1.e99*km, // 800*nm
		1.e99*km, // 700*nm
		1.e99*km, // 600*nm
		1.e99*km, // 500*nm
		1.e99*km, // 400*nm
		1.e99*km, // 300*nm
		1.e99*km, // 200*nm
		1.e99*km // 100*nm
	};
	static G4double vacuumRayleighScatteringLength[8] = {
		1.e99*km, // 800*nm
		1.e99*km, // 700*nm
		1.e99*km, // 600*nm
		1.e99*km, // 500*nm
		1.e99*km, // 400*nm
		1.e99*km, // 300*nm
		1.e99*km, // 200*nm
		1.e99*km // 100*nm
	};
	G4MaterialPropertiesTable* pmtVacuum_MPT = new G4MaterialPropertiesTable();
	pmtVacuum_MPT->AddProperty("RINDEX", photonEnergiesForVacuum,
			vacuumRindex, 8);
	pmtVacuum_MPT->AddProperty("ABSLENGTH", photonEnergiesForVacuum,
			vacuumAbslength, 8);
	pmtVacuum_MPT->AddProperty("RAYLEIGH", photonEnergiesForVacuum,
			vacuumRayleighScatteringLength, 8);
	fPmtVacuum->SetMaterialPropertiesTable(pmtVacuum_MPT);

	// Optical properties of PMT glass SCHOTT 8337B.
	const G4double wavelengthsForSchott8337B[9] = {
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
	static G4double photonEnergiesForSchott8337B[9];
	for(G4int i = 0; i < 9; i++)
		photonEnergiesForSchott8337B[i] =
			CLHEP::h_Planck*CLHEP::c_light/wavelengthsForSchott8337B[i];
	// Following numbers were obtained from formula for Borosilicate crown glass
	// (N-BK10) at http://refractiveindex.info/?group=SCHOTT&material=N-BK10:
	// n^2 − 1 = (0.888308131*lambda^2)/(lambda^2 − 0.00516900822) +
	// (0.328964475*lambda^2)/(lambda^2 − 0.0161190045) +
	// (0.984610769*lambda^2)/(lambda^2 − 99.7575331)
	// N-BK10 is different from SCHOTT 8337B but it was the closest I could find
	// the dispersion curve for among the Borosilicate Crown glasses. It has an
	// refractive index of n = 1.49782 at 587.6nm where as SCHOTT 8337B should
	// have n = 1.476 at 587.6nm. Fix later.
	static G4double schott8337BRindex[9] = {
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
	static G4double schott8337BAbslength[9] = {
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
			schott8337BRindex, 9);
	schott_8337B_MPT->AddProperty("ABSLENGTH", photonEnergiesForSchott8337B,
			schott8337BAbslength, 9);
	fSchott8337B->SetMaterialPropertiesTable(schott_8337B_MPT);

	static G4double wavelengthForBialkali[11] =
	{
		700*nm,
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
	static G4double photonEnergiesForBialkali[11];
	for(G4int i=0; i<11; i++)
		photonEnergiesForBialkali[i] =
			CLHEP::h_Planck*CLHEP::c_light/wavelengthForBialkali[i];
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
	static G4double KCsSbRindex[11] =
	{
		2.96, // 700*nm, simple constant extrapolation from 680nm.
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
	static G4double KCsSbKindex[11] =
	{
		0.33, // 700*nm, simple constant extrapolation from 680nm.
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

	//static G4double RbCsSbRindex[11] =
	//{
	//	3.13, // 700*nm, simple constant extrapolation from 680nm.
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
	//static G4double RbCsSbKindex[11] =
	//{
	//	0.35, // 700*nm, simple constant extrapolation from 680nm.
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

	static G4double*
		photocathodeRindex = KCsSbRindex;
	static G4double*
		photocathodeKindex = KCsSbKindex;
	//static G4double*
	//	photocathodeRindex = RbCsSbRindex;
	//static G4double*
	//	photocathodeKindex = RbCsSbKindex;

	// Quantum efficiency obtained from Planacon PMT spec-sheet for XP85012.
	// Reference: http://www.photonis.com/en/ism/63-planacon.html. Rev10-Sept12.
	// Quantum efficiency of photon detection at photocathode surface.
	static G4double photocathodeQuatumEff[11] = 
	{
		0.00, // 700nm
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
	static G4double photonhitLateralDimensionAlongPhotocathodeSurface[2] =
	{
		0.0*mm,
		1.0*mm
	};
	static G4double photocathodeThickness[2] =
	{
		20.*nm,
		20.*nm
	};
	G4MaterialPropertiesTable* photocathode_MPT = new G4MaterialPropertiesTable();
	photocathode_MPT->AddProperty("EFFICIENCY", photonEnergiesForBialkali,
			photocathodeQuatumEff, 11);
	photocathode_MPT->AddProperty("RINDEX", photonEnergiesForBialkali,
			photocathodeRindex, 11);
	photocathode_MPT->AddProperty("KINDEX", photonEnergiesForBialkali,
			photocathodeKindex, 11);
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

//
// Visualization attributes.
//
void MTCG4DetectorConstruction::SetVisualizationAttributes() {

	// Make colours.
	G4Colour  white   (1.0, 1.0, 1.0);
	G4Colour  gray    (0.5, 0.5, 0.5);
	G4Colour  lgray   (.85, .85, .85);
	G4Colour  red     (1.0, 0.0, 0.0);
	G4Colour  blue    (0.0, 0.0, 1.0);
	G4Colour  cyan    (0.0, 1.0, 1.0);
	G4Colour  magenta (1.0, 0.0, 1.0);
	G4Colour  yellow  (1.0, 1.0, 0.0);
	G4Colour  orange  (.75, .55, 0.0);
	G4Colour  lblue   (0.0, 0.0, .75);
	G4Colour  lgreen  (0.0, .75, 0.0);
	G4Colour  green   (0.0, 1.0, 0.0);
	G4Colour  brown   (0.7, 0.4, 0.1);
	
	// World volume.
	fWorldLogical->SetVisAttributes(G4VisAttributes::Invisible);

	// Scintillator cube color.
	if(fScintVisualization == scintVisColor)
		fScintLogical->SetVisAttributes(new G4VisAttributes(cyan));
	if(fScintVisualization == scintVisGray)
		fScintLogical->SetVisAttributes(new G4VisAttributes(gray));
	if(fScintVisualization == scintVisOff)
		fScintLogical->SetVisAttributes(G4VisAttributes::Invisible);

	// PMT color.
	if(fPmtsArePlaced) {
		if(fPmtVisualization == pmtVisColor) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->
				SetVisAttributes(new G4VisAttributes(lblue));
			fPmtGlassWindowLogical->SetVisAttributes(new G4VisAttributes(lblue));
			fPmtBlackWrappingLogical->
				SetVisAttributes(new G4VisAttributes(gray));
			fPmtRearBoardLogical->SetVisAttributes(new G4VisAttributes(lgreen));
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(new G4VisAttributes(red));
		}
		if(fPmtVisualization == pmtVisGray) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtGlassWindowLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtBlackWrappingLogical->
				SetVisAttributes(new G4VisAttributes(gray));
			fPmtRearBoardLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(new G4VisAttributes(gray));
		}
		if(fPmtVisualization == pmtVisOff) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassWindowLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtBlackWrappingLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtRearBoardLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(G4VisAttributes::Invisible);
		}
	}
	if(fFrameIsPlaced) {
		//// Peripheral structure color.
		//if(fPeripheralGeometryVisualization == peripheralVisColor) {
		//	for(unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//}
		//if(fPeripheralGeometryVisualization == peripheralVisGray) {
		//	for(unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for(unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//}
		//if(fPeripheralGeometryVisualization == peripheralVisOff) {
		//	for(unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for(unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for(unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for(unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for(unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//}
	}

	//// Test marker volume.
 	//// Used for testing.
	//G4VisAttributes* testMarkerAttributes = new G4VisAttributes(G4Colour(1., 0., 0., .1)); // This is used to confirm PMT orientation.
	//testMarkerAttributes->SetForceSolid(true);
	//testMarkerLogical->SetVisAttributes(testMarkerAttributes);
}

// Currently gives seg-fault. Fix later.
void MTCG4DetectorConstruction::UpdateGeometry()
{
	G4cout << "fWorldPhysical = " << fWorldPhysical << G4endl;
	if(!fWorldPhysical) return;

	//
	// Clean old geometry, if any.
	//
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4cout << "geometry is closed = " << G4GeometryManager::GetInstance() ->
		IsGeometryClosed() << G4endl;

	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	//G4RegionStore::GetInstance()->Clean();
	G4LogicalSkinSurface::CleanSurfaceTable();
	G4LogicalBorderSurface::CleanSurfaceTable();
	G4SurfaceProperty::CleanSurfacePropertyTable();
	G4cout << "fAPmtSD: " << fAPmtSD << G4endl;
	if(fAPmtSD) delete fAPmtSD;
	//if(fSDManager) delete fSDManager;

	//if(fWorldSolid) delete fWorldSolid;
	//if(fWorldLogical) delete fWorldLogical;
	//if(fWorldPhysical) delete fWorldPhysical;

	//if(fWorldSolid) {
	//	G4cout << "fWorldSolid exists.\n";
	// 	delete fWorldSolid;
	//}
	//if(fWorldLogical) {
	//	G4cout << "fWorldLogical exists.\n";
	// 	delete fWorldLogical;
	//}
	//if(fWorldPhysical) {
	//	G4cout << "fWorldPhysical exists.\n";
	// 	delete fWorldPhysical;
	//}

	// Delete left-over pointers? Is this really needed?
	//if(fScintSolid) delete fScintSolid;
	//if(fScintLogical) delete fScintLogical;
	//if(fScintPhysical) delete fScintPhysical;

	//if(fPmtBoundingVolumeSolid) delete fPmtBoundingVolumeSolid;
	//if(fPmtBoundingVolumeLogical) delete fPmtBoundingVolumeLogical;
	//if(fPmtBoundingVolume1Physical) delete fPmtBoundingVolume1Physical;
	//if(fPmtBoundingVolume2Physical) delete fPmtBoundingVolume2Physical;
	//if(fPmtBoundingVolume3Physical) delete fPmtBoundingVolume3Physical;
	//if(fPmtBoundingVolume4Physical) delete fPmtBoundingVolume4Physical;
	//if(fPmtBoundingVolume5Physical) delete fPmtBoundingVolume5Physical;
	//if(fPmtBoundingVolume6Physical) delete fPmtBoundingVolume6Physical;
	//if(fPmtBoundingVolume7Physical) delete fPmtBoundingVolume7Physical;
	//if(fPmtBoundingVolume8Physical) delete fPmtBoundingVolume8Physical;
	//if(fPmtBoundingVolume9Physical) delete fPmtBoundingVolume9Physical;
	//if(fPmtBoundingVolume10Physical) delete fPmtBoundingVolume10Physical;
	//if(fPmtBoundingVolume11Physical) delete fPmtBoundingVolume11Physical;
	//if(fPmtBoundingVolume12Physical) delete fPmtBoundingVolume12Physical;
	//if(fPmtBoundingVolume13Physical) delete fPmtBoundingVolume13Physical;
	//if(fPmtBoundingVolume14Physical) delete fPmtBoundingVolume14Physical;
	//if(fPmtBoundingVolume15Physical) delete fPmtBoundingVolume15Physical;
	//if(fPmtBoundingVolume16Physical) delete fPmtBoundingVolume16Physical;
	//if(fPmtBoundingVolume17Physical) delete fPmtBoundingVolume17Physical;
	//if(fPmtBoundingVolume18Physical) delete fPmtBoundingVolume18Physical;
	//if(fPmtBoundingVolume19Physical) delete fPmtBoundingVolume19Physical;
	//if(fPmtBoundingVolume20Physical) delete fPmtBoundingVolume20Physical;
	//if(fPmtBoundingVolume21Physical) delete fPmtBoundingVolume21Physical;
	//if(fPmtBoundingVolume22Physical) delete fPmtBoundingVolume22Physical;
	//if(fPmtBoundingVolume23Physical) delete fPmtBoundingVolume23Physical;
	//if(fPmtBoundingVolume24Physical) delete fPmtBoundingVolume24Physical;

	//if(fPmtGlassHousingSolid) delete fPmtGlassHousingSolid;
	//if(fPmtGlassHousingLogical) delete fPmtGlassHousingLogical;
	//if(fPmtGlassHousingPhysical) delete fPmtGlassHousingPhysical;

	//if(fPmtGlassWindowSolid) delete fPmtGlassWindowSolid;
	//if(fPmtGlassWindowLogical) delete fPmtGlassWindowLogical;
	//if(fPmtGlassWindowPhysical) delete fPmtGlassWindowPhysical;

	//if(fPmtRearBoardSolid) delete fPmtRearBoardSolid;
	//if(fPmtRearBoardLogical) delete fPmtRearBoardLogical;
	//if(fPmtRearBoardPhysical) delete fPmtRearBoardPhysical;

	//if(fPmtBlackWrappingSolid) delete fPmtBlackWrappingSolid;
	//if(fPmtBlackWrappingLogical) delete fPmtBlackWrappingLogical;
	//if(fPmtBlackWrappingPhysical) delete fPmtBlackWrappingPhysical;

	//if(fPmtInnerVacuumSolid) delete fPmtInnerVacuumSolid;
	//if(fPmtInnerVacuumLogical) delete fPmtInnerVacuumLogical;
	//if(fPmtInnerVacuumPhysical) delete fPmtInnerVacuumPhysical;

	//if(fDynodeSolid) delete fDynodeSolid;
	//if(fDynodeLogical) delete fDynodeLogical;
	//if(fDynodePhysical) delete fDynodePhysical;

	//if(fPmtOuterVacuumSolid) delete fPmtOuterVacuumSolid;
	//if(fPmtOuterVacuumLogical) delete fPmtOuterVacuumLogical;
	//if(fPmtOuterVacuumPhysical) delete fPmtOuterVacuumPhysical;

	// Define new ones.
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	//G4RegionStore::GetInstance()->UpdateMaterialList(fWorldPhysical);
}

void MTCG4DetectorConstruction::SetScintMaterial()
{
	// Construct string to be used to identify scintillator properties.
	G4String
		neutronCaptureMaterialString,
		dopingFractionString,
		enrichmentString;
	if(fNeutronCaptureMaterial == "boron")
		neutronCaptureMaterialString = "Boron";
	else if(fNeutronCaptureMaterial == "lithium")
		neutronCaptureMaterialString = "Lithium";
	else
		G4Exception("MTCG4DetectorConstruction::SetScintMaterial()",
			 	"",
				FatalErrorInArgument,
				"Scintillator material was not correctly specified.");
	if(fDopingFraction == 0.*perCent)
		dopingFractionString = "Zero";
	else if(fDopingFraction == 1.*perCent)
		dopingFractionString = "One";
	else if(fDopingFraction == 2.5*perCent)
		dopingFractionString = "TwoAndHalf";
	else if(fDopingFraction == 5.*perCent)
		dopingFractionString = "Five";
	else
		G4Exception("MTCG4DetectorConstruction::SetScintMaterial()",
			 	"",
				FatalErrorInArgument,
				"Scintillator material was not correctly specified.");
	if(fDopantIsEnriched == true)
		enrichmentString = "Enriched";
	else if(fDopantIsEnriched == false)
		enrichmentString = "Natural";
	else
		G4Exception("MTCG4DetectorConstruction::SetScintMaterial()",
			 	"",
				FatalErrorInArgument,
				"Scintillator material was not correctly specified.");

	// Choose scintillator material to be used with given properties.
	G4String materialChoice =
		"EJ254_"
		+ dopingFractionString
		+ "Percent"
		+ enrichmentString
		+ neutronCaptureMaterialString
		+ "Doped";
	G4cout << materialChoice << G4endl;

	// Set material.
	fScintMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if(fScintLogical) {
		assert(fScintMaterial != NULL);
		fScintLogical->SetMaterial(fScintMaterial);
		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "scintillator material was updated to: "
			<< materialChoice
			<< G4endl;
	}
	return;
}

// Density approximated through extrapolation of data given by
// EJ254 data sheet.
// Density for Lithium doped scintillator needs to be found.
// For now, I just use the same extrapolation as that of the case of
// Boron dopend EJ254 scintillator for the both cases of Boron and
// Lithium doped scintillator.
G4double MTCG4DetectorConstruction::GetEJ254Density
(G4double dopingFraction) const
{
	G4double ej254Density = 0;
	if(dopingFraction < 2.5*perCent)
		ej254Density =
			( (1.023 - 1.021)/(2.5*perCent - 1.*perCent) *
				(dopingFraction	- 2.5*perCent) + 1.023) * g/cm3;
 	else if(dopingFraction >= 2.5*perCent)
		ej254Density =
			( (1.026 - 1.023)/(5.*perCent - 2.5*perCent) *
				(dopingFraction	- 2.5*perCent) + 1.023) * g/cm3;
 	return ej254Density;
}

// Set doping dependent fractional content of Anthracene
// according to extrapolation of EJ254 data sheet by Eljen Technology.
G4double MTCG4DetectorConstruction::GetAnthraceneFraction
(G4double dopingFraction) const
{
	G4double anthraceneFraction = 0;
	if(dopingFraction < 2.5*perCent)
		anthraceneFraction =
			(.56 - .60)/(2.5*perCent - 1.*perCent)*
			(dopingFraction - 2.5*perCent)
			+ .56;
	else
		anthraceneFraction =
			(.48 - .56)/(5.*perCent - 2.5*perCent)*
		   	(dopingFraction - 2.5*perCent)
			+ .56;
 	return anthraceneFraction;
}

G4double MTCG4DetectorConstruction::GetPolyvinyltolueneFraction
(G4double dopingFraction, G4double anthraceneFraction) const
{
	G4double polyvinyltolueneFraction = 0;
	polyvinyltolueneFraction = 1. - dopingFraction - anthraceneFraction;
	return polyvinyltolueneFraction;
}
