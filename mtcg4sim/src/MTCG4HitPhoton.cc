/** @file MTCG4HitPhoton.cc
  For MTCG4HitPhoton class.

  This file is part of the GenericLAND software library.
  $Id: MTCG4HitPhoton.cc,v 1.1 2005/01/21 22:04:17 kamland0 Exp $

  @author Glenn Horton-Smith, December 2004
  */

#include "MTCG4HitPhoton.hh"

MTCG4HitPhoton::MTCG4HitPhoton()
{
	// Initialize private members.
	fTime = -100;				/// time of hit 
	fPMTID = 0;					/// ID number of PMT the HitPhoton hit
	fAnodeRow = 0;				/// Anode row number 1~8.
	fAnodeColumn = 0;			/// Anode column number 1~8.
	fKE = 0;					/// kinetic energy 
	fCount = 0;					/// count of photons, often 1
	for (int i = 0; i < 3; ++i) {
		fPosition[i] = 0;		/// x,y,z components of position
		fMomentum[i] = 0;		/// x,y,z components of momentum (normalized?)
		fPolarization[i] = 0;	/// x,y,z components of polarization
	}
}
