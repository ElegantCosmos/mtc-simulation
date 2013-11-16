//
// MTCG4PhysicsMessenger.cc by Mich 111221
//
#include "MTCG4PhysicsMessenger.hh"
#include "MTCG4PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"

MTCG4PhysicsMessenger::MTCG4PhysicsMessenger(MTCG4PhysicsList* Phys)
:physicsList(Phys)
{
	physicsDirectory = new G4UIdirectory("/MTCG4/physics/");
	physicsDirectory->SetGuidance("Physics options.");

	commandForProducingCerenkov = new
		G4UIcmdWithABool("/MTCG4/physics/produceCerenkov", this);
	commandForProducingCerenkov->SetGuidance("Produce Cerenkov light.");
	commandForProducingCerenkov->SetGuidance("true: produce, false: do not produce.");
	commandForProducingCerenkov->SetParameterName("cerenkovIsTurnedOn", false);
	commandForProducingCerenkov->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForProducingScintillation = new
		G4UIcmdWithABool("/MTCG4/physics/produceScintillation", this);
	commandForProducingScintillation->
		SetGuidance("Produce Scintillation light.");
	commandForProducingScintillation->SetGuidance("true: produce, false: do not produce.");
	commandForProducingScintillation->SetParameterName("scintillationIsTurnedOn", false);
	commandForProducingScintillation->AvailableForStates(G4State_PreInit,
			G4State_Idle);

	commandForMaxNumPhotonsPerStep = new
		G4UIcmdWithAnInteger("/MTCG4/physics/setMaxNumPhotonsPerStep", this);
	commandForMaxNumPhotonsPerStep->
		SetGuidance("Set the maximum number of photons produced by the simulation per particle step.");
	commandForMaxNumPhotonsPerStep->SetParameterName("maxNumPhotonsPerStep", false);
	commandForMaxNumPhotonsPerStep->
		SetRange("maxNumPhotonsPerStep >= 0 && maxNumPhotonsPerStep <= 1000");
	commandForMaxNumPhotonsPerStep->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForNeutronHPThermalScattering = new
		G4UIcmdWithABool("/MTCG4/physics/setNeutronHPThermalScattering", this);
	commandForNeutronHPThermalScattering->
		SetGuidance("Take into account the chemical bindings of atoms in a molecule for thermal neutron scattering in a solid medium.");
	commandForNeutronHPThermalScattering->SetGuidance("true: yes, false: no.");
	commandForNeutronHPThermalScattering->SetParameterName("NeutronHPThermalScatteringIsTurnedOn", false);
	commandForNeutronHPThermalScattering->AvailableForStates(G4State_PreInit, G4State_Idle);

	commandForVerbosity = new G4UIcmdWithAnInteger("/MTCG4/physics/setVerbosity", this);
	commandForVerbosity->SetGuidance("Set verbosity setting for physics list.");
	commandForVerbosity->AvailableForStates(G4State_PreInit, G4State_Idle);
}

MTCG4PhysicsMessenger::~MTCG4PhysicsMessenger()
{
	delete commandForProducingCerenkov;
	delete commandForProducingScintillation;
	delete commandForMaxNumPhotonsPerStep;
	delete commandForNeutronHPThermalScattering;
	delete commandForVerbosity;
}

void MTCG4PhysicsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == commandForProducingCerenkov){
		physicsList->SetCerenkovProduction(commandForProducingCerenkov->GetNewBoolValue(newValue));
	}
	else if(command == commandForProducingScintillation){
		physicsList->SetScintillationProduction(commandForProducingScintillation->GetNewBoolValue(newValue));
	}
	else if(command == commandForMaxNumPhotonsPerStep){
		physicsList->SetNbOfPhotonsCerenkov(commandForMaxNumPhotonsPerStep->GetNewIntValue(newValue));
	}
	else if(command == commandForNeutronHPThermalScattering){
		physicsList->SetNeutronHPThermalScattering(commandForNeutronHPThermalScattering->GetNewBoolValue(newValue));
	}
	else if(command == commandForVerbosity){
		physicsList->SetVerbose(commandForVerbosity->GetNewIntValue(newValue));
	}
}

G4String MTCG4PhysicsMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String answer;
	if(command == commandForProducingCerenkov){
		answer =
			commandForProducingCerenkov->ConvertToString(physicsList->GetCerenkovProduction());
	}
	else if(command == commandForProducingScintillation){
		answer =
			commandForProducingScintillation->ConvertToString(physicsList->GetScintillationProduction());
	}
	else if(command == commandForMaxNumPhotonsPerStep){
		answer = "Value is not yet set.";
	}
	else if(command == commandForNeutronHPThermalScattering){
		answer =
			commandForNeutronHPThermalScattering->ConvertToString(physicsList->GetNeutronHPThermalScattering());
	}
	else if(command == commandForVerbosity){
		answer = "Value is not yet set.";
	}
	return answer;
}
