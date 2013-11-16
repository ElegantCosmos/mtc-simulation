#ifndef MTCG4_EVENT_ACTION_HH
#define MTCG4_EVENT_ACTION_HH 1

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"

class G4Event;

class MTCG4EventAction : public G4UserEventAction
{
	public:
		MTCG4EventAction();
		~MTCG4EventAction();

	public:
		void BeginOfEventAction(const G4Event*);
		void EndOfEventAction(const G4Event*);
		void SetNeutrinoKineticEnergyOfEventAction(G4double kineticEnergy);
		G4double GetNeutrinoKineticEnergyOfEventAction();
		void SetNeutrinoMomentumUnitVector(G4ThreeVector momentumUnitVector);
		G4ThreeVector GetNeutrinoMomentumUnitVector();
		void SetCosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino(G4double);
		G4double GetCosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino();
		void SetCosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino(G4double);
		G4double GetCosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino();

	private:
		G4double neutrinoKineticEnergy;
		G4ThreeVector neutrinoMomentumUnitVector;
		G4double cosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino;
	  G4double cosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino;	
};

#endif
