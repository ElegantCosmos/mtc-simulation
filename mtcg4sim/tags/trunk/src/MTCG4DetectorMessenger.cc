//
// MTCG4DetectorMessenger.cc Mich 111212
//

#include "MTCG4DetectorMessenger.hh"
#include "MTCG4DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"

MTCG4DetectorMessenger::MTCG4DetectorMessenger(MTCG4DetectorConstruction*	Det)
:detectorConstruction(Det)
{
	MTCG4Directory = new G4UIdirectory("/MTCG4/");
	MTCG4Directory->SetGuidance("UI commands for MTCG4 simulation.");

	detectorDirectory = new G4UIdirectory("/MTCG4/detector/");
	detectorDirectory->SetGuidance("Detector control.");

	commandForNeutronCaptureDopantMaterial = new
		G4UIcmdWithAString("/MTCG4/detector/setNeutronCaptureDopantMaterial", this);
	commandForNeutronCaptureDopantMaterial->
		SetGuidance("Select dopant material of the scintillator for neutron capture enhancement.");
	commandForNeutronCaptureDopantMaterial->
		SetGuidance("Choose \"lithium\" or \"boron\" or \"none\".");
	commandForNeutronCaptureDopantMaterial->SetParameterName("dopant", false);
	commandForNeutronCaptureDopantMaterial->SetCandidates("boron lithium none");
	commandForNeutronCaptureDopantMaterial->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForDopantEnrichment = new
		G4UIcmdWithABool("/MTCG4/detector/setDopantErichment", this);
	commandForDopantEnrichment->
		SetGuidance("Set enrichment fraction of dopant material in scintillator to be 100% or that of what is found in natural abundance.");
	commandForDopantEnrichment->SetGuidance("true: 100%, false: natural abundance.");
	commandForDopantEnrichment->SetParameterName("dopantEnrichment", false);
	commandForDopantEnrichment->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForPlacingMCPs = new G4UIcmdWithABool("/MTCG4/detector/setMCPPlacement", this);
	commandForPlacingMCPs->SetGuidance("Place MCPs on all faces of scintillator.");
	commandForPlacingMCPs->SetGuidance("true: place, false: do not place.");
	commandForPlacingMCPs->SetParameterName("MCPsArePlaced", false);
	commandForPlacingMCPs->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandToUpdateDetector = new G4UIcmdWithoutParameter("/MTCG4/detector/update",this);
	commandToUpdateDetector->SetGuidance("Update detector geometry.");
	commandToUpdateDetector->SetGuidance("This command MUST be applied before \"beamOn\" ");
	commandToUpdateDetector->SetGuidance("if you changed geometrical value(s) of the detector.");
	commandToUpdateDetector->AvailableForStates(G4State_Idle);

	//commandForThermalScatteringWithChemicallyBoundAtoms = new
	//	G4UIcmdWithABool("/MTCG4/detector/setThermalScatteringWithChemicallyBoundAtoms", this);
	//commandForThermalScatteringWithChemicallyBoundAtoms->SetGuidance("Set
	//		thermal scattering of neutrons off of atoms in a medium to take into
	//		accound the chemical binding of the atoms to their molecules. The model
	//		applied is S(alpha, beta) and is relavent for energies < 4eV.");
	//commandForThermalScatteringWithChemicallyBoundAtoms->SetGuidance("1: apply,
	//		0: do not apply.");
	//commandForThermalScatteringWithChemicallyBoundAtoms->AvailableForStates(G4State_PreInit,
	//		G4State_Idle);
	
	commandForDopingFraction = new G4UIcmdWithADouble("/MTCG4/detector/setDopingFraction", this);
	commandForDopingFraction->SetGuidance("Set the fraction of dopant material for neutron capture in the scintillator.");
	commandForDopingFraction->SetGuidance("Available options are those given by Eljen Technology: 0.01, 0.025, 0.05.");
	commandForDopingFraction->SetParameterName("dopingFraction", false);
	commandForDopingFraction->SetRange("dopingFraction == 0. || dopingFraction == .01 || dopingFraction	== .025 || dopingFraction == .05");
	commandForDopingFraction->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForOpticalSurfaceProperties = new
		G4UIcmdWithABool("/MTCG4/detector/setOpticalSurfaceProperties", this);
	commandForOpticalSurfaceProperties->
		SetGuidance("Apply optical surface properties to photo-sensitive MCP pixels for photon detection.");
	commandForOpticalSurfaceProperties->
		SetGuidance("This allows for certain features of pixels such as quantum efficiency as a function of photon wavelength to be used.");
	commandForOpticalSurfaceProperties->
		SetGuidance("If this feature is turned off, the quantum efficiency will be 100% by default.");
	commandForOpticalSurfaceProperties->
		SetGuidance("true: turn on, false: turn off");
	commandForOpticalSurfaceProperties->SetParameterName("opticalSurfacePropertiesAreSet",
			false);
	commandForOpticalSurfaceProperties->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	detectorVisualisationDirectory = new
		G4UIdirectory("/MTCG4/detector/visualisation/");
	detectorVisualisationDirectory->SetGuidance("Detector visualisation control.");

	commandForScintillatorVisualisation = new
		G4UIcmdWithABool("/MTCG4/detector/visualisation/setScintillatorVisualisation", this);
	commandForScintillatorVisualisation->
		SetGuidance("Set visualisation attributes for detector scintillator.");
	commandForScintillatorVisualisation->
		SetGuidance("true: visible, false: invisible.");
	commandForScintillatorVisualisation->SetParameterName("scintillatorIsVisible", false);
	commandForScintillatorVisualisation->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForMCPModuleVisualisation = new
		G4UIcmdWithABool("/MTCG4/detector/visualisation/setMCPModuleVisualisation", this);
	commandForMCPModuleVisualisation->
		SetGuidance("Set visualisation attributes for MCP modules.");
	commandForMCPModuleVisualisation->SetGuidance("true: visible, false: invisible.");
	commandForMCPModuleVisualisation->SetParameterName("MCPModulesAreVisible", false);
	commandForMCPModuleVisualisation->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForMCPPixelVisualisation = new
		G4UIcmdWithABool("/MTCG4/detector/visualisation/setMCPPixelVisualisation", this);
	commandForMCPPixelVisualisation->
		SetGuidance("Set visualisation attributes for MCP pixels.");
	commandForMCPPixelVisualisation->SetGuidance("true: visible, false: invisible.");
	commandForMCPPixelVisualisation->SetParameterName("MCPPixelsAreVisible", false);
	commandForMCPPixelVisualisation->AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4DetectorMessenger::~MTCG4DetectorMessenger()
{
	delete commandForNeutronCaptureDopantMaterial;
	delete commandForDopantEnrichment;
	delete commandForPlacingMCPs;
	delete commandToUpdateDetector;
	//delete commandForThermalScatteringWithChemicallyBoundAtoms;
	delete commandForDopingFraction;
	delete commandForOpticalSurfaceProperties;
	delete commandForScintillatorVisualisation;
	delete commandForMCPModuleVisualisation;
	delete commandForMCPPixelVisualisation;
}

void MTCG4DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == commandForNeutronCaptureDopantMaterial){
		detectorConstruction->SetNeutronCaptureDopantMaterial(newValue);
	}
	else if(command == commandForDopantEnrichment){
		detectorConstruction->SetDopantEnrichment(commandForDopantEnrichment->GetNewBoolValue(newValue));
	}
	else if(command == commandForPlacingMCPs){
		detectorConstruction->SetMCPPlacement(commandForPlacingMCPs->GetNewBoolValue(newValue));
	}
	else if(command == commandToUpdateDetector){
		detectorConstruction->UpdateGeometry();
	}
	//else if(command == commandForThermalScatteringWithChemicallyBoundAtoms){
	//	detectorConstruction->SetThermalScatteringWithChemicallyBoundAtoms(newValue);
	//}
	else if(command == commandForOpticalSurfaceProperties){
		detectorConstruction->SetOpticalSurfaceProperties(commandForOpticalSurfaceProperties->GetNewBoolValue(newValue));
	}
	else if(command == commandForDopingFraction){
		detectorConstruction->SetDopingFraction(commandForDopingFraction->GetNewDoubleValue(newValue));
	}
	else if(command == commandForScintillatorVisualisation){
		detectorConstruction->SetScintillatorVisualisation(commandForScintillatorVisualisation->GetNewBoolValue(newValue));
	}
	else if(command == commandForMCPModuleVisualisation){
		detectorConstruction->SetMCPModuleVisualisation(commandForMCPModuleVisualisation->GetNewBoolValue(newValue));
	}
	else if(command == commandForMCPPixelVisualisation){
		detectorConstruction->SetMCPPixelVisualisation(commandForMCPPixelVisualisation->GetNewBoolValue(newValue));
	}
}

G4String MTCG4DetectorMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == commandForNeutronCaptureDopantMaterial){
		answer = detectorConstruction->GetNeutronCaptureDopantMaterial();
	}
	else if(command == commandForDopantEnrichment){
		answer =
			commandForDopantEnrichment->ConvertToString(detectorConstruction->GetDopantEnrichment());
	}
	else if(command == commandForDopingFraction){
		answer =
			commandForDopingFraction->ConvertToString(detectorConstruction->GetDopingFraction());
	}
	else if(command == commandForPlacingMCPs){
		answer =
			commandForPlacingMCPs->ConvertToString(detectorConstruction->GetMCPPlacement());
	}
	else if(command == commandForOpticalSurfaceProperties){
		answer =
			commandForOpticalSurfaceProperties->ConvertToString(detectorConstruction->GetOpticalSurfaceProperties());
	}
	else if(command == commandForScintillatorVisualisation){
		answer =
			commandForScintillatorVisualisation->ConvertToString(detectorConstruction->GetScintillatorVisualisation());
	}
	else if(command == commandForMCPModuleVisualisation){
		answer =
			commandForMCPModuleVisualisation->ConvertToString(detectorConstruction->GetMCPModuleVisualisation());
	}
	else if(command == commandForMCPPixelVisualisation){
		answer =
			commandForMCPPixelVisualisation->ConvertToString(detectorConstruction->GetMCPPixelVisualisation());
	}
	return answer;
}
