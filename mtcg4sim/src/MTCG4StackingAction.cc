#include "MTCG4StackingAction.hh"
#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4Trajectory.hh"
//#include "MTCG4TrackInformation.hh"
//#include "HistoManager.hh"
#include "MTCG4StackingMessenger.hh"

#include "G4ClassificationOfNewTrack.hh"
//#include "G4VTouchable.hh"
//#include "G4TouchableHistory.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessType.hh"
#include "G4HadronicProcessType.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4StackingAction::MTCG4StackingAction(/*MTCG4RunAction* RA, MTCG4EventAction* EA, HistoManager* HM*/)
	/*:fRunAction(RA), fEventAction(EA), fHistoManager(HM)*/
	: fStage(0)
{
	fOpticalPhotonTrackingStatus = trackImmediately;
	//  fKillSecondary  = 0;
	fStackMessenger = new MTCG4StackingMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4StackingAction::~MTCG4StackingAction()
{
	delete fStackMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <typeinfo>
G4ClassificationOfNewTrack
MTCG4StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
	//// Print out energy of gammas from neutron capture.
	//if(/*aTrack->GetCreatorProcess()->GetProcessSubType() == 121
	//		&& */aTrack->GetDefinition() == G4Gamma::GammaDefinition()
	//		&& aTrack->GetParentID() == 1) {
	//	ofstream fout("capture_gamma_energy.dat", ofstream::app);
	//	//fout << aTrack->GetKineticEnergy()/MeV << std::endl;
	//	fout << aTrack->GetKineticEnergy()/MeV
	//	   	<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessType()
	//		<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessSubType()
	//	   	<< std::endl;
	//	fout.close();
	//}

	//// Print out energy of alpha from neutron capture.
	//if(aTrack->GetDefinition() == G4Alpha::AlphaDefinition()
	//		&& aTrack->GetParentID() == 1) {
	//	ofstream fout("alpha_energy.dat", ofstream::app);
	//	fout << aTrack->GetKineticEnergy()/MeV
	//		<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessType()
	//		<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessSubType()
	//		<< endl;
	//	fout.close();
	//}

	//// Print out energy of 7Li from neutron capture.
	//if(aTrack->GetDefinition()->GetAtomicNumber() == 3
	//		&& aTrack->GetDefinition()->GetAtomicMass() == 7
	//		&& aTrack->GetParentID() == 1) {
	//	ofstream fout("7li_energy.dat", ofstream::app);
	//	fout << aTrack->GetKineticEnergy()/MeV
	//		<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessType()
	//		<< " "
	//		<< aTrack->GetCreatorProcess()->GetProcessSubType()
	//		<< endl;
	//	fout.close();
	//}

	// Some introductory message.
	//G4cout
	//	<< aTrack->GetParticleDefinition()->GetParticleName()
	//	<< " with particleID "
	//	<< aTrack->GetTrackID()
	//	<< " at step "
	//	<< aTrack->GetCurrentStepNumber()
	//	<< ", and with parentID "
	//	<< aTrack->GetParentID()
	//	<< " found.\n";

	// Set default particle classification as fUrgent.
	// Will reassign different classification depending on conditions later.
	//G4ClassificationOfNewTrack classification = fWaiting;
	G4ClassificationOfNewTrack classification = fUrgent;

	//// What to do with primary.
	//if (aTrack->GetParentID() == 0)
	//{
	//	classification = fUrgent; // Keep primary particles.
	//}

	//// What to do with alpha or Li7.
	//else if(aTrack->GetDefinition()->GetPDGEncoding() == 1000020040
	//		|| aTrack->GetDefinition()->GetPDGEncoding() == 1000030070)
	//{
	//	MTCG4Trajectory* trajectoryOfAlphaOrLithium7Parent
	//		= GetTrajectory(aTrack->GetParentID());
	//	// If parent of alpha is primary neutron.
	//	if(trajectoryOfAlphaOrLithium7Parent->GetPDGEncoding() == 2112
	//			&& trajectoryOfAlphaOrLithium7Parent->GetParentID() == 0)
	//	{
	//		classification = fUrgent;
	//		G4cout << "Photon will be saved.\n";
	//	}
	//}

	//// If particle is secondary electron or positron.
	//if(aTrack->GetDefinition() == G4Electron::ElectronDefinition()) {
	//	//return fKill;
	//	return fUrgent;
	//}
	//if(aTrack->GetDefinition() == G4Positron::PositronDefinition()) {
	//	return fKill;
	//}

	//// If particle is secondary gamma.
	//if(aTrack->GetDefinition() == G4Gamma::GammaDefinition()) {
	//	//return fKill;
	// 	return fUrgent;
	//}
	//if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
	//{
	//	//return fKill;
	//	return fUrgent;
	//}
	
	// If particle is secondary photon.
	if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()) {
		switch(fStage)
		{
			case 0:
				if (fOpticalPhotonTrackingStatus == killImmediately)
					classification = fKill;
				else if (fOpticalPhotonTrackingStatus == trackImmediately)
					classification = fUrgent;
				else if (fOpticalPhotonTrackingStatus == saveAndTrack)
					classification = fWaiting;
				else if (fOpticalPhotonTrackingStatus == saveAndKill)
					classification = fWaiting;
				else
					classification = fUrgent;
				break;

			default:
				assert(fOpticalPhotonTrackingStatus != killImmediately);
				assert(fOpticalPhotonTrackingStatus != trackImmediately);
				if (fOpticalPhotonTrackingStatus == saveAndTrack)
					classification = fUrgent;
				else if (fOpticalPhotonTrackingStatus == saveAndKill)
					classification = fKill;
				else
					classification = fUrgent;
				//G4int parentID = aTrack->GetParentID();
				//// As long as particle is a secondary particle.
				//while(parentID > 0)
				//{
				//	MTCG4Trajectory* parentTrajectory = GetTrajectory(parentID);
				//	if(!parentTrajectory) {
				//		G4cerr
				//			<< "Photon trajectory trace back failed - no parent found."
				//			<< G4endl;
				//		G4cerr
				//			<< "ParentID: "
				//			<< aTrack->GetParentID()
				//			<< G4endl;
				//		//classification = fKill;
				//		break;
				//	}
				//	else {
				//		//G4cout << ">> Parent found!" << G4endl;
				//		//G4cout << ">> Parent name is " <<
				//		//	parentTrajectory->GetParticleName() << "!" <<
				//		//	G4endl;

				//		// If parent particle was created by NeutronInelastic or
				//		// nCapture processes, save track.
				//		G4String creatorProcessName =
				//			parentTrajectory->GetCreatorProcessName();
				//		G4int parentPdgEncoding =
				//			parentTrajectory->GetPDGEncoding();
				//		if(creatorProcessName == "NeutronInelastic"
				//				/*|| creatorProcessName == "nCapture"*/
				//				&&
				//				// Additional condition that the optical photon
				//				// was created by a 7Li or alpha.
				//				(parentPdgEncoding/10==100003007||parentPdgEncoding/10==100002004)
				//				) {
				//			MTCG4Trajectory* trajectoryOfParentOfParent
				//				= GetTrajectory(parentTrajectory->GetParentID());
				//			// If parent of alpha or Li7 is primary neutron.
				//			if(trajectoryOfParentOfParent->GetPDGEncoding() == 2112
				//					&& trajectoryOfParentOfParent->GetParentID() == 0)
				//			classification = fUrgent;
				//			break;
				//		}

				//		//// If parent particle is a alpha or lithium7.
				//		//// Identify them by atomic number and atomic mass to
				//		//// include all excitation energies of alpha or lithium7.
				//		//// The 1's digit for the PDG encoding is 0 for ground
				//		//// state and 1 for all other excitation energy levels.
				//		//// We want to find the isotopes of any energy so I get
				//		//// rid of the 1's digit and match it accordingly to the
				//		//// PDG ecoding for alpha or Li7.
				//		//if(
				//		//		(parentTrajectory->GetPDGEncoding())/10==100002004 // Alpha.
				//		//		|| (parentTrajectory->GetPDGEncoding())/10 == 100003007) // Li7.
				//		//{
				//		//	MTCG4Trajectory* trajectoryOfAlphaOrLithium7Parent
				//		//		= GetTrajectory(parentTrajectory->GetParentID());
				//		//	// If parent of alpha or Li7 is primary neutron.
				//		//	if(trajectoryOfAlphaOrLithium7Parent->GetPDGEncoding() == 2112
				//		//			&& trajectoryOfAlphaOrLithium7Parent->GetParentID() == 0)
				//		//	{
				//		//		classification = fUrgent;
				//		//		G4cout << "Photon is from primary produced "
				//		//			<< "Li7/alpha.\n";
				//		//		G4cout << "Photon will be saved.\n";
				//		//		break;
				//		//	}
				//		//}

				//		//// If parent is primary neutron.
				//		//if(parentTrajectory->GetPDGEncoding() == 2112
				//		//		&& parentTrajectory->GetParentID() == 0)
				//		//{
				//		//	G4cout << "Photon is from primary neutron, ";

				//		//	G4cout << "with process "
				//		//	<< parentTrajectory->GetCreatorProcessName()
				//		//	<< ", it will be killed.\n";
				//		//	//classification = fUrgent;
				//		//	classification = fKill;
				//		//	break;
				//		//}

				//		//// All other parents.
				//		//else{
				//		//	G4cout << "Photon will be killed.\n";
				//		//	classification = fKill;
				//		//	break;
				//		//}
				//	}
				//	parentID = parentTrajectory->GetParentID();
				//}
				//if(classification == fKill) {
				//	MTCG4Trajectory* trajectoryOfParent =
				//		GetTrajectory(aTrack->GetParentID());
				//	G4String parentName = trajectoryOfParent->GetParticleName();
				//	G4cout << aTrack->GetDefinition()->GetParticleName() << " with "
				//		<< parentName << " parent will be killed.\n";
				//	G4cout << " produced by " <<
				//		aTrack->GetCreatorProcess()->GetProcessName() << G4endl;
				//}
				break;
		}
	}

	//if(aTrack->GetParentID() != 0) {
	//	MTCG4Trajectory* trajectoryOfParent
	//		= GetTrajectory(aTrack->GetParentID());
	//	if(!(GetTrajectory(aTrack->GetParentID())->GetParticleName())) {
	//		G4cout
	//			<< "Parent trajectory not found.\n";
	//	}
	//	else{
	//	G4cout
	//		<< "Parent is: "
	//		<< GetTrajectory(aTrack->GetParentID())->GetParticleName()
	//		<< G4endl;
	//	}
	//}

	////count secondary particles
	////  run_action->CountParticles(aTrack->GetDefinition());

	////
	////energy spectrum of secondaries
	////
	////  G4double energy = aTrack->GetKineticEnergy();
	////  G4double charge = aTrack->GetDefinition()->GetPDGCharge();

	////  if (charge != 0.) fHistoManager->FillHisto(2,energy);


	//// Stack or delete secondarie S
	///*
	//if (fKillSecondary) {
	//	if (fKillSecondary == 1) {
	//		fEventAction->AddEnergy(energy);
	//	}  
	//	classification = fKill;
	//}
	//*/
	//
	////G4cout << "Non-photon particle.\n";
	//
	//if(classification == fKill) G4cout << "Particle will be killed!\n";
	//else G4cout << "Particle will be saved!\n";
	//G4cout << "\n\n" << G4endl;
	return classification;
}

MTCG4Trajectory* MTCG4StackingAction::GetTrajectory(G4int trackID)
{
	G4TrajectoryContainer* container = 
		G4RunManager::GetRunManager()->GetCurrentEvent()->
		GetTrajectoryContainer();
	if(!container) {
		G4cerr << "No trajectory container!\n";
	 	return 0;
	}
	const G4int nTraj = container->entries();
	//G4cout << "entries in container: " << nTraj << G4endl;
	for(G4int iTraj = 0; iTraj < nTraj; ++iTraj) {
		MTCG4Trajectory* traj =
		   	static_cast<MTCG4Trajectory*>((*container)[iTraj]);
		//MTCG4Trajectory* traj = (MTCG4Trajectory*)((container->GetVector())[iTraj]);
		//G4cout << "tr1ID: " << traj->GetTrackID() << G4endl;
		//G4cout << "trackID: " << trackID << G4endl;
		if(traj->GetTrackID()==trackID) return traj;
	}
	return NULL;
	//G4cout << "test1\n";
	//G4TrajectoryContainer* container =
	//	G4RunManager::GetRunManager()->GetCurrentEvent()->GetTrajectoryContainer();
	//G4cout << "test2\n";
	//if(!container) {
	//	G4cout
	//		<< "No trajectory container!"
	//		<< G4endl;
	//	return 0;
	//}
	//G4cout << "test3\n";
	////size_t nTrajSize = container->size();
	////for(size_t i=0;i<nTrajSize;i++) {
	////	MTCG4Trajectory* tr1 = (MTCG4Trajectory*)((*container)[i]);
	////	G4cout << "tr1ID: " << tr1->GetTrackID() << G4endl;
	////	G4cout << "trackID: " << trackID << G4endl;
	////	if(tr1->GetTrackID()==trackID) return tr1;
	////}
	//G4int nTraj = container->entries();
	//G4cout << "test4\n";
	//G4cout << "entries in container: " << nTraj << G4endl;
	//for(G4int i=0;i<nTraj;i++) {
	//	MTCG4Trajectory* tr1 = (MTCG4Trajectory*)((*container)[i]);
	//	G4cout << "tr1ID: " << tr1->GetTrackID() << G4endl;
	//	G4cout << "trackID: " << trackID << G4endl;
	//	if(tr1->GetTrackID()==trackID) return tr1;
	//}
	//G4cout << "test5\n";
	//return 0;
}

void MTCG4StackingAction::NewStage()
{
	fStage++;
	stackManager->ReClassify();
	return;
}

void MTCG4StackingAction::PrepareNewEvent()
{
	fStage = 0;
}
