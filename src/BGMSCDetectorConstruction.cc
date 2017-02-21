#include "BGMSCDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4NistManager.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4VSolid.hh"
#include "G4Sphere.hh"
#include "G4UserLimits.hh"

#define radius 5 // Half of diameter!!
#define distance 100 //Distance between SourceGNP and BystanderGNP

using namespace CLHEP;


BGMSCDetectorConstruction::BGMSCDetectorConstruction()
    :fStepLimit(NULL)
{}

//BGMSCDetectorConstruction::~BGMSCDetectorConstruction()
//{
//  delete fStepLimit;
//}

G4VPhysicalVolume* BGMSCDetectorConstruction::Construct()
{
    // Cleanup old geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    G4VisAttributes* visAttributes = new G4VisAttributes;
    visAttributes->SetForceWireframe(true);
    visAttributes->SetForceAuxEdgeVisible(true);

    G4NistManager* nistManager = G4NistManager::Instance();
    G4Material* Au = nistManager->FindOrBuildMaterial("G4_Au");
    G4Material* vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4Material* water = nistManager->FindOrBuildMaterial("G4_WATER");

    // World
    G4Box* world = new G4Box("World", 0.5*um, 0.5*um, 0.5*um);   //halfx=0.05um
    G4LogicalVolume *worldLogic = new G4LogicalVolume(world, water, "WorldLogic");
    G4VPhysicalVolume *worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLogic, "WorldPhys", 0, false, 0);
    worldLogic->SetVisAttributes(visAttributes);

    // Nano Tubs
    G4Tubs* nanoTubs = new G4Tubs("NanoTubs", 0*nm, radius*nm, radius*nm, 0*deg, 360*deg);
    G4LogicalVolume *nanoTubsLogic = new G4LogicalVolume(nanoTubs, Au, "NanoTubsLogic");
    new G4PVPlacement(0, G4ThreeVector(0, 0, (2*radius+distance)*nm), nanoTubsLogic, "NanoTubsPhys", worldLogic, 0, 0);// (-490*nm, -490*nm, -490*nm) (0, 0, (2*radius+distance)*nm)
    //visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    nanoTubsLogic->SetVisAttributes((visAttributes));

//    // Pseudo disc source
//    G4Tubs* DiscSource = new G4Tubs("DiscSource", 0, radius*nm, 1.0*nm, 0*deg, 360*deg);
//    G4LogicalVolume *DiscSourceLogic = new G4LogicalVolume(DiscSource, vacuum, "DiscSourceLogic");
//    new G4PVPlacement(0, G4ThreeVector(0, 0, -(radius+1)*nm), DiscSourceLogic,"DiscSourcePhys", worldLogic, 0, 0);
//    DiscSourceLogic->SetVisAttributes(visAttributes);

//    // Bystander GNP
//    G4Tubs *nanoPart2 = new G4Tubs("NanoPart2", 0, radius*nm, radius*nm, 0*deg, 360*deg);
//    G4LogicalVolume *nanoPart2Logic = new G4LogicalVolume(nanoPart2, Au, "NanoPart2Logic");
//    new G4PVPlacement(0, G4ThreeVector(0, 0, 3*radius*nm), nanoPart2Logic, "NanoPart2Phys", worldLogic, 0, 0);
//    //visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
//    nanoPart2Logic->SetVisAttributes(visAttributes);

    G4double maxStep = 1*nm;
    fStepLimit = new G4UserLimits(maxStep);
    //nanoTubsLogic->SetUserLimits(fStepLimit);

    return worldPhys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BGMSCDetectorConstruction::SetMaxStep(G4double maxStep)
{
    if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}
