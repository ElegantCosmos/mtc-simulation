//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file persistency/P01/src/PESdsIO.cc
/// \brief Implementation of the PESdsIO class
//
#include <sstream>

#include "PESdsIO.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "G4RunManager.hh"

#include "MTCG4EventAction.hh"
#include "MTCG4HitPmtCollection.hh"

#include "libmtc-sds.hpp"

static PESdsIO *PESdsIOManager = NULL;

mtc::sds::SDS *PESdsIO::fSdsFile = NULL;
//mtc::sds::SDS::isOpen;
struct mtc::sds::RunDescriptorStruct PESdsIO::fRun;
struct mtc::sds::EventDescriptorStruct PESdsIO::fEvent;
G4String PESdsIO::fFileName;

PESdsIO::PESdsIO()
{
	InitializeRunInfo();
}

PESdsIO::~PESdsIO()
{}

void PESdsIO::CreateInstance(
		G4String fileName,
		G4String runName,
		G4String runType,
		G4String initials,
		G4int timeSec,
		G4int timeNanoSec,
		G4int timePicoSec,
		G4String location,
		G4String temperature,
		G4String comments)
{
	// Set output file name.
	fFileName = fileName;

	// Set run descriptor struct member fields.
	fRun.runName = runName;
	fRun.type = runType;
	fRun.initials = initials;
	fRun.time.sec = timeSec;
	fRun.time.nsec = timeNanoSec;
	fRun.time.psec = timePicoSec;
	fRun.location = location;
	fRun.temperature = temperature;
	fRun.comments = comments;

	PESdsIOManager = new PESdsIO();
}

PESdsIO* PESdsIO::GetInstance()
{
	if (!PESdsIOManager)
		PESdsIOManager = new PESdsIO();
	return PESdsIOManager;
}

void PESdsIO::ResetInstance()
{
	if (fSdsFile) {
		if (fSdsFile->isOpen()) fSdsFile->close();
		delete fSdsFile;
		fSdsFile = NULL;
	}
	delete PESdsIOManager;
	PESdsIOManager = NULL;
}

void PESdsIO::InitializeRunInfo()
{
	// Create run tree in output root file.
	unlink(fFileName);

	fSdsFile = new mtc::sds::SDS(fFileName.c_str());
	fSdsFile->open();
	fSdsFile->addRun(fRun);

	//// Confirm.
	//std::cout << "Created run" << std::endl;
	//std::vector<std::string> runs = fSdsFile->runs();
	//for (size_t i = 0; i < runs.size(); ++i)
	//{
	//	fRun = fSdsFile->runDescriptor(runs[i]);
	//	std::cout << fRun.runName << std::endl;
	//	std::cout << "\t" << fRun.type << std::endl;
	//	std::cout << "\t" << fRun.initials << std::endl;
	//	std::cout << "\t" << fRun.time.sec <<
	//	  	"." << fRun.time.nsec <<
	//	   	"." << fRun.time.psec << std::endl;
	//	std::cout << "\t" << fRun.location << std::endl;
	//	std::cout << "\t" << fRun.temperature << std::endl;
	//	std::cout << "\t" << fRun.comments << std::endl;
	//}
}

void PESdsIO::Write(const G4Event* theEvent)
{
	//fSdsFile->addRun(fRun);
	MTCG4HitPmtCollection* theHitPmts =
		((MTCG4EventAction*)(G4RunManager::GetRunManager()->
			GetUserEventAction()))->GetTheHitPmtCollection();
	//std::cout << "theHitPmts->GetEntries() = " << theHitPmts->GetEntries() <<
	//	std::endl;
	if (theHitPmts->GetEntries() == 0) return; // Don't write if event is empty.

	// Set event descriptor member fields.
	fEvent.number = theEvent->GetEventID();
	fEvent.firstHitTime = fRun.time; // Use beginning time of run as base time.

	// Find earliest hit time of event and register to event descriptor struct.
	G4double earliestHitTime = std::numeric_limits<G4double>::max()*ns;
	for (G4int iPmt = 0; iPmt < theHitPmts->GetEntries(); iPmt++) {
		MTCG4HitPmt *pmt = theHitPmts->GetPmt(iPmt);
		if (!pmt) continue;
		for (G4int iHit = 0; iHit < pmt->GetEntries(); iHit++) {
			MTCG4HitPhoton *photon = pmt->GetPhoton(iHit);
			if (!photon) continue;
			if (earliestHitTime > photon->GetTime())
				earliestHitTime = photon->GetTime();
		}
	}
	// Time since last occuring event is not taken into account. If multiple
	// events are simulated, all photon hits are timed as if all events happened
	// simultaneously (in their seperate parallel worlds) from the unix time of
	// when the simulation was started. Fix later.
	fEvent.firstHitTime.sec += G4int(earliestHitTime/s); // Record seconds.
	fEvent.firstHitTime.nsec += (G4int(earliestHitTime/ns))%1000000000; // ns.
	fEvent.firstHitTime.psec += (G4int(earliestHitTime/ns*1000))%1000; // ps.

	// Add event and confirm.
	fSdsFile->addEvent(fRun.runName, fEvent);
	//std::cout << "Created Event" << std::endl;
	//std::vector <struct mtc::sds::EventDescriptorStruct> events = fSdsFile ->
	//	events(fRun.runName);
	//for (size_t iEvent = 0; iEvent < events.size(); ++iEvent) {
	//	mtc::sds::EventDescriptorStruct evt = events[iEvent];
	//	std::cout << evt.number << std::endl;
	//	std::cout << "\t" << evt.firstHitTime.sec <<  "." <<
	//		evt.firstHitTime.nsec << "." << evt.firstHitTime.psec << std::endl;
	//}

	// Add all photon hits for event.
	//cout << "Adding pixel data to last event" << std::endl;
	for (G4int pmtID = 1; pmtID <= 24; pmtID++) { // PMT ID is 1 through 24.
		MTCG4HitPmt *pmt = theHitPmts->GetPmt_ByID(pmtID);
		if (!pmt) continue; // Ignore if no PMT with given ID exists.
		//G4cout << "pmt->GetEntries() = " << pmt->GetEntries() << G4endl;
		// Row ID is 1 through 8.
		for (G4int rowID = 1; rowID <= 8; rowID++) {
			// Column ID is 1 through 8.
			for (G4int columnID = 1; columnID <= 8; columnID++) {
				std::vector<float> data;
				for (G4int iEntry = 0; iEntry < pmt->GetEntries(); iEntry++) {
					MTCG4HitPhoton *photon = pmt->GetPhoton(iEntry);

					// Check if photon hit exists for particular pixel.
					if (!photon) continue;
					if (photon->GetAnodeRow() != rowID) continue;
					if (photon->GetAnodeColumn() != columnID) continue;

					// Store hit time relative to earliest hit time.
					data.push_back(photon->GetTime()/ns - earliestHitTime/ns);
					//std::cout << "hittime: " << photon->GetTime()/ns <<
					//	std::endl;
					//std::cout << "earliest: " << earliestHitTime/ns <<
					//	std::endl;
					//std::cout << "photoncount: " << photon->GetCount() <<
					//	std::endl;
					data.push_back(photon->GetCount());
				}
				if (data.size() != 0) { // Save to SDS file if hits exist.
					G4int pmtAnodeID = pmtID*100 +	rowID*10 + columnID;
					//G4cout << "fRun.runName: " << fRun.runName << G4endl;
					//G4cout << "fEvent: " << fEvent.number << G4endl;
					fSdsFile->addPixel(fRun.runName, fEvent, pmtAnodeID, data);
				}
				//// Check photon info contained in SDS files.
				//G4cout
				// 	<< "Reading back pixel data for pixel ID "
				// 	<< pmtAnodeID << G4endl;
				//data = fSdsFile->getPixel(fRun.runName, fEvent, pmtAnodeID);
				//for (size_t i = 0; i < data.size(); ++i)
				//{
				//	std::cout << i << " : " << data[i] << std::endl;
				//}
			}
		}
	}
}

void PESdsIO::Close()
{
	fSdsFile->close();
}
