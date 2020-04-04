

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction()
{ }

DetectorConstruction::~DetectorConstruction()
{ }

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Get nist material manager
	G4NistManager* nist = G4NistManager::Instance();
	
	//----------------------------------------------------
	// Material definitions
	//----------------------------------------------------

	G4String name, symbol;             //a=mass of a mole;
	G4double a, z, density;            //z=mean number of protons;

	G4int ncomponents, natoms;

	G4Material* Air= nist->FindOrBuildMaterial("G4_AIR");

	//-----------------------------------------------------
	// Define Simple Elements
	//-----------------------------------------------------



	//H
	G4Material* H = nist->FindOrBuildMaterial("G4_H");

	//C
	G4Material* C = nist->FindOrBuildMaterial("G4_C");


	//-----------------------------------------------------
	// Define Scintillator Puck Material
	//-----------------------------------------------------
	density = 1.023*g/cm3;
	G4Material* BC408 = new G4Material(name="BC408", density, ncomponents = 2);
	BC408->AddMaterial(H, 51.3*perCent);
	BC408->AddMaterial(C, 48.7*perCent);



	//------------------------------------------------------
	// Optical properties
	//------------------------------------------------------

	G4double photonEnergy[] = {2.72*eV, 2.75*eV, 2.78*eV, 2.82*eV, 2.84*eV,
			                   2.88*eV, 2.92*eV, 2.95*eV, 2.98*eV, 3.02*eV};

	const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);  //divide by size of data type to get the size of the actual list in bytes

	//BC408

	G4double BC408RefractionIndex[nEntries] = {1.58,1.58,1.58,1.58,1.58,
			                                   1.58,1.58,1.58,1.58,1.58,};

	G4double BC408AbsorptionLength[nEntries] = {210.*cm,210.*cm,210.*cm,210.*cm,210.*cm,
			                                    210.*cm,210.*cm,210.*cm,210.*cm,210.*cm};

	G4MaterialPropertiesTable* BC408MPT = new G4MaterialPropertiesTable();

	BC408MPT->AddProperty("RINDEX",photonEnergy,BC408RefractionIndex,
						  nEntries);
	BC408MPT->AddProperty("ABSLENGTH",photonEnergy,BC408AbsorptionLength,
						  nEntries);

	G4double ScintFast[nEntries] = { 1.00, 1.00, 1.00, 1.00, 1.00,
	                                 1.00, 1.00, 1.00, 1.00, 1.00};

	BC408MPT->AddProperty("FASTCOMPONENT",photonEnergy,ScintFast,nEntries);

	BC408MPT->AddConstProperty("SCINTILLATIONYIELD",10./keV);
	BC408MPT->AddConstProperty("RESOLUTIONSCALE",1.);
	BC408MPT->AddConstProperty("FASTTIMECONSTANT",2.1*ns);
	BC408MPT->AddConstProperty("YIELDRATIO",1.);

	BC408->SetMaterialPropertiesTable(BC408MPT);

	// Vacuum

	G4double airRefractionIndex[nEntries] = {1.0,1.0,1.0,1.0,1.0,
			                                 1.0,1.0,1.0,1.0,1.0};

	G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
	airMPT->AddProperty("RINDEX",photonEnergy,airRefractionIndex,
							nEntries);


	Air->SetMaterialPropertiesTable(airMPT);

	//----------------------------------------------------
	//Detector Geometry
	//----------------------------------------------------

	//
	// World
	//

	G4double WorldSize= 80.*cm;

	G4Box* solidWorld = new G4Box("World",		       	                  //its name
							   WorldSize/2,WorldSize/2,WorldSize/2);  //its size

	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,      	//its solid
										 Air,	        //its material
										 "World");		    //its name

	G4VPhysicalVolume* physWorld = new G4PVPlacement(0,			    //no rotation
									   G4ThreeVector(),	//at (0,0,0)
									   "World",		    //its name
									   logicWorld,		//its logical volume
									   NULL,		    //its mother  volume
									   false,	       	//no boolean operation
									   0);			    //copy number

	//
	// Detector
	//

	G4double ScintHalfLength = 25.*cm;
	G4double ScintHalfHeight = 6.5*cm;

	G4Box* solidScintillator = new G4Box("Scintillator", ScintHalfLength,
			                         ScintHalfLength, ScintHalfHeight);

	G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator,
			                                                 BC408,
															 "Scintillator");

	G4ThreeVector positionScintillator = G4ThreeVector(0.*cm,0.*cm,0.*cm);

	G4VPhysicalVolume* physScintillator = new G4PVPlacement(0,                      //no rotation
			                                                positionScintillator,   //position in world
															logicScintillator,      //logical volume
															"Scintillator",         //its name
															logicWorld,             //its mother (logical) volume
															false,                  //no boolean operations
															0);                     //its copy number


	//------------------------------------------------------
	// Surfaces and boundary processes
	//------------------------------------------------------

	// Reflector - scintillator surface

	G4OpticalSurface* OpRefCrySurface =
	new G4OpticalSurface("RefCrySurface");

	OpRefCrySurface->SetType(dielectric_dielectric);
	OpRefCrySurface->SetModel(glisur);
	OpRefCrySurface->SetFinish(polished);

	//------------------------------------------------------
	// visualization attributes
	//------------------------------------------------------

	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

	//Green color for scintillator crystal
	G4VisAttributes* Att1= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logicScintillator->SetVisAttributes(Att1);


	return physWorld;

}
















