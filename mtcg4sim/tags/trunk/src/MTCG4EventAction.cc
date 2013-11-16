//////////////////////////////////////////////////////////
// MTCG4EventAction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include "G4Event.hh"
#include "G4EventManager.hh"

#include "MTCG4EventAction.hh"

MTCG4EventAction::MTCG4EventAction()
{}

MTCG4EventAction::~MTCG4EventAction()
{}

// Method Called At Beginning Of Action //
void MTCG4EventAction::BeginOfEventAction(const G4Event*)
{
}

// Method Called At End Of Action //
void MTCG4EventAction::EndOfEventAction(const G4Event* theEvent)
{
	G4int eventNumber = theEvent->GetEventID();
	G4cout << "Event " << eventNumber << " done!\n\n";
}

void MTCG4EventAction::SetNeutrinoKineticEnergyOfEventAction(G4double kineticEnergy)
{
	neutrinoKineticEnergy = kineticEnergy;
}

G4double MTCG4EventAction::GetNeutrinoKineticEnergyOfEventAction()
{
	return neutrinoKineticEnergy;
}

void MTCG4EventAction::SetNeutrinoMomentumUnitVector(G4ThreeVector momentumUnitVector)
{
	neutrinoMomentumUnitVector = momentumUnitVector;
}

G4ThreeVector MTCG4EventAction::GetNeutrinoMomentumUnitVector()
{
	return neutrinoMomentumUnitVector;
}

void MTCG4EventAction::SetCosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino(G4double theta){
	cosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino = theta;
}

G4double MTCG4EventAction::GetCosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino(){
	return cosineOfNormalAngleOfNeutronInitialMomentumWRTNeutrino;
}

void MTCG4EventAction::SetCosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino(G4double theta){
	cosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino = theta;
}

G4double MTCG4EventAction::GetCosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino(){
	return cosineOfNormalAngleOfPositronInitialMomentumWRTNeutrino;
}
