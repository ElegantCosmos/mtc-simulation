//////////////////////////////////////////////////////////
// MTCG4DetectorConstruction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4DetectorMessenger.hh"
#include "MTCG4PhysicsList.hh"
#include "MTCG4PixelSD.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVParameterised.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
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

#include "globals.hh"
#include "cmath"

//
// Some detector geometry constants and other constants.
//

// (mm) Length of one side of World Volume cube.
#define WORLD_DIMENSION	1000

// (mm) Length of one side of sctintillator cube. Our current mTC scintillator cube dimension is 130mm on one edge.
#define SCINTILLATOR_DIMENSION	130
//#define SCINTILLATOR_DIMENSION	1000

// (mm) Length of one side of MCP face.
#define MCP_FACE_DIMENSION 59

// (mm) Thickness of MCP.
#define MCP_THICKNESS 25

// (mm) Border thickness around edge of MCP face that is not the active area.
#define MCP_BORDER 3

// (mm) Border thickness along inside edge of MCP active area face that is not photo-sensitive.
#define MCP_ACTIVE_AREA_BORDER 0.5

// (mm) Thickness of buffer in between scintillator surface and MCP. This may be some sort of container that holds the scintillator cube inside, such as a glass container. This feature is not used at this moment.
#define SCINTILLATOR_BUFFER_THICKNESS 0

// (mm) Length of one side of MCP photo-sensitive pixel.
#define PIXEL_FACE_DIMENSION 5.9

// (mm) Thickness of photo-sensitive pixel.
#define PIXEL_THICKNESS 1

// (mm) Spacing between neighboring face centers of photo-sensitive pixels.
#define PIXEL_SPACING 6.5

// Number of different photon energies that will be referenced for optical properties of scintillator, e.g. scintillation yield spectrum, etc.
#define NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254 21

// Number of different photon energies that will be referenced for opical properties of air.
#define NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR 4

// Number of different photon energies that will be referenced for optical properties of MCP pixels, e.g. quantum efficiency, real and imaginary refractive index, etc.
#define NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL 11

//
// Physical constants used in this simulation.
//
// c_light = Speed of light in vacuum 2.99792458e+8 * m/s. // Already included in G4PhysicalConstants.hh
// h_Planck = Planck's constant 6.6260755e-34 * joule*s or 4.13566e-12 MeV*ns. // Already included in G4PhysicalConstants.hh
const G4double EJ254RefractiveIndex = 1.58;

MTCG4PixelSD* MTCG4DetectorConstruction::aPixelSD;

MTCG4DetectorConstruction::MTCG4DetectorConstruction()
	:worldMaterial(0), scintillatorMaterial(0), /*scintillatorMaterial_TS(0),*/ pixelMaterial(0), MCPMaterial(0),
	worldSolid(0), worldLogical(0), worldPhysical(0), // Intialize some pointers. I don't know if this is really necessary.
	scintillatorSolid(0), scintillatorLogical(0), scintillatorPhysical(0),
	MCPSolid(0), MCPLogical(0), MCPPhysical(0)
{
	MCPsArePlaced = true; // Flag to place MCPs on scintillator surfaces.
	scintillatorBufferThickness = SCINTILLATOR_BUFFER_THICKNESS*mm;
	neutronCaptureMaterial = "boron";
	dopingFraction = 0.05;
	dopantIsEnriched = false;
	opticalSurfacePropertiesAreSet = true;
	scintillatorVisualisation = true;
	mcpModuleVisualisation = false;
	mcpPixelVisualisation = false;
	neutronHPThermalScatteringIsTurnedOn =
		((MTCG4PhysicsList*)G4RunManager::GetRunManager()->GetUserPhysicsList())->GetNeutronHPThermalScattering();
	detectorMessenger = new MTCG4DetectorMessenger(this);
	//G4cout << "neutronHPThermalScatteringIsTurnedOn = " << neutronHPThermalScatteringIsTurnedOn << G4endl;

	// Material definitions.
	DefineMaterials();
}

MTCG4DetectorConstruction::~MTCG4DetectorConstruction()
{
	delete detectorMessenger;
}

G4VPhysicalVolume* MTCG4DetectorConstruction::Construct()
{
	//G4cout << "dopingFraction = " << dopingFraction << G4endl;
	//G4cout << "neutronHPThermalScatteringIsTurnedOn = " << neutronHPThermalScatteringIsTurnedOn << G4endl;
	
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
	
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
	
	// Geometry definitions.
	SetupGeometry();

	// Visualisation attributes.
	SetVisualisationAttributes();

	// Geometry surface properties.
	SetSurfaceProperties();

	return worldPhysical;
} 

void MTCG4DetectorConstruction::DefineMaterials()
{
	G4double a; // Atomic mass of element.
	G4double z; // Atomic number of element.
	G4int n; // Number of nucleons in element atom.
	G4double density;
	G4int nComponents, nAtoms;
	G4double fractionMass;
	G4double relativeAbundance;
	G4String name, symbol;
	G4double dopingFraction;
	G4double anthraceneFraction;
  G4double enrichmentFraction;

	//
	// Elements.
 	//

	// Nitrogen.
	G4Element* N = new G4Element(name = "Nitrogen", symbol = "N", z =7., a = 14.01*g/mole);

	// Oxygen.
	G4Element* O = new G4Element(name = "Oxygen"  , symbol = "O", z =8., a = 16.00*g/mole);

	// Potassium.
	G4Element* K = new G4Element(name = "Potassium"  , symbol = "K", z = 19., a = 39.10*g/mole);
	
	// Cesium.
	G4Element* Cs = new G4Element(name = "Cesium"  , symbol = "Cs", z = 55., a = 132.91*g/mole);

	// Antimony.
	G4Element* Sb = new G4Element(name = "Antimony"  , symbol = "Sb", z = 51., a = 121.76*g/mole);
	
	// Helium.
	G4Element* H = new G4Element("Hydrogen", symbol="H", z=1., a=1.01*g/mole);

	// Carbon.
	G4Element* C = new G4Element("Carbon", symbol="C", z=6., a=12.01*g/mole);

	//
	// Materials definitions.
	//

	// Aluminum.
	G4Material* aluminum = new G4Material(name = "Aluminum", z = 13., a = 26.9815386*g/mole, density = 2.7*g/cm3);

	// Air.
	G4Material* air = new G4Material(name = "Air", density = 1.290*mg/cm3, nComponents = 2);
	air->AddElement(N, fractionMass = 70.*perCent);
	air->AddElement(O, fractionMass = 30.*perCent);

	// Bialkali photocathode material.
	// Following numbers are made-up.
	G4Material* bialkaliPhotoCathode = new G4Material(name =
			"BialkaliPhotoCathode", density = 0.100*g/cm3, nComponents = 3);
	bialkaliPhotoCathode->AddElement(K, fractionMass = 37.5*perCent);
	bialkaliPhotoCathode->AddElement(Cs, fractionMass = 37.5*perCent);
	bialkaliPhotoCathode->AddElement(Sb, fractionMass = 25.0*perCent);

	//
	// Material properties such as refractive index.
	//
	// Set different photon wavelengths that will be referenced in calculating
	// properties of air.
	//
	const G4double wavelengthForAir[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR] = 
	{
		700*nm,
		600*nm,
		500*nm,
		400*nm,
	};
	static G4double photonEnergiesForAir[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR];
	for(G4int i = 0; i < NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR; i++) {
		G4double energy = h_Planck*c_light/(EJ254RefractiveIndex*wavelengthForAir[i]); // Set order of photon energies for air in increasing order.
		photonEnergiesForAir[i] = energy;
	}
	//
	// Dispersion formula used for air: n-1 = C1/(C2-lambda^-2)+C3/(C4-lambda^-2),
	// where C1 = 5792105E-8; C2 = 238.0185; C3 = 167917E-8; C4 = 57.362 and
	// lambda is in units of um.
	// Reference: Philip E. Ciddor. Refractive index of air: new equations for the
	// visible and near infrared, Appl. Optics 35, 1566-1573 (1996)
	//
	static G4double airRefIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR] =
	{
		1.000275804, // 700*nm
		1.000276983, // 600*nm
		1.000278974, // 500*nm
		1.000282762 // 400*nm
	};
	static G4double airAbsLength[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR] = {
		35.*m, // 700*nm
		35.*m, // 600*nm
		35.*m, // 500*nm
		35.*m // 400*nm
	}; // Fix later.
	//
	// Attenuation length formula used for Rayleigh scattering.
	// Attenuation length = 1/alpha where alpha =
	// 2*(2*pi/lambda)^4/(3*pi*N)*(n-1)^2, lambda = wavelengthForAir, N(density of
	// molecules) = 2.7e19(cm^-3), n = refractive index.
	// Reference: T. Tsang, Classical electrodynamics, Blue Sky and Red Sunset,
	// p.202.
	// 
	static G4double
		airRayleighScatteringLength[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR] = {
			257677.*m, // 700*nm
			137906.*m, // 600*nm
			65559.9*m, // 500*nm
			26138.7*m, // 400*nm
		};
	G4MaterialPropertiesTable* air_MPT = new G4MaterialPropertiesTable();
	air_MPT->AddProperty("RINDEX", photonEnergiesForAir, airRefIndex, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR);
	air_MPT->AddProperty("ABSLENGTH", photonEnergiesForAir, airAbsLength, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR);
	air_MPT->AddProperty("RAYLEIGH", photonEnergiesForAir, airRayleighScatteringLength, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_AIR);
	air->SetMaterialPropertiesTable(air_MPT);

	// Define Polyvinyltoluene (PVT).
	// PVT data taken from following paper:
	// "Radiation damage in polyvinyltoluene (PVT)"
	// Elsevier Volume 63, Issue 1, January 2002, Pages 89–92,
	// L. Torrisi, Dipartimento di Fisica, Università di Messina, Ctr. Papardo Sperone, 31–98166 S. Agata, Messina, Italy
	G4Material* polyvinyltoluene = new G4Material("polyvinyltoluene", density=1.032*g/cm3, nComponents=2);
	polyvinyltoluene->AddElement(C, nAtoms=9);
	polyvinyltoluene->AddElement(H, nAtoms=10);

	// Define Anthracene.	
	G4Material* anthracene = new G4Material("anthracene", density=1.25*g/cm3, nComponents=2);
	anthracene->AddElement(C, nAtoms=14);
	anthracene->AddElement(H, nAtoms=10);

	// Define enriched and natural Boron.
	G4Isotope* B10 = new G4Isotope("Boron10", z=5, n=10, a=10.0129370*g/mole);
	G4Isotope* B11 = new G4Isotope("Boron11", z=5, n=11, a=11.0093054*g/mole);
	G4Element* B_natural = new G4Element("Natrual_Boron", symbol="B_natural", nComponents=2);
	// Set the enrichment fraction of Boron as the
	// fractional ratio B10/(B10+B11).
	B_natural -> AddIsotope(B10, enrichmentFraction = 0.1997);
	B_natural -> AddIsotope(B11, relativeAbundance = 1. - enrichmentFraction);
	G4Element* B_enriched = new G4Element("Enriched_Boron", symbol="B_enriched", nComponents=2);
	B_enriched -> AddIsotope(B10, enrichmentFraction = 1.);
	B_enriched -> AddIsotope(B11, relativeAbundance = 1. - enrichmentFraction);	
	
	// Define enriched and natural Lithium.
	G4Isotope* Li6 = new G4Isotope("Lithium6", z=3, n=6, a=6.015122795*g/mole);
	G4Isotope* Li7 = new G4Isotope("Lithium7", z=3, n=7, a=7.01600455*g/mole);
	G4Element* Li_natural = new G4Element("Enriched_Lithium", symbol="Li_natural", nComponents=2);
	// Set the enrichment fraction of Lithium as the
	// fractional ratio Li6/(Li7+Li6).
	Li_natural -> AddIsotope(Li6, enrichmentFraction = 0.075);
	Li_natural -> AddIsotope(Li7, relativeAbundance = 1. - enrichmentFraction);
	G4Element* Li_enriched = new G4Element("Enriched_Lithium", symbol="Li_enriched", nComponents=2);
	Li_enriched -> AddIsotope(Li6, enrichmentFraction = 1.);
	Li_enriched -> AddIsotope(Li7, relativeAbundance = 1. - enrichmentFraction);
	
	// Set doping fractions for each type of doping fraction.
	// The mass fraction of all other materials comprising the scintillator
	// are set to be a function of the doping fraction.

	//
	// Boron doped scintillator.
	//

	// Scintillator doped with 1 percent natural Boron.
	dopingFraction = .01;
	G4Material* EJ254_onePercentNaturalBoronDoped = new G4Material("EJ254_onePercentNaturalBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_onePercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_onePercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_onePercentNaturalBoronDoped->AddElement(B_natural, fractionMass = dopingFraction);

	// Scintillator doped with 2.5 percent natural Boron.
	dopingFraction = .025;
	G4Material* EJ254_twoAndHalfPercentNaturalBoronDoped = new G4Material("EJ254_twoAndHalfPercentNaturalBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_twoAndHalfPercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_twoAndHalfPercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_twoAndHalfPercentNaturalBoronDoped->AddElement(B_natural, fractionMass = dopingFraction);

	// Scintillator doped with 5 percent natural Boron.
	dopingFraction = .05;
	G4Material* EJ254_fivePercentNaturalBoronDoped = new G4Material("EJ254_fivePercentNaturalBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_fivePercentNaturalBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_fivePercentNaturalBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_fivePercentNaturalBoronDoped->AddElement(B_natural, fractionMass = dopingFraction);
	
	// Scintillator doped with 1 percent enriched Boron.
	dopingFraction = .01;
	G4Material* EJ254_onePercentEnrichedBoronDoped = new G4Material("EJ254_onePercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_onePercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_onePercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_onePercentEnrichedBoronDoped->AddElement(B_enriched, fractionMass = dopingFraction);

	// Scintillator doped with 2.5 percent enriched Boron.
	dopingFraction = .025;
	G4Material* EJ254_twoAndHalfPercentEnrichedBoronDoped = new G4Material("EJ254_twoAndHalfPercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_twoAndHalfPercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_twoAndHalfPercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_twoAndHalfPercentEnrichedBoronDoped->AddElement(B_enriched, fractionMass = dopingFraction);
	
	// Scintillator doped with 5 percent enriched Boron.
	dopingFraction = .05;
	G4Material* EJ254_fivePercentEnrichedBoronDoped = new G4Material("EJ254_fivePercentEnrichedBoronDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_fivePercentEnrichedBoronDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_fivePercentEnrichedBoronDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_fivePercentEnrichedBoronDoped->AddElement(B_enriched, fractionMass = dopingFraction);

	//
	// Boron doped scintillator.
	//
	
	// Scintillator doped with 1 percent natural Lithium.
	dopingFraction = .01;
	G4Material* EJ254_onePercentNaturalLithiumDoped = new G4Material("EJ254_onePercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_onePercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_onePercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_onePercentNaturalLithiumDoped->AddElement(Li_natural, fractionMass = dopingFraction);

	// Scintillator doped with 2.5 percent natural Lithium.
	dopingFraction = .025;
	G4Material* EJ254_twoAndHalfPercentNaturalLithiumDoped = new G4Material("EJ254_twoAndHalfPercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_twoAndHalfPercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_twoAndHalfPercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_twoAndHalfPercentNaturalLithiumDoped->AddElement(Li_natural, fractionMass = dopingFraction);
	
	// Scintillator doped with 5 percent natural Lithium.
	dopingFraction = .05;
	G4Material* EJ254_fivePercentNaturalLithiumDoped = new G4Material("EJ254_fivePercentNaturalLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_fivePercentNaturalLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_fivePercentNaturalLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_fivePercentNaturalLithiumDoped->AddElement(Li_natural, fractionMass = dopingFraction);
	
	// Scintillator doped with 1 percent enriched Lithium.
	dopingFraction = .01;
	G4Material* EJ254_onePercentEnrichedLithiumDoped = new G4Material("EJ254_onePercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_onePercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_onePercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_onePercentEnrichedLithiumDoped->AddElement(Li_enriched, fractionMass = dopingFraction);

	// Scintillator doped with 2.5 percent enriched Lithium.
	dopingFraction = .025;
	G4Material* EJ254_twoAndHalfPercentEnrichedLithiumDoped = new G4Material("EJ254_twoAndHalfPercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_twoAndHalfPercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_twoAndHalfPercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_twoAndHalfPercentEnrichedLithiumDoped->AddElement(Li_enriched, fractionMass = dopingFraction);
	
	// Scintillator doped with 5 percent enriched Lithium.
	dopingFraction = .05;
	G4Material* EJ254_fivePercentEnrichedLithiumDoped = new G4Material("EJ254_fivePercentEnrichedLithiumDoped", density = GetEJ254Density(dopingFraction), nComponents = 3);
	EJ254_fivePercentEnrichedLithiumDoped->AddMaterial(anthracene, anthraceneFraction = GetAnthraceneFraction(dopingFraction));
	EJ254_fivePercentEnrichedLithiumDoped->AddMaterial(polyvinyltoluene, fractionMass = GetPolyvinyltolueneFraction(dopingFraction, anthraceneFraction));
	EJ254_fivePercentEnrichedLithiumDoped->AddElement(Li_enriched, fractionMass = dopingFraction);

	
	//
	// Material properties.
	//
	
	// Set different photon wavelengths that will be referenced in calculating
	// scintillator properties, e.g. decay time constant.
	const G4double wavelengthForEJ254[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254] =
	{
		500*nm,
		495*nm,
		490*nm,
		485*nm,
		480*nm,
		475*nm,
		470*nm,
		465*nm,
		460*nm,
		455*nm,
		450*nm,
		445*nm,
		440*nm,
		435*nm,
		430*nm,
		425*nm,
		420*nm,
		415*nm,
		410*nm,
		405*nm,
		400*nm,
	};
	static G4double photonEnergiesForEJ254[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254];
	// Set order of photon energies for EJ254 in increasing order.
	for(G4int i = 0; i < NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254; i++) {
		G4double energy = h_Planck*c_light/(EJ254RefractiveIndex*wavelengthForEJ254[i]);
		photonEnergiesForEJ254[i] = energy;
	}
	static G4double EJ254Scint[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254] =
	{
		0.05, // 500*nm
    	0.10, // 495*nm
    	0.10, // 490*nm
    	0.15, // 485*nm
    	0.20, // 480*nm
    	0.20, // 475*nm
    	0.30, // 470*nm
    	0.40, // 465*nm
    	0.45, // 460*nm
    	0.50, // 455*nm
    	0.55, // 450*nm
    	0.65, // 445*nm
    	0.75, // 440*nm
    	0.85, // 435*nm
    	0.95, // 430*nm
    	1.00, // 425*nm
    	0.90, // 420*nm
    	0.60, // 415*nm
    	0.30, // 410*nm
    	0.10, // 405*nm
    	0.00 // 400*nm
	};
	static G4double EJ254ScintSlow[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254] =
	{
		0.05, // 500*nm
    	0.10, // 495*nm
    	0.10, // 490*nm
    	0.15, // 485*nm
    	0.30, // 480*nm
    	0.20, // 475*nm
    	0.30, // 470*nm
    	0.40, // 465*nm
    	0.45, // 460*nm
    	0.50, // 455*nm
    	0.75, // 450*nm
    	0.65, // 445*nm
    	0.75, // 440*nm
    	0.85, // 435*nm
    	0.95, // 430*nm
    	1.00, // 425*nm
    	0.90, // 420*nm
    	0.60, // 415*nm
    	0.30, // 410*nm
    	0.10, // 405*nm
    	0.00 // 400*nm
	};
	static G4double EJ254RefIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254] =
	{
		1.58, // 500*nm
		1.58, // 495*nm
		1.58, // 490*nm
		1.58, // 485*nm
		1.58, // 480*nm
		1.58, // 475*nm
		1.58, // 470*nm
		1.58, // 465*nm
		1.58, // 460*nm
		1.58, // 455*nm
		1.58, // 450*nm
		1.58, // 445*nm
		1.58, // 440*nm
		1.58, // 435*nm
		1.58, // 430*nm
		1.58, // 425*nm
		1.58, // 420*nm
		1.58, // 415*nm
		1.58, // 410*nm
		1.58, // 405*nm
		1.58 // 400*nm
	}; // Fix later.
	static G4double EJ254AbsLength[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254] = {
		150.*cm, // 500*nm
		150.*cm, // 495*nm
		150.*cm, // 490*nm
		150.*cm, // 485*nm
		150.*cm, // 480*nm
		150.*cm, // 475*nm
		150.*cm, // 470*nm
		150.*cm, // 465*nm
		150.*cm, // 460*nm
		150.*cm, // 455*nm
		150.*cm, // 450*nm
		150.*cm, // 445*nm
		150.*cm, // 440*nm
		150.*cm, // 435*nm
		150.*cm, // 430*nm
		150.*cm, // 425*nm
		150.*cm, // 420*nm
		150.*cm, // 415*nm
		150.*cm, // 410*nm
		150.*cm, // 405*nm
		150.*cm // 400*nm
	}; // Fix later.
	G4MaterialPropertiesTable* EJ254_MPT = new G4MaterialPropertiesTable();
	EJ254_MPT->AddProperty("FASTCOMPONENT", photonEnergiesForEJ254, EJ254Scint, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254);
	//
	// Slow component of scintillator will not be used for now.
	//
	EJ254_MPT->AddProperty("SLOWCOMPONENT", photonEnergiesForEJ254, EJ254ScintSlow, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254);
	EJ254_MPT->AddProperty("RINDEX", photonEnergiesForEJ254, EJ254RefIndex, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254);
	EJ254_MPT->AddProperty("ABSLENGTH", photonEnergiesForEJ254, EJ254AbsLength, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_EJ254);
	static G4double scintillationYield;
	//
	// The following scintillation yield numbers are taken from the EJ254_onePercentNaturalBoronDoped data sheet for the case of Boron doping only.
	// We will have to use these numbers for now until we can find those of the case for Lithium.
	//
	if(dopingFraction < .025)
		scintillationYield = ((8600. - 9200.)/(.025 - .01)*(dopingFraction
				- .025) + 8600.)/MeV;
	else if(dopingFraction >= .025)
		scintillationYield = ((7500. - 8600.)/(.05 - .025)*(dopingFraction
				- .025) + 8600.)/MeV;
	EJ254_MPT->AddConstProperty("SCINTILLATIONYIELD", scintillationYield);
	EJ254_MPT->AddConstProperty("RESOLUTIONSCALE", 1.0); // Fix later.
	EJ254_MPT->AddConstProperty("FASTTIMECONSTANT", 2.2*ns); // Fix later.
	//
	// Slow component will not be used for now.
	//
	EJ254_MPT->AddConstProperty("SLOWTIMECONSTANT", 10.*ns);
	//
	// Yield ratio is not needed for now when there is only one component.
	// Yield ratio is the relative strength of the fast component as a fraction of total scintillation yield.
	//
	EJ254_MPT->AddConstProperty("YIELDRATIO", .5); // Fix later.
	// Set Birks constant for quenching factor.
	G4double birksConst = 0.126*mm/MeV;
	
	EJ254_onePercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_onePercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_twoAndHalfPercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_twoAndHalfPercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_fivePercentNaturalBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_fivePercentNaturalBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_onePercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_onePercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_twoAndHalfPercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_twoAndHalfPercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_fivePercentEnrichedBoronDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_fivePercentEnrichedBoronDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_onePercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_onePercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_twoAndHalfPercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_twoAndHalfPercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_fivePercentNaturalLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_fivePercentNaturalLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_onePercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_onePercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_twoAndHalfPercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_twoAndHalfPercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
	EJ254_fivePercentEnrichedLithiumDoped->SetMaterialPropertiesTable(EJ254_MPT);
	EJ254_fivePercentEnrichedLithiumDoped->GetIonisation()->SetBirksConstant(birksConst);
}

//
// Detector geometry.
//
void MTCG4DetectorConstruction::SetupGeometry()
{
 	//
	// Set half-lengths: Geant defines geometry dimensions using half-lengths.
	//
	const G4double worldHalfX = .5*WORLD_DIMENSION*mm;
	const G4double worldHalfY = .5*WORLD_DIMENSION*mm;
	const G4double worldHalfZ = .5*WORLD_DIMENSION*mm;
	scintHalfX = .5*SCINTILLATOR_DIMENSION*mm;
	scintHalfY = .5*SCINTILLATOR_DIMENSION*mm;
	scintHalfZ = .5*SCINTILLATOR_DIMENSION*mm;
	const G4double MCPFaceHalfDimension = .5*MCP_FACE_DIMENSION*mm;
	const G4double MCPHalfThickness = .5*MCP_THICKNESS*mm;
	const G4double MCPBorder = MCP_BORDER*mm;
	const G4double MCPActiveAreaFaceHalfDimension = MCPFaceHalfDimension - MCPBorder;
	const G4double MCPActiveAreaBorder = MCP_ACTIVE_AREA_BORDER*mm;
	const G4double MCPPixelVolumeAreaFaceHalfDimension = MCPActiveAreaFaceHalfDimension - MCPActiveAreaBorder;
	const G4double pixelFaceHalfDimension = .5*PIXEL_FACE_DIMENSION*mm;
	const G4double pixelHalfThickness = .5*PIXEL_THICKNESS*mm;
	const G4double pixelHalfSpacing = .5*PIXEL_SPACING*mm;


	//
	// Set materials to be used for detector construction.
	//
	worldMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial("Air");
	pixelMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial("BialkaliPhotoCathode");
	MCPMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial("Aluminum");
	
	// Construct string to be used to identify scintillator properties.
	G4String
		scintillatorMaterialString,
		neutronCaptureMaterialString,
		dopingFractionString,
		enrichmentString;
	if(neutronCaptureMaterial == "boron")
		neutronCaptureMaterialString = "Boron";
	else if(neutronCaptureMaterial == "lithium")
		neutronCaptureMaterialString = "Lithium";
	else{
		G4cout << "Scintillator material was not correctly specified." << G4endl;
		return;
	}
	if(dopingFraction == .01)
		dopingFractionString = "one";
	else if(dopingFraction == 0.025)
		dopingFractionString = "twoAndHalf";
	else if(dopingFraction == 0.05)
		dopingFractionString = "five";
	else{
		G4cout << "Scintillator material was not correctly specified." << G4endl;
		return;
	}
	if(dopantIsEnriched == true)
		enrichmentString = "Enriched";
	else if(dopantIsEnriched == false)
		enrichmentString = "Natural";
	else{
		G4cout << "Scintillator material was not correctly specified." << G4endl;
		return;
	}

	// Choose scintillator material to be used with given properties.
	scintillatorMaterialString =
	"EJ254_"
	+ dopingFractionString
	+ "Percent"
	+ enrichmentString
	+ neutronCaptureMaterialString
	+ "Doped";
	G4cout << "scintillator material choice: "
		<< scintillatorMaterialString
		<< G4endl;
	scintillatorMaterial = SetScintillatorMaterial(scintillatorMaterialString);

	//
	// World volume.
 	//
	worldSolid = new G4Box("world_solid", worldHalfX, worldHalfY, worldHalfZ);
	worldLogical = new G4LogicalVolume(worldSolid, worldMaterial, "world_logical");
	worldPhysical = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), worldLogical, "world_physical", 0, false, 0); // This is the world volume. All that Geant knows.

	//
	// Scintillator volume.
 	//
	scintillatorSolid = new G4Box("scintillator_solid", scintHalfX, scintHalfY, scintHalfZ);
//#if THERMAL_SCATTERING_FLAG
//	scintillatorLogical = new G4LogicalVolume(scintillatorSolid, scintillatorMaterial_TS, "scintillator_logical"); // Scintillator volume for thermal scattering.
//#else
	scintillatorLogical = new G4LogicalVolume(scintillatorSolid, scintillatorMaterial, "scintillator_logical");	// Scintillator volume for non-thermal scattering.
//#endif
	scintillatorPhysical = new G4PVPlacement(0, // Rotation matrix is 0. The scintillator cube is placed without any rotation.
			G4ThreeVector(0., 0., 0.), // Translation vector is 0. The scintillator cube is placed in the center of the world volume.
			scintillatorLogical,
			"scintillator_physical",
			worldLogical, // It is placed in the world logical volume.
			false,
			0);

	if(MCPsArePlaced == true){
		//
		// MCP volumes.
		// "MCP quad-set" means the collection of 4 MCPs on one face of the scintillator cube.
		// All quad-set faces, and therefor all MCP faces, are facing inward toward the center of the scintillating cube.
		// The center of the scintillating cube is placed at the Cartesian origin x = y = z = 0.
	 	//
		MCPQuadSetSolid = new G4Box("MCP_quad_set_solid", 2.*MCPFaceHalfDimension, 2.*MCPFaceHalfDimension, MCPHalfThickness);
		//
		// Here we create a template MCP quad-set solid whos center is placed at the cartesian origin x = y = z = 0, and the face is facing in the +z direction.
		//
		MCPQuadSetLogical = new G4LogicalVolume(MCPQuadSetSolid, MCPMaterial, "MCP_quad_set_logical");
		//
		// The 0th quad-set refers to the 4 MCPs on the +x face.
		// The top of the quad-set face is pointing in the +z direction.
		//
		G4RotationMatrix MCPRotationMatrix0;
		MCPRotationMatrix0.rotateX(.5*pi);
		MCPRotationMatrix0.rotateZ(-.5*pi);
		//
		// We rotate -.5*pi because we want the pixels of the MCP (that are placed along the face of the MCP) to be facing inward toward the center of the scintillating cube.
		//
		G4ThreeVector MCPTranslationVector0;
		MCPTranslationVector0 = G4ThreeVector(scintHalfZ+MCPHalfThickness+scintillatorBufferThickness, 0, 0);
		MCPQuadSetPhysical0 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix0, MCPTranslationVector0), MCPQuadSetLogical, "MCP_quad_set_physical_0", worldLogical, false, 0);
		//
		// The 1st quad-set refers to the 4 MCPs on the -x face.
		// The top of the quad-set face is pointing in the +z direction.
		//
		G4RotationMatrix MCPRotationMatrix1;
		MCPRotationMatrix1.rotateX(.5*pi);
		MCPRotationMatrix1.rotateZ(.5*pi);
		G4ThreeVector MCPTranslationVector1;
		MCPTranslationVector1 = G4ThreeVector(-(scintHalfZ+MCPHalfThickness+scintillatorBufferThickness), 0, 0);
		MCPQuadSetPhysical1 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix1, MCPTranslationVector1), MCPQuadSetLogical, "MCP_quad_set_physical_1", worldLogical, false, 1);
		//
		// The 2nd quad-set refers to the 4 MCPs on the +y face.
		// The top of the quad-set face is pointing in the +z direction.
		//
		G4RotationMatrix MCPRotationMatrix2;
		MCPRotationMatrix2.rotateX(.5*pi);
		G4ThreeVector MCPTranslationVector2;
		MCPTranslationVector2 = G4ThreeVector(0, scintHalfZ+MCPHalfThickness+scintillatorBufferThickness, 0);
		MCPQuadSetPhysical2 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix2, MCPTranslationVector2), MCPQuadSetLogical, "MCP_quad_set_physical_2", worldLogical, false, 2);
		//
		// The 3rd quad-set refers to the 4 MCPs on the -y face.
		// The top of the quad-set face is pointing in the +z direction.
		//
		G4RotationMatrix MCPRotationMatrix3;
		MCPRotationMatrix3.rotateX(.5*pi);
		MCPRotationMatrix3.rotateZ(pi);
		G4ThreeVector MCPTranslationVector3;
		MCPTranslationVector3 = G4ThreeVector(0, -(scintHalfZ+MCPHalfThickness+scintillatorBufferThickness), 0);
		MCPQuadSetPhysical3 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix3, MCPTranslationVector3), MCPQuadSetLogical, "MCP_quad_set_physical_3", worldLogical, false, 3);
		//
		// The 4th quad-set refers to the 4 MCPs on the +z face.
		// The top of the quad-set face is pointing in the +x direction.
		//
		G4RotationMatrix MCPRotationMatrix4;
		MCPRotationMatrix4.rotateX(pi);
		MCPRotationMatrix4.rotateZ(.5*pi);
		G4ThreeVector MCPTranslationVector4;
		MCPTranslationVector4 = G4ThreeVector(0, 0, scintHalfZ+MCPHalfThickness+scintillatorBufferThickness);
		MCPQuadSetPhysical4 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix4, MCPTranslationVector4), MCPQuadSetLogical, "MCP_quad_set_physical_4", worldLogical, false, 4);   	
		//
		// The 5th quad-set refers to the 4 MCPs on the -z face.
		// The top of the quad-set face is pointing in the -x direction.
		//
		G4RotationMatrix MCPRotationMatrix5;
		MCPRotationMatrix5.rotateZ(.5*pi);
		G4ThreeVector MCPTranslationVector5;	
		MCPTranslationVector5 = G4ThreeVector(0, 0, -(scintHalfZ+MCPHalfThickness+scintillatorBufferThickness));
		MCPQuadSetPhysical5 = new G4PVPlacement(G4Transform3D(MCPRotationMatrix5, MCPTranslationVector5), MCPQuadSetLogical, "MCP_quad_set_physical_5", worldLogical, false, 5);

		//
		// "MCP row" means the horizontal row of MCPs if the top of the quad-set were pointing vertically up.
		//
		MCPRowSolid = new G4Box("MCP_row_solid", 2.*MCPFaceHalfDimension, MCPFaceHalfDimension, MCPHalfThickness);
		MCPRowLogical = new G4LogicalVolume(MCPRowSolid, MCPMaterial, "MCP_row_logical");
		MCPRowPhysical = new G4PVReplica("MCP_row_physical", MCPRowLogical, MCPQuadSetLogical, kYAxis, 2, 2.*MCPFaceHalfDimension);

		//
		// "MCP" means one multi-channel plate.
		//
		MCPSolid = new G4Box("MCP_solid", MCPFaceHalfDimension, MCPFaceHalfDimension, MCPHalfThickness);
		MCPLogical = new G4LogicalVolume(MCPSolid, MCPMaterial, "MCP_logical");
		MCPPhysical = new G4PVReplica("MCP_physical", MCPLogical, MCPRowLogical, kXAxis, 2, 2.*MCPFaceHalfDimension);

		//
		// Test marker volume.
		//
		//	testMarkerSolid = new G4Box("test_solid", pixelHalfThickness, pixelHalfThickness, pixelHalfThickness); // This is used to confirm MCP orientation.
		//	testMarkerLogical = new G4LogicalVolume(testMarkerSolid, MCPMaterial, "test_logical");
		//	testMarkerPhysical = new G4PVPlacement(0 , G4ThreeVector(MCPFaceHalfDimension-pixelHalfThickness, MCPFaceHalfDimension-pixelHalfThickness, MCPHalfThickness-pixelHalfThickness), testMarkerLogical, "test_physical", MCPLogical, false, 0);

		//
		// "MCP active area" means the active face area (excluding the non-active border around this face) of the MCP.
		//
		MCPActiveAreaSolid = new G4Box("MCP_active_area_solid", MCPFaceHalfDimension-MCPBorder, MCPFaceHalfDimension-MCPBorder, pixelHalfThickness);
		MCPActiveAreaLogical = new G4LogicalVolume(MCPActiveAreaSolid, MCPMaterial, "MCP_active_area_logical");
		MCPActiveAreaPhysical = new G4PVPlacement(0, G4ThreeVector(0, 0, MCPHalfThickness-pixelHalfThickness), MCPActiveAreaLogical,
				"MCP_active_area_physical", MCPLogical, false, 0);

		//
		// "MCP pixel area" means the total face area of the MCPs when a the individual pixels and the spacings in between the pixels is not discriminated.
		//
		MCPPixelVolumeAreaSolid = new G4Box("MCP_pixel_area_solid", MCPPixelVolumeAreaFaceHalfDimension, MCPPixelVolumeAreaFaceHalfDimension, pixelHalfThickness);
		MCPPixelVolumeAreaLogical = new G4LogicalVolume(MCPPixelVolumeAreaSolid, MCPMaterial, "MCP_pixel_area_logical");
		MCPPixelVolumeAreaPhysical = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), MCPPixelVolumeAreaLogical,
				"MCP_pixel_area_physical", MCPActiveAreaLogical, false, 0);

		//
		// Pixel volumes.
		// "Pixel-volume-row" means the horizontal row of pixel volumes if the top of the MCP were pointing vertically up. 
		//
		pixelVolumeRowSolid = new G4Box("pixel_row_solid", MCPPixelVolumeAreaFaceHalfDimension, pixelHalfSpacing, pixelHalfThickness);
		pixelVolumeRowLogical = new G4LogicalVolume(pixelVolumeRowSolid, MCPMaterial, "pixel_row_logical");
		pixelVolumeRowPhysical = new G4PVReplica("pixel_row_physical", pixelVolumeRowLogical, MCPPixelVolumeAreaLogical, kYAxis, 8, 2.*MCPPixelVolumeAreaFaceHalfDimension/8.);

		//
		// "Pixel-volume" means the volume of the pixel and an added constant border in 3 dimensions.
		// The pixel-volume is defined so that the pixel-volume-row is one solid bar without any divisions.
		//
		pixelVolumeSolid = new G4Box("pixel_volume_solid", pixelHalfSpacing, pixelHalfSpacing, pixelHalfThickness);
		pixelVolumeLogical = new G4LogicalVolume(pixelVolumeSolid, MCPMaterial, "pixel_volume_logical");
		pixelVolumePhysical = new G4PVReplica("pixel_volume_physical", pixelVolumeLogical, pixelVolumeRowLogical, kXAxis, 8, 2.*MCPPixelVolumeAreaFaceHalfDimension/8.);

		//
		// "Pixel" means the actual pixel volume that is sensitive to photons arriving at its surface.
		//
		pixelSolid = new G4Box("pixel_solid", pixelFaceHalfDimension, pixelFaceHalfDimension, pixelHalfThickness);
		pixelLogical = new G4LogicalVolume(pixelSolid, pixelMaterial, "pixel");
		pixelPhysical = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), pixelLogical, "pixel_physical", pixelVolumeLogical, false, 0);

		//
		// Sensitive detectors.
		// Get the sensitive detector manager and define aPixelSD if it is not yet defined.
		//
		SDman = G4SDManager::GetSDMpointer();
		if(!aPixelSD){
			G4String MTCG4PixelSDName = "/MTCG4Pixel/MTCG4PixelSD";
			aPixelSD = new MTCG4PixelSD( MTCG4PixelSDName );
			SDman->AddNewDetector( aPixelSD );
			pixelLogical->SetSensitiveDetector( aPixelSD );
		}
	}
}

//
// Geometry surface properties.
// This process is what makes a photon sensitive surface along the border of a geometrical shape such as our MCP pixel volumes.
// Important note: this needs to be done after the definitions of materials, as I have done in this code, in order to work.
//
void MTCG4DetectorConstruction::SetSurfaceProperties(){
	if(MCPsArePlaced == true){
		//
		// Using Quantum Efficiency is the proper way to do detector photon sensing in Geant.
		//
		if(opticalSurfacePropertiesAreSet){ 
			const G4double wavelengthForBialkali[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] =
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
			static G4double photonEnergiesForBialkali[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL];
			for(G4int i=0; i<NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL; i++) {
				G4double energy = h_Planck*c_light/(EJ254RefractiveIndex*wavelengthForBialkali[i]);
				photonEnergiesForBialkali[i] = energy;
			}
			
			//
			// There are two types of bialkali photocathodes simulated below.
			// For now we will pick KCsSb bialkali material as our photocathode material.
			// We do not know the content of our photocathode in the Planacon MCP tubes,
			// but we can pick the the correct one, once we know.
			//

			// Index of refraction (real part) and extinction coefficient (imaginary part of index
			// of refraction) for blue-sensitive bialkali (KCsSb) photocathode.
			// Reference: Nuclear Instruments and Methods in Physics Research Section A: Accelerators,
			// Spectrometers, Detectors and Associated Equipment
			// Volume 539, Issues 1-2, 21 February 2005, Pages 217-235
			//
			static G4double pixelRealRIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] =
			{
				2.96, // 700*nm, not listed in reference so used simple constant extrapolation.
				2.95, // 650*nm
				2.99, // 600*nm
				3.17333, // 550*nm
				3.00, // 500*nm
				2.81333, // 450*nm
				2.24667, // 400*nm
				1.92, // 350*nm
				1.92, // 300*nm, not listed in reference so used simple constant extrapolation.
				1.92, // 250*nm, not listed in reference so used simple constant extrapolation.
				1.92 // 200*nm, not listed in reference so used simple constant extrapolation.
			};
			static G4double pixelImaginaryRIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] =
			{
				0.33, // 700*nm, not listed in reference so used simple constant extrapolation.
				0.34, // 650*nm
				0.39333, // 600*nm
				0.59667, // 550*nm
				1.06, // 500*nm
				1.46, // 450*nm
				1.69667, // 400*nm
				1.69, // 350*nm
				1.69, // 300*nm, not listed in reference so used simple constant extrapolation.
				1.69, // 250*nm, not listed in reference so used simple constant extrapolation.
				1.69 // 200*nm, not listed in reference so used simple constant extrapolation.
			};
			////
			//// Index of refraction (real part) and extinction coefficient (imaginary part of index
			//// of refraction) for green-enhanced bialkali (RbCsSb) photocathode.
			//// Reference: Nuclear Instruments and Methods in Physics Research Section A: Accelerators,
			//// Spectrometers, Detectors and Associated Equipment
			//// Volume 539, Issues 1-2, 21 February 2005, Pages 217-235
			////
			//static G4double pixelRealRIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] =
			//{
			//	3.13, // 700*nm, not listed in reference so used simple constant extrapolation.
			//	3.14, // 650*nm
			//	3.24, // 600*nm
			//	3.26, // 550*nm
			//	3.16, // 500*nm
			//	2.58667, // 450*nm
			//	2.24667, // 400*nm
			//	2.07, // 350*nm
			//	2.07, // 300*nm, not listed in reference so used simple constant extrapolation.
			//	2.07, // 250*nm, not listed in reference so used simple constant extrapolation.
			//	2.07 // 200*nm, not listed in reference so used simple constant extrapolation.
			//};
			//static G4double pixelImaginaryRIndex[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] =
			//{
			//	0.35, // 700*nm, not listed in reference so used simple constant extrapolation.
			//	0.37, // 650*nm
			//	0.44, // 600*nm
			//	0.82667, // 550*nm
			//	1.21, // 500*nm
			//	1.38333, // 450*nm
			//	1.18333, // 400*nm
			//	1.22, // 350*nm
			//	1.22, // 300*nm, not listed in reference so used simple constant extrapolation.
			//	1.22, // 250*nm, not listed in reference so used simple constant extrapolation.
			//	1.22 // 200*nm, not listed in reference so used simple constant extrapolation.
			//};

			//
			// Quantum efficiency calculated from Planacon MCP spec-sheet for XP85012.
			// Conversion from spectral response to quantum efficiency was done using
			// formula QE = (Sk/lambda)*(h*c/e) where Sk is spectral response in amperes
			// per watt, lambda is optical wavelength in vacuum, h is plank's constant,
			// c is speed of light in vacuum, e is electron charge in coulombs.
			// Reference: www.burle.com/cgi/byteserver.pl/pdf/ads/image_ad04.pdf.
			//
			static G4double pixelQuantumEfficiency[NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL] = // Quantum efficiency of photon detection at pixel surface.
			{
				0.000, // 700*nm
				0.001, // 650*nm
				0.008, // 600*nm
				0.045, // 550*nm
				0.124, // 500*nm
				0.220, // 450*nm
				0.248, // 400*nm
				0.213, // 350*nm
				0.165, // 300*nm
				0.100, // 250*nm
				0.005 // 200*nm
			};
			G4MaterialPropertiesTable* pixel_MPT = new G4MaterialPropertiesTable();
			pixel_MPT->AddProperty("EFFICIENCY", photonEnergiesForBialkali, pixelQuantumEfficiency, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL);
			pixel_MPT->AddProperty("REALRINDEX", photonEnergiesForBialkali, pixelRealRIndex, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL);
			pixel_MPT->AddProperty("IMAGINARYRINDEX", photonEnergiesForBialkali, pixelImaginaryRIndex, NUMBER_OF_PHOTON_ENERGY_ENTRIES_FOR_BIALKALI_PIXEL_MATERIAL);
			G4OpticalSurface* pixelOpticalSurface = new G4OpticalSurface("optical_surface", glisur, polished, dielectric_metal);
			pixelOpticalSurface->SetMaterialPropertiesTable(pixel_MPT);
			new G4LogicalSkinSurface("pixel_surface", pixelLogical, pixelOpticalSurface);
		}
	}
}

//
// Visualisation attributes.
//
void MTCG4DetectorConstruction::SetVisualisationAttributes(){
	// Make colours.
	G4Colour  white   (1.0, 1.0, 1.0);
	G4Colour  grey    (0.5, 0.5, 0.5);
	G4Colour  lgrey   (.85, .85, .85);
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
	
	worldLogical->SetVisAttributes(G4VisAttributes::Invisible);
	if(scintillatorVisualisation){
	//
	// Scintillator cube color.
 	//
		G4VisAttributes* scintillatorAttributes = new G4VisAttributes(white);
		//scintillatorAttributes->SetForceSolid(true);
		scintillatorLogical->SetVisAttributes(scintillatorAttributes);
	}
	else{
		scintillatorLogical->SetVisAttributes(G4VisAttributes::Invisible);
	}

	if(MCPsArePlaced == true){
		if(mcpPixelVisualisation){
			//
			// Pixel color.
		 	//
			G4VisAttributes* pixelAttributes = new G4VisAttributes(cyan);
			//pixelAttributes->SetForceSolid(true);
			pixelLogical->SetVisAttributes(pixelAttributes);
		}
		else{
			pixelLogical->SetVisAttributes(G4VisAttributes::Invisible);
		}

		if(mcpModuleVisualisation){
			G4VisAttributes* MCPAttributes = new G4VisAttributes(magenta);
			MCPLogical->SetVisAttributes(MCPAttributes);
		}
		else{
			MCPLogical->SetVisAttributes(G4VisAttributes::Invisible);
		}

		pixelVolumeLogical->SetVisAttributes(G4VisAttributes::Invisible);
		pixelVolumeRowLogical->SetVisAttributes(G4VisAttributes::Invisible);
		MCPPixelVolumeAreaLogical->SetVisAttributes(G4VisAttributes::Invisible);
		MCPActiveAreaLogical->SetVisAttributes(G4VisAttributes::Invisible);
		MCPRowLogical->SetVisAttributes(G4VisAttributes::Invisible);
		MCPQuadSetLogical->SetVisAttributes(G4VisAttributes::Invisible);
	}

	//
	// Test marker volume.
 	// Used for testing.
	//
	//G4VisAttributes* testMarkerAttributes = new G4VisAttributes(G4Colour(1., 0., 0., .1)); // This is used to confirm MCP orientation.
	//testMarkerAttributes->SetForceSolid(true);
	//testMarkerLogical->SetVisAttributes(testMarkerAttributes);
}

void MTCG4DetectorConstruction::UpdateGeometry()
{
	//
	//// Clean old geometry, if any.
	//
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4SurfaceProperty::CleanSurfacePropertyTable();
	
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

G4Material* MTCG4DetectorConstruction::SetScintillatorMaterial(G4String materialChoice)
{
	G4Material* pttoMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	//if(pttoMaterial){
	//	if(scintillatorLogical){
	//		scintillatorLogical -> SetMaterial(scintillatorMaterial);
	//		G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
	//	}
	//}
	return pttoMaterial;
}

// Density approximated through extrapolation of data given by
// EJ254 data shieet.
// Density for Lithium doped scintillator needs to be found.
// For now, I just use the same extrapolation as that of the case of
// Boron dopend EJ254 scintillator for the both cases of Boron and
// Lithium doped scintillator.
G4double MTCG4DetectorConstruction::GetEJ254Density(G4double dopingFraction){
	static G4double EJ254Density;
	if(dopingFraction < .025)
		EJ254Density =
			((1.023 - 1.021)/(.025 - .01)*(dopingFraction	- .025) + 1.023)*g/cm3;
	else if(dopingFraction >= .025)
		EJ254Density =
			((1.026 - 1.023)/(.05 - .025)*(dopingFraction	- .025) + 1.023)*g/cm3;
	return EJ254Density;
}

// Set doping dependent fractional content of Anthracene
// according to extrapolation of EJ254 data sheet by Eljen Technology.
G4double MTCG4DetectorConstruction::GetAnthraceneFraction(G4double dopingFraction){
	static G4double anthraceneFraction;
	if(dopingFraction < .025)
		anthraceneFraction =
			(.56 - .60)/(.025 - .01)*(dopingFraction - .025) + .56;
	else
		anthraceneFraction =
			(.48 - .56)/(.05 - .025)*(dopingFraction - .025) + .56;
	return anthraceneFraction;
}

G4double MTCG4DetectorConstruction::GetPolyvinyltolueneFraction(G4double dopingFraction, G4double anthraceneFraction){
	static G4double polyvinyltolueneFraction;
	polyvinyltolueneFraction = 1. - dopingFraction - anthraceneFraction;
	return polyvinyltolueneFraction;
}
