#ifndef __MTCG4HitPhoton_hh__
#define __MTCG4HitPhoton_hh__
/** @file MTCG4HitPhoton.hh
  Declares MTCG4HitPhoton class and helper functions.

  This file is part of the GenericLAND software library.
  $Id: MTCG4HitPhoton.hh,v 1.1 2005/01/21 22:04:10 kamland0 Exp $

  @author Glenn Horton-Smith
  */

#include <iostream>
#include "globals.hh"

/** MTCG4HitPhoton stores information about a photon that makes a
  photoelectron in a PMT.  With count>1, it records multiple
  p.e. made at the same time at a PMT.

  The general contract for MTCG4HitPhoton is as follows:
  - remember PMT ID, Time, position, KE, momentum, polarization, and count
  - provide Get/Set for all of the above, plus AddCount for count
  - initialize count=1, time & id = some invalid value

  This is almost the same "general contract" that was implemented
  for KLG4sim's KLHitPhoton by O. Tajima and G. Horton-Smith, but
  the code was rewritten for GLG4sim in December 2004.

  @author Glenn Horton-Smith
  */

class MTCG4HitPhoton {
public:
	MTCG4HitPhoton();
	~MTCG4HitPhoton() {;}

	inline void SetPMTID(G4int id) { fPMTID= id; }
	inline void SetAnodeRow(G4int row) { fAnodeRow = row; };
	inline void SetAnodeColumn(G4int column) { fAnodeColumn = column; }
	inline void SetTime(G4double t) { fTime= t; }
	/// set kinetic energy and wavelength of photon.
	inline void SetKineticEnergy(G4double KE) { fKE= KE; }
	inline void AddKineticEnergy(G4double KE) { fKE += KE; } // Multiphoton hit.
	/// set wavelength and kinetic energy of photon
	inline void SetWavelength(G4double wl) { fKE= 2*pi*hbarc/wl; }
	inline void SetPosition(G4double x, G4double y, G4double z)
	{ fPosition[0]= x; fPosition[1]= y; fPosition[2]= z; }
	inline void SetMomentum(G4double x, G4double y, G4double z)
	{ fMomentum[0]= x; fMomentum[1]= y; fMomentum[2]= z; }
	inline void SetPolarization(G4double x, G4double y, G4double z)
	{ fPolarization[0]= x; fPolarization[1]= y; fPolarization[2]= z; }
	inline void SetCount(G4int count) { fCount= count; }
	inline void AddCount(G4int dcount) { fCount+= dcount; } // Multiphoton hit.

	inline G4int GetPMTID() const { return fPMTID; }
	inline G4int GetAnodeRow() const { return fAnodeRow; }
	inline G4int GetAnodeColumn() const { return fAnodeColumn; }
	inline G4double GetTime() const { return fTime; }
	inline G4double GetKineticEnergy() const { return fKE; }
	inline G4double GetWavelength() const { return 2*pi*hbarc/fKE; }

	template <class T> inline void GetPosition(T &x, T &y, T &z) const;
	template <class T> inline void GetMomentum(T &x, T &y, T &z) const;
	template <class T> inline void GetPolarization(T &x, T &y, T &z) const;
	inline G4int GetCount() const { return fCount; }
	void Print(std::ostream &) const;

private:
	G4double	fTime;				/// time of hit 
	G4int		fPMTID;				/// ID number of PMT the HitPhoton hit
	G4int		fAnodeRow;			/// Anode row number 1~8.
	G4int		fAnodeColumn;		/// Anode column number 1~8.
	G4double	fKE;				/// kinetic energy 
	G4double	fPosition[3];		/// x,y,z components of position
	G4double	fMomentum[3];	/// x,y,z components of momentum (normalized?)
	G4double	fPolarization[3];	/// x,y,z components of polarization
	G4int		fCount;				/// count of photons, often 1
};

template <class T> inline void 
MTCG4HitPhoton::GetPosition(T &x, T &y, T &z) const {
	x= fPosition[0];
	y= fPosition[1];
	z= fPosition[2];
}

template <class T> inline void 
MTCG4HitPhoton::GetMomentum(T &x, T &y, T &z) const {
	x= fMomentum[0];
	y= fMomentum[1];
	z= fMomentum[2];
}

template <class T> inline void 
MTCG4HitPhoton::GetPolarization(T &x, T &y, T &z) const {
	x= fPolarization[0];
	y= fPolarization[1];
	z= fPolarization[2];
}


/** comparison function for sorting MTCG4HitPhoton pointers
*/
	inline bool
Compare_HitPhotonPtr_TimeAscending(const MTCG4HitPhoton *a,
		const MTCG4HitPhoton *b)
{
	return a->GetTime() < b->GetTime();
}

#endif // __MTCG4HitPhoton_hh__
