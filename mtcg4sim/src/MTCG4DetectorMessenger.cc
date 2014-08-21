//
// MTCG4DetectorMessenger.cc Mich 2011/12/12
//

#include "MTCG4DetectorMessenger.hh"
#include "MTCG4DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"

MTCG4DetectorMessenger::MTCG4DetectorMessenger(MTCG4DetectorConstruction* det)
:fDetector(det)
{
	fMtcG4Dir = new G4UIdirectory("/mtcg4/");
	fMtcG4Dir->SetGuidance("UI commands for MTCG4 simulation.");

	fDetectorDir = new G4UIdirectory("/mtcg4/detector/");
	fDetectorDir->SetGuidance("Detector control.");

	fSetNeutronCaptureDopantMaterialCmd = new
		G4UIcmdWithAString("/mtcg4/detector/setNeutronCaptureDopantMaterial", this);
	fSetNeutronCaptureDopantMaterialCmd->
		SetGuidance("Select dopant material of the scintillator for neutron capture enhancement.");
	fSetNeutronCaptureDopantMaterialCmd->
		SetGuidance("Choose \"lithium\" or \"boron\" or \"none\".");
	fSetNeutronCaptureDopantMaterialCmd->SetParameterName("dopant", false);
	fSetNeutronCaptureDopantMaterialCmd->SetCandidates("boron lithium none");
	fSetNeutronCaptureDopantMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetDopantEnrichmentCmd = new
		G4UIcmdWithABool("/mtcg4/detector/setDopantErichment", this);
	fSetDopantEnrichmentCmd->
		SetGuidance("Set enrichment fraction of dopant material in scintillator to be 100% or that of what is found in natural abundance.");
	fSetDopantEnrichmentCmd->SetGuidance("true: 100%, false: natural abundance.");
	fSetDopantEnrichmentCmd->SetParameterName("dopantEnrichment", false);
	fSetDopantEnrichmentCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetPmtPlacementCmd = new G4UIcmdWithABool("/mtcg4/detector/setPmtPlacement", this);
	fSetPmtPlacementCmd->SetGuidance("Place PMTs on all faces of scintillator.");
	fSetPmtPlacementCmd->SetGuidance("true: place, false: do not place.");
	fSetPmtPlacementCmd->SetParameterName("PmtsArePlaced", false);
	fSetPmtPlacementCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	//fSetThermalScatteringWithChemicallyBoundAtomsCmd = new
	//	G4UIcmdWithABool("/mtcg4/detector/setThermalScatteringWithChemicallyBoundAtoms", this);
	//fSetThermalScatteringWithChemicallyBoundAtomsCmd->SetGuidance("Set
	//		thermal scattering of neutrons off of atoms in a medium to take into
	//		accound the chemical binding of the atoms to their molecules. The model
	//		applied is S(alpha, beta) and is relavent for energies < 4eV.");
	//fSetThermalScatteringWithChemicallyBoundAtomsCmd->SetGuidance("1: apply,
	//		0: do not apply.");
	//fSetThermalScatteringWithChemicallyBoundAtomsCmd->AvailableForStates(G4State_PreInit,
	//		G4State_Idle);
	
	fSetDopingFractionCmd = new G4UIcmdWithADouble("/mtcg4/detector/setDopingFraction", this);
	fSetDopingFractionCmd->SetGuidance("Set the fraction of dopant material for neutron capture in the scintillator.");
	fSetDopingFractionCmd->SetGuidance("Available options are those given by Eljen Technology: 0.01, 0.025, 0.05.");
	fSetDopingFractionCmd->SetParameterName("dopingFraction", false);
	fSetDopingFractionCmd->SetRange("dopingFraction == 0. || dopingFraction == .01 || dopingFraction	== .025 || dopingFraction == .05");
	fSetDopingFractionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDetectorVisDir = new
		G4UIdirectory("/mtcg4/detector/visualization/");
	fDetectorVisDir->SetGuidance("Detector visualization control.");

	fSetScintillatorVisCmd = new
		G4UIcmdWithAnInteger("/mtcg4/detector/visualization/setScintillatorVis", this);
	fSetScintillatorVisCmd->
		SetGuidance("Set visualization attributes for detector scintillator.");
	fSetScintillatorVisCmd->
		SetGuidance("0: no visualization, 1: uniform gray, 2: color by part.");
	fSetScintillatorVisCmd->SetParameterName("scintillatorIsVisible", false);
	fSetScintillatorVisCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fSetPmtVisCmd = new
		G4UIcmdWithAnInteger("/mtcg4/detector/visualization/setPmtModuleVis", this);
	fSetPmtVisCmd->
		SetGuidance("Set visualization attributes for PMT modules.");
	fSetPmtVisCmd->
		SetGuidance("0: no visualization, 1: uniform gray, 2: color by part.");
	fSetPmtVisCmd->SetParameterName("PmtModulesAreVisible", false);
	fSetPmtVisCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fUpdateDetectorGeometryCmd = new
		G4UIcmdWithoutParameter("/mtcg4/detector/update", this);
	fUpdateDetectorGeometryCmd->
		SetGuidance("Update detector geometry.");
	fUpdateDetectorGeometryCmd->
		SetGuidance("This command MUST be applied before \"beamOn\" ");
	fUpdateDetectorGeometryCmd->
		SetGuidance("if you changed geometrical value(s).");
	fUpdateDetectorGeometryCmd->AvailableForStates(G4State_Idle);
}

MTCG4DetectorMessenger::~MTCG4DetectorMessenger()
{
	delete fSetNeutronCaptureDopantMaterialCmd;
	delete fSetDopantEnrichmentCmd;
	delete fSetPmtPlacementCmd;
	//delete fSetThermalScatteringWithChemicallyBoundAtomsCmd;
	delete fSetDopingFractionCmd;
	delete fSetScintillatorVisCmd;
	delete fSetPmtVisCmd;
	delete fUpdateDetectorGeometryCmd;
	delete fMtcG4Dir;
}

void MTCG4DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue)
{
	if(cmd == fSetNeutronCaptureDopantMaterialCmd){
		fDetector->SetNeutronCaptureDopantMaterial(newValue);
		fDetector->SetScintMaterial();
	}

	else if(cmd == fSetDopantEnrichmentCmd){
		fDetector->
			SetDopantEnrichment(fSetDopantEnrichmentCmd->
					GetNewBoolValue(newValue));
		fDetector->SetScintMaterial();
	}

	else if(cmd == fSetDopingFractionCmd){
		fDetector->SetDopingFraction(fSetDopingFractionCmd->
				GetNewDoubleValue(newValue));
		fDetector->SetScintMaterial();
	}

 	else if(cmd == fSetPmtPlacementCmd)
		fDetector->SetPmtPlacement(fSetPmtPlacementCmd->
				GetNewBoolValue(newValue));

	//else if(cmd == fSetThermalScatteringWithChemicallyBoundAtomsCmd)
	//	fDetector->
	//	SetThermalScatteringWithChemicallyBoundAtoms(newValue);

	else if(cmd == fSetScintillatorVisCmd)
		fDetector->
			SetScintVis(fSetScintillatorVisCmd->GetNewIntValue(newValue));

	else if(cmd == fSetPmtVisCmd)
		fDetector->SetPmtModuleVis(fSetPmtVisCmd->GetNewIntValue(newValue));

	else if(cmd == fUpdateDetectorGeometryCmd)
		fDetector->UpdateGeometry();
}

G4String MTCG4DetectorMessenger::GetCurrentValue(G4UIcommand* cmd)
{
	G4String answer;
	if(cmd == fSetNeutronCaptureDopantMaterialCmd)
		answer = fDetector->GetNeutronCaptureDopantMaterial();
	else if(cmd == fSetDopantEnrichmentCmd)
		answer =
			fSetDopantEnrichmentCmd->ConvertToString(fDetector->
					GetDopantEnrichment());
	else if(cmd == fSetDopingFractionCmd)
		answer =
			fSetDopingFractionCmd->ConvertToString(fDetector->
					GetDopingFraction());
	else if(cmd == fSetPmtPlacementCmd)
		answer =
			fSetPmtPlacementCmd->ConvertToString(fDetector->
					GetPmtPlacement());
	else if(cmd == fSetScintillatorVisCmd)
		answer =
			fSetScintillatorVisCmd->
			ConvertToString(fDetector->GetScintVis());
	else if(cmd == fSetPmtVisCmd)
		answer =
			fSetPmtVisCmd->ConvertToString(fDetector->
					GetPmtModuleVis());
	return answer;
}
