// This file is part of the KamLAND software library.
// $Id: MTCG4PmtSD.cc,v 1.36 2006/05/05 21:29:40 kamland0 Exp $
//
//  MTCG4PmtSD.cc
//
//   Records total number of hits on each PMT.
//   Uses Geant4-style hit collection to record hit time, place, etc.
//
//   Author:  Glenn Horton-Smith, 2000/01/28 (replaces Ikeda version)
//   Modification history:
//    2001.02.02:  Add ROOT output support [GAHS]
//    2001.04.30:  Portable Format support [Tajima+GAHS]
//    2001.05.03:  Deleted waveform recording [GAHS]
//    2001.09.18:  Delete all event output stuff [GAHS]
//

#include "MTCG4PmtSD.hh"
#include "MTCG4EventAction.hh"
#include "MTCG4HitPhoton.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"

//#include "MTCG4Scint.hh"  // for doScintilllation and total energy deposition info
#include "G4VSolid.hh" // for access to solid store
#include "MTCG4DetectorConstruction.hh"
#include "Randomize.hh"

#include <string.h>  // for memset

G4VSolid* MTCG4PmtSD::balloon_solid= 0;

MTCG4PmtSD::MTCG4PmtSD(G4String name, G4int arg_max_pmts, G4int arg_pmt_no_offset,
		 G4int arg_my_id_pmt_size)
:G4VSensitiveDetector(name)
{
  max_pmts= arg_max_pmts;
  pmt_no_offset= arg_pmt_no_offset;
  my_id_pmt_size= arg_my_id_pmt_size;

  hit_sum= new G4int[max_pmts];
}

MTCG4PmtSD::~MTCG4PmtSD()
{
  if (hit_sum)
    delete[] hit_sum;
}


void MTCG4PmtSD::Initialize(G4HCofThisEvent* /* HCE */ )
{
  memset(hit_sum, 0, sizeof(hit_sum[0])*max_pmts);
  n_pmt_hits= n_pmts_hit= 0;
}


G4bool MTCG4PmtSD::ProcessHits(G4Step* /* aStep */ , G4TouchableHistory*)
{
  // NOTE: ProcessHits should never be called anymore, because
  // hits are decided by MTCG4PMTOpticalModel, which calls MTCG4PmtSD::SimpleHit()
  // in order to avoid having to create a bogus G4Step object.
  ////////////////////////////////////////////////////////////////

  return false;
}


// Here is the real "hit" routine, used by MTCG4PMTOpticalModel and by ProcessHits
// It is more efficient in some ways.
void MTCG4PmtSD::SimpleHit(
	 	G4int ipmt,
		G4int row_no,
		G4int column_no,
		G4double time,
		G4double kineticEnergy,
		const G4ThreeVector &hit_position,
		const G4ThreeVector &hit_momentum,
		const G4ThreeVector &hit_polarization,
		G4int  /* generation */ ,
		G4int iWeight )
{
  G4int pmt_index = ipmt - pmt_no_offset;
  if (pmt_index < 0 || pmt_index >= max_pmts)
    {
      G4cerr << "Error: MTCG4PmtSD::SimpleHit [" << GetName() << "] passed ipmt="
	     << ipmt << ", but max_pmts=" << max_pmts
	     << " and offset=" << pmt_no_offset << " !" << G4endl;
      return;
    }
  
  hit_sum[pmt_index]+= iWeight;
  
  // for PMT Hit Photon Collection --tajima
  // Always create hit_photons, because ROOT now uses them too. --GAHS.
  MTCG4HitPhoton* hit_photon = new MTCG4HitPhoton();
  hit_photon->SetPMTID((G4int)ipmt);
	hit_photon->SetAnodeRow((G4int)row_no);
	hit_photon->SetAnodeColumn((G4int)column_no);
  hit_photon->SetTime( time );
  hit_photon->SetKineticEnergy( kineticEnergy );
  hit_photon->SetPosition( 
			  hit_position.x(),
			  hit_position.y(),
			  hit_position.z()
			  );
  hit_photon->SetMomentum( 
			  hit_momentum.x(),
			  hit_momentum.y(),
			  hit_momentum.z()
			  );
  hit_photon->SetPolarization( 
			      hit_polarization.x(),
			      hit_polarization.y(),
			      hit_polarization.z()
			      );
  //  hit_photon->SetGeneration((short)generation);
  hit_photon->SetCount( iWeight );
    
  //  MTCG4EventAction::GetTheHitPhotons()->AddHitPhoton(hit_photon);
  MTCG4EventAction::GetTheHitPmtCollection()->DetectPhoton(hit_photon);
}


void MTCG4PmtSD::EndOfEvent(G4HCofThisEvent* /* HCE */ )
{
  G4int ipmt;
    
  n_pmt_hits=0;
  n_pmts_hit=0;
  for (ipmt=0; ipmt<max_pmts; ipmt++) {
    if (hit_sum[ipmt]) {
      n_pmt_hits+= hit_sum[ipmt];
      n_pmts_hit++;
    }
  }

}

void MTCG4PmtSD::clear()
{} 

void MTCG4PmtSD::DrawAll()
{}

void MTCG4PmtSD::PrintAll()
{
}
