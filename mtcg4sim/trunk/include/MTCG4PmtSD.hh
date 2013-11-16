// This file is part of the KamLAND software library.
// $Id: MTCG4PmtSD.hh,v 1.17 2006/05/05 21:34:44 kamland0 Exp $
//
//  MTCG4PmtSD.hh
//
//   Records total number of hits and "waveform" on each PMT,
//   with no PMT TTS or digitizer bandwidth effects included.
//   (Convolve the hit waveform with the TTS+digitizer single-hit function
//   to get the resulting digitized waveform.)
//
//   Bypasses Geant4 hit collection mechanism.
//
//   Author:  Glenn Horton-Smith, 2000/01/28 
//   Modification history:
//    2001.02.02:  Add ROOT output support [GAHS]
//    2001.04.30:  Portable Format support [Tajima+GAHS]
//    2001.05.01:  Allow PMT numbers to have big offsets [GAHS, for OD]
//    2001.09.18:  Delete all event output stuff [GAHS]
//

#ifndef MTCG4PmtSD_h
#define MTCG4PmtSD_h 1

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class MTCG4PmtSD : public G4VSensitiveDetector
{
  protected:
      G4int max_pmts;
      G4int pmt_no_offset;
      G4int my_id_pmt_size;

      // enum { max_waveform_ns= 200 };
      static G4VSolid* balloon_solid;
  
  public:
      G4int *hit_sum;  /* indexed by pmt number */
      //typedef G4int waveform_t[max_waveform_ns];
      //waveform_t *hit_waveform; /* indexed by pmt number */
  
      G4int n_pmt_hits;   /* # of hits,       calculated at EndOfEvent */
      G4int n_pmts_hit;   /* # of PMTs hit,   calculated at EndOfEvent */

  public:
  // member functions
      MTCG4PmtSD(G4String name,
	      G4int max_pmts=1920,
	      G4int pmt_no_offset=0,
	      G4int my_id_pmt_size= -1); 
      virtual ~MTCG4PmtSD();

      virtual void Initialize(G4HCofThisEvent*HCE);
      virtual void EndOfEvent(G4HCofThisEvent*HCE);
      virtual void clear();
      virtual void DrawAll();
      virtual void PrintAll();

      void SimpleHit(
				 	G4int ipmt,
					G4int row_no,
					G4int column_no,
		      G4double time,
		      G4double kineticEnergy,
		      const G4ThreeVector & position,
		      const G4ThreeVector & momentum,
		      const G4ThreeVector & polarization,
		      G4int generation,
		      G4int iWeight );
    
  protected:
      virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
};

#endif
