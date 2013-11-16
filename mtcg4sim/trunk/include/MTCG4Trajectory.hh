#ifndef MTCG4Trajectory_h
#define MTCG4Trajectory_h 1

#include "G4VTrajectory.hh"
#include "G4Trajectory.hh"
#include "G4Allocator.hh"
#include <cstdlib>
#include "G4ThreeVector.hh"
#include "G4ios.hh"     
#include <vector>
#include "globals.hh" 
#include "G4ParticleDefinition.hh" 
#include "G4TrajectoryPoint.hh"   
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ProcessType.hh"

class	G4Polyline;

typedef std::vector<G4VTrajectoryPoint*> MTCG4TrajectoryPointContainer;

class MTCG4Trajectory : public G4Trajectory
{
	public:
		MTCG4Trajectory();
		MTCG4Trajectory(const G4Track* aTrack);
		MTCG4Trajectory(MTCG4Trajectory &);
		virtual ~MTCG4Trajectory();

		inline void* operator new(size_t);
		inline void  operator delete(void*);
		inline int operator == (const MTCG4Trajectory& right) const
		{return (this==&right);} 

		inline G4int GetTrackID() const
		{ return fTrackID; }
		inline G4int GetParentID() const
		{ return fParentID; }
		inline G4String GetParticleName() const
		{ return ParticleName; }
		inline G4double GetCharge() const
		{ return PDGCharge; }
		inline G4int GetPDGEncoding() const
		{ return PDGEncoding; }
		inline G4String GetCreatorProcessName() const
		{ return fCreatorProcessName; }
		inline G4ProcessType GetCreatorProcessType() const
		{ return fCreatorProcessType; }
		inline G4int GetCreatorProcessSubType() const
		{ return fCreatorProcessSubType; }
		inline const G4ThreeVector& GetMomentum() const
		{ return momentum; }
		inline const G4ThreeVector& GetVertexPosition() const
		{ return vertexPosition; }
		inline G4double GetGlobalTime() const
		{ return globalTime; }
		virtual int GetPointEntries() const
		{ return positionRecord->size(); }
		virtual G4VTrajectoryPoint* GetPoint(G4int i) const 
		{ return (*positionRecord)[i]; }

		virtual void ShowTrajectory(std::ostream& os = G4cout) const;
		virtual void DrawTrajectory(G4int i_mode=0) const;
		virtual void AppendStep(const G4Step* aStep);
		virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

		G4ParticleDefinition* GetParticleDefinition();

	private:
		MTCG4TrajectoryPointContainer* positionRecord;
		G4int					fTrackID;
		G4int                	fParentID;
		G4ParticleDefinition*	fpParticleDefinition;
		G4String			 	fCreatorProcessName;
		G4ProcessType		 	fCreatorProcessType;
		G4int				 	fCreatorProcessSubType;
		G4String             	ParticleName;
		G4double             	PDGCharge;
		G4int                	PDGEncoding;
		G4ThreeVector        	momentum;
		G4ThreeVector        	vertexPosition;
		G4double             	globalTime;

};

extern G4Allocator<MTCG4Trajectory> myTrajectoryAllocator;

inline void* MTCG4Trajectory::operator new(size_t)
{
	void* aTrajectory;
	aTrajectory = (void*)myTrajectoryAllocator.MallocSingle();
	return aTrajectory;
}

inline void MTCG4Trajectory::operator delete(void* aTrajectory)
{
	myTrajectoryAllocator.FreeSingle((MTCG4Trajectory*)aTrajectory);
}
//#include "G4VTrajectory.hh"
//#include "G4Allocator.hh"
//#include <cstdlib>
//#include "G4ThreeVector.hh"
//#include "G4ios.hh"     
//#include <vector>
//#include "globals.hh" 
//#include "G4ParticleDefinition.hh" 
//#include "G4TrajectoryPoint.hh"   
//#include "G4Track.hh"
//#include "G4Step.hh"
//
//class G4Polyline;
//
//typedef std::vector<G4VTrajectoryPoint*> MTCG4TrajectoryPointContainer;
//
//class MTCG4Trajectory : public G4VTrajectory
//{
//	public:
//	MTCG4Trajectory();
//	MTCG4Trajectory(const G4Track* aTrack);
//	MTCG4Trajectory(MTCG4Trajectory &);
//	virtual ~MTCG4Trajectory();
//
//	inline void* operator new(size_t);
//	inline void  operator delete(void*);
//	inline int operator == (const MTCG4Trajectory& right) const
//	{return (this==&right);} 
//
//	inline G4int GetTrackID() const
//	{ return fTrackID; }
//	inline G4int GetParentID() const
//	{ return fParentID; }
//	inline G4String GetParticleName() const
//	{ return ParticleName; }
//	inline G4double GetCharge() const
//	{ return PDGCharge; }
//	inline G4int GetPDGEncoding() const
//	{ return PDGEncoding; }
//	inline const G4ThreeVector& GetMomentum() const
//	{ return momentum; }
//	inline const G4ThreeVector& GetVertexPosition() const
//	{ return vertexPosition; }
//	inline G4double GetGlobalTime() const
//	{ return globalTime; }
//	virtual G4int GetPointEntries() const
//	{ return positionRecord->size(); }
//	virtual G4VTrajectoryPoint* GetPoint(G4int i) const 
//	{ return (*positionRecord)[i]; }
//
//	inline G4ThreeVector GetInitialMomentum() const
//	{ return G4ThreeVector(0, 0, 0); }
//
//	virtual void ShowTrajectory(std::ostream& os = G4cout) const;
//	virtual void DrawTrajectory(G4int i_mode = 0) const;
//	virtual void AppendStep(const G4Step* aStep);
//	virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);
//
//	G4ParticleDefinition* GetParticleDefinition();
//
//	private:
//	MTCG4TrajectoryPointContainer* positionRecord;
//	G4int                        fTrackID;
//	G4int                        fParentID;
//	G4ParticleDefinition*        fpParticleDefinition;
//	G4String                     ParticleName;
//	G4double                     PDGCharge;
//	G4int                        PDGEncoding;
//	G4ThreeVector                momentum;
//	G4ThreeVector                vertexPosition;
//	G4double                     globalTime;
//
//};
//
//extern G4Allocator<MTCG4Trajectory> myTrajectoryAllocator;
//
//inline void* MTCG4Trajectory::operator new(size_t)
//{
//	void* aTrajectory;
//	aTrajectory = (void*)myTrajectoryAllocator.MallocSingle();
//	return aTrajectory;
//}
//
//inline void MTCG4Trajectory::operator delete(void* aTrajectory)
//{
//	myTrajectoryAllocator.FreeSingle((MTCG4Trajectory*)aTrajectory);
//}

#endif
