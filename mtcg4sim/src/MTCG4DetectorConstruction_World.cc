#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include "MTCG4DetectorConstruction.hh"

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

//
// Setup world.
//
void MTCG4DetectorConstruction::SetupWorld()
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
}

//
// Material properties for air (for world volume).
//
void MTCG4DetectorConstruction::SetAirMaterialProperties()
{
	// Set different photon wavelengths that will be referenced in calculating
	// optical properties of air.
	const int N_SPEC_POINTS = 7;
	const G4double WAVELENGTHS[N_SPEC_POINTS] = 
	{
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		300*nm,
		200*nm
	};
	// Set order of photon energies for air in increasing order.
	G4double photonEnergies[N_SPEC_POINTS];
	for (G4int i = 0; i < N_SPEC_POINTS; ++i) {
		photonEnergies[i] = h_Planck*c_light/WAVELENGTHS[i];
	}
	// Dispersion formula used for air: n-1 =
	// C1/(C2-lambda^-2)+C3/(C4-lambda^-2), where C1 = 5792105E-8; C2 = 238.0185;
	// C3 = 167917E-8; C4 = 57.362 and lambda is in units of um.  Reference:
	// Philip E. Ciddor. Refractive index of air: new equations for the visible
	// and near infrared, Appl. Optics 35, 1566-1573 (1996)
	G4double rIndices[N_SPEC_POINTS] =
	{
		1.0002750, // 800*nm
		1.0002758, // 700*nm
		1.0002770, // 600*nm
		1.0002790, // 500*nm
		1.0002828, // 400*nm
		1.0002916, // 300*nm
		1.0003238 // 200*nm
	};
	G4double absLengths[N_SPEC_POINTS] = {
		1.0e6*mm, // 800*nm
		1.0e6*mm, // 700*nm
		1.0e6*mm, // 600*nm
		1.0e6*mm, // 500*nm
		1.0e6*mm, // 400*nm
		1.0e6*mm, // 300*nm
		1.0e6*mm // 200*nm
	}; // Fix later.
	// Attenuation length formula used for Rayleigh scattering.
	// Attenuation length = 1/alpha where alpha =
	// 2*(2*pi/lambda)^4/(3*pi*N)*(n-1)^2.
	// lambda = WAVELENGTHS(cm), N(density of molecules) = 2.7e19(cm^-3),
	// n = refractive index.
	// Reference: T. Tsang, Classical electrodynamics, Blue Sky and Red Sunset,
	// p.202.
	G4double rayleighScatteringLengths[N_SPEC_POINTS] = {
		4.42161e+05*m, // 800*nm
		2.57685e+05*m, // 700*nm
		1.37889e+05*m, // 600*nm
		6.55476e+04*m, // 500*nm
		2.61316e+04*m, // 400*nm
		7.77670e+03*m, // 300*nm
		1.24581e+03*m // 200*nm
	};
	G4MaterialPropertiesTable* air_MPT = new G4MaterialPropertiesTable();
	air_MPT->AddProperty("RINDEX", photonEnergies, rIndices, N_SPEC_POINTS);
	air_MPT->AddProperty("ABSLENGTH", photonEnergies, absLengths,
			N_SPEC_POINTS);
	air_MPT->AddProperty("RAYLEIGH", photonEnergies,
			rayleighScatteringLengths, N_SPEC_POINTS);
	fAir->SetMaterialPropertiesTable(air_MPT);
}

