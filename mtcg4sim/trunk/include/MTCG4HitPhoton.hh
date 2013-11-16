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
  MTCG4HitPhoton() { }

  void SetPMTID(G4int id) { fPMTID= id; }
	void SetAnodeRow(G4int row) { fAnodeRow = row; };
	void SetAnodeColumn(G4int column) { fAnodeColumn = column; }
  void SetTime(G4double t) { fTime= t; }
  void SetKineticEnergy(G4double KE);
  void SetWavelength(G4double wl);
  void SetPosition(G4double x, G4double y, G4double z);
  void SetMomentum(G4double x, G4double y, G4double z);
  void SetPolarization(G4double x, G4double y, G4double z);
  void SetCount(G4int count) { fCount= count; }
  void AddCount(G4int dcount) { fCount+= dcount; }

  G4int GetPMTID() const { return fPMTID; }
	G4int GetAnodeRow() const { return fAnodeRow; }
	G4int GetAnodeColumn() const { return fAnodeColumn; }
  G4double GetTime() const { return fTime; }
  G4double GetKineticEnergy() const;
  G4double GetWavelength() const;
  template <class T> inline void GetPosition(T &x, T &y, T &z) const;
  template <class T> inline void GetMomentum(T &x, T &y, T &z) const;
  template <class T> inline void GetPolarization(T &x, T &y, T &z) const;
  G4int GetCount() const { return fCount; }
  void Print(std::ostream &) const;
  
private:
  G4double fTime;							/// time of hit 
  G4int fPMTID;								/// ID number of PMT the HitPhoton hit
	G4int fAnodeRow;									/// Anode row number
	G4int fAnodeColumn;							/// Anode column number
  G4double fKE;								/// kinetic energy 
  G4double fPosition[3];			/// x,y,z components of position
  G4double fMomentum[3];			/// x,y,z components of momentum (normalized?)
  G4double fPolarization[3];	/// x,y,z components of polarization
  G4int fCount;								/// count of photons, often 1
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
