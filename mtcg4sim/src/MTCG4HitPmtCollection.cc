/** @file MTCG4HitPmtCollection.cc
    For MTCG4HitPmtCollection class.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4HitPmtCollection.cc,v 1.1 2005/01/21 22:04:17 kamland0 Exp $

    @author Glenn Horton-Smith, December 2004
*/

#include "MTCG4HitPmtCollection.hh"
#include <algorithm>
#include <G4ios.hh>

MTCG4HitPmtCollection::MTCG4HitPmtCollection()
{
	fNewPmt = 0;
}

MTCG4HitPmtCollection::~MTCG4HitPmtCollection()
{
  Clear();
}


/** clear out AND DELETE HitPmts (and HitPhotons) that were detected,
    resetting this HitPmtCollection to be empty */
void MTCG4HitPmtCollection::Clear()
{
  for(size_t i=0; i<fPmt.size(); i++){
    fPmt[i]->Clear();
    delete fPmt[i];
  }
  fPmt.clear();
  fHitMap.clear();
	fNewPmt = 0;
}

/** find or make appropriate HitPmt, and DetectPhoton in that HitPmt */
void MTCG4HitPmtCollection::DetectPhoton(MTCG4HitPhoton* new_photon)
{
  MTCG4HitPmt* hitpmtptr= GetPmt_ByID( new_photon->GetPMTID() );

  if( hitpmtptr != NULL ) {
    // found a HitPmt with this ID
    hitpmtptr->DetectPhoton(new_photon);
  }
  else {
    // make a HitPmt with this ID
    fPmt.push_back( new MTCG4HitPmt((short)new_photon->GetPMTID()) );
    fPmt[fPmt.size()-1]->DetectPhoton(new_photon);
    fHitMap.insert(
		   std::make_pair(
				  (short)new_photon->GetPMTID(),
				  (MTCG4HitPmt*)fPmt[fPmt.size()-1]
				  )
		   );
		fNewPmt++;
  }
}

void MTCG4HitPmtCollection::SortTimeAscending()
{ 
  for(size_t i=0; i<fPmt.size(); i++)
    fPmt[i]->SortTimeAscending();
  std::sort(fPmt.begin(), fPmt.end(), Compare_HitPMTPtr_TimeAscending ); 
}

/** return the number of HitPmts in internal collection */
int MTCG4HitPmtCollection::GetEntries() const
{
  return fPmt.size();
}

int MTCG4HitPmtCollection::GetTotalPE() const
{
	int ret = 0;
	for (size_t i = 0; i < fPmt.size(); ++i)
		ret += fPmt.at(i)->GetTotalEntries();
	return ret;
}

/** return the i-th HitPmt in internal collection */
MTCG4HitPmt* MTCG4HitPmtCollection::GetPmt(int i) const
{
  return fPmt[i];
}

/** return pointer to HitPmt with given id if in collection,
    or NULL if no such HitPmt is in collection */
MTCG4HitPmt* MTCG4HitPmtCollection::GetPmt_ByID(int id) const
{
  std::map<short, MTCG4HitPmt*> :: const_iterator p = fHitMap.find((short)id);
  if( p != fHitMap.end() ) {
    // found a HitPmt with this ID
    return p->second;
  }
  else {
    // no HitPmt
    return NULL;
  }
}

/// print out HitPmts
void MTCG4HitPmtCollection::Print(std::ostream &os) const
{
  for(size_t i=0; i<fPmt.size(); i++) {
    fPmt[i]->Print(os);
  }
}
