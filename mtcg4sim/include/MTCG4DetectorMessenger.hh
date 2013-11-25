//
// MTCG4DetectorMessenger.hh by Mich 111221
//
#ifndef MTCG4_DETECTOR_MESSENGER_HH
#define MTCG4_DETECTOR_MESSENGER_HH

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
	void SetNewValue(G4UIcommand* cmd, G4String newValue);
	G4String GetCurrentValue(G4UIcommand* cmd);

private:
	MTCG4DetectorConstruction* fDetector;

	G4UIdirectory*			fMtcG4Dir;
	G4UIdirectory*			fDetectorDir;
	G4UIdirectory*			fDetectorVisDir;

	G4UIcmdWithAString*		fSetNeutronCaptureDopantMaterialCmd;
	G4UIcmdWithABool*		fSetDopantEnrichmentCmd;
	G4UIcmdWithABool*		fSetPmtPlacementCmd;
	//G4UIcmdWithABool*		fSetThermalScatteringWithChemicallyBoundAtomsCmd;
	G4UIcmdWithADouble*		fSetDopingFractionCmd;
	G4UIcmdWithAnInteger*	fSetScintillatorVisCmd;
	G4UIcmdWithAnInteger*	fSetPmtVisCmd;
	G4UIcmdWithoutParameter* fUpdateDetectorGeometryCmd;
};

#endif
