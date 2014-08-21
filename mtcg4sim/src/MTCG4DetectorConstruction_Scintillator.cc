#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"

#include "MTCG4DetectorConstruction.hh"

//
// Some detector geometry constants and other constants.
//

// (mm) Length of one side of sctintillator cube. Our current mTC scintillator
// cube dimension is 130mm on one edge.
//const G4double SCINT_DIMENSION = 130.*mm;
const G4double SCINT_DIMENSION = 100000.*mm;

// (mm) Thickness of buffer in between scintillator surface and PMT. This may be
// some sort of container that holds the scintillator cube inside, such as a
// glass container. This feature is not used at this moment so is set to 0.
const G4double SCINT_BUFFER_THICKNESS = 0.*mm;

//
// Setup scintillator.
//
void MTCG4DetectorConstruction::SetupScintillator()
{
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
}

//
// Material properties for scintillator.
//
void MTCG4DetectorConstruction::SetScintMaterialProperties()
{
	const int N_SPEC_POINTS_SCINT = 14;
	// Set different photon wavelengths that will be referenced in calculating
	// scintillator properties, e.g. decay time constant.
	const G4double WAVELENGTHS_SCINT[N_SPEC_POINTS_SCINT] =
	{
		800*nm,
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
		400*nm,
		200*nm
	};
	// Set order of photon energies for EJ254 in increasing order.
	G4double photonEnergies_Scint[N_SPEC_POINTS_SCINT];
	for (G4int i = 0; i < N_SPEC_POINTS_SCINT; ++i) {
		photonEnergies_Scint[i] = h_Planck*c_light/WAVELENGTHS_SCINT[i];
	}
	G4double scintFast_EJ254[N_SPEC_POINTS_SCINT] =
	{
		0.00, // 800*nm, arbitrary cutoff
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
		0.02, // 400*nm
		0.00 // 200*nm
	};
	G4double scintSlow_EJ254[N_SPEC_POINTS_SCINT] = // Identical. Fix later.
	{
		0.00, // 800*nm, arbitrary cutoff
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
		0.02, // 400*nm
		0.00 // 200*nm
	};

	// Refractive index for EJ254 plastic scintillator.
	// Formula used for refractive index n where n^2 = (nI)^2 + A/(lambda^2 -
	// lambda_0^2).  nI = 1.53319, A = 20,690nm^2, lambda_0 = 196.6nm.  nI was
	// fitted such that lambda = 425nm => n = 1.58 as is reported in the EJ254
	// data sheet. Other constants were extracted as is from the following
	// reference.
	// Reference: Journal of Colloid and Interface Science, Volume 118, Issue 2,
	// August 1987, Pages 314–325, Measurement of the complex refractive index
	// of polyvinyltoluene in the UV, visible, and near IR: Application to the
	// size determination of PVT latices. T. Depireux, F. Dumont, A. Watillon.
	const int N_SPEC_POINTS_RINDEX = 16;
	const G4double WAVELENGTHS_RINDEX[N_SPEC_POINTS_RINDEX] =
	{
		800*nm,
		700*nm,
		600*nm,
		500*nm,
		400*nm,
		300*nm,
		280*nm,
		260*nm,
		240*nm,
		220*nm,
		210*nm,
		208*nm,
		206*nm,
		204*nm,
		202*nm,
		200*nm
	};
	G4double photonEnergies_RIndex[N_SPEC_POINTS_RINDEX];
	for (int i = 0; i < N_SPEC_POINTS_RINDEX; ++i) {
		photonEnergies_RIndex[i] = h_Planck*c_light/WAVELENGTHS_RINDEX[i];
	}
	const G4double nI = 1.53319, A = 20690*nm*nm, lambda_0 = 196.6*nm;
	G4double rIndices[N_SPEC_POINTS_RINDEX];
	for (int i = 0; i < N_SPEC_POINTS_RINDEX; ++i) {
		rIndices[i] =
			sqrt(nI*nI + A/(WAVELENGTHS_RINDEX[i]*WAVELENGTHS_RINDEX[i] -
						lambda_0*lambda_0));
	}

	// Absorption lengths for PVT based scintillator NE-110 below obtained from
	// "Measurement of the reflectivities and absorption lengths at different
	// wavelengths of plastic scintillator and acrylglass: Calculations of the
	// light attenuation in large area detectors", G. Kettenring, Nuclear
	// Instruments and Methods, volume 131, issue 3, 28 Dec. 1975, p. 451~456.
	const int N_SPEC_POINTS_ABS = 8;
	const G4double WAVELENGTHS_ABS_LENGTH[N_SPEC_POINTS_ABS] =
	{
		800*nm,
		633*nm,
		515*nm,
		502*nm,
		488*nm,
		473*nm,
		458*nm,
		200*nm
	};
	G4double photonEnergies_AbsLength[N_SPEC_POINTS_ABS];
	for (G4int i = 0; i < N_SPEC_POINTS_ABS; ++i) {
		photonEnergies_AbsLength[i] =
			h_Planck*c_light/WAVELENGTHS_ABS_LENGTH[i];
	}
	G4double absLengths_EJ254[N_SPEC_POINTS_ABS] = {
		418*cm, // 800*nm const extrapolation from 633*nm.
		418*cm, // 633*nm,
		311*cm, // 515*nm,
		291*cm, // 502*nm,
		264*cm, // 488*nm,
		236*cm, // 473*nm,
		204*cm, // 458*nm
		204*cm // 200*nm const extrapolation from 458*nm.
	};
	G4double reemission_EJ254[N_SPEC_POINTS_SCINT] = { // Same as fast scint.
		0.00, // 800*nm, arbitrary cutoff
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
		0.02, // 400*nm
		0.00 // 200*nm
	};

	G4MaterialPropertiesTable* EJ254_MPT = new G4MaterialPropertiesTable();
	EJ254_MPT->AddProperty("FASTCOMPONENT", photonEnergies_Scint,
			scintFast_EJ254, N_SPEC_POINTS_SCINT);
	EJ254_MPT->AddProperty("SLOWCOMPONENT", photonEnergies_Scint,
			scintSlow_EJ254, N_SPEC_POINTS_SCINT);
	EJ254_MPT->AddProperty("RINDEX", photonEnergies_RIndex, rIndices,
			N_SPEC_POINTS_RINDEX);
	//EJ254_MPT->AddProperty("ABSLENGTH", photonEnergies_AbsLength,
	//		absLengths_EJ254, N_SPEC_POINTS_ABS); // Now using WLSABSLENGTH.
	EJ254_MPT->AddProperty("WLSABSLENGTH", photonEnergies_AbsLength,
			absLengths_EJ254, N_SPEC_POINTS_ABS);
	EJ254_MPT->AddProperty("WLSCOMPONENT", photonEnergies_Scint,
			reemission_EJ254, N_SPEC_POINTS_SCINT);
	EJ254_MPT->AddConstProperty("WLSTIMECONSTANT", 0.0*ns); // No WLS delay.
	// The following scintillation yield numbers are taken from the
	// EJ254 data sheet for the case of Boron
	// doping only. We will have to use these numbers for now until we can find
	// those of the case for Lithium.
	static G4double scintillationYield;
	if (fDopingFraction < 2.5*perCent) {
		scintillationYield =
			( (8600. - 9200.)/(2.5*perCent - 1.*perCent) *
			 (fDopingFraction - 2.5*perCent) + 8600.) / MeV;
	}
	else if (fDopingFraction >= 2.5*perCent) {
		scintillationYield =
			( (7500. - 8600.)/(5.*perCent - 2.5*perCent) *
				(fDopingFraction - 2.5*perCent) + 8600.) / MeV;
	}
	EJ254_MPT->AddConstProperty("SCINTILLATIONYIELD", scintillationYield);
	EJ254_MPT->AddConstProperty("RESOLUTIONSCALE", 1.0); // Fix later.
	EJ254_MPT->AddConstProperty("FASTTIMECONSTANT", 2.2*ns); // Fix later.
	EJ254_MPT->AddConstProperty("SLOWTIMECONSTANT", 2.2*ns);
	// Yield ratio is not needed for now when there is only one component.
	// Yield ratio is the relative strength of the fast component as a fraction
	// of total scintillation yield.
	EJ254_MPT->AddConstProperty("YIELDRATIO", 1.); // Fix later.
	// Set Birks constant for scintillation photon quenching.
	// This number was derived from simulation of the 7Li/alpha daughters
	// produced from neutron capture on 10B. The EJ254 data sheet explains that
	// the photon yield from 7Li/alpha is equivalent to 76keV visible photon
	// deposition of an e-. The data sheet also mentions that 1% natural Boron
	// doped EJ254 gives 9,200 photons/MeV of e-, so these two numbers where
	// used for the derivation of Birks constant.
	G4double birksConst = 0.110*mm/MeV;
	
	fEJ254_OnePercentNaturalBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentNaturalBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentNaturalBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_FivePercentNaturalBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentNaturalBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_OnePercentEnrichedBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentEnrichedBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentEnrichedBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_FivePercentEnrichedBoronDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentEnrichedBoronDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_OnePercentNaturalLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentNaturalLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentNaturalLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_FivePercentNaturalLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentNaturalLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_OnePercentEnrichedLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_OnePercentEnrichedLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_TwoAndHalfPercentEnrichedLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
	fEJ254_FivePercentEnrichedLithiumDoped->
		SetMaterialPropertiesTable(EJ254_MPT);
	fEJ254_FivePercentEnrichedLithiumDoped->GetIonisation()->
		SetBirksConstant(birksConst);
} // SetScintMaterialProperties()

void MTCG4DetectorConstruction::SetScintMaterial()
{
	// Construct string to be used to identify scintillator properties.
	G4String materialChoice; 
	G4String
		neutronCaptureMaterialString,
		dopingFractionString,
		enrichmentString;
	if (fDopingFraction == 0.*perCent) {
		// Set scintillator material name.
		materialChoice = "EJ254_ZeroPercentDoped";
	}
	else {
		// Which dopant to use?
		if (fDopant == boron) {
			neutronCaptureMaterialString = "Boron";
		}
		else if (fDopant == lithium) {
			neutronCaptureMaterialString = "Lithium";
		}
		else {
			G4Exception("MTCG4DetectorConstruction::SetScintMaterial()", "",
					FatalErrorInArgument,
					"Scintillator material was not correctly specified.");
		}
		// What doping percentage to use?
		if (fDopingFraction == 1.*perCent) {
			dopingFractionString = "One";
		}
		else if (fDopingFraction == 2.5*perCent) {
			dopingFractionString = "TwoAndHalf";
		}
		else if (fDopingFraction == 5.*perCent) {
			dopingFractionString = "Five";
		}
		else {
			G4Exception("MTCG4DetectorConstruction::SetScintMaterial()", "",
					FatalErrorInArgument,
					"Scintillator material was not correctly specified.");
		}
		// What doping enrichment to use?
		if (fDopantIsEnriched == true) {
			enrichmentString = "Enriched";
		}
		else if (fDopantIsEnriched == false) {
			enrichmentString = "Natural";
		}
		else {
			G4Exception("MTCG4DetectorConstruction::SetScintMaterial()", "",
					FatalErrorInArgument,
					"Scintillator material was not correctly specified.");
		}
		// Set scintillator material name.
		materialChoice =
			"EJ254_"
			+ dopingFractionString
			+ "Percent"
			+ enrichmentString
			+ neutronCaptureMaterialString
			+ "Doped";
	}

	// Choose scintillator material to be used with given properties.
	G4cout << materialChoice << G4endl;

	// Set material.
	fScintMaterial =
		G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
	if (fScintLogical) {
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
// A linear fit was done yielding the function y = a + b*x; where a = 1.01981, b
// = 1.24435, x = doping fraction 0.001, 0.0025, 0.005, y = density in g/cc.
// Density of scintillator with no doping is found to be 1.020 g/cm3 when we
// extrapolate this fit to 0% doping fraction.
G4double MTCG4DetectorConstruction::GetEJ254Density(
		G4double dopingFraction) const
{
	G4double ej254Density = 0;
	if (dopingFraction >= 0.*perCent && dopingFraction < 1.*perCent) {
		ej254Density =
			( (1.021 - 1.020)/(1.*perCent - 0.*perCent) *
				(dopingFraction	- 0.*perCent) + 1.020) * g/cm3;
	}
	else if (dopingFraction >= 1.*perCent && dopingFraction < 2.5*perCent) {
		ej254Density =
			( (1.023 - 1.021)/(2.5*perCent - 1.*perCent) *
				(dopingFraction	- 1.*perCent) + 1.021) * g/cm3;
	}
 	else if (dopingFraction >= 2.5*perCent && dopingFraction <= 5.*perCent) {
		ej254Density =
			( (1.026 - 1.023)/(5.*perCent - 2.5*perCent) *
				(dopingFraction	- 2.5*perCent) + 1.023) * g/cm3;
	}
	else {
		G4Exception("MTCG4DetectorConstruction::GetEJ254Density",
				"",
				FatalErrorInArgument,
				"Doping fraction is outside of allowed range.");
	}
	//std::cout << "dopingFraction: " << dopingFraction << std::endl;
	//std::cout << "ej254Density(g/cm3): " << ej254Density*cm3/g << std::endl;
 	return ej254Density;
}

// Find Hydrogen/Carbon mass fraction of EJ-254 scintillator.
// These numbers are derived from the EJ-254 data sheet.
// Although the data sheet is only presented for Boron doping, we assume the
// same values hold for Lithium doping too.
// The Hydrogen count density was fitted to a 2nd degree polynomial: y =
// a+b*x+c*x*x; where a = 5.15167e+22, b = 9e+22, c = -6.66667e+24, x = doping
// fraction of 0.001, 0.0025, 0.005. y is the density in number/cm3. This gave a
// hydrogen count density at 0% doping of 5.15e22/cm3.
// The Carbon count density was fitted to a linear function: y = a + b*x; where
// a = 4.66388e+22, b = -4.4898e+23, x = doping fraction of 0.001, 0.0025,
// 0.005. y is the density in number/cm3. This gave a carbon count density at 0%
// doping of 4.66e+22/cm3;
// First find the count density of each element and then return the mass
// fraction of each element with respect to the whole while taking into account
// the doping mass fraction.
G4int MTCG4DetectorConstruction::GetHydrogenCarbonMassFractions(
		G4double dopingFraction,
	   	G4double &hydrogenMassFraction,
		G4double &carbonMassFraction) const
{
	const double hydrogenAtomicMass = 1.00794*g/mole;
	const double carbonAtomicMass = 12.0107*g/mole;
	double hydrogenCountDensity = 0; // Per cm^3.
	double carbonCountDensity = 0; // Per cm^3.
	if (dopingFraction >= 0.*perCent && dopingFraction < 1.*perCent) {
		hydrogenCountDensity =
			( (5.16e22 - 5.15e22)/(1.*perCent - 0.*perCent) *
			  (dopingFraction - 0.*perCent) + 5.15e22) /cm3;
		carbonCountDensity =
			( (4.62e22 - 4.66e+22)/(1.*perCent - 0.*perCent) *
			  (dopingFraction - 0.*perCent) + 4.66e+22) /cm3;
	}
	else if (dopingFraction >= 1.*perCent && dopingFraction < 2.5*perCent) {
		hydrogenCountDensity =
			( (5.17e22 - 5.16e22)/(2.5*perCent - 1.*perCent) *
			  (dopingFraction - 1.*perCent) + 5.16e22) /cm3;
		carbonCountDensity =
			( (4.55e22 - 4.62e22)/(2.5*perCent - 1.*perCent) *
			  (dopingFraction - 1.*perCent) + 4.62e22) /cm3;
	}
	else if (dopingFraction >= 2.5*perCent && dopingFraction <= 5.*perCent) {
		hydrogenCountDensity =
			( (5.18e22 - 5.17e22)/(5*perCent - 2.5*perCent) *
			  (dopingFraction - 2.5*perCent) + 5.17e22) /cm3;
		carbonCountDensity =
			( (4.44e22 - 4.55e22)/(5*perCent - 2.5*perCent) *
			  (dopingFraction - 2.5*perCent) + 4.55e22) /cm3;
	}
	else {
		G4Exception("MTCG4DetectorConstruction::GetHydrogenCarbonMassFractions",
				"",
				FatalErrorInArgument,
				"Doping fraction is outside of allowed range.");
	}
	double hydrogenMassDensity =
		hydrogenCountDensity/Avogadro*hydrogenAtomicMass;
	double carbonMassDensity =
	   	carbonCountDensity/Avogadro*carbonAtomicMass;
	hydrogenMassFraction =
		hydrogenMassDensity/
		( (hydrogenMassDensity + carbonMassDensity)/(1.-dopingFraction) );
	carbonMassFraction =
		carbonMassDensity/
		( (hydrogenMassDensity + carbonMassDensity)/(1.-dopingFraction) );
	std::cout << "dopingFraction: " << dopingFraction << std::endl;
	std::cout << "hydrogenCountDensity(/cm3): " << hydrogenCountDensity*cm3
		<< std::endl;
	std::cout << "carbonCountDensity(/cm3): " << carbonCountDensity*cm3
		<< std::endl;
	return 0;
}
