#ifndef __MTCG4HitPmt_hh__
#define __MTCG4HitPmt_hh__
/** @file MTCG4HitPmt.hh
    Declares MTCG4HitPmt class and helper functions.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4HitPmt.hh,v 1.1 2005/01/21 22:04:10 kamland0 Exp $
    
    @author Glenn Horton-Smith, December 2004
*/

#include "MTCG4HitPhoton.hh"
#include <vector>
#include <cstddef>

/** MTCG4HitPmt stores information about a PMT that detected one or more
    photoelectrons.

    The general contract for MTCG4HitPmt is as follows:
      - remember ID and all photons that made photoelectrons in this PMT
      - provide Clear, DetectPhoton, SortTimeAscending, GetID, GetEntries,
        GetPhoton, and Print(ostream &) functions
      - take "ownership" of the photons registered by DetectPhoton,
        i.e., delete them when Clear or destrictor is called

    This is almost the same "general contract" that was implemented
    for KLG4sim's KLHitPMT by O. Tajima and G. Horton-Smith, but the
    code was rewritten for GLG4sim in December 2004.

    @author Glenn Horton-Smith
*/

#include <vector>

class MTCG4HitPmt {
public:
  MTCG4HitPmt(G4int ID);
  ~MTCG4HitPmt();

  void Clear();
  void DetectPhoton(MTCG4HitPhoton*);
  void SortTimeAscending();

  G4int GetID() const { return fID; }

	// There is no anode row nor column ID for MTCG4HitPmt.
	// In the future, maybe these my be included if we decide to change this into
	// a MTCG4HitPixel.
	//G4int GetAnodeRow() const { return fAnodeRow; }
	//G4int GetAnodeColumn() const { return fAnodeColumn; }

	// Get number of photon hits for whole PMT.
  G4int GetEntries() const { return fPhotons.size(); }

	// Get number of photon hits at specifice anode.
	// anode is in format row_id*10 + column_id.
	G4int GetEntriesInAnode(G4int anode);

  MTCG4HitPhoton* GetPhoton(G4int i) const { return fPhotons[i]; }

  void Print(std::ostream &, bool fullDetailsMode=false);

  static const size_t kApproxMaxIndividualHitPhotonsPerPMT;
  static const G4double kMergeTime;
  
private:
  G4int fID;
	//G4int fAnodeRow;
	//G4int fAnodeColumn;
  std::vector<MTCG4HitPhoton*> fPhotons;
};


/** comparison function for sorting MTCG4HitPmt pointers
 */
inline bool
Compare_HitPMTPtr_TimeAscending(const MTCG4HitPmt *a,
				const MTCG4HitPmt *b)
{
  // put empties at the end
  if (!a || a->GetEntries()<=0)
    return false;
  if (!b || b->GetEntries()<=0)
    return true;
  return a->GetPhoton(0)->GetTime() < b->GetPhoton(0)->GetTime();
}



#endif // __MTCG4HitPmt_hh__
