#include "MTCG4StackingAction.hh"
#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4Trajectory.hh"
//#include "MTCG4TrackInformation.hh"
//#include "HistoManager.hh"
//#include "StackingMessenger.hh"

#include "G4ClassificationOfNewTrack.hh"
//#include "G4VTouchable.hh"
//#include "G4TouchableHistory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4StackingAction::MTCG4StackingAction(/*MTCG4RunAction* RA, MTCG4EventAction* EA, HistoManager* HM*/)
	/*:runaction(RA), eventaction(EA), histoManager(HM)*/
{
	//  killSecondary  = 0;
	//  stackMessenger = new StackingMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTCG4StackingAction::~MTCG4StackingAction()
{
	//  delete stackMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
MTCG4StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
	G4ClassificationOfNewTrack status;
	if (aTrack->GetParentID() == 0){
	   	status = fUrgent; // Keep primary particles.
//		G4cout << "Primary particle found!\n";
	}
	  if (aTrack->GetDefinition() == G4Gamma::GammaDefinition()) return fUrgent;
	  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) return fUrgent;
//	if(aTrack->GetDefinition() == G4Gamma::GammaDefinition() || aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
////		G4cout << "Gamma detected!\n";
//		G4int parentID = aTrack->GetParentID();
//		while(parentID > 0)
//		{
//			MTCG4Trajectory* parentTrajectory = GetParentTrajectory(parentID);
//			if(parentTrajectory == 0)
//			{
////				G4cout << ">> Trajectory trace back failed - no parent found." << G4endl;
//				break;
//			}
//			else{
////				G4cout << ">> Parent found!" << G4endl;
////				G4cout << ">> Parent name " << parentTrajectory->GetParticleName() << "!" << G4endl;
//				if(parentTrajectory->GetPDGEncoding() == 1000020040 || parentTrajectory->GetPDGEncoding() == 1000030070){ // If particle is a alpha or lithium7. Actually I should identify Li7[0.0] by atomic number and atomic mass to include all Li with all excitation energies, but I will do this later.
//					MTCG4Trajectory* trajectoryOfAlphaOrLithium7Parent = GetParentTrajectory(parentTrajectory->GetParentID());
//					if(trajectoryOfAlphaOrLithium7Parent->GetPDGEncoding() == 2112 && trajectoryOfAlphaOrLithium7Parent->GetParentID() == 0) // If parent of alpha is primary neutron.
//					{
////						G4cout << "Photon saved.\n";
//						return fUrgent;
//					}
//				}
//				parentID = parentTrajectory->GetParentID();
//			}
//		}
////		G4cout << "Killed photon!\n";
//		return fKill;
//	}

	//count secondary particles
	//  run_action->CountParticles(aTrack->GetDefinition());

	//
	//energy spectrum of secondaries
	//
	//  G4double energy = aTrack->GetKineticEnergy();
	//  G4double charge = aTrack->GetDefinition()->GetPDGCharge();

	//  if (charge != 0.) histoManager->FillHisto(2,energy);


	//  //stack or delete secondaries
	//  if (killSecondary) {
	//	  if (killSecondary == 1) {
	//		  eventaction->AddEnergy(energy);
	//	  }  
	//	  status = fKill;
	//  }
	//
//	G4cout << "Non-photon particle.\n";
	status = fUrgent;
	return status;
}

MTCG4Trajectory* MTCG4StackingAction::GetParentTrajectory(G4int parentID)
{
	G4TrajectoryContainer* container =
		G4RunManager::GetRunManager()->GetCurrentEvent()->GetTrajectoryContainer();
	if(container==0) return 0;
//	size_t nTraj = container->size();
//	for(size_t i=0;i<nTraj;i++)
//	{
//		MTCG4Trajectory* tr1 = (MTCG4Trajectory*)((*container)[i]);
//		if(tr1->GetTrackID()==parentID) return tr1;
//	}
	G4int nTraj = container->entries();
//	G4cout << "entries in container: " << nTraj << G4endl;
	for(G4int i=0;i<nTraj;i++)
	{
		MTCG4Trajectory* tr1 = (MTCG4Trajectory*)((*container)[i]);
		if(tr1->GetTrackID()==parentID) return tr1;
	}
//	G4cout << "2entries in container: " << nTraj << G4endl;
	return 0;
}
