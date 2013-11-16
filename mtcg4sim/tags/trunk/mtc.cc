// -------------------------------------------------------------
//											MTCG4.cc by Mich 100301
// -------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
//#include "QGSP_BERT_HP.hh"

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4PhysicsList.hh"
//#include "LXePhysicsList.hh"
//#include "ExN06PhysicsList.hh"
#include "MTCG4PrimaryGeneratorAction.hh"
#include "MTCG4RunAction.hh"
#include "MTCG4SteppingAction.hh"
#include "MTCG4StackingAction.hh"
#include "MTCG4EventAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

// *************************************************************
//											main
// *************************************************************
int main(int argc, char** argv)
{
  // User Verbose output class
  //
  //G4VSteppingVerbose* verbosity = new ExN06SteppingVerbose;
  //G4VSteppingVerbose::SetInstance(verbosity);
	
	// construct default run manager
	//
	G4RunManager* runManager = new G4RunManager;
	G4cout << G4endl;

	// Set mandatory user initialization classes...
	//
	//G4VUserPhysicsList* physics = new QGSP_BERT_HP;
	G4VUserPhysicsList* physics = new MTCG4PhysicsList;
	//G4VUserPhysicsList* physics = new LXePhysicsList;
	//G4VUserPhysicsList* physics = new ExN06PhysicsList;
	runManager->SetUserInitialization(physics);
	//runManager->SetUserInitialization(new MTCG4PhysicsList);
	//
	G4VUserDetectorConstruction* detector = new MTCG4DetectorConstruction;
	runManager->SetUserInitialization(detector);
	//
	G4VUserPrimaryGeneratorAction* gen_action = new MTCG4PrimaryGeneratorAction;
	runManager->SetUserAction(gen_action);

#ifdef G4VIS_USE
	// Visualization Manager
	//
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
#endif
	
	// Set user action classes.
	//
	G4UserRunAction* run_action = new MTCG4RunAction;
	runManager->SetUserAction(run_action);
	//
	G4UserEventAction* event_action = new MTCG4EventAction;
	runManager->SetUserAction(event_action);
	//
	//TrackingAction* trackingaction = new TrackingAction(detector, run_action, event_action/*, histo*/);
	//runManager->SetUserAction(trackingaction);
	//
	G4UserSteppingAction* step_action = new MTCG4SteppingAction;
	runManager->SetUserAction(step_action);
	//
	G4UserStackingAction* stacking_action = new MTCG4StackingAction;
	runManager->SetUserAction(stacking_action);

	// Initialize G4 kernel
	//
	runManager->Initialize();

	// Get the pointer to the User Interface manager
	//
	G4UImanager* UImanager = G4UImanager::GetUIpointer();  

	if (argc==1)// Define UI session for interactive mode.
	{
#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
      ui->SessionStart();
      delete ui;
#endif
	}
	else           // Batch mode
	{
		//			G4cout << "Starting /control/execute\n";
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);
	}

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}
