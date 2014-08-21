#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include "MTCG4DetectorConstruction.hh"

#include "CADMesh.hh"

// Used by MTCG4DetectorConstruction::SetupGeometry()
void MTCG4DetectorConstruction::SetupPeripheralGeometries()
{
	G4NistManager* nist = G4NistManager::Instance();
	G4Material* fFrameMaterial =
		nist->FindOrBuildMaterial("G4_POLYOXYMETHYLENE"); // Delrin.
	G4Material* fLegMaterial =
		nist->FindOrBuildMaterial("G4_Al"); // Aluminum.

	//
	// MTC CAD design import to create peripheral structures.
	// 

	// Offset of the origin of the MTCG4Sim coordinate system from the origin of
	// the CAD world coordinate system. As of now, the CAD world coordinate
	// system is also set to be the center of the cube. When importing
	// additional CAD components into MTCG4Sim, please first make sure that the
	// origin of the CAD world coordinate system coincides with the center of
	// the cube.
	const G4ThreeVector offset = G4ThreeVector(0.*mm, 0.*mm, 0.*mm);

	// File format of CAD files used.
	const G4String cadFormat = "STL";

	//
	// End plates of frame structure that holds the scintillating cube.
	//

	// 'const_cast<char*>' to a dynamically allocated non-const char array needs
	// to be used in the CADMesh() constructors below. This is because the
	// CADMesh constructor takes in a pointer to non-const char.
	// I think this was a mistake on the part of the programmer that wrote
	// CADMesh. He probably should have used (const char*) for the type accepted
	// by the constructor. If 'const_cast<char*>' is not used, warning messages
	// appear such as "warning: deprecated conversion from string constant to
	// ‘char*’", or the program will segmentation fault. I'm not sure about the
	// memory leaks associated with not deleting the dynamic non-const char
	// array. Will have to fix this later if it turns out to be a problem.

	for (unsigned int iEndPlate = 0; iEndPlate < 2; ++iEndPlate) {
		std::ostringstream oss;
		oss << iEndPlate+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameEndPlate" + oss.str() + ".stl";
		G4String logicalName = "frameEndPlateLogical" + oss.str();
		G4String physicalName = "frameEndPlatePhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh( buffer1, buffer2, mm, offset, false);
		fFrameEndPlateSolid[iEndPlate] = mesh->TessellatedMesh();
		fFrameEndPlateLogical[iEndPlate] = new G4LogicalVolume(
					fFrameEndPlateSolid[iEndPlate],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameEndPlatePhysical[iEndPlate] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameEndPlateLogical[iEndPlate],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Side plates of frame structure that holds the scintillating cube.
	//

	for (unsigned int iSidePlate = 0; iSidePlate < 4; ++iSidePlate) {
		std::ostringstream oss; oss << iSidePlate+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameSidePlate" + oss.str() + ".stl";
		G4String logicalName = "frameSidePlateLogical" + oss.str();
		G4String physicalName = "frameSidePlatePhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameSidePlateSolid[iSidePlate] = mesh->TessellatedMesh();
		fFrameSidePlateLogical[iSidePlate] = new G4LogicalVolume(
					fFrameSidePlateSolid[iSidePlate],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameSidePlatePhysical[iSidePlate] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameSidePlateLogical[iSidePlate],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// End clamps of frame structure that holds the scintillating cube.
	//

	for (unsigned int iEndClamp = 0; iEndClamp < 8; ++iEndClamp) {
		std::ostringstream oss; oss << iEndClamp+1;
		G4String stlFileName =
			fCadFilesPath + "frameEndClamp" + oss.str() + ".stl";
		G4String logicalName = "frameEndClampLogical" + oss.str();
		G4String physicalName = "frameEndClampPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameEndClampSolid[iEndClamp] = mesh->TessellatedMesh();
		fFrameEndClampLogical[iEndClamp] = new G4LogicalVolume(
					fFrameEndClampSolid[iEndClamp],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameEndClampPhysical[iEndClamp] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameEndClampLogical[iEndClamp],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Side clamps of frame structure that holds the scintillating cube.
	//

	for (unsigned int iSideClamp = 0; iSideClamp < 16; ++iSideClamp) {
		std::ostringstream oss; oss << iSideClamp+1;
		G4String stlFileName =
		   	fCadFilesPath + "frameSideClamp" + oss.str() + ".stl";
		G4String logicalName = "frameSideClampLogical" + oss.str();
		G4String physicalName = "frameSideClampPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fFrameSideClampSolid[iSideClamp] = mesh->TessellatedMesh();
		fFrameSideClampLogical[iSideClamp] = new G4LogicalVolume(
					fFrameSideClampSolid[iSideClamp],
				   	fFrameMaterial,
					logicalName.c_str());
		fFrameSideClampPhysical[iSideClamp] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fFrameSideClampLogical[iSideClamp],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}

	//
	// Legs holding frame structure that holds the scintillating cube.
	//

	for (unsigned int iLeg = 0; iLeg < 4; ++iLeg) {
		std::ostringstream oss; oss << iLeg+1;
		G4String stlFileName =
		   	fCadFilesPath + "leg" + oss.str() + ".stl";
		G4String logicalName = "legLogical" + oss.str();
		G4String physicalName = "legPhysical" + oss.str();
		char *buffer1 = new char[stlFileName.size() + 1];
		buffer1 = const_cast<char*>(stlFileName.c_str());
		char *buffer2 = new char[cadFormat.size() + 1];
		buffer2 = const_cast<char*>(cadFormat.c_str());
		CADMesh *mesh = new CADMesh(buffer1, buffer2, mm, offset, false);
		fLegSolid[iLeg] = mesh->TessellatedMesh();
		fLegLogical[iLeg] = new G4LogicalVolume(
					fLegSolid[iLeg],
				   	fLegMaterial,
					logicalName.c_str());
		fLegPhysical[iLeg] = new G4PVPlacement(
					NULL,
				   	G4ThreeVector(),
				   	fLegLogical[iLeg],
					physicalName.c_str(),
				   	fWorldLogical,
				   	false,
				   	0);
		delete mesh;
	}
} // SetupPeripheralGeometries()
