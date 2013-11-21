/** @file MTCG4HitPhoton.cc
    For MTCG4HitPhoton class.
    
    This file is part of the GenericLAND software library.
    $Id: MTCG4HitPhoton.cc,v 1.1 2005/01/21 22:04:17 kamland0 Exp $

    @author Glenn Horton-Smith, December 2004
*/

#include "MTCG4HitPhoton.hh"
#include <CLHEP/Units/PhysicalConstants.h>
using namespace CLHEP;

/// set kinetic energy and wavelength of photon.
void
MTCG4HitPhoton::SetKineticEnergy(G4double KE) {
  fKE= KE;
}

/// set wavelength and kinetic energy of photon
void 
MTCG4HitPhoton::SetWavelength(G4double wl) {
  fKE= 2*pi*hbarc/wl;
}

void 
MTCG4HitPhoton::SetPosition(G4double x, G4double y, G4double z) {
  fPosition[0]= x;
  fPosition[1]= y;
  fPosition[2]= z;
}

void 
MTCG4HitPhoton::SetMomentum(G4double x, G4double y, G4double z) {
  fMomentum[0]= x;
  fMomentum[1]= y;
  fMomentum[2]= z;
}

void 
MTCG4HitPhoton::SetPolarization(G4double x, G4double y, G4double z) {
  fPolarization[0]= x;
  fPolarization[1]= y;
  fPolarization[2]= z;
}


G4double 
MTCG4HitPhoton::GetKineticEnergy() const {
  return fKE;
}

G4double 
MTCG4HitPhoton::GetWavelength() const {
  return 2*pi*hbarc/fKE;
}

