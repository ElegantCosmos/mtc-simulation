//
// MTCG4PhysicsMessenger.hh by Mich 111221
//
#ifndef MTCG4PhysicsMessenger_h
#define MTCG4PhysicsMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4PhysicsList;
class G4UIdirectory;
class G4UIcmdWithABool;
class	G4UIcmdWithAnInteger;

class MTCG4PhysicsMessenger: public G4UImessenger
{
	public:
		MTCG4PhysicsMessenger(MTCG4PhysicsList*);
		~MTCG4PhysicsMessenger();
	
	public:
		void SetNewValue(G4UIcommand*, G4String);
		G4String GetCurrentValue(G4UIcommand*);

	private:
		MTCG4PhysicsList* physicsList;
		G4UIdirectory*			physicsDirectory;
		G4UIcmdWithABool*		commandForProducingCerenkov;
		G4UIcmdWithABool*		commandForProducingScintillation;
		G4UIcmdWithABool*		commandForNeutronHPThermalScattering;
		G4UIcmdWithAnInteger*	commandForMaxNumPhotonsPerStep;
		G4UIcmdWithAnInteger*	commandForVerbosity;
};

#endif
