/** @file MTCG4PmtOpticalModel.hh
    Defines a FastSimulationModel class for handling optical photon
    interactions with PMT: partial reflection, transmission, absorption,
    and hit generation.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4PmtOpticalModel.hh,v 1.5 2006/05/09 15:47:52 kamland0 Exp $
    
    @author Glenn Horton-Smith, March 20, 2001.
    @author Dario Motta, Feb. 23 2005: Formalism light interaction with photocathode.
*/

#ifndef __MTCG4PmtOpticalModel_hh__
#define __MTCG4PmtOpticalModel_hh__

#include "G4VFastSimulationModel.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;

class MTCG4PmtOpticalModel : public G4VFastSimulationModel,
			    public G4UImessenger
{
public:
	//-------------------------
	// Constructor, destructor
	//-------------------------
	MTCG4PmtOpticalModel (G4String, G4VPhysicalVolume*);
	// Note: There is no MTCG4PmtOpticalModel(G4String) constructor.
	~MTCG4PmtOpticalModel ();

	//------------------------------
	// Virtual methods of the base
	// class to be coded by the user
	//------------------------------

	G4bool IsApplicable(const G4ParticleDefinition&);
	G4bool ModelTrigger(const G4FastTrack &);
	void DoIt(const G4FastTrack&, G4FastStep&);

	// following two methods are for G4UImessenger
	void SetNewValue(G4UIcommand * command,G4String newValues);
	G4String GetCurrentValue(G4UIcommand * command);

	//-------------------
	// Utility functions
	//-------------------                                                                                
	// added DR 092704 -- code extracted from constructor
	// This function is only called in two places: from the constructor;
	//  and from TKLDetectorConstruction::GetMaterialProperties(), during
	//  which the old MPVs are no longer necessarily valid, so that new
	//  ones must be fetched.
	void InitMPVs(G4VPhysicalVolume*);

private:
	// material property vector pointers, initialized in constructor, 
	// so we don't have to look them up every time DoIt is called.
	G4MaterialPropertyVector * fRIndexGlass;        // function of photon energy
	G4MaterialPropertyVector * fAttenuationGlass; // function of wavelength
	G4MaterialPropertyVector * fRIndexPhotocathode; // function of photon energy
	G4MaterialPropertyVector * fKIndexPhotocathode; // n~ = n+ik, as in M.D.Lay
	G4MaterialPropertyVector * fEfficiencyPhotocathode; // necessary?
	G4MaterialPropertyVector * fThicknessPhotocathode; // function of Z (mm)
	G4MaterialPropertyVector * fEffVSRhoPhotocathode;  // f'n of sqrt(X^2+Y^2)
	// interior solid (vacuum), also initialized in constructor,
	// so we don't have to look it up each time DoIt is called.
	// Note it is implicitly assumed everywhere in the code that this solid
	// is vacuum-filled and placed in the body with no offset or rotation.
	G4VSolid * fPmtInnerVacuumSolid;
	G4VPhysicalVolume * fPmtInnerVacuumPhys;
	G4VSolid * fPmtGlassWindowSolid;
	G4VPhysicalVolume * fPmtGlassWindowPhys;

	// "luxury level" -- how fancy should the optical model be?
	static G4int fLuxLevel;

	// verbose level -- how verbose to be (diagnostics and such)
	static G4int fVerbosity;

	// directory for commands
	static G4UIdirectory* fgCmdDir;

	// "current values" of many parameters, for efficiency
	// [I claim it is quicker to access these than to
	// push them on the stack when calling CalculateCoefficients, Reflect, etc.]
	// The following are set by DoIt() prior to any CalculateCoefficients()
	// call.
	G4double fPhotonEnergy; // energy of current photon
	G4double fWavelength;   // wavelength of current photon
	G4double fN1;           // index of refraction of curr. medium at wavelength
	G4double fN2, fK2;      // index of refraction of photocathode at wavelength
	G4double fN3;           // index of refraction of far side at wavelength
	G4double fEfficiency;   // efficiency of photocathode at wavelength (?)
	G4double fThickness;    // thickness of photocathode at current position
	G4double fCosTheta1;    // cosine of angle of incidence
	// The following are set by CalculateCoefficients()
	// and used by DoIt(), Refract(), and Reflect():
	G4double fSinTheta1;    // sine of angle of incidence
	G4double fSinTheta3;    // sine of angle of refraction
	G4double fCosTheta3;    // cosine of angle of refraction
	G4double fR_s;          // reflection coefficient for s-polarized light
	G4double fT_s;          // transmission coefficient for s-polarized light
	G4double fR_p;          // reflection coefficient for p-polarized light
	G4double fT_p;          // transmission coefficient for p-polarized light
	G4double fR_n;          // reference for fR_s/p at normal incidence
	G4double fT_n;          // reference for fT_s/p at normal incidence

	// private methods
	void CalculateCoefficients(); // calculate and set fR_s, etc.
	void Reflect(G4ThreeVector &dir, G4ThreeVector &pol, G4ThreeVector &norm);
	void Refract(G4ThreeVector &dir, G4ThreeVector &pol, G4ThreeVector &norm);

	// Geometrical boundary values for volume where the fast simulation occurs.
	G4double fPhotocathodeFaceDimension;
	G4double fPixelPitch;
	G4double fFastSimulationVolumeExtentInX;
	G4double fFastSimulationVolumeExtentInY;
	G4double fFastSimulationVolumeExtentInZBottom;
	G4double fGeometricTolerance;

	// Pointers to Envelope volumes.
	G4VPhysicalVolume* fEnvelopePhys;
	G4LogicalVolume* fEnvelopeLog;

	// Translation vectors of physical volumes wrt the fast simulation envelope
	// volume. In our case this is the physical volume named
	// "fPmtGlassHousingPhysical".
	// Translation vector of physical volume "fPmtGlassWindowPhys".
	G4ThreeVector fPmtGlassWindowPhysTranslation;
	// Translation vector of physical volume "fPmtInnerVacuumPhys".
	G4ThreeVector fPmtInnerVacuumPhysTranslation;
};

#endif
