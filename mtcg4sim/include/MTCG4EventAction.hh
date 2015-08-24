#ifndef MTCG4_EVENT_ACTION_HH
#define MTCG4_EVENT_ACTION_HH 1

#include "globals.hh"

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"

#include "MTCG4HitPmtCollection.hh"

class G4Event;
class PESdsIO;
class StepRootIO;

class MTCG4EventAction : public G4UserEventAction
{
public:
	MTCG4EventAction();
	~MTCG4EventAction();

public:
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);
	inline void SetNeutrinoKineticEnergyOfEventAction(G4double val)
	{ fNuKineticEnergy = val; }
	inline G4double GetNeutrinoKineticEnergyOfEventAction() const
	{ return fNuKineticEnergy; }
	inline void SetNeutrinoMomentumUnitVector(G4ThreeVector val)
	{ fNuMomentumUnitVector = val; }
	inline G4ThreeVector GetNeutrinoMomentumUnitVector() const
	{ return fNuMomentumUnitVector; }
	inline void SetCosOfNormalAngleOfNeutronInitMomentumWrtNeutrino(G4double
			val)
	{ fCosOfNormAngleOfNeutronInitMomentumWrtNeutrino = val; }
	inline G4double GetCosineOfNormalAngleOfNeutronInitialMomentumWrtNeutrino()
		const
	{ return fCosOfNormAngleOfNeutronInitMomentumWrtNeutrino; }
	inline void SetCosOfNormalAngleOfPositronInitMomentumWrtNeutrino(G4double
			val)
	{ fCosOfNormAngleOfPositronInitMomentumWrtNeutrino = val; }
	inline G4double GetCosOfNormalAngleOfPositronInitMomentumWrtNeutrino() const
	{ return fCosOfNormAngleOfPositronInitMomentumWrtNeutrino; }

public:
	static MTCG4HitPmtCollection *GetTheHitPmtCollection()
	{ return &fTheHitPmtCollection; }
	void DoStepOutToRoot();
	void PrintTrajectories(const G4Event*);
	void WritePESds(const G4Event*);

private:
	static MTCG4HitPmtCollection fTheHitPmtCollection;

	G4double		fNuKineticEnergy;
	G4ThreeVector	fNuMomentumUnitVector;
	G4double		fCosOfNormAngleOfNeutronInitMomentumWrtNeutrino;
	G4double		fCosOfNormAngleOfPositronInitMomentumWrtNeutrino;	

private:
	PESdsIO*	fPEOut;
	StepRootIO*	fStepOutToRoot;
};

#endif
