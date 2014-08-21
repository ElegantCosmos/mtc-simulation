#include "G4NistManager.hh"

#include "MTCG4DetectorConstruction.hh"

//
// Define all materials to be used in detector construction.
//
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
	//fPmtVacuum = nistManager->
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

	//// Do not use PVT any more. Now we use exact C/H/10B count from EJ-254
	//// data sheet.
	//// Define Polyvinyltoluene (PVT).
	//// PVT data taken from following paper:
	//// "Radiation damage in polyvinyltoluene (PVT)"
	//// Elsevier Volume 63, Issue 1, January 2002, Pages 89–92,
	//// L. Torrisi, Dipartimento di Fisica, Università di Messina, Ctr. Papardo
	//// Sperone, 31–98166 S. Agata, Messina, Italy
	//G4Material* polyvinyltoluene =
	// 	new G4Material("polyvinyltoluene", density=1.032*g/cm3, ncomponents=2);
	//polyvinyltoluene->AddElement(C, natoms=9);
	//polyvinyltoluene->AddElement(H, natoms=10);

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
	
	//
	// Non-doped scintillator.
	//

	G4double dopingFraction, hydrogenMassFraction, carbonMassFraction;
	// scintillator doped with 0 percent doping.
	dopingFraction = 0.*perCent;
	GetHydrogenCarbonMassFractions(dopingFraction, hydrogenMassFraction,
			carbonMassFraction);
	fEJ254_ZeroPercentNaturalBoronDoped = new G4Material(
			"EJ254_ZeroPercentDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 2);
	fEJ254_ZeroPercentNaturalBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_ZeroPercentNaturalBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// Set doping fractions for each type of doping fraction.
	// The mass fraction of all other materials comprising the scintillator
	// are set to be a function of the doping fraction.

	//
	// Boron doped scintillator.
	//

	// scintillator doped with 1 percent natural Boron.
	dopingFraction = 1.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_OnePercentNaturalBoronDoped = new G4Material(
	//		"EJ254_OnePercentNaturalBoronDoped",
	//		density =
	//			GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_OnePercentNaturalBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_OnePercentNaturalBoronDoped->AddElement(BNatural,
	//		fractionMass = dopingFraction);
	fEJ254_OnePercentNaturalBoronDoped = new G4Material(
			"EJ254_OnePercentNaturalBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_OnePercentNaturalBoronDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_OnePercentNaturalBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_OnePercentNaturalBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// scintillator doped with 2.5 percent natural Boron.
	dopingFraction = 2.5*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_TwoAndHalfPercentNaturalBoronDoped = new G4Material(
	//		"EJ254_TwoAndHalfPercentNaturalBoronDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddElement(BNatural,
	//		fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped = new G4Material(
			"EJ254_TwoAndHalfPercentNaturalBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// scintillator doped with 5 percent natural Boron.
	dopingFraction = 5.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_FivePercentNaturalBoronDoped = new G4Material(
	//		"EJ254_FivePercentNaturalBoronDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_FivePercentNaturalBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_FivePercentNaturalBoronDoped->AddElement(BNatural,
	//		fractionMass = dopingFraction);
	fEJ254_FivePercentNaturalBoronDoped = new G4Material(
			"EJ254_FivePercentNaturalBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_FivePercentNaturalBoronDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_FivePercentNaturalBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_FivePercentNaturalBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
	
	// scintillator doped with 1 percent enriched Boron.
	dopingFraction = 1.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_OnePercentEnrichedBoronDoped = new G4Material(
	//		"EJ254_OnePercentEnrichedBoronDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_OnePercentEnrichedBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_OnePercentEnrichedBoronDoped->AddElement(BEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_OnePercentEnrichedBoronDoped = new G4Material(
			"EJ254_OnePercentEnrichedBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_OnePercentEnrichedBoronDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_OnePercentEnrichedBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_OnePercentEnrichedBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// scintillator doped with 2.5 percent enriched Boron.
	dopingFraction = 2.5*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_TwoAndHalfPercentEnrichedBoronDoped = new G4Material(
	//		"EJ254_TwoAndHalfPercentEnrichedBoronDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddElement(BEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped = new G4Material(
			"EJ254_TwoAndHalfPercentEnrichedBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
	
	// scintillator doped with 5 percent enriched Boron.
	dopingFraction = 5.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_FivePercentEnrichedBoronDoped = new G4Material(
	//		"EJ254_FivePercentEnrichedBoronDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_FivePercentEnrichedBoronDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_FivePercentEnrichedBoronDoped->AddElement(BEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_FivePercentEnrichedBoronDoped = new G4Material(
			"EJ254_FivePercentEnrichedBoronDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_FivePercentEnrichedBoronDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_FivePercentEnrichedBoronDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_FivePercentEnrichedBoronDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	//
	// Lithium doped scintillator.
	//
	
	// scintillator doped with 1 percent natural Lithium.
	dopingFraction = 1.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_OnePercentNaturalLithiumDoped = new G4Material(
	//		"EJ254_OnePercentNaturalLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_OnePercentNaturalLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_OnePercentNaturalLithiumDoped->AddElement(
	//		LiNatural,
	//		fractionMass = dopingFraction);
	fEJ254_OnePercentNaturalLithiumDoped = new G4Material(
			"EJ254_OnePercentNaturalLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_OnePercentNaturalLithiumDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_OnePercentNaturalLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_OnePercentNaturalLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// scintillator doped with 2.5 percent natural Lithium.
	dopingFraction = 2.5*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_TwoAndHalfPercentNaturalLithiumDoped = new G4Material(
	//		"EJ254_TwoAndHalfPercentNaturalLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddElement(
	//		LiNatural,
	//		fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped = new G4Material(
			"EJ254_TwoAndHalfPercentNaturalLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
	
	// scintillator doped with 5 percent natural Lithium.
	dopingFraction = 5.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_FivePercentNaturalLithiumDoped = new G4Material(
	//		"EJ254_FivePercentNaturalLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_FivePercentNaturalLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_FivePercentNaturalLithiumDoped->AddElement(
	//		LiNatural,
	//		fractionMass = dopingFraction);
	fEJ254_FivePercentNaturalLithiumDoped = new G4Material(
			"EJ254_FivePercentNaturalLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_FivePercentNaturalLithiumDoped->AddElement(
			BNatural,
			fractionMass = dopingFraction);
	fEJ254_FivePercentNaturalLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_FivePercentNaturalLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
	
	// scintillator doped with 1 percent enriched Lithium.
	dopingFraction = 1.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_OnePercentEnrichedLithiumDoped = new G4Material(
	//		"EJ254_OnePercentEnrichedLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_OnePercentEnrichedLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_OnePercentEnrichedLithiumDoped->AddElement(
	//		LiEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_OnePercentEnrichedLithiumDoped = new G4Material(
			"EJ254_OnePercentEnrichedLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_OnePercentEnrichedLithiumDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_OnePercentEnrichedLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_OnePercentEnrichedLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);

	// scintillator doped with 2.5 percent enriched Lithium.
	dopingFraction = 2.5*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_TwoAndHalfPercentEnrichedLithiumDoped = new G4Material(
	//		"EJ254_TwoAndHalfPercentEnrichedLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddElement(
	//		LiEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped = new G4Material(
			"EJ254_TwoAndHalfPercentEnrichedLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
	
	// scintillator doped with 5 percent enriched Lithium.
	dopingFraction = 5.*perCent;
	GetHydrogenCarbonMassFractions(
			dopingFraction,
		   	hydrogenMassFraction,
			carbonMassFraction);
	//fEJ254_FivePercentEnrichedLithiumDoped = new G4Material(
	//		"EJ254_FivePercentEnrichedLithiumDoped",
	//		density = GetEJ254Density(dopingFraction),
	//		ncomponents = 2);
	//fEJ254_FivePercentEnrichedLithiumDoped->AddMaterial(
	//		polyvinyltoluene,
	//		fractionMass = 100*perCent - dopingFraction);
	//fEJ254_FivePercentEnrichedLithiumDoped->AddElement(
	//		LiEnriched,
	//		fractionMass = dopingFraction);
	fEJ254_FivePercentEnrichedLithiumDoped = new G4Material(
			"EJ254_FivePercentEnrichedLithiumDoped",
			density = GetEJ254Density(dopingFraction),
			ncomponents = 3);
	fEJ254_FivePercentEnrichedLithiumDoped->AddElement(
			BEnriched,
			fractionMass = dopingFraction);
	fEJ254_FivePercentEnrichedLithiumDoped->AddElement(
			H,
			fractionMass = hydrogenMassFraction);
	fEJ254_FivePercentEnrichedLithiumDoped->AddElement(
			C,
			fractionMass = carbonMassFraction);
}// DefineMaterial().


