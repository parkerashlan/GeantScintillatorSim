

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
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
#include "G4LogicalSkinSurface.hh"

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
	G4double density;            //z=mean number of protons;

	G4int ncomponents;

	G4Material* Air= nist->FindOrBuildMaterial("G4_AIR");

	//-----------------------------------------------------
	// Define Simple Elements
	//-----------------------------------------------------

	//H
	G4Material* H = nist->FindOrBuildMaterial("G4_H");

	//C
	G4Material* C = nist->FindOrBuildMaterial("G4_C");

	//----------------------------------------------------
	// Define SiPM material
	//----------------------------------------------------

	//Silicon
	G4Material* Silicon = nist->FindOrBuildMaterial("G4_Si");


	//-----------------------------------------------------
	// Define Scintillator Puck Material
	//-----------------------------------------------------
	density = 1.023*g/cm3;
	G4Material* BC408 = new G4Material(name="BC408", density, ncomponents = 2);
	BC408->AddMaterial(H, 8.4*perCent);
	BC408->AddMaterial(C, 91.6*perCent); //derived from the ratio of H to C on Gobain Crystals Datasheet

	//-----------------------------------------------------
	// Define Wrapping for Scintillator
	//-----------------------------------------------------

	G4Material *Teflon = nist->FindOrBuildMaterial("G4_TEFLON");



	//------------------------------------------------------
	// Optical properties
	//------------------------------------------------------

	G4double photonEnergy[] = {2.72*eV, 2.75*eV, 2.78*eV, 2.82*eV, 2.84*eV,
			                   2.88*eV, 2.92*eV, 2.95*eV, 2.98*eV, 3.02*eV}; //derived from spectrum found on datasheet, used FWHM

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

	// Air

	G4double airRefractionIndex[nEntries] = {1.0,1.0,1.0,1.0,1.0,
			                                 1.0,1.0,1.0,1.0,1.0};
	G4double airReflectivity[nEntries] = {0. , 0. ,0. ,0. ,0. ,
										  0. ,0. ,0. ,0. ,0. };


	G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
	airMPT->AddProperty("RINDEX",photonEnergy,airRefractionIndex,
							nEntries);
	airMPT->AddProperty("REFLECTIVITY", photonEnergy, airReflectivity, nEntries);


	Air->SetMaterialPropertiesTable(airMPT);

	//SiPM

	G4double SiRefractionIndex[nEntries] = {3.97, 3.97,3.97,3.97,3.97,
											3.97,3.97,3.97,3.97,3.97};

	G4double SiAbsorptionLength[nEntries] = {0.01*mm, 0.01*mm,0.01*mm,0.01*mm,0.01*mm,
			                                 0.01*mm,0.01*mm,0.01*mm,0.01*mm,0.01*mm};

	G4double SiReflectivity[nEntries] = {0. , 0. ,0. ,0. ,0. ,
										  0. ,0. ,0. ,0. ,0. };

	G4MaterialPropertiesTable* siMPT = new G4MaterialPropertiesTable();

	siMPT->AddProperty("RINDEX", photonEnergy, SiRefractionIndex, nEntries);
	siMPT->AddProperty("ABSLENGTH", photonEnergy, SiAbsorptionLength, nEntries);
	//siMPT->AddProperty("REFLECTIVITY", photonEnergy, SiReflectivity, nEntries);

	Silicon->SetMaterialPropertiesTable(siMPT);

	//Wrapper

	G4double TefRefractionIndex[nEntries] = {1.32,1.32,1.32,1.32,1.32,
			 	 	 	 	 	 	 	 	 1.32,1.32,1.32,1.32,1.32};

	G4double TefAbsorptionLength[nEntries] = {94*cm, 94*cm, 94*cm, 94*cm, 94*cm,
											 94*cm, 94*cm, 94*cm, 94*cm, 94*cm};

	G4double TefReflectivity[nEntries] = {0. , 0. ,0. ,0. ,0. ,
										  0. ,0. ,0. ,0. ,0. };

	G4MaterialPropertiesTable* TefMPT = new G4MaterialPropertiesTable();

	TefMPT->AddProperty("RINDEX", photonEnergy, TefRefractionIndex, nEntries);
	TefMPT->AddProperty("ABSLENGTH", photonEnergy, TefAbsorptionLength, nEntries);
	//TefMPT->AddProperty("REFLECTIVITY", photonEnergy, TefReflectivity, nEntries);

	Teflon->SetMaterialPropertiesTable(TefMPT);

	//----------------------------------------------------
	//Detector Geometry
	//----------------------------------------------------

	// World

	G4double WorldSize= 25.*cm;
	
	G4Box* solidWorld = new G4Box("World",
								  WorldSize,
								  WorldSize,
								  WorldSize);


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

	// Scintillator

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
	
//    G4VPhysicalVolume* physScint = new G4PVPlacement(0,
//    		                                        positionScintillator,
//													"Scintillator",
//													logicScintillator,
//													physWorld,
//													false,
//													0);

    G4VPhysicalVolume* physScint = new G4PVPlacement(transform,             //rotation,position
                      	  	  	  	  	  	         logicScintillator,            //its logical volume
													 "Scintillator",             //its name
													 logicWorld,             //its mother  volume
													 false,                 //no boolean operation
													 0,                 //copy number
													 0);       // checking overlaps

    // SiPM Detector


    G4double SiPMHalfLength = 1.5*mm;
    G4double SiPMHalfHeight = 0.25*mm;

    G4Box* solidSiPM = new G4Box("SiPM", SiPMHalfLength, SiPMHalfLength, SiPMHalfHeight);

    G4Box* backSiPM = new G4Box("BackSiPM", SiPMHalfLength, SiPMHalfLength, SiPMHalfHeight);

    G4LogicalVolume* logicbackSiPM = new G4LogicalVolume(solidSiPM, Silicon, "BackSiPM");

    G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM, Silicon, "SiPM");

    G4ThreeVector positionSiPM = G4ThreeVector(25.5*mm, 0.*mm, 15.*mm);


//    G4VPhysicalVolume* physSiPM = new G4PVPlacement(0,
//    		                                        positionSiPM,
//													"SiPM",
//													logicSiPM,
//													physWorld,
//													false,
//													0);
    G4RotationMatrix rotmSiPM = G4RotationMatrix();

    rotmSiPM.rotateY(90.*deg);

    G4ThreeVector translateSiPM = G4ThreeVector(25.25*mm, 0.*mm, 15.*mm);

    G4ThreeVector translatebackSiPM = G4ThreeVector(25.75*mm, 0.*mm, 15.*mm);

    G4Transform3D transformbackSiPM = G4Transform3D(rotmSiPM, translatebackSiPM);

    G4Transform3D transformSiPM = G4Transform3D(rotmSiPM, translateSiPM);

    G4VPhysicalVolume* physSiPM = new G4PVPlacement(transformSiPM,             //rotation,position
                      	  	  	  	  	  	         logicSiPM,            //its logical volume
													 "SiPM",             //its name
													 logicWorld,             //its mother  volume
													 false,                 //no boolean operation
													 0,                 //copy number
													 0);       // checking overlaps

    G4VPhysicalVolume* physbackSiPM = new G4PVPlacement(transformbackSiPM,             //rotation,position
                      	  	  	  	  	  	         logicbackSiPM,            //its logical volume
													 "BackSiPM",             //its name
													 logicWorld,             //its mother  volume
													 false,                 //no boolean operation
													 0,                 //copy number
													 0);       // checking overlaps

    // Wrapper

    G4double wrapperhalflength = 30.*mm;
    G4double wrapperheight = 11.5*mm;

    G4Box* outerWrapper = new G4Box("innerWrapper", wrapperhalflength, wrapperhalflength, wrapperheight);
    G4Box* innerWrapper = new G4Box("outerWrapper", ScintHalfLength+0.01*mm, ScintHalfLength+0.01*mm, ScintHalfHeight+0.01*mm);
    G4SubtractionSolid* WrappernoCav = new G4SubtractionSolid("WrapperNoCav", outerWrapper, innerWrapper);

    G4double lenCavity = 2.5*mm;
    G4ThreeVector tranCavity = G4ThreeVector(24.*mm, 15.*mm, 0.*mm);

    G4Box* SiPMCavity = new G4Box("SiPMCavity", lenCavity, 4.*mm, ScintHalfHeight-2);
    G4SubtractionSolid* solidWrapper = new G4SubtractionSolid("Wrapper", WrappernoCav, SiPMCavity, 0, tranCavity);

    G4LogicalVolume* logicWrapper = new G4LogicalVolume(solidWrapper, Teflon, "Wrapper");

    G4VPhysicalVolume* physWrapper = new G4PVPlacement(transform,             //rotation,position
                      	  	  	  	  	  	         logicWrapper,            //its logical volume
													 "Wrapper",             //its name
													 logicWorld,             //its mother  volume
													 false,                 //no boolean operation
													 0,                 //copy number
													 0);       // checking overlaps

	


	//------------------------------------------------------
	// Surfaces and boundary processes
	//------------------------------------------------------

	// Air - scintillator surface
//
//	G4OpticalSurface* AirScintSurface =
//	new G4OpticalSurface("AirScintSurface");
//
//	AirScintSurface->SetType(dielectric_dielectric);
//	AirScintSurface->SetModel(unified);
//	AirScintSurface->SetFinish(polished);
//
//	new G4LogicalSkinSurface("AirScintSurface",logicScintillator, AirScintSurface);

//	//Scintillator Skin
//    G4OpticalSurface* SiPMSkin =
//    new G4OpticalSurface("SiPMSkin");
//
//    SiPMSkin->SetType(dielectric_metal);
//    SiPMSkin->SetModel(unified);
//    SiPMSkin->SetFinish(polished);
//
//    new G4LogicalSkinSurface("SiPMSkin", logicSiPM, SiPMSkin);

	 //SiPM - Scint
	G4OpticalSurface* SiPMScintSurface =
	new G4OpticalSurface("SiPMScintSurface");

	SiPMScintSurface->SetType(dielectric_metal);
	SiPMScintSurface->SetModel(unified);
	SiPMScintSurface->SetFinish(polished);

	new G4LogicalBorderSurface("SiPMScintSurface",physScint,physSiPM,SiPMScintSurface);



	//------------------------------------------------------
	// visualization attributes
	//------------------------------------------------------

	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

	//Green color for scintillator crystal
	G4VisAttributes* Att1 = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logicScintillator->SetVisAttributes(Att1);

	//Yellow color for SiPM
	G4VisAttributes* Att2 = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
	logicSiPM->SetVisAttributes(Att2);

	//color for wrapper
	G4VisAttributes* Att3 = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
	logicWrapper->SetVisAttributes(Att3);

	//color for back of SiPM
	G4VisAttributes* Att4 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
	logicbackSiPM->SetVisAttributes(Att4);



	return physWorld;

}
















