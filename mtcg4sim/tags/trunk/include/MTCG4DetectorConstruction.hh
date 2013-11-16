//
//    DetectorConstruction.hh by Mich 120417
//

#ifndef MTCG4_DETECTOR_CONSTRUCTION_HH
#define MTCG4_DETECTOR_CONSTRUCTION_HH 1

#include "MTCG4PixelSD.hh"

#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "globals.hh"
//
// Class definitions.
//
class MTCG4DetectorMessenger;
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4SubtractionSolid;

class MTCG4DetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		MTCG4DetectorConstruction();
		~MTCG4DetectorConstruction();

	public:
		void SetNeutronCaptureDopantMaterial(G4String material)
		{ neutronCaptureMaterial = material; }
		inline G4String GetNeutronCaptureDopantMaterial() const
		{ return neutronCaptureMaterial; }
		void SetDopantEnrichment(G4bool value)
		{ dopantIsEnriched = value; }
		inline G4bool GetDopantEnrichment() const
		{ return dopantIsEnriched; }
		void SetDopingFraction(G4double value)
		{ dopingFraction = value; }
		inline G4double GetDopingFraction() const
		{	return dopingFraction; }
		inline G4double GetScintillatorHalfLengthX() const
		{ return scintHalfX; }
		inline G4double GetScintillatorHalfLengthY() const
		{ return scintHalfY; }
		inline G4double GetScintillatorHalfLengthZ() const
		{ return scintHalfZ; }
		inline G4double GetScintillatorBufferThickness() const
		{ return scintillatorBufferThickness; }
		void SetOpticalSurfaceProperties(G4bool value)
		{ opticalSurfacePropertiesAreSet = value; }
		inline G4bool GetOpticalSurfaceProperties()
		{ return opticalSurfacePropertiesAreSet; }
		void SetScintillatorVisualisation(G4bool value)
		{ scintillatorVisualisation = value; }
		inline G4bool GetScintillatorVisualisation()
		{ return scintillatorVisualisation; }
		void SetMCPModuleVisualisation(G4bool value)
		{ mcpModuleVisualisation = value; }
		inline G4bool GetMCPModuleVisualisation()
		{ return mcpModuleVisualisation; }
		void SetMCPPixelVisualisation(G4bool value)
		{ mcpPixelVisualisation = value; }
		inline G4bool GetMCPPixelVisualisation()
		{ return mcpPixelVisualisation; }
		void SetMCPPlacement(G4bool value)
		{ MCPsArePlaced = value; }
		inline G4bool GetMCPPlacement() const
		{ return MCPsArePlaced; }

	public:
		G4VPhysicalVolume* Construct();
		void DefineMaterials();
		G4Material* SetScintillatorMaterial(G4String);
		void SetupGeometry();
		void SetVisualisationAttributes();
		void SetSurfaceProperties();
		void UpdateGeometry();
		G4double GetEJ254Density(G4double);
		G4double GetAnthraceneFraction(G4double);
		G4double GetPolyvinyltolueneFraction
			(G4double, G4double);
	private:
		
		//
		// Parameters.
		//
		G4bool MCPsArePlaced;
		G4double scintHalfX,
						 scintHalfY,
						 scintHalfZ;
		G4double scintillatorBufferThickness;
		G4String neutronCaptureMaterial;
		G4bool dopantIsEnriched;
		G4double dopingFraction;
		G4bool opticalSurfacePropertiesAreSet;
		G4bool scintillatorVisualisation,
					 mcpModuleVisualisation,
					 mcpPixelVisualisation;
		G4bool neutronHPThermalScatteringIsTurnedOn;		
	
		//
		// Choices of scintillator types.
		//
		//G4Material* EJ254_onePercentNaturalBoronDoped;
		//G4Material* EJ254_twoAndHalfPercentNaturalBoronDoped;
		//G4Material* EJ254_fivePercentNaturalBoronDoped;
		//G4Material* EJ254_onePercentEnrichedBoronDoped;
		//G4Material* EJ254_twoAndHalfPercentEnrichedBoronDoped;
		//G4Material* EJ254_fivePercentEnrichedBoronDoped;
		//G4Material* EJ254_onePercentNaturalLithiumDoped;
		//G4Material* EJ254_twoAndHalfPercentNaturalLithiumDoped;
		//G4Material* EJ254_fivePercentNaturalLithiumDoped;
		//G4Material* EJ254_onePercentEnrichedLithiumDoped;
		//G4Material* EJ254_twoAndHalfPercentEnrichedLithiumDoped;
		//G4Material* EJ254_fivePercentEnrichedLithiumDoped;

		//
		// Materials comprising detector.
		//
		G4Material* worldMaterial;
		G4Material* scintillatorMaterial;
		//G4Material* scintillatorMaterial_TS;
		G4Material* pixelMaterial;
		G4Material* MCPMaterial;
		
		//
		// Logical and physical volumes.
		//
		G4Box* worldSolid;
		G4LogicalVolume* worldLogical;
		G4VPhysicalVolume* worldPhysical;

		G4Box* scintillatorSolid;
		G4LogicalVolume* scintillatorLogical;
		G4VPhysicalVolume* scintillatorPhysical;

		G4Box* MCPQuadSetSolid;               	
		G4LogicalVolume* MCPQuadSetLogical;
		G4VPhysicalVolume* MCPQuadSetPhysical0;
		G4VPhysicalVolume* MCPQuadSetPhysical1;
		G4VPhysicalVolume* MCPQuadSetPhysical2;
		G4VPhysicalVolume* MCPQuadSetPhysical3;
		G4VPhysicalVolume* MCPQuadSetPhysical4;
		G4VPhysicalVolume* MCPQuadSetPhysical5;

		G4Box* MCPRowSolid;               	
		G4LogicalVolume* MCPRowLogical;
		G4VPhysicalVolume* MCPRowPhysical;

		G4Box* MCPSolid;
		G4LogicalVolume* MCPLogical;
		G4VPhysicalVolume* MCPPhysical;

		G4Box* MCPActiveAreaSolid;               	
		G4LogicalVolume* MCPActiveAreaLogical;
		G4VPhysicalVolume* MCPActiveAreaPhysical;

		G4Box* MCPPixelVolumeAreaSolid;               	
		G4LogicalVolume* MCPPixelVolumeAreaLogical;
		G4VPhysicalVolume* MCPPixelVolumeAreaPhysical;

		G4Box* pixelVolumeRowSolid;
		G4LogicalVolume* pixelVolumeRowLogical;
		G4VPhysicalVolume* pixelVolumeRowPhysical;

		G4Box* pixelVolumeSolid;               						
		G4LogicalVolume* pixelVolumeLogical;   						
		G4VPhysicalVolume* pixelVolumePhysical;						

		G4Box* pixelSolid;
		G4LogicalVolume* pixelLogical;
		G4VPhysicalVolume* pixelPhysical;

		//
		// This is used to confirm MCP orientation.
		// Only used for testing purposes.
		//
		//G4Box* testMarkerSolid;
		//G4LogicalVolume* testMarkerLogical;
		//G4VPhysicalVolume* testMarkerPhysical;

		//
		// Sensitive detector.
		//
		G4SDManager* SDman;
		static MTCG4PixelSD* aPixelSD;

		//
		// Messenger.
		//
		MTCG4DetectorMessenger* detectorMessenger;
};

#endif
