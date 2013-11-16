#ifndef MTCG4_RUN_ACTION_HH
#define MTCG4_RUN_ACTION_HH 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Timer;
class G4Run;

class MTCG4RunAction : public G4UserRunAction
{
	public:
		MTCG4RunAction();
		~MTCG4RunAction();

	public:
		void BeginOfRunAction(const G4Run*);
		void EndOfRunAction(const G4Run*);

	private:
		G4Timer* timer;
};

#endif
