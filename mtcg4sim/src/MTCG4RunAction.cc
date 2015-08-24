//////////////////////////////////////////////////////////
// MTCG4RunAction.cc by Mich 110502
//////////////////////////////////////////////////////////

#include <unistd.h>
#include <iostream>
#include <sstream>

#include "G4Timer.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

#include "MTCG4RunAction.hh"
#include "MTCG4RunMessenger.hh"

#include "PESdsIO.hh"
#include "StepRootIO.hh"
#include "StepTextIO.hh"

MTCG4RunAction::MTCG4RunAction()
{
	fRunID = -1; // Default nonsense run ID number.
	fTimer = new G4Timer;
	fMessenger = new MTCG4RunMessenger(this);
}

MTCG4RunAction::~MTCG4RunAction()
{
	delete fTimer;
	delete fMessenger;
}

void MTCG4RunAction::BeginOfRunAction(const G4Run* theRun)
{
	G4cout << "### Run " << theRun->GetRunID() << " start." << G4endl; 
	fTimer->Start(); // Start timer.

	// Create step output in ROOT format.
	CreateStepRootOutput();

	// Create step output in text format.
	//CreateStepTextOutput();

	// Photo-electron data output in SDS format.
	//CreatePESdsOutput(theRun);
}

void MTCG4RunAction::EndOfRunAction(const G4Run* theRun)
{
	this->ProcessStepRootOut();
	//this->ProcessStepTextOut();
	//this->ProcessPESds();

	// Stop timer and print info.
	fTimer->Stop();
	G4cout << "number of event = " << theRun->GetNumberOfEvent() 
		<< " " << *fTimer << G4endl;
}

void MTCG4RunAction::CreatePESdsOutput(const G4Run* theRun)
{
	// Get run time info.
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	// Get run info.
	G4String outputPath = GetOutputPathName();
	G4String peOutputFileName = GetPEOutputFileName();
	std::ostringstream oss;
	oss << theRun->GetRunID();
	G4String runName = "run" + oss.str();
	G4String runType = "monte_carlo";
	G4String initials = "ms";
	G4int timeSec = time.tv_sec;
	G4int timeNanoSec = time.tv_nsec;
	G4int timePicoSec = 0;
	char hostName[80];
	gethostname(hostName, sizeof(hostName));
	G4String location = G4String(hostName);
	G4String temperature = "21C";
	G4String comments = "mtcg4im";

	// Object for PE output to user.
	PESdsIO::CreateInstance(
			outputPath + peOutputFileName + ".root",
			runName,
			runType,
			initials,
			timeSec,
			timeNanoSec,
			timePicoSec,
			location,
			temperature,
			comments);
}

void MTCG4RunAction::CreateStepRootOutput()
{
	// Get output filename and path from MTCG4RunAction.
	G4String outputPath = GetOutputPathName();
	G4String stepOutputFileName = GetStepOutputFileName();

	// Object for ROOT file output to user.
	StepRootIO::CreateInstance(outputPath + stepOutputFileName + ".root");
}

void MTCG4RunAction::CreateStepTextOutput()
{
	// Get output filename and path from MTCG4RunAction.
	G4String outputPath = GetOutputPathName();
	G4String stepOutputFileName = GetStepOutputFileName();

	// Object for text output to user.
	StepTextIO::CreateInstance(outputPath + stepOutputFileName + ".dat");
}

void MTCG4RunAction::ProcessPESds()
{
	// Delete output object singletons.	
	fPESdsOut = PESdsIO::GetInstance();
	fPESdsOut->ResetInstance();
}

void MTCG4RunAction::ProcessStepTextOut()
{
	fStepTextOut = StepTextIO::GetInstance();
	fStepTextOut->ResetInstance();
}

void MTCG4RunAction::ProcessStepRootOut()
{
	fStepRootOut = StepRootIO::GetInstance();
	fStepRootOut->WriteAtEndOfRun();
	fStepRootOut->ResetInstance();
}
