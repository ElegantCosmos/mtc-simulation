/** @file MTCG4PmtOpticalModel.cc
  Defines a FastSimulationModel class for handling optical photon
  interactions with PMT: partial reflection, transmission, absorption,
  and hit generation.

  This file is part of the GenericLAND software library.
  $Id: MTCG4PmtOpticalModel.cc,v 1.10 2007/03/18 01:48:39 kamland0 Exp $

  @author Glenn Horton-Smith, March 20, 2001.
  @author Dario Motta, Feb. 23 2005: Formalism light interaction with photocathode.
  */

//#include "local_g4compat.hh"
#include "G4RunManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"

//#include "mtcg4_global_definitions.hh"
#include "MTCG4PmtOpticalModel.hh"
#include "MTCG4PmtSD.hh"
#include "MTCG4DetectorConstruction.hh"
#include "MTCG4SteppingAction.hh"


G4UIdirectory* MTCG4PmtOpticalModel::fgCmdDir = NULL;
// initialize verbosity and "luxury level"
G4int MTCG4PmtOpticalModel::fVerbosity = 0;
G4int MTCG4PmtOpticalModel::fLuxLevel = 2;


// local helper function
static G4Envelope* GetOrMakeEnvelope(G4LogicalVolume*lv)
{
	//#if G4VERSIONCODE<40800
	//  return lv;
	//#else
	if (lv->IsRootRegion())
		return lv->GetRegion();
	G4Region* region = new G4Region(lv->GetName()+"_optical_model_region");
	region->AddRootLogicalVolume(lv);
	return region;
	//#endif
}

// constructor -- also handles all initialization
MTCG4PmtOpticalModel::MTCG4PmtOpticalModel(
		G4String modelName,
		G4VPhysicalVolume *envelope_phys)
:G4VFastSimulationModel(
		modelName,
		GetOrMakeEnvelope(envelope_phys->GetLogicalVolume()))
{
	// Initialize fPhotonEnergy to a nonsense value to indicate that the other
	// values are not initialized.
	fPhotonEnergy = -1.0;

	// Save a few geometry values for later use.
	fEnvelopePhys = envelope_phys;
	fEnvelopeLog = envelope_phys->GetLogicalVolume();
	//G4cout << "envelope name = " << fEnvelopeLog->GetName() << G4endl;
	fPixelPitch =
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		 GetUserDetectorConstruction())->GetPixelPitch();
	fPhotocathodeFaceDimension = 
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		 GetUserDetectorConstruction())->GetPhotocathodeFaceDimension();
	fFastSimulationVolumeExtentInX = .5*fPhotocathodeFaceDimension;
	fFastSimulationVolumeExtentInY = .5*fPhotocathodeFaceDimension;
	fFastSimulationVolumeExtentInZBottom =
		((MTCG4DetectorConstruction*)G4RunManager::GetRunManager()->
		 GetUserDetectorConstruction())->GetBottomOfPmtInnerVacuumExtentInZ();
	fGeometricTolerance = G4GeometryTolerance::GetInstance()->
		GetSurfaceTolerance();

	// Get PMT outer vacuum.
	G4VPhysicalVolume *pmtOuterVacuumPhys = NULL;
	for (G4int iDaughter = 0; iDaughter < fEnvelopeLog->GetNoDaughters();
			++iDaughter)
	{
		G4VPhysicalVolume* daughter = fEnvelopeLog->GetDaughter(iDaughter);
		std::string name = daughter->GetName();
		if (name == "pmtOuterVacuumPhysical") {
			pmtOuterVacuumPhys = daughter;
			break;
		}
	}
	assert(pmtOuterVacuumPhys); // Pointer must exist.
	G4LogicalVolume* pmtOuterVacuumLog = pmtOuterVacuumPhys->GetLogicalVolume();
	assert(pmtOuterVacuumLog);

	// Get PMT inner vacuum.
	fPmtInnerVacuumPhys = NULL;
	for (G4int iDaughter = 0; iDaughter < pmtOuterVacuumLog->GetNoDaughters();
			++iDaughter)
	{
		G4VPhysicalVolume* daughter = pmtOuterVacuumLog->GetDaughter(iDaughter);
		std::string name = daughter->GetName();
		if (name == "pmtInnerVacuumPhysical") {
			fPmtInnerVacuumPhys = daughter;
			break;
		}
	}
	assert(fPmtInnerVacuumPhys); // Inner vacuum phys pointer must exist.
	fPmtInnerVacuumSolid =
		fPmtInnerVacuumPhys->GetLogicalVolume()->GetSolid();

	// Get PMT glass window.
	fPmtGlassWindowPhys = NULL;
	for (G4int iDaughter = 0; iDaughter < fEnvelopeLog->GetNoDaughters();
			++iDaughter)
	{
		G4VPhysicalVolume* daughter = fEnvelopeLog->GetDaughter(iDaughter);
		std::string name = daughter->GetName();
		if (name == "pmtGlassWindowPhysical") {
			fPmtGlassWindowPhys = daughter;
			break;
		}
	}
	assert(fPmtGlassWindowPhys);
	fPmtGlassWindowSolid =
		fPmtGlassWindowPhys->GetLogicalVolume()->GetSolid();

	// The following two are translation vectors wrt the fast simulation
	// envelope volume "pmt_glass_housing_physical". We should actually call
	// G4FastTrack::GetPrimaryTrack()->GetTouchable()->GetVolume(i_depth)->
	// GetTranslation() recursively for i_depth = 0, 1, ...,
	// G4FastTrack::GetPrimaryTrack()->GetTouchable()->GetHistoryDepth(),
	// until the envelope volume is reached respectively for the two vectors
	// below.
	// But I don't know how to call this method when the primary particle track
	// has yet to reach "pmt_inner_vacuum_physical" and I already need to get
	// the translation vector of this volume wrt the envelope volume. I don't
	// know how to do this so I will leave the code for now as the following. It
	// is sufficient as long as the mother volume of
	// "pmt_inner_vacuum_physical", "pmt_outer_vacuum_physical" has no
	// translation wrt the envelope volume "pmt_glass_housing_physical" as is
	// the case in the present code. Mich 20130117.
	fPmtGlassWindowPhysTranslation =
		fPmtGlassWindowPhys->GetObjectTranslation();
	fPmtInnerVacuumPhysTranslation =
		fPmtInnerVacuumPhys->GetObjectTranslation();

	// Now fetch needed MPVs
	InitMPVs(envelope_phys);

	// add UI commands
	if ( fgCmdDir == NULL ) {
		fgCmdDir = new G4UIdirectory("/PMTOpticalModel/");
		fgCmdDir->SetGuidance("PMT optical model control.");
		G4UIcommand *cmd;

		cmd = new G4UIcommand("/PMTOpticalModel/verbose",this);
		cmd->SetGuidance("Set verbose level");
		cmd->SetParameter(new G4UIparameter("level", 'i', false));

		cmd = new G4UIcommand("/PMTOpticalModel/luxlevel",this);
		cmd->SetGuidance("Set \"luxury level\" for PMT Optical Model\n"
				" 0 == standard \"black bucket\": photons stop in PC, maybe make pe, \n"
				" 1 == shiny translucent brown film: photons only stop if they make a PE, otherwise 50/50 chance of reflecting/transmitting\n"
				" 2 or greater == full model\n"
				"The default value is 2."
				);
		cmd->SetParameter(new G4UIparameter("level", 'i', false));    
	}
}


// destructor
MTCG4PmtOpticalModel::~MTCG4PmtOpticalModel ()
{
	// nothing to delete
	// Note: The "MaterialPropertyVector"s are owned by the material, not us.
}


// IsApplicable() method overriding virtual function of G4VFastSimulationModel
// returns true if model is applicable to given particle.
// -- see also Geant4 docs
	G4bool
MTCG4PmtOpticalModel::IsApplicable(const G4ParticleDefinition &particleType)
{
	return ( &particleType == G4OpticalPhoton::OpticalPhotonDefinition() ||
			&particleType == G4Gamma::GammaDefinition() );
}


// ModelTrigger() method overriding virtual function of G4VFastSimulationModel
// returns true if model should take over this specific track.
// -- see also Geant4 docs
	G4bool
MTCG4PmtOpticalModel::ModelTrigger(const G4FastTrack &fastTrack)
{
	//// we trigger if the track position is above the equator
	//// or if it is on the equator and heading up
	//if ( fastTrack.GetPrimaryTrackLocalPosition().z() >
	//		G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() )
	//  return true;
	//if ( fastTrack.GetPrimaryTrackLocalPosition().z() >
	//		-G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() &&
	//		fastTrack.GetPrimaryTrackLocalDirection().z() > 0.0 )
	//  return true;
	//return false;

	if( fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom + fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().x()) <
			fFastSimulationVolumeExtentInX - fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().y()) <
			fFastSimulationVolumeExtentInY - fGeometricTolerance )
		return true;
	if(	fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().z() <
			fFastSimulationVolumeExtentInZBottom + fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().x()) <
			fFastSimulationVolumeExtentInX + fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().y()) <
			fFastSimulationVolumeExtentInY + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalDirection().z() > 0.0 )
		return true;
	if( fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().x() >
			fFastSimulationVolumeExtentInX - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().x() <
			fFastSimulationVolumeExtentInX + fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().y()) <
			fFastSimulationVolumeExtentInY + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalDirection().x() < 0.0 )
		return true;
	if( fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().x() >
			-fFastSimulationVolumeExtentInX - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().x() <
			-fFastSimulationVolumeExtentInX + fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().y()) <
			fFastSimulationVolumeExtentInY + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalDirection().x() > 0.0 )
		return true;
	if( fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom - fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().x()) <
			fFastSimulationVolumeExtentInX + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().y() >
			-fFastSimulationVolumeExtentInY - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().y() <
			-fFastSimulationVolumeExtentInY + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalDirection().y() > 0.0 )
		return true;
	if( fastTrack.GetPrimaryTrackLocalPosition().z() >
			fFastSimulationVolumeExtentInZBottom - fGeometricTolerance
			&& std::fabs(fastTrack.GetPrimaryTrackLocalPosition().x()) <
			fFastSimulationVolumeExtentInX + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().y() >
			+fFastSimulationVolumeExtentInY - fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalPosition().y() <
			+fFastSimulationVolumeExtentInY + fGeometricTolerance
			&& fastTrack.GetPrimaryTrackLocalDirection().y() < 0.0 )
		return true;
	return false;
}

// DoIt() method overriding virtual function of G4VFastSimulationModel
// does the fast simulation for this track.  It is basically a faster but
// complete tracking code for the two-volume case.  It is a monster.
// -- see also Geant4 docs and comments below
	void
MTCG4PmtOpticalModel::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
	// Logic summary:
	//  1) If track is outside the "pmt_inner_vacuum" vacuum, then track
	//     is advanced to either the pmt_inner_vacuum/body interface or to the body/outside
	//     interface, whichever comes first.
	//   a) If track hits the body/outside interface, then we just update 
	//      its position and time and return;
	//   b) else, we do the thin-layer reflection/transmission/absorption thing
	//      with n1 =n_glass and n3 =1.0:
	//    i) Make a binary random decision on whether to absorb a photon.
	//       Probability is equal to "weight" of track times absorption coeff.
	//       (If P > 1.0, weight hit by floor(P+G4UniformRand()).)  
	//       If photon is absorbed, then we are done tracking.
	//    ii) A binary random decision is made on whether to reflect or
	//       refract (transmit) the remaining track, if any.
	//  2) If track is in the "pmt_inner_vacuum" vacuum (either on entry to DoIt or
	//     after transmission in step 1.b.ii), then we advance to the
	//     pmt_inner_vacuum/body interface (surface of pmt_inner_vacuum).  Then we do the
	//     thin-layer reflection/transmission/absoprtion thing exactly as
	//     in step 1.b, but with n1 =1.0 and n3 =n_glass.
	//  3) Steps 1 and 2 are repeated until track hits the body outer surface,
	//     descends below the equator, or is completely absorbed on the
	//     photocathode.  Note it is best to check if step 1 applies even
	//     after doing step 1, to avoid making assumptions about the geometry.
	//     (It could happen someday that we have a photocathode with a negative
	//     curvature someplace.)
	//
	// Tracking elsewhere in the PMT (in the vacuum or glass below the equator,
	// for example) is handled by the usual Geant4 tracking.  If we ever
	// need to handle more than two volumes or more than one surface, then
	// we should use a general Geant4 "navigator" as in ExN05EMShowerModel,
	// but for this simple case, we can be more efficient with this custom
	// coding.  -GHS.

	G4double dist, dist1;
	G4ThreeVector pos;
	G4ThreeVector dir;
	G4ThreeVector pol;
	G4ThreeVector norm;
	G4double time;
	G4int weight;
	G4double energy;
	G4double n_glass;
	// Get sensitive detector.
	// Not a very good way to get from envelope daughters by index number but
	// we'll keep it this the way for the sake of speed, as this has to be done
	// for every photon.
	G4VSensitiveDetector* detector =
		fastTrack.GetEnvelopeLogicalVolume()->GetDaughter(0)
		->GetLogicalVolume()->GetSensitiveDetector();
	//#if G4VERSIONCODE<40800
	//    fastTrack.GetEnvelope()->GetSensitiveDetector();
	//#else
	//fastTrack.GetEnvelopeLogicalVolume()->GetSensitiveDetector();
	//#endif  
	enum EWhereAmI { kInGlass, kInVacuum } whereAmI;
	int ipmt = -1;
	G4String physicalVolumeName;

	// find which pmt we are in
	// The following doesn't work anymore (due to new geometry optimization?)
	//ipmt =fastTrack.GetEnvelopePhysicalVolume()/*->GetMother()*/->GetCopyNo();
	//physicalVolumeName = fastTrack.GetEnvelopePhysicalVolume()/*->GetMother()*/->GetName();
	//G4cout << "physicalVolumeName = " << physicalVolumeName << G4endl;
	//G4cout << "ipmt = " << ipmt << G4endl;
	// so we do this:
	{
		const G4VTouchable* touch = fastTrack.GetPrimaryTrack()->GetTouchable();
		int nd = touch->GetHistoryDepth();
		for(G4int id = 0; id < nd; ++id) {
			if(touch->GetVolume(id) == fastTrack.GetEnvelopePhysicalVolume()) {
				ipmt = touch->GetReplicaNumber(id + 1) + 1;
				physicalVolumeName = touch->GetVolume(id + 1)->GetName();
				//G4cout << "physicalVolumeName = " << physicalVolumeName <<
				//	G4endl;
				//G4cout << "ipmt = " << ipmt << G4endl;
				break;
			}
		}
		if (ipmt <= 0) {
			G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument,
					"could not find envelope -- where am I !?!");
		}
	}

	// get position and direction in local coordinates
	pos = fastTrack.GetPrimaryTrackLocalPosition(); // wrt glass housing coord.
	dir = fastTrack.GetPrimaryTrackLocalDirection(); // Returns unit vector.
	pol = fastTrack.GetPrimaryTrackLocalPolarization(); // Returns unit vector.
	//G4cout << "pos = " << pos << G4endl;
	//G4cout << "dir = " << dir << G4endl;
	//G4cout << "sin(theta) = " << sin(dir.theta()/rad) << G4endl;

	// get weight and time
	time = fastTrack.GetPrimaryTrack()->GetGlobalTime();  // "global" is correct
	//G4cout << "time = " << time << G4endl;
	weight = (G4int)( fastTrack.GetPrimaryTrack()->GetWeight() );

	// get n_glass, fN2, fK2, etc., for this wavelength
	energy = fastTrack.GetPrimaryTrack()->GetKineticEnergy();
	if ( energy == fPhotonEnergy ) // equal to last energy?
	{
		// use old values
		if ( fN1 == 1.0 )
			n_glass = fN3;
		else
			n_glass = fN1;
	}
	else
	{
		fPhotonEnergy = energy;
		fWavelength = twopi*hbarc / energy;
		n_glass = fRIndexGlass->Value( energy );
		fN1 = n_glass; // just in case we exit before setting fN1
		fN2 = fRIndexPhotocathode->Value( energy );
		fK2 = fKIndexPhotocathode->Value( energy );
		fN3 = 1.0;     // just in case we exit before setting fN3
		fEfficiency = fEfficiencyPhotocathode->Value( energy );
	}

	// for attenuation inside the glass
	G4double glass_abslength = fAttenuationGlass->Value( energy );
	G4double abs_limit_glass_pathlength = -log(G4UniformRand()) * glass_abslength;
	G4double glass_pathlength = 0.0;

	// initialize "whereAmI"
	if ( fastTrack.GetPrimaryTrack()->GetVolume() == fPmtInnerVacuumPhys )
		whereAmI = kInVacuum;
	else
		whereAmI = kInGlass;

	G4int iloop;
	G4int max_iloop = 100;
	for (iloop =0; iloop<max_iloop; iloop++) {
		bool exiting = false;

		if ( whereAmI == kInGlass )
		{ // in the glass
			// advance to next interface
			//G4cout << "\nIn glass, advance to next interface.\n";
			//G4cout << "pos = " << pos << G4endl;
			//G4cout << "dir = " << dir << G4endl;
			//G4cout << "sin(theta) = " << sin(dir.theta()/rad) << G4endl;
			//G4cout << "time = " << time << G4endl;
			dist1 = fPmtGlassWindowSolid->
				DistanceToOut( pos-fPmtGlassWindowPhysTranslation, dir );
			//G4cout << "fPmtGlassWindowPhysTranslation = "
			//	<< fPmtGlassWindowPhysTranslation << G4endl;
			//G4cout << "distance to out from "
			//	<< fPmtGlassWindowSolid->GetName() << " = "
			//	<< dist1/mm << "mm"
			//	<< G4endl;
			//dist = fPmtInnerVacuumSolid->DistanceToIn(
			//		pos, dir );
			dist = fPmtInnerVacuumSolid->DistanceToIn(
					pos-fPmtInnerVacuumPhysTranslation, dir );
			//G4cout << "fPmtInnerVacuumPhysTranslation = "
			//	<< fPmtInnerVacuumPhysTranslation << G4endl;
			//G4cout << "distance to in to "
			//   	<< fPmtInnerVacuumSolid->GetName()
			//	<< " = " << dist/mm << "mm" << G4endl;
			if (dist1 < dist)
			{
				// we hit the envelope outer surface, not the inner surface
				dist = dist1;
				// Is the photon headed out of the edge surfaces of the Fast
				// Simulation rectangular box volume?
				if(dir.x() != 0. && dir.y() != 0.){
					// Find minimum distance to the edges.
					G4double dist_x = 0, dist_y = 0;
					if(dir.x() > 0.)
						dist_x = fabs( (fFastSimulationVolumeExtentInX - pos.x()) / dir.x() );
					else if(dir.x() < 0.)
						dist_x = fabs( (pos.x() - (-fFastSimulationVolumeExtentInX)) / dir.x() );
					if(dir.y() > 0.)
						dist_y = fabs( (fFastSimulationVolumeExtentInY - pos.y()) / dir.y() );
					else if(dir.y() < 0.)
						dist_y = fabs( (pos.y() - (-fFastSimulationVolumeExtentInY)) / dir.y() );
					dist1 = std::min(dist_x, dist_y);
					// If it is shorter than any other distance, we need to
					// increment by this distance.
					if(dist1 < dist)
						dist = dist1;
				}
				//if ( dir.z() < 0.0 ) // headed towards equator?
				//{ // make sure we don't cross the equator
				//	dist1 = - pos.z() / dir.z(); // distance to equator
				//	if (dist1 < dist)
				//		dist = dist1;
				//}
				exiting = true;
			}
			pos += dist*dir;
			//G4cout << G4endl;
			//G4cout << "pos = " << pos << G4endl;
			time += dist*n_glass/c_light;
			//G4cout << "time = " << time << G4endl;
			glass_pathlength += dist;
			//G4cout << "dist = " << dist << G4endl;
			//G4cout << "glass_pathlength = " << glass_pathlength/mm << "mm" << G4endl;
			fN1 = n_glass;
			fN3 = 1.0;
		}
		else 
		{ // in the "pmt_inner_vacuum" vacuum
			// advance to next interface
			//G4cout << "\nIn vacuum\n";
			dist = fPmtInnerVacuumSolid->DistanceToOut( pos-fPmtInnerVacuumPhysTranslation, dir );
			if ( dist < 0.0 )
			{
				G4cerr << "MTCG4PmtOpticalModel::DoIt(): "
					<< "Warning, strangeness detected! pmt_inner_vacuum->DistanceToOut()="
					<< dist << G4endl;
				dist = 0.0;
			}
			pos += dist*dir;
			//G4cout << "pos = " << pos << G4endl;
			//G4cout << "dir = " << dir << G4endl;
			//G4cout << "sin(theta) = " << sin(dir.theta()/rad) << G4endl;
			time += dist/c_light;
			//G4cout << "time = " << time << G4endl;
			if(
					(pos.z()>fFastSimulationVolumeExtentInZBottom-fGeometricTolerance
					 &&
					 pos.z()<fFastSimulationVolumeExtentInZBottom+fGeometricTolerance)
					||
					(pos.x()>fFastSimulationVolumeExtentInX-fGeometricTolerance&&
					 pos.x()<fFastSimulationVolumeExtentInX+fGeometricTolerance)
					||
					(pos.x()>-fFastSimulationVolumeExtentInX-fGeometricTolerance&&
					 pos.x()<-fFastSimulationVolumeExtentInX+fGeometricTolerance)
					||
					(pos.y()>fFastSimulationVolumeExtentInY-fGeometricTolerance&&
					 pos.y()<fFastSimulationVolumeExtentInY+fGeometricTolerance)
					||
					(pos.y()>-fFastSimulationVolumeExtentInY-fGeometricTolerance&&
					 pos.y()<-fFastSimulationVolumeExtentInY+fGeometricTolerance)
			  )
				exiting = true;
			//if ( pos.z() < G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() ) // we're passing through the equator
			//	exiting = true;
			fN1 = 1.0;
			fN3 = n_glass;
		}

		//G4cout << "Absorption limit = " << abs_limit_glass_pathlength/mm << "mm" << G4endl;
		//G4cout << "glass pathlength = " << glass_pathlength/mm << "mm" << G4endl;

		//if photon is absorbed by glass, kill photon and break out of loop here
		if(glass_pathlength > abs_limit_glass_pathlength) 
		{ 
			//G4cout << "Photon absorbed by glass" << G4endl; // Mich's comment.
			weight = 0; //absorbed by glass, killing photon
			break; 
		}

		// if exiting, break out of loop here
		if(exiting){
			//G4cout << "Photon exiting fast simulation volume." << G4endl;
			break;
		}

		// get outward-pointing normal in local coordinates at this position
		norm = fPmtInnerVacuumSolid->
			SurfaceNormal( pos - fPmtInnerVacuumPhysTranslation );
		// reverse sign if incident from glass, so normal points into region "3"
		if ( whereAmI == kInGlass )
			norm *= -1.0; // in principle, this is more efficient than norm = -norm;

		// set fThickness and fCosTheta1
		fThickness =  fThicknessPhotocathode->Value( pos.z() );
		fCosTheta1 = dir * norm;
		if ( fCosTheta1 < 0.0 ) {
			G4cerr << "MTCG4PmtOpticalModel::DoIt(): "
				<< " The normal points the wrong way!\n"
				<< "  norm: " << norm << G4endl
				<< "  dir:  " << dir << G4endl
				<< "  fCosTheta1:  " << fCosTheta1 << G4endl
				<< "  pos:  " << pos << G4endl
				<< "  whereAmI:  " << (int)(whereAmI) << G4endl
				<< " Reversing normal!" << G4endl;
			fCosTheta1 = -fCosTheta1;
			norm = -norm;
		}

		// Now calculate coefficients
		CalculateCoefficients();

		// Calculate Transmission, Reflection, and Absorption coefficients
		G4double T,R,A,An,collection_eff;
		G4double E_s2;
		if ( fSinTheta1 > 0.0 )
		{
			E_s2 = ( pol * dir.cross( norm ) ) / fSinTheta1;
			E_s2*= E_s2;
		}
		else
			E_s2 = 0.0;
		T = fT_s * E_s2  +  fT_p * (1.0-E_s2);
		R = fR_s * E_s2  +  fR_p * (1.0-E_s2);
		A = 1.0 - (T+R);
		An = 1.0 - (fT_n+fR_n); //The absorption at normal incidence
		//G4cout << "A = " << A << G4endl;
		//G4cout << "An = " << An << G4endl;
		//G4cout << "R = " << R << G4endl;
		//G4cout << "T = " << T << G4endl;
		collection_eff = fEfficiency/An; // net QE = fEfficiency for normal inc.
		if (fEffVSRhoPhotocathode != NULL) {
			// multiply by EFFVSRHO correction -- relative efficiency vs rho_xy
			collection_eff *= fEffVSRhoPhotocathode->Value( pos.perp() );
		}

# ifdef G4DEBUG
		if (A < 0.0 || A > 1.0+1e-6 || collection_eff < 0.0 || collection_eff > 1.0+1e-6) {
			G4cerr << "MTCG4PmtOpticalModel::DoIt(): Strange coefficients!\n";
			G4cerr<<"T, R, A, An, weight: "<<T<<" "<<R<<" "<<A<<" "<<An<<" "<<weight<<G4endl;
			G4cerr<<"collection eff, std QE: "<<collection_eff<<" "<<fEfficiency<<G4endl;
			G4cerr<<"========================================================="<<G4endl;
			A = collection_eff = 0.5; // safe values???
		}
# endif

		// Now decide how many pe we make.
		// When weight == 1, probability of a pe is A*collection_eff.
		// There is a certain correlation between "a pe is made" and
		// "the track is absorbed", which is implemented correctly below for
		// the weight == 1 case, and as good as can be done for weight>1 case.
		G4double mean_N_pe = weight*A*collection_eff;
		G4double ranno_absorb = G4UniformRand();
		G4int N_pe = (G4int)( mean_N_pe + (1.0-ranno_absorb) );
		if (N_pe > 0) { // Photon creates PEs.
			//G4cout << "N_pe > 0" << G4endl;
			//G4cout << "detector = " << detector << G4endl;
			//G4cout << "detector->isActive() = " << detector->isActive() << G4endl;

			// Set flag to indicate photon was detected to user step.
			((MTCG4SteppingAction*)
			 G4RunManager::GetRunManager()->GetUserSteppingAction())->
				SetPhotonDetectedAtEndOfStep(true);

			if ( detector != NULL && detector->isActive() ) {
				// Get pixel row/column ID number.
				G4double row_id = static_cast<G4int>(
						(0.5*fPhotocathodeFaceDimension - pos.y()) /
						fPixelPitch); // Row ID decreases with Y of photon hit.
				G4double column_id = static_cast<G4int>(
						(0.5*fPhotocathodeFaceDimension + pos.x()) /
						fPixelPitch); // Column ID increases with X of hit pos.
				row_id++; column_id++; // Make sure row/column are 1~8.

				// Get actual hit pixel coordinates in world coordinate system.
				G4double pixel_x =
					floor(pos.x()/PMT_PIXEL_FACE_PITCH)*
					PMT_PIXEL_FACE_PITCH + 0.5*PMT_PIXEL_FACE_PITCH;
				pos.setX(pixel_x);
				G4double pixel_y =
					floor(pos.y()/PMT_PIXEL_FACE_PITCH)*
					PMT_PIXEL_FACE_PITCH + 0.5*PMT_PIXEL_FACE_PITCH;
				pos.setY(pixel_y);
				//G4cout << "before rot/trans x: " << pos.getX()
				//	<< ", y: " << pos.getY()
				//	<< ", z: " << pos.getZ() << G4endl;
				const G4VTouchable* touch =
				   	fastTrack.GetPrimaryTrack()->GetTouchable();
				// Get max history depth index.
				const G4int maxHistoryDepth = touch->GetHistoryDepth();
				// Get starting history depth index.
				G4int startHistoryDepth = 0;
				for (G4int iDepth = 0; iDepth <= maxHistoryDepth; ++iDepth) {
					G4VPhysicalVolume* phys = touch->GetVolume(iDepth);
					if (phys == fEnvelopePhys) {
						startHistoryDepth = iDepth; // Envelope volume is start.
						break;
					}
				}
				// Get net translation/rotation and apply to pixel starting from
				// the history depth of the Physical Envelope volume for which
				// the pixel's initial (x, y, z) coordinates are with respect
				// to.
				G4ThreeVector netTransWrtWorld(0, 0, 0);
				G4RotationMatrix netRotationWrtWorld;
				for (G4int iDepth = startHistoryDepth;
						iDepth <= maxHistoryDepth; ++iDepth)
			   	{
					G4VPhysicalVolume* physVol = touch->GetVolume(iDepth);
					//G4cout << "iDepth: " << iDepth;
					//G4cout << ", physName: " << physVol->GetName();
					//G4cout << ", trans: "
					//	<< physVol->GetTranslation().getX() << ", "
					//	<< physVol->GetTranslation().getY() << ", "
					//	<< physVol->GetTranslation().getZ() << "\n";

					// Rotate pixels according to nested nth mother rotation.
					G4RotationMatrix rot = physVol->GetObjectRotationValue();
					netRotationWrtWorld *= rot;
					pos = pos.transform(rot);

					// Translate pixels according to nested nth mother
					// translation.
					G4ThreeVector trans = physVol->GetObjectTranslation();
					netTransWrtWorld += trans;
					pos += trans;
				}
				//G4cout << "net trans.x: " << netTransWrtWorld.getX()
				//	<< ", y: " << netTransWrtWorld.getY()
				//	<< ", z: " << netTransWrtWorld.getZ() << G4endl;

				//G4cout << "ipmt = " << ipmt << G4endl;
				//G4cout << "row_id = " << row_id << G4endl;
				//G4cout << "column_id = " << column_id << G4endl;
				//G4cout << "time = " << time << G4endl;
				//G4cout << "PE is recorded" << G4endl; // Mich's comment.
				//G4cout << "after rot/trans x: " << pos.getX()
				//	<< ", y: " << pos.getY()
				//	<< ", z: " << pos.getZ() << G4endl;

				// Create photon hit.
				((MTCG4PmtSD *)detector)->SimpleHit(
					ipmt,
					(G4int)row_id,
					(G4int)column_id,
					time,
					energy,
					pos,
					dir,
					pol,
					0,
					N_pe );
			}
		}

		// Now maybe absorb the track.
		// The probability is independent of weight, and the entire
		// track is either absorbed or not as a whole.
		// This is consistent with how the track is treated in other
		// processes (absorption, G4OpBoundary, etc.), and the statistics
		// for the final number of pe detected overall is made consistent
		// by the poissonian statistics of number of tracks implemented in
		// MTCG4Scint.  (Actually, for weights > 1, the correlation between
		// number absorbed and number transmitted/reflected and subsequently
		// absorbed cannot be made as big as it should be, so the width of
		// the distribution of the total number of pe per event will be slightly
		// too large.  Only the weight =1 case is "guaranteed" to get everything
		// correct, assuming there are no bugs in the code.)
		if ( ranno_absorb < A) {
			//G4cout << "Photon absorbed." << G4endl;
			weight = 0;
			break;
		}

		// reflect or refract the unabsorbed track
		if ( G4UniformRand() < R/(R+T) )
		{ // reflect
			//G4cout << "Photon reflected." << G4endl;
			Reflect( dir, pol, norm );
		}
		else
		{ // transmit 
			//G4cout << "Photon refracted." << G4endl;
			Refract( dir, pol, norm );
			if ( whereAmI == kInGlass )
				whereAmI = kInVacuum;
			else
				whereAmI = kInGlass;
		}

	}//End iloop 

	fastStep.SetPrimaryTrackFinalPosition( pos );
	fastStep.SetPrimaryTrackFinalTime( time );
	fastStep.SetPrimaryTrackFinalMomentum( dir );
	fastStep.SetPrimaryTrackFinalPolarization( pol );
	// fastStep.SetPrimaryTrackPathLength( trackLength ); // does anyone care?
	if (weight <= 0) {
		fastStep.ProposeTrackStatus(fStopAndKill);
		if (weight < 0) {
			G4cerr << "MTCG4PmtOpticalModel::DoIt(): Logic error, weight = "
				<< weight << G4endl;
		}
	}
	else {
		// in case multiphoton has been partly absorbed and partly reflected
		fastStep.SetPrimaryTrackFinalEventBiasingWeight( weight );
	}
	if (iloop >= max_iloop) {
		G4cerr << "MTCG4PmtOpticalModel::DoIt(): Too many loops, particle trapped!"
			<< " Killing it." << G4endl;
		fastStep.ProposeTrackStatus(fStopAndKill);
	}

	return;
}


// CalculateCoefficients() method used by DoIt() above.
// *** THE PHYSICS, AT LAST!!! :-) ***
// Correct formalism implemented by Dario Motta (CEA-Saclay) 23 Feb 2005

	void
MTCG4PmtOpticalModel::CalculateCoefficients()
	// calculate and set fR_s, etc.
{
	//G4cout << "Calculating coefficients with luxlevel = " << fLuxLevel << G4endl;
	if (fLuxLevel <= 0) {
		// no reflection or transmission, just a black "light bucket"
		// 100% absorption, and QE will be renormalized later
		fR_s = fR_p = 0.0;
		fT_s = fT_p = 0.0;
		fR_n = 0.0;
		fT_n = 0.0;
		return;
	}
	else if (fLuxLevel == 1) {
		// this is what was calculated before, when we had no good defaults
		// for cathode thickness and complex rindex
		// set normal incidence coefficients: 50/50 refl/transm if not absorb.
		fR_n = fT_n = 0.5*(1.0 - fEfficiency);
		// set sines and cosines
		fSinTheta1 = sqrt(1.0-fCosTheta1*fCosTheta1);
		fSinTheta3 = fN1/fN3 * fSinTheta1;
		if (fSinTheta3 > 1.0) {
			// total non-transmission -- what to do?
			// total reflection or absorption
			fCosTheta3 = 0.0;
			fR_s = fR_p = 1.0 - fEfficiency;
			fT_s = fT_p = 0.0;
			return;
		}
		fCosTheta3 = sqrt(1.0-fSinTheta3*fSinTheta3);
		fR_s = fR_p = fR_n;
		fT_s = fT_p = fT_n;
		return;
	}  
	// else...

	// declare the prototypes of some useful functions
	G4complex carcsin(G4complex theta); //complex sin^-1
	G4complex gfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj);
	G4complex rfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj);
	G4complex trfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj,
			G4complex tk);

	// declare some useful constants
	G4complex _n2comp(fN2,-fK2); //complex photocathode refractive index
	G4complex eta =twopi*_n2comp*fThickness/fWavelength;
	G4complex zi(0.,1.); //imaginary unit

	// declare local variables

	G4complex theta1,theta2,theta3,delta;//geometric parameters
	G4complex r12,r23,t12,t21,t23;//reflection- and transmission-related terms
	G4complex ampr,ampt; //relfection and transmission amplitudes

	// first set sines and cosines
	fSinTheta1 = sqrt(1.0-fCosTheta1*fCosTheta1);
	fSinTheta3 = fN1/fN3 * fSinTheta1;
	if (fSinTheta3 > 1.0) {
		// total non-transmission -- what to do???
		// these variables only used to decide refracted track direction,
		// so doing the following should be okay:
		fSinTheta3 = 1.0;
	}
	fCosTheta3 = sqrt(1.0-fSinTheta3*fSinTheta3);

	// Determine all angles
	theta1 =asin(fSinTheta1);//incidence angle
	theta2 =carcsin((fN1/_n2comp)*fSinTheta1);//complex angle in the photocathode
	theta3 =carcsin((_n2comp/fN3)*sin(theta2));//angle of refraction into vacuum
	if (imag(theta3)<0.) theta3 =conj(theta3);//needed! (sign ambiguity arcsin)

	delta =eta*cos(theta2);

	//Calculation for the s-polarization

	r12 =rfunc(fN1,_n2comp,theta1,theta2);
	r23 =rfunc(_n2comp,fN3,theta2,theta3);
	t12 =trfunc(fN1,_n2comp,theta1,theta1,theta2);
	t21 =trfunc(_n2comp,fN1,theta2,theta2,theta1);
	t23 =trfunc(_n2comp,fN3,theta2,theta2,theta3);

	ampr =r12+(t12*t21*r23*exp(-2.*zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));
	ampt =(t12*t23*exp(-zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));

	//And finally...!
	fR_s =real(ampr*conj(ampr));
	fT_s =real(gfunc(fN3,fN1,theta3,theta1)*ampt*conj(ampt));

	//Calculation for the p-polarization

	r12 =rfunc(fN1,_n2comp,theta2,theta1);
	r23 =rfunc(_n2comp,fN3,theta3,theta2);
	t12 =trfunc(fN1,_n2comp,theta1,theta2,theta1);
	t21 =trfunc(_n2comp,fN1,theta2,theta1,theta2);
	t23 =trfunc(_n2comp,fN3,theta2,theta3,theta2);

	ampr =r12+(t12*t21*r23*exp(-2.*zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));
	ampt =(t12*t23*exp(-zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));

	//And finally...!
	fR_p =real(ampr*conj(ampr));
	fT_p =real(gfunc(fN3,fN1,theta3,theta1)*ampt*conj(ampt));

	//Now calculate the reference values at normal incidence (to scale QE)

	delta = eta;
	//Calculation for both polarization (the same at normal incidence)
	r12 =rfunc(fN1,_n2comp,0.,0.);
	r23 =rfunc(_n2comp,fN3,0.,0.);
	t12 =trfunc(fN1,_n2comp,0.,0.,0.);
	t21 =trfunc(_n2comp,fN1,0.,0.,0.);
	t23 =trfunc(_n2comp,fN3,0.,0.,0.);

	ampr =r12+(t12*t21*r23*exp(-2.*zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));
	ampt =(t12*t23*exp(-zi*delta))/(1.+r12*r23*exp(-2.*zi*delta));

	//And finally...!
	fR_n =real(ampr*conj(ampr));
	fT_n =real(gfunc(fN3,fN1,0.,0.)*ampt*conj(ampt));


# ifdef G4DEBUG
	if (fVerbosity >= 10) {
		G4cout<<"=> lam, n1, n2: "<<fWavelength/nanometer<<" "<<fN1<<" "<<_n2comp<<G4endl;      
		G4cout<<"=> Angles: "<<real(theta1)/degree<<" "<<theta2/degree<<" "
			<<theta3/degree<<G4endl;      
		G4cout<<"Rper, Rpar, Tper, Tpar: "<<fR_s<<" "<<fR_p<<" "<<fT_s<<" "<<fT_p;
		G4cout<<"\nRn, Tn : "<<fR_n<<" "<<fT_n;
		G4cout<<"\n-------------------------------------------------------"<<G4endl;
	}
# endif

}

G4complex carcsin(G4complex theta) //complex sin^-1 
{
	G4complex zi(0.,1.);
	G4complex value =(1./zi)*(log(zi*theta+sqrt(1.-theta*theta)));
	return value; 
}

G4complex gfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj)
{
	G4complex value =(ni*cos(ti))/(nj*cos(tj));
	return value;
}

G4complex rfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj)
{
	G4complex value =(ni*cos(ti)-nj*cos(tj))/(ni*cos(ti)+nj*cos(tj));
	return value;
}

G4complex trfunc(G4complex ni, G4complex nj, G4complex ti, G4complex tj,
		G4complex tk)
{
	G4complex value =2.*(ni*cos(ti))/(ni*cos(tj)+nj*cos(tk));
	return value;
}


// Reflect() method, used by DoIt()
	void
MTCG4PmtOpticalModel::Reflect(G4ThreeVector &dir,
		G4ThreeVector &pol,
		G4ThreeVector &norm)
{
	dir -= 2.*(dir*norm)*norm;
	pol -= 2.*(pol*norm)*norm;
}

// Refract() method, used by DoIt()
	void
MTCG4PmtOpticalModel::Refract(G4ThreeVector &dir,
		G4ThreeVector &pol,
		G4ThreeVector &norm)
{
	dir = (fCosTheta3 - fCosTheta1*fN1/fN3)*norm + (fN1/fN3)*dir;
	pol = (pol-(pol*dir)*dir).unit();
}


// user command handling functions
	void
MTCG4PmtOpticalModel::SetNewValue(G4UIcommand * cmd, G4String newValue)
{
	G4String cmdName = cmd->GetCommandName();
	if (cmdName == "verbose") {
		fVerbosity = strtol((const char *)newValue, NULL, 0);
	}
	else if (cmdName == "luxlevel") {
		fLuxLevel = strtol((const char *)newValue, NULL, 0);
	}
	else {
		G4Exception( "MTCG4PmtOpticalModel.cc", "1", FatalErrorInArgument,
				(cmdName + ": " + newValue + " option not found.").c_str() );
	}
	return;
}

	G4String
MTCG4PmtOpticalModel::GetCurrentValue(G4UIcommand * cmd)
{
	G4String cmdName = cmd->GetCommandName();
	if (cmdName == "verbose") {
		char outbuff[64];
		sprintf(outbuff, "%d", fVerbosity);
		return G4String(outbuff);
	}
	else if (cmdName == "luxlevel") {
		char outbuff[64];
		sprintf(outbuff, "%d", fLuxLevel);
		return G4String(outbuff);
	}
	else {
		return (cmdName+" is not a valid PMTOpticalModel command");
	}
}


// function added: DR 092704
//  (see include file)
// code extracted from constructor
	void
MTCG4PmtOpticalModel::InitMPVs(G4VPhysicalVolume* envelope_phys)
{
	// get material properties vectors
	// ... material properties of glass
	G4LogicalVolume* envelope_log = envelope_phys->GetLogicalVolume();
	G4MaterialPropertiesTable* glass_pt =
		envelope_log->GetMaterial()->GetMaterialPropertiesTable();
	if (glass_pt == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "glass lacks a properties table!");

	fRIndexGlass = glass_pt->GetProperty("RINDEX");
	if (fRIndexGlass == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "glass does not have RINDEX!");

	fAttenuationGlass = glass_pt->GetProperty("ABSLENGTH");
	if (fAttenuationGlass == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "glass does not have ABSLENGTH!");

	// ... material properties of photocathode (first get photocathode surface)
	G4LogicalBorderSurface* pc_log_surface =
		G4LogicalBorderSurface::GetSurface(fPmtGlassWindowPhys/*envelope_phys*/, fPmtInnerVacuumPhys);
	if (pc_log_surface == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "no photocathode logical surface!?!");

	//#if (G4VERSIONCODE < 40600 )
	//  // it used to be so easy...
	//  G4OpticalSurface* pc_opsurf = pc_log_surface->GetOpticalSurface();
	//#else
	// G4LogicalSurface::GetOpticalSurface() function was eliminated from Geant4
	// version 6.  G4OpticalSurface now inherits from G4SurfaceProperty.
	// We have to trust that the G4SurfaceProperty returned by
	// pc_log_surface->GetSurfaceProperty() will be a G4OpticalSurface,
	// and cast the pointer accordingly by faith alone!
	G4OpticalSurface* pc_opsurf = (G4OpticalSurface*)
		( pc_log_surface->GetSurfaceProperty() );
	//#endif

	if (pc_opsurf == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "no photocathode optical surface!?!");
	G4MaterialPropertiesTable* pc_pt =
		pc_opsurf->GetMaterialPropertiesTable();
	if (pc_pt == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "photocathode lacks a properties table!");

	fRIndexPhotocathode = pc_pt->GetProperty("RINDEX");
	if (fRIndexPhotocathode == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "photocathode does not have RINDEX!");

	fKIndexPhotocathode = pc_pt->GetProperty("KINDEX");
	if (fKIndexPhotocathode == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "photocathode does not have KINDEX!");

	fThicknessPhotocathode = pc_pt->GetProperty("THICKNESS");
	if (fThicknessPhotocathode == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "photocathode does not have THICKNESS!");

	fEfficiencyPhotocathode = pc_pt->GetProperty("EFFICIENCY");
	if (fEfficiencyPhotocathode == NULL)
		G4Exception("MTCG4PmtOpticalModel", "", FatalErrorInArgument, "photocathode does not have EFFICIENCY!");

	fEffVSRhoPhotocathode = pc_pt->GetProperty("EFFVSRHO");
	// it's okay for the above to be null
}
