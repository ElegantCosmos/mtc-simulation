/** @file MTCG4HitPmt.cc
    For MTCG4HitPmt class.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4HitPmt.cc,v 1.2 2005/07/12 20:38:57 kamland0 Exp $

    @author Glenn Horton-Smith, December 2004
*/

#include "MTCG4HitPmt.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

#include <algorithm>
#include <G4ios.hh>

#ifdef G4DEBUG
#define IFDEBUG(A) A
#else
#define IFDEBUG(A)
#endif

/// controls when to start trying to merge HitPhotons
const size_t MTCG4HitPmt::kApproxMaxIndividualHitPhotonsPerPMT = 100;

/// hit merging window in ns
const G4double MTCG4HitPmt::kMergeTime= 0.1*ns; // Timing resolution of MTC.

MTCG4HitPmt::MTCG4HitPmt(G4int ID)
{
  fID= ID;
}

MTCG4HitPmt::~MTCG4HitPmt()
{
  Clear();
}

/** clear out AND DELETE HitPhotons that were detected, resetting this
    HitPMT to have no HitPhotons */
void MTCG4HitPmt::Clear()
{
  for (size_t i=0; i<fPhotons.size(); i++)
    delete fPhotons[i];
  fPhotons.clear();
}

/** Add HitPhoton, or try to merge with another HitPhoton when number
    of HitPhotons is bigger than kApproxMaxIndividualHitPhotonsPerPMT.
    
    If number of HitPhotons stored is less than
    kApproxMaxIndividualHitPhotonsPerPMT, just add it to the list.
    
    If number of HitPhotons stored is greater than or equal to
    kApproxMaxIndividualHitPhotonsPerPMT, sort the vector, then find
    the HitPhoton that immediately preceeds it in sorted vector.  If
    this HitPhoton's time is within kMergeTime of the preceeding
    HitPhoton, then add this HitPhoton's count to that closest
    preceeding HitPhoton.  Otherwise, look at following HitPhoton and
    merge with it if time is is within kMergeTime.  Otherwise, insert
    a new HitPhoton, allowing the size of the vector to grow beyond
    kApproxMaxIndividualHitPhotonsPerPMT.

    The time of "merged HitPhotons" is set to the time of the earliest
    HitPhoton in the merged set, because of the importance of the
    leading edge in the electronics and the analysis.  Due to the
    smallness of kMergeTime (1 ns), the effect is expected to be
    small.

    Note that MTCG4HitPmt immediately "owns" any HitPhoton passed to
    DetectPhoton, and can delete the HitPhoton object at will.  In
    particular, the MTCG4HitPhoton pointed to by the new_photon
    argument is deleted if merging with an existing HitPhoton occurs.

    @param new_photon  New HitPhoton to add (or merge).
*/  
void MTCG4HitPmt::DetectPhoton(MTCG4HitPhoton* new_photon) {
	if (fPhotons.size() < kApproxMaxIndividualHitPhotonsPerPMT) {
		fPhotons.push_back(new_photon);
	}
	else {
		SortTimeAscending();
		std::vector<MTCG4HitPhoton*>::iterator it2, it1;
		it2= lower_bound(fPhotons.begin(), fPhotons.end(), new_photon,
				Compare_HitPhotonPtr_TimeAscending);
		it1= it2;
		if ( it1 == fPhotons.begin() ) { // New photon time <= it1 == it2.
			// photon is earlier than any recorded so far -- always insert!
			fPhotons.insert(it1, new_photon);
		}
		else {
			it1--; // it1 < new photon time <= it2. it2 could be iterator end.
			if ( (new_photon->GetTime() - (*it1)->GetTime() < kMergeTime) &&
					// Added extra condition for anode row and column to be same
					// for merging photon hits. --Mich
					(new_photon->GetAnodeRow() == (*it1)->GetAnodeRow()) &&
					(new_photon->GetAnodeColumn() == (*it1)->GetAnodeColumn()) )
			{
				// close to earlier photon -- merge with earlier photon
				IFDEBUG(if (new_photon->GetTime() - (*it1)->GetTime() < 0.0) G4cerr << "MTCG4HitPmt STRANGE merge " << new_photon->GetTime() << " with non-earlier photon " << (*it1)->GetTime() << G4endl );
				(*it1)->AddCount( new_photon->GetCount() );
				(*it1)->AddKineticEnergy( new_photon->GetKineticEnergy() );
				delete new_photon;
				new_photon= 0;
			}
			else if ( it2 != fPhotons.end() &&
					((*it2)->GetTime() - new_photon->GetTime() < kMergeTime) &&
					// Added extra condition for anode row and column to be same
					// for merging photon hits. -- Mich
					((*it2)->GetAnodeRow() == new_photon->GetAnodeRow()) &&
					((*it2)->GetAnodeColumn() == new_photon->GetAnodeColumn()) )
			{
				// not after last photon, and close to later photon
				IFDEBUG(if ((*it2)->GetTime() - new_photon->GetTime() < 0.0) G4cerr << "MTCG4HitPmt STRANGE merge " << new_photon->GetTime() << " with non-later photon " << (*it2)->GetTime() << G4endl);
				(*it2)->AddCount( new_photon->GetCount() );
				(*it2)->AddKineticEnergy( new_photon->GetKineticEnergy() );
				(*it2)->SetTime( new_photon->GetTime() );
				delete new_photon;
				new_photon= 0;
			}
			else {
				// after last photon or not close to any photon
				fPhotons.insert(it2, new_photon);
			}
		}
	}

}

/// sort HitPhotons so earliest are first
void MTCG4HitPmt::SortTimeAscending() {
  std::sort(fPhotons.begin(), fPhotons.end(),
	    Compare_HitPhotonPtr_TimeAscending);
}

/// print out HitPhotons.
void MTCG4HitPmt::Print(std::ostream &os, bool /*fullDetailsMode*/)
{
	// Added by Mich to do ofstream to file.
	G4int event_id =
	 	G4EventManager::GetEventManager() -> GetConstCurrentEvent() -> GetEventID();
	//G4cout << "fPhotons.size() = " << fPhotons.size() << G4endl;
	for(size_t i = 0; i < fPhotons.size(); i++){
	os
		<< std::setw(3) << event_id << " "
	 	<< std::setw(2) << fID
	 	<< fPhotons[i]->GetAnodeRow()
	 	<< fPhotons[i]->GetAnodeColumn() << " "
		<< std::setw(3) << fPhotons.size() << " "
	 	<< std::setw(10) << fPhotons[i]->GetTime()/ns << " "
	 	<< std::setw(4) << fPhotons[i]->GetCount() << G4endl;
	}
	// End of added by Mich.

  //os << " PMTID= " << fID
  //   << "  number of HitPhotons = " << fPhotons.size() << G4endl;
  //if (fullDetailsMode==false) {
  //  for (size_t i=0; i<fPhotons.size(); i++)
  //    os << "  Hit time= " << fPhotons[i]->GetTime()
	// << " count= " << fPhotons[i]->GetCount() << G4endl;
  //}
  //else {
  //  for (size_t i=0; i<fPhotons.size(); i++) {
  //    os << "  Hit time= " << fPhotons[i]->GetTime() << G4endl;
  //    os << "      count= " << fPhotons[i]->GetCount() << G4endl;
  //    os << "      wavelength= " << fPhotons[i]->GetWavelength() << G4endl;
  //    G4double x,y,z;
  //    fPhotons[i]->GetPosition(x,y,z);
  //    os << "      position= " << x << " " << y << " " << z << G4endl;
  //    fPhotons[i]->GetMomentum(x,y,z);
  //    os << "      momentum= " << x << " " << y << " " << z << G4endl;
  //    fPhotons[i]->GetPolarization(x,y,z);
  //    os << "      polarization= " << x << " " << y << " " << z << G4endl;
  //  }
  //}    
}

G4int MTCG4HitPmt::GetEntriesInAnode(G4int anode) const
{
	// Anode number anode is expressed in the format:
	// <row_number>*10 + <column_number>.
	G4int ret = 0;
	for(std::vector<MTCG4HitPhoton*>::const_iterator it = fPhotons.begin();
			it != fPhotons.end(); it++)
		if( (*it)->GetAnodeRow() == (G4int(anode/10))%10
				&& (*it)->GetAnodeColumn() == (G4int(anode/1))%10 )
		 	ret++;
	return ret;
}

G4int MTCG4HitPmt::GetTotalEntries() const
{
	// Anode number anode is expressed in the format:
	// <row_number>*10 + <column_number>.
	G4int ret = 0;
	for(std::vector<MTCG4HitPhoton*>::const_iterator it = fPhotons.begin();
			it != fPhotons.end(); it++)
		 	ret++;
	return ret;
}
