#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "MTCG4DetectorConstruction.hh"

//
// Visualization attributes.
//
void MTCG4DetectorConstruction::SetVisAttributes() {

	// Make colours.
	G4Colour  white   (1.0, 1.0, 1.0);
	G4Colour  gray    (0.5, 0.5, 0.5);
	G4Colour  lgray   (.85, .85, .85);
	G4Colour  red     (1.0, 0.0, 0.0);
	G4Colour  blue    (0.0, 0.0, 1.0);
	G4Colour  cyan    (0.0, 1.0, 1.0);
	G4Colour  magenta (1.0, 0.0, 1.0);
	G4Colour  yellow  (1.0, 1.0, 0.0);
	G4Colour  orange  (.75, .55, 0.0);
	G4Colour  lblue   (0.0, 0.0, .75);
	G4Colour  lgreen  (0.0, .75, 0.0);
	G4Colour  green   (0.0, 1.0, 0.0);
	G4Colour  brown   (0.7, 0.4, 0.1);
	
	// World volume.
	fWorldLogical->SetVisAttributes(G4VisAttributes::Invisible);

	// Scintillator cube color.
	if (fScintVis == scintVisColor)
		fScintLogical->SetVisAttributes(new G4VisAttributes(cyan));
	if (fScintVis == scintVisGray)
		fScintLogical->SetVisAttributes(new G4VisAttributes(gray));
	if (fScintVis == scintVisOff)
		fScintLogical->SetVisAttributes(G4VisAttributes::Invisible);

	// PMT color.
	if (fPmtsArePlaced) {
		if (fPmtVis == pmtVisColor) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->
				SetVisAttributes(new G4VisAttributes(lblue));
			fPmtGlassWindowLogical->SetVisAttributes(new G4VisAttributes(lblue));
			fPmtBlackWrappingLogical->
				SetVisAttributes(new G4VisAttributes(gray));
			fPmtRearBoardLogical->SetVisAttributes(new G4VisAttributes(lgreen));
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(new G4VisAttributes(red));
		}
		if (fPmtVis == pmtVisGray) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtGlassWindowLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtBlackWrappingLogical->
				SetVisAttributes(new G4VisAttributes(gray));
			fPmtRearBoardLogical->SetVisAttributes(new G4VisAttributes(gray));
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(new G4VisAttributes(gray));
		}
		if (fPmtVis == pmtVisOff) {
			fPmtBoundingVolumeLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassHousingLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtGlassWindowLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtBlackWrappingLogical->
				SetVisAttributes(G4VisAttributes::Invisible);
			fPmtRearBoardLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtOuterVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fPmtInnerVacuumLogical->SetVisAttributes(G4VisAttributes::Invisible);
			fDynodeLogical->SetVisAttributes(G4VisAttributes::Invisible);
		}
	}
	if (fFrameIsPlaced) {
		//// Peripheral structure color.
		//if (fPeripheralGeometryVis == peripheralVisColor) {
		//	for (unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//}
		//if (fPeripheralGeometryVis == peripheralVisGray) {
		//	for (unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//	for (unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(new
		//				G4VisAttributes(gray));
		//}
		//if (fPeripheralGeometryVis == peripheralVisOff) {
		//	for (unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate)
		//		fFrameEndPlateLogical[iEndPlate]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for (unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate)
		//		fFrameSidePlateLogical[iSidePlate]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for (unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp)
		//		fFrameEndClampLogical[iEndClamp]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for (unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp)
		//		fFrameSideClampLogical[iSideClamp]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//	for (unsigned int iLeg = 0; iLeg < 4; ++iLeg)
		//		fLegLogical[iLeg]->SetVisAttributes(
		//				G4VisAttributes::Invisible);
		//}
	}

	//// Test marker volume.
 	//// Used for testing.
	//G4VisAttributes* testMarkerAttributes = new G4VisAttributes(G4Colour(1., 0., 0., .1)); // This is used to confirm PMT orientation.
	//testMarkerAttributes->SetForceSolid(true);
	//testMarkerLogical->SetVisAttributes(testMarkerAttributes);
}

