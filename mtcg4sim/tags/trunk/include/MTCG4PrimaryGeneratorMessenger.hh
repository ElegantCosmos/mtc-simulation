//
// MTCG4PrimaryGeneratorMessenger.hh by Mich 111222
//

#ifndef MTCG4PrimaryGeneratorMessenger_h
#define MTCG4PrimaryGeneratorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class MTCG4PrimaryGeneratorMessenger: public G4UImessenger
{
	public:
		MTCG4PrimaryGeneratorMessenger(MTCG4PrimaryGeneratorAction*);
		~MTCG4PrimaryGeneratorMessenger();

	public:
		void SetNewValue(G4UIcommand*, G4String);
		G4String GetCurrentValue(G4UIcommand*);

	private:
		MTCG4PrimaryGeneratorAction* primaryGeneratorAction;

		G4UIdirectory*			primaryGeneratorDirectory;
		G4UIcmdWithAString* commandForIncomingNeutrinoDirection;
		G4UIcmdWithAString* commandForInverseBetaDecayInteractionVertex;
		G4UIcmdWithABool*		commandForReadingInPrimaryParticleDataFile;
		G4UIcmdWithADoubleAndUnit* commandForInverseBetaDecayDaughterParticleKineticEnergy;
		G4UIcmdWithABool*		commandForShootingPrimaryPositron;
		G4UIcmdWithABool*		commandForShootingPrimaryNeutron;
};

#endif
