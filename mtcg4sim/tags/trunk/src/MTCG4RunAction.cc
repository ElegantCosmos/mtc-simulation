//////////////////////////////////////////////////////////
// MTCG4RunAction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include "G4Timer.hh"
#include "G4Run.hh"

#include "MTCG4RunAction.hh"

MTCG4RunAction::MTCG4RunAction()
{
	timer = new G4Timer;
}

MTCG4RunAction::~MTCG4RunAction()
{
	delete timer;
}

void MTCG4RunAction::BeginOfRunAction(const G4Run* theRun)
{
	G4cout << "### Run " << theRun->GetRunID() << " start." << G4endl; 
	timer->Start();
}

void MTCG4RunAction::EndOfRunAction(const G4Run* theRun)
{   
	// I don't know what this is but keep it here to be safe.
	timer->Stop();
	G4cout << "number of event = " << theRun->GetNumberOfEvent() 
		<< " " << *timer << G4endl;
}
