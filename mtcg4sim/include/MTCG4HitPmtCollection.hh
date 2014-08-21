#ifndef __MTCG4HitPmtCollection_hh__
#define __MTCG4HitPmtCollection_hh__
/** @file MTCG4HitPmtCollection.hh
    Declares MTCG4HitPmtCollection class and helper functions.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4HitPmtCollection.hh,v 1.1 2005/01/21 22:04:10 kamland0 Exp $
    
    @author Glenn Horton-Smith, December 2004
*/

#include "MTCG4HitPmt.hh"
#include <vector>
#include <map>

/** MTCG4HitPmtCollection stores MTCG4HitPmt objects.

    The general contract for MTCG4HitPmtCollection is as follows:
      - efficiently keep a bunch of MTCG4HitPmts with GLG4HitPhotons
      - provide Clear, DetectPhoton, SortTimeAscending, GetEntries, GetPmt,
        and Print(ostream &) functions
      - note the GLG4HitPhotons passed to DetectPhoton will become the
        "owned" by the MTCG4HitPmtCollection and its MTCG4HitPmts, and they
	will take care of deletion.

    This is almost the same "general contract" that was implemented
    for KLG4sim's KLHitPMTCollection by O. Tajima and G. Horton-Smith,
    but the code was rewritten for GLG4sim in December 2004.

    @author Glenn Horton-Smith
*/
class MTCG4HitPhoton;

class MTCG4HitPmtCollection {
public:
  MTCG4HitPmtCollection();
  virtual ~MTCG4HitPmtCollection();

  void Clear();
  void DetectPhoton(MTCG4HitPhoton*);
  void SortTimeAscending();
  int GetEntries() const;
  int GetTotalPE() const;
  MTCG4HitPmt* GetPmt(int i) const;
  MTCG4HitPmt* GetPmt_ByID(int id) const;

  void Print(std::ostream &) const;

	int fNewPmt;
private:
  std::vector<MTCG4HitPmt*> fPmt;
  std::map<short, MTCG4HitPmt*> fHitMap;  
};


#endif // __MTCG4HitPmtCollection_hh__
