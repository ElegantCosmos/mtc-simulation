//
// MTCG4DetectorMessenger.cc Mich 111212
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

	commandForNeutronCaptureDopantMaterial = new
		G4UIcmdWithAString("/mtcg4/detector/setNeutronCaptureDopantMaterial", this);
	commandForNeutronCaptureDopantMaterial->
		SetGuidance("Select dopant material of the scintillator for neutron capture enhancement.");
	commandForNeutronCaptureDopantMaterial->
		SetGuidance("Choose \"lithium\" or \"boron\" or \"none\".");
	commandForNeutronCaptureDopantMaterial->SetParameterName("dopant", false);
	commandForNeutronCaptureDopantMaterial->SetCandidates("boron lithium none");
	commandForNeutronCaptureDopantMaterial->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForDopantEnrichment = new
		G4UIcmdWithABool("/mtcg4/detector/setDopantErichment", this);
	commandForDopantEnrichment->
		SetGuidance("Set enrichment fraction of dopant material in scintillator to be 100% or that of what is found in natural abundance.");
	commandForDopantEnrichment->SetGuidance("true: 100%, false: natural abundance.");
	commandForDopantEnrichment->SetParameterName("dopantEnrichment", false);
	commandForDopantEnrichment->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForPlacingPMTs = new G4UIcmdWithABool("/mtcg4/detector/setPMTPlacement", this);
	commandForPlacingPMTs->SetGuidance("Place PMTs on all faces of scintillator.");
	commandForPlacingPMTs->SetGuidance("true: place, false: do not place.");
	commandForPlacingPMTs->SetParameterName("PMTsArePlaced", false);
	commandForPlacingPMTs->AvailableForStates(G4State_PreInit, G4State_Idle);

	//commandForThermalScatteringWithChemicallyBoundAtoms = new
	//	G4UIcmdWithABool("/mtcg4/detector/setThermalScatteringWithChemicallyBoundAtoms", this);
	//commandForThermalScatteringWithChemicallyBoundAtoms->SetGuidance("Set
	//		thermal scattering of neutrons off of atoms in a medium to take into
	//		accound the chemical binding of the atoms to their molecules. The model
	//		applied is S(alpha, beta) and is relavent for energies < 4eV.");
	//commandForThermalScatteringWithChemicallyBoundAtoms->SetGuidance("1: apply,
	//		0: do not apply.");
	//commandForThermalScatteringWithChemicallyBoundAtoms->AvailableForStates(G4State_PreInit,
	//		G4State_Idle);
	
	commandForDopingFraction = new G4UIcmdWithADouble("/mtcg4/detector/setDopingFraction", this);
	commandForDopingFraction->SetGuidance("Set the fraction of dopant material for neutron capture in the scintillator.");
	commandForDopingFraction->SetGuidance("Available options are those given by Eljen Technology: 0.01, 0.025, 0.05.");
	commandForDopingFraction->SetParameterName("dopingFraction", false);
	commandForDopingFraction->SetRange("dopingFraction == 0. || dopingFraction == .01 || dopingFraction	== .025 || dopingFraction == .05");
	commandForDopingFraction->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDetectorVisualizationDir = new
		G4UIdirectory("/mtcg4/detector/visualization/");
	fDetectorVisualizationDir->SetGuidance("Detector visualization control.");

	commandForScintillatorVisualization = new
		G4UIcmdWithAnInteger("/mtcg4/detector/visualization/setScintillatorVisualization", this);
	commandForScintillatorVisualization->
		SetGuidance("Set visualization attributes for detector scintillator.");
	commandForScintillatorVisualization->
		SetGuidance("0: no visualization, 1: uniform gray, 2: color by part.");
	commandForScintillatorVisualization->SetParameterName("scintillatorIsVisible", false);
	commandForScintillatorVisualization->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForPMTVisualization = new
		G4UIcmdWithAnInteger("/mtcg4/detector/visualization/setPMTModuleVisualization", this);
	commandForPMTVisualization->
		SetGuidance("Set visualization attributes for PMT modules.");
	commandForPMTVisualization->
		SetGuidance("0: no visualization, 1: uniform gray, 2: color by part.");
	commandForPMTVisualization->SetParameterName("PMTModulesAreVisible", false);
	commandForPMTVisualization->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForUpdatingDetectorGeometry = new
		G4UIcmdWithoutParameter("/mtcg4/detector/update", this);
	commandForUpdatingDetectorGeometry->
		SetGuidance("Update detector geometry.");
	commandForUpdatingDetectorGeometry->
		SetGuidance("This command MUST be applied before \"beamOn\" ");
	commandForUpdatingDetectorGeometry->
		SetGuidance("if you changed geometrical value(s).");
	commandForUpdatingDetectorGeometry->AvailableForStates(G4State_Idle);
}

MTCG4DetectorMessenger::~MTCG4DetectorMessenger()
{
	delete commandForNeutronCaptureDopantMaterial;
	delete commandForDopantEnrichment;
	delete commandForPlacingPMTs;
	//delete commandForThermalScatteringWithChemicallyBoundAtoms;
	delete commandForDopingFraction;
	delete commandForScintillatorVisualization;
	delete commandForPMTVisualization;
	delete commandForUpdatingDetectorGeometry;
	delete fMtcG4Dir;
}

void MTCG4DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == commandForNeutronCaptureDopantMaterial){
		fDetector -> SetNeutronCaptureDopantMaterial(newValue);
		fDetector -> SetScintMaterial();
	}

	else if(command == commandForDopantEnrichment){
		fDetector ->
			SetDopantEnrichment(commandForDopantEnrichment ->
					GetNewBoolValue(newValue));
		fDetector -> SetScintMaterial();
	}

	else if(command == commandForDopingFraction){
		fDetector -> SetDopingFraction(commandForDopingFraction ->
				GetNewDoubleValue(newValue));
		fDetector -> SetScintMaterial();
	}

 	else if(command == commandForPlacingPMTs)
		fDetector -> SetPMTPlacement(commandForPlacingPMTs ->
				GetNewBoolValue(newValue));

	//else if(command == commandForThermalScatteringWithChemicallyBoundAtoms)
	//	fDetector ->
	//	SetThermalScatteringWithChemicallyBoundAtoms(newValue);

	else if(command == commandForScintillatorVisualization)
		fDetector ->
			SetScintVisualization(commandForScintillatorVisualization ->
					GetNewIntValue(newValue));

	else if(command == commandForPMTVisualization)
		fDetector -> SetPMTModuleVisualization(commandForPMTVisualization
				-> GetNewIntValue(newValue));

	else if(command == commandForUpdatingDetectorGeometry)
		fDetector->UpdateGeometry();
}

G4String MTCG4DetectorMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == commandForNeutronCaptureDopantMaterial)
		answer = fDetector -> GetNeutronCaptureDopantMaterial();
	else if(command == commandForDopantEnrichment)
		answer =
			commandForDopantEnrichment -> ConvertToString(fDetector ->
					GetDopantEnrichment());
	else if(command == commandForDopingFraction)
		answer =
			commandForDopingFraction -> ConvertToString(fDetector ->
					GetDopingFraction());
	else if(command == commandForPlacingPMTs)
		answer =
			commandForPlacingPMTs -> ConvertToString(fDetector ->
					GetPMTPlacement());
	else if(command == commandForScintillatorVisualization)
		answer =
			commandForScintillatorVisualization ->
			ConvertToString(fDetector -> GetScintVisualization());
	else if(command == commandForPMTVisualization)
		answer =
			commandForPMTVisualization -> ConvertToString(fDetector ->
					GetPMTModuleVisualization());
	return answer;
}
