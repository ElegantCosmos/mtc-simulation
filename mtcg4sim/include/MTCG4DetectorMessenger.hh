//
// MTCG4DetectorMessenger.hh by Mich 111221
//
#ifndef MTCG4DetectorMessenger_h
#define MTCG4DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MTCG4DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;

class MTCG4DetectorMessenger: public G4UImessenger
{
	public:
		MTCG4DetectorMessenger(MTCG4DetectorConstruction*);
		~MTCG4DetectorMessenger();
	
	public:
		void SetNewValue(G4UIcommand*, G4String);
		G4String GetCurrentValue(G4UIcommand*);

	private:
		MTCG4DetectorConstruction* fDetector;

		G4UIdirectory* fMtcG4Dir;
		G4UIdirectory* fDetectorDir;
		G4UIdirectory* fDetectorVisualizationDir;

		G4UIcmdWithAString*		commandForNeutronCaptureDopantMaterial;
		G4UIcmdWithABool*		commandForDopantEnrichment;
		G4UIcmdWithABool*		commandForPlacingPMTs;
		//G4UIcmdWithABool*		commandForThermalScatteringWithChemicallyBoundAtoms;
		G4UIcmdWithADouble*		commandForDopingFraction;
		G4UIcmdWithAnInteger*	commandForScintillatorVisualization;
		G4UIcmdWithAnInteger*	commandForPMTVisualization;
		G4UIcmdWithoutParameter* commandForUpdatingDetectorGeometry;
};

#endif
