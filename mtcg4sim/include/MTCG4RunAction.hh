#ifndef MTCG4_RUN_ACTION_HH
#define MTCG4_RUN_ACTION_HH 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Timer;
class G4Run;
class MTCG4RunMessenger;
class PESdsIO;
class StepTextIO;
class StepRootIO;

class MTCG4RunAction : public G4UserRunAction
{
	public:
		MTCG4RunAction();
		~MTCG4RunAction();

	public:
		void BeginOfRunAction(const G4Run*);
		void EndOfRunAction(const G4Run*);

		void CreatePESdsOutput(const G4Run* theRun);
		void CreateStepRootOutput();
		void CreateStepTextOutput();
		void ProcessPESds();
		void ProcessStepTextOut();
		void ProcessStepRootOut();

		inline void SetRunID(G4int runID)
		{ fRunID = runID; }
		G4int GetRunID() const
		{ return fRunID; }
		inline void SetOutputPathName(G4String path)
		{ fOutputPathName = path; }
		inline G4String GetOutputPathName() const
		{ return fOutputPathName; }
		inline void SetStepOutputFileName(G4String name)
		{ fStepOutputPathName = name; }
		inline G4String GetStepOutputFileName() const
		{ return fStepOutputPathName; }
		inline void SetPEOutputFileName(G4String name)
		{ fPEOutputFileName = name; }
		inline G4String GetPEOutputFileName() const
		{ return fPEOutputFileName; }

	private:
		G4int		fRunID;
		PESdsIO*	fPESdsOut;
		StepTextIO* fStepTextOut;
		StepRootIO*	fStepRootOut;
		G4String	fOutputPathName;
		G4String	fStepOutputPathName;
		G4String	fPEOutputFileName;

		MTCG4RunMessenger* fMessenger;

		G4Timer* fTimer;
};

#endif
