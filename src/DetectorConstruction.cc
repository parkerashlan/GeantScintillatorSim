

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SystemOfUnits.hh"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/Rotation.h"

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
		
	G4double pressure    = 3.e-18*pascal;
	G4double temperature = 2.73*kelvin;
	density     = 1.e-25*g/cm3;
		
	G4Material* Air = new G4Material(name="Galactic", z=1., a=1.01*g/mole, 
											density,kStateGas,temperature,pressure);
//	G4String name, symbol;             //a=mass of a mole;
//	G4double density;            //z=mean number of protons;
//
//	G4int ncomponents;
//
//	G4Material* Air= nist->FindOrBuildMaterial("G4_AIR");

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
	BC408->AddMaterial(H, 8.4*perCent);
	BC408->AddMaterial(C, 91.6*perCent); //derived from the ratio of H to C on Gobain Crystals Datasheet



	//------------------------------------------------------
	// Optical properties
	//------------------------------------------------------

	G4double photonEnergy[] = {2.72*eV, 2.75*eV, 2.78*eV, 2.82*eV, 2.84*eV,
			                   2.88*eV, 2.92*eV, 2.95*eV, 2.98*eV, 3.02*eV}; //derived from 

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

	G4double WorldSize= 5.*m;
	
	G4Box* solidWorld = new G4Box("World",
								  WorldSize,
								  WorldSize,
								  WorldSize);

//	G4Sphere* solidWorld = new G4Sphere("World",		       	                  //its name
//							   	   	  WorldSize,
//									  WorldSize + 3,
//									  0.*deg,
//									  180.*deg,
//									  0.*deg,
//									  180.*deg);  //its size

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

	G4double ScintHalfLength = 25.*mm;
	G4double ScintHalfHeight = 6.5*mm;

	G4Box* solidScintillator = new G4Box("Scintillator", ScintHalfLength,
			                         ScintHalfLength, ScintHalfHeight);

	G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator,
			                                                 BC408,
															 "Scintillator");

	G4ThreeVector positionScintillator = G4ThreeVector(0.*cm,0.*cm,0.*cm);
	
	G4RotationMatrix rotm = G4RotationMatrix();
	rotm.rotateX(90*deg);
	G4ThreeVector translation = G4ThreeVector(0., 0., 0.);
	G4Transform3D transform = G4Transform3D(rotm, translation);
	
//	G4RotationMatrix* RotMat = G4RotationMatrix(90. ,90. ,90. ,0. ,180. , 90.);
	
	//G4VPhysicalVolume* physScintillator = 
//	new G4PVPlacement(RotMat,                      //no rotation
//			                                                positionScintillator,   //position in world
//	  													    logicScintillator,      //logical volume
//															"Scintillator",         //its name
//															logicWorld,             //its mother (logical) volume
//															false,                  //no boolean operations
//															0);                     //its copy number
	
    new G4PVPlacement(transform,             //rotation,position
                      logicScintillator,            //its logical volume
                      "Scintillator",             //its name
                      logicWorld,             //its mother  volume
                      false,                 //no boolean operation
                      0,                 //copy number
                      0);       // checking overlaps 
	


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
















