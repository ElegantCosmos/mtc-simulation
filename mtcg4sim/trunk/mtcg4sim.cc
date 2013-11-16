// -------------------------------------------------------------
//											MTCG4.cc by Mich 100301
// -------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
//#include "QGSP_BERT_HP.hh"

#include "MTCG4DetectorConstruction.hh"
#include "MTCG4PhysicsList.hh"
//#include "LXePhysicsList.hh"
//#include "ExN06PhysicsList.hh"
#include "MTCG4PrimaryGeneratorAction.hh"
#include "MTCG4RunAction.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4StackingAction.hh"
#include "MTCG4TrackingAction.hh"
#include "MTCG4SteppingAction.hh"

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
	G4VUserDetectorConstruction* detector = new MTCG4DetectorConstruction;
	runManager->SetUserInitialization(detector);
	//
	//G4VUserPhysicsList* physics = new MTCG4PhysicsList;
	//runManager->SetUserInitialization(physics);
	G4PhysListFactory factory;
	G4VModularPhysicsList* phys = 0;
	//PhysicsListMessenger* mess = 0;
	//G4String physName = "QGSP_BIC_HP";
	//G4String physName = "Shielding";
	G4String physName = "";

	// Physics List name defined via 3nd argument
	//if (argc==3) { physName = argv[2]; }

	// Physics List name defined via environment variable
	if("" == physName) {
		char* path = getenv("PHYSLIST");
		if (path) { physName = G4String(path); }
	}

	// reference PhysicsList via its name
	if("" != physName && factory.IsReferencePhysList(physName)) {
		phys = factory.GetReferencePhysList(physName);

		// added extra EM options
		//phys->RegisterPhysics(new G4EmUserPhysics(1));

		// instantiated messenger
		//mess = new PhysicsListMessenger();
	}

	// local Physics List
	//if(!phys) { phys = new MTCG4PhysicsList(); }
	if(!phys){
		G4cout << "using physics list MTCG4PhysicsList" << G4endl;
	   	runManager->SetUserInitialization(new MTCG4PhysicsList);
   	}
	else{
		G4cout << "using physics list " << physName << G4endl;
		// define physics
		runManager->SetUserInitialization(phys);
	}

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
	G4UserTrackingAction* tracking_action = new MTCG4TrackingAction;
	runManager->SetUserAction(tracking_action);
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
      //G4UIExecutive * ui = new G4UIExecutive(argc, argv, "qt");
      G4UIExecutive * ui = new G4UIExecutive(argc, argv, "tcsh");
#ifdef G4VIS_USE
      //UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
      ui->SessionStart();
      delete ui;
#endif
	}
	else           // Batch mode
	{
		for(G4int i = 1; i < argc; i++){
			G4String command = "/control/execute ";
			G4String fileName = argv[i];
			UImanager->ApplyCommand(command+fileName);
		}
	}

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}
