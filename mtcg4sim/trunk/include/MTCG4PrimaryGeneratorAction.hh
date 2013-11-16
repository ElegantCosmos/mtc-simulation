//
// MTCG4PrimaryGeneratorAction.hh
//

#ifndef MTCG4_PRIMARY_GENERATOR_ACTION_HH
#define MTCG4_PRIMARY_GENERATOR_ACTION_HH 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class MTCG4PrimaryGeneratorMessenger;
class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;

class MTCG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	MTCG4PrimaryGeneratorAction();
	~MTCG4PrimaryGeneratorAction();

public:
	void SetInputPath(G4String path)
	{ fInputPath = path; }
	inline G4String GetInputPath()
	{ return fInputPath; }
	void SetIncomingNeutrinoDirection(G4String neutrinoDirection)
	{ fNuDirectionDescription = neutrinoDirection; }
	inline G4String GetIncomingNeutrinoDirection()
	{ return fNuDirectionDescription; }
	void SetInverseBetaDecayInteractionVertex(G4String vertex)
	{ fInverseBetaDecayInteractionVertex = vertex; }
	inline G4String GetInverseBetaDecayInteractionVertex()
	{ return fInverseBetaDecayInteractionVertex; }
	void SetFlagForReadingInPrimaryParticleDataFile(G4bool flag)
	{ fFlagForReadingInPrimaryParticleDataFile = flag; }
	inline G4bool GetFlagForReadingInPrimaryParticleDataFile()
	{ return fFlagForReadingInPrimaryParticleDataFile; }
	void SetInverseBetaDecayDaughterParticleKineticEnergy(G4double energy)
	{ fInverseBetaDecayDaughterParticleKineticEnergy = energy; }
	inline G4double GetInverseBetaDecayDaughterParticleKineticEnergy()
	{ return fInverseBetaDecayDaughterParticleKineticEnergy; }
	void SetFlagForShootingPrimaryPositron(G4bool flag)
	{ fPrimaryPositronIsShot = flag; }
	inline G4bool GetFlagForShootingPrimaryPositron()
	{ return fPrimaryPositronIsShot; }
	void SetFlagForShootingPrimaryNeutron(G4bool flag)
	{ fPrimaryNeutronIsShot = flag; }
	inline G4bool GetFlagForShootingPrimaryNeutron()
	{ return fPrimaryNeutronIsShot; }
	void SetPrimaryDataFileName(G4String name)
	{ fPrimaryDataFileName = name; }
	void SetFlagForShootingParticleGun(G4bool flag)
	{ fParticleGunIsShot = flag; }
	inline G4bool GetFlagForShootingParticleGun()
	{ return fParticleGunIsShot; }
	void SetFlagForShootingGeneralParticleSource(G4bool flag)
	{ fGeneralParticleSourceIsShot = flag; }
	inline G4bool GetFlagForShootingGeneralParticleSource()
	{ return fGeneralParticleSourceIsShot; }
	void SetFlagForShootingLaser(G4bool flag)
	{ fLaserIsShot = flag; }
	inline G4bool GetFlagForShootingLaser()
	{ return fLaserIsShot; }
	void SetFlagForShootingCosmicRayMuon(G4bool flag)
	{ fCosmicRayMuonIsShot = flag; }
	inline G4bool GetFlagForShootingCosmicRayMuon()
	{ return fCosmicRayMuonIsShot; }

	void GeneratePrimaries(G4Event*);
	void GenerateFilesAndHeadersForRun();
	void GenerateFilesAndHeadersForEvent(G4Event*);
	void GenerateIBDPrimaries(G4Event*);
	void GenerateCosmicRayMuons(G4Event*);

private:
	// Parameters.
	G4bool		fInputFileIsOpened;
	G4String	fInputPath;
	G4String	fPrimaryDataFileName;
	G4String	fNuDirectionDescription;
	G4String	fInverseBetaDecayInteractionVertex;
	G4bool		fFlagForReadingInPrimaryParticleDataFile;
	G4double	fInverseBetaDecayDaughterParticleKineticEnergy;
	G4bool		fPrimaryPositronIsShot;
	G4bool		fPrimaryNeutronIsShot;
	G4bool		fParticleGunIsShot;
	G4bool		fGeneralParticleSourceIsShot;
	G4bool		fLaserIsShot;
	G4bool		fCosmicRayMuonIsShot;
	//G4bool		fPrimaryElectronAntiNeutrinoIsShot;

private:
	MTCG4PrimaryGeneratorMessenger	*fMessenger;

	G4ParticleGun	*fParticleGun;
	G4GeneralParticleSource	*fGeneralParticleSource;

	std::ifstream	*fPrimaryParticleDataInput;
	G4ParticleGun	*fPrimaryNeutronGun;
	G4ParticleGun	*fPrimaryPositronGun;
	G4ParticleGun	*fPrimaryLaserGun;
	G4ParticleGun	*fPrimaryCosmicRayMuonGun;
	//G4ParticleGun *fPrimaryElectronAntiNuGun;

private:
	G4double fGammaEnergy;
};

#endif
