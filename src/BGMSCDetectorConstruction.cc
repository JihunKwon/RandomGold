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
#include "Randomize.hh"
#include "G4UserLimits.hh"

#include "math.h"
using namespace CLHEP;

///////////////////////////////////////////////////////////////////
#define CELL_SIDE (12.6 * um)
#define CLUSTER_SIDE (4.6 * um)
#define VESICLE_COUNT 164
#define VESICLE_DIAM (260 * nm)
#define GNP_DIAM (50 * nm) // diameter!!
#define GNP_COUNT 1000
///////////////////////////////////////////////////////////////////

BGMSCDetectorConstruction::BGMSCDetectorConstruction()
    :fStepLimit(NULL)
{
    m_dCellSide = CELL_SIDE;
    m_nGnpCount = GNP_COUNT;
    m_strDistribution = "Cluster";
}

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

    // Build materials
    G4NistManager* nistManager = G4NistManager::Instance();
    G4Material* Au = nistManager->FindOrBuildMaterial("G4_Au");
    G4Material* vacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4Material* water = nistManager->FindOrBuildMaterial("G4_WATER");

    // Define Cell
    G4Box* pCellBox = new G4Box("CellBox", CELL_SIDE/2, CELL_SIDE/2, CELL_SIDE/2);
    G4LogicalVolume *pCellLogic = new G4LogicalVolume(pCellBox, water, "CellLog");
    G4VPhysicalVolume *pCellPhys = new G4PVPlacement(0, G4ThreeVector(), pCellLogic, "CellLog", 0, false, 0);

    if (m_strDistribution == "Homogeneous")
    {
        DistributeGnpsRandom(pCellLogic);
    }
    else if(m_strDistribution == "Cluster")
    {
        SVesicleInfo aryVesicleInfo[VESICLE_COUNT];
        DistributeVesiclesInSphereCluster(pCellLogic, aryVesicleInfo);
        DistributeGnpsInVesicles(pCellLogic, aryVesicleInfo);
    }
    else if(m_strDistribution == "None")
    {
    }
    else
    {
        G4Exception("Invalid distribution name", "001", G4ExceptionSeverity::FatalException, "Comment");
    }

    G4double maxStep = 1*nm;
    fStepLimit = new G4UserLimits(maxStep);

    return pCellPhys;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
///////////////////////////HOMOGENEOUS////////////////////////////////////////
// Distribute GNP randomly in the cell (without vesicles).
void BGMSCDetectorConstruction::DistributeGnpsRandom(G4LogicalVolume *pCellLog)
{
    struct SGnpInfo
    {
        double dPosX;
        double dPosY;
        double dPosZ;
    };

    SGnpInfo *aryGnpInfo = new SGnpInfo [m_nGnpCount];

    G4Material *pMaterialGold = G4NistManager::Instance()->FindOrBuildMaterial("G4_Au");
    assert(pMaterialGold != NULL);

    // Set visible attributes
    G4VisAttributes* pVisAttributes = new G4VisAttributes;
    pVisAttributes->SetForceWireframe(true);
    pVisAttributes->SetForceAuxEdgeVisible(true);
    pVisAttributes->SetForceSolid(false);
    pVisAttributes->SetVisibility(true);
    pVisAttributes->SetColor(255. / 255., 215. / 255., 0.); // gold

    // Create the Sphere object
    G4Sphere* pGnpSphere = new G4Sphere("GNP", 0., GNP_DIAM / 2, 0*deg, 360*deg, 0*deg, 180*deg);
    G4LogicalVolume *pGnpLog = new G4LogicalVolume(pGnpSphere, pMaterialGold, "GNPLogic");

    printf("Distributing GNPs randomly...");

    for (int nGnpIdx = 0; nGnpIdx < m_nGnpCount; nGnpIdx++)
    {
        retry:
        // Compute a random position for the GNP
        double dGnpX = (CELL_SIDE * G4UniformRand()) - CELL_SIDE/2.0; //set x between -6.3um and 6.3um
        double dGnpY = (CELL_SIDE * G4UniformRand()) - CELL_SIDE/2.0;
        double dGnpZ = (CELL_SIDE * G4UniformRand()) - CELL_SIDE/2.0;

                // Special case for one GNP at the center.
                if (m_nGnpCount == 1)
                {
                    dGnpX = 0.0;
                    dGnpY = 0.0;
                    dGnpZ = 0.0;
                }

                // Check if this GNP is over-lapping an existing GNP
                for (int nExistingGnpIdx = 0; nExistingGnpIdx < nGnpIdx; nExistingGnpIdx++)
                {
                    double dExistingX = aryGnpInfo[nExistingGnpIdx].dPosX;
                    double dExistingY = aryGnpInfo[nExistingGnpIdx].dPosY;
                    double dExistingZ = aryGnpInfo[nExistingGnpIdx].dPosZ;

                    double dx = dExistingX - dGnpX;
                    double dy = dExistingY - dGnpY;
                    double dz = dExistingZ - dGnpZ;

                    // Don't bother computing sqrt if the distance is large on any axis.
                    //if ((fabs(dx) <= GNP_DIAM) && (fabs(dy) <= GNP_DIAM) && (fabs(dz) <= GNP_DIAM))
                    if (fabs(dx) > GNP_DIAM) continue;
                    if (fabs(dy) > GNP_DIAM) continue;
                    if (fabs(dz) > GNP_DIAM) continue;

                    double dDist = sqrt(dx*dx + dy*dy + dz*dz);
                    if (dDist <= GNP_DIAM)
                        goto retry;
                }
                printf("%d (%le, %le, %le)\n", nGnpIdx, dGnpX, dGnpY, dGnpZ);

                int nCopyNumber = nGnpIdx; //Why?

                new G4PVPlacement(0, G4ThreeVector(dGnpX, dGnpY, dGnpZ), pGnpLog, "GnpPhys", pCellLog, false, nCopyNumber);

                pGnpLog->SetVisAttributes(pVisAttributes);

                aryGnpInfo[nGnpIdx].dPosX = dGnpX; //Why?
                aryGnpInfo[nGnpIdx].dPosY = dGnpY;
                aryGnpInfo[nGnpIdx].dPosZ = dGnpZ;
    }
    delete [] aryGnpInfo;
}

///////////////////////////CLUSTER////////////////////////////////////////
void BGMSCDetectorConstruction::DistributeVesiclesInSphereCluster(G4LogicalVolume* pCellLog, SVesicleInfo aryVesicleInfo[])
{
    G4Material* pMaterialWater = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER"); //"G4_Au"
    assert(pMaterialWater != NULL);

    // Set visible attributes
    G4VisAttributes* pVisClusterAttributes = new G4VisAttributes;
    pVisClusterAttributes->SetForceWireframe(true);
    pVisClusterAttributes->SetForceAuxEdgeVisible(true);
    pVisClusterAttributes->SetForceSolid(false);
    pVisClusterAttributes->SetVisibility(true);
    pVisClusterAttributes->SetColor(1.0, 1.0, 1.0);

    G4VisAttributes* pVisVesicleAttributes = new G4VisAttributes;
    pVisVesicleAttributes->SetForceWireframe(true);
    pVisVesicleAttributes->SetForceAuxEdgeVisible(true);
    pVisVesicleAttributes->SetForceSolid(false);
    pVisVesicleAttributes->SetVisibility(true);
    pVisVesicleAttributes->SetColor(0.0, 1.0, 1.0);

    double dClusterRadius = CLUSTER_SIDE / 2.;
    double dClusterX = 0.0;
    double dClusterY = 0.0;
    double dClusterZ = 0.0;

    for (int nVesicleIdx = 0; nVesicleIdx < VESICLE_COUNT; nVesicleIdx++)
    {
        retry:
        // Generate a random point on the vesicle
        double dTheta = G4UniformRand() * 360.*deg;
        double dPhi = (180.*deg * G4UniformRand());
        double dRho = dClusterRadius * G4UniformRand();
        double dVesicleX = dClusterX + dRho*cos(dTheta)*sin(dPhi);
        double dVesicleY = dClusterY + dRho*sin(dTheta)*sin(dPhi);
        double dVesicleZ = dClusterZ + dRho*cos(dPhi);

        // Check if this vesicle is over-lapping an existing vesicle.
        for (int nExistingIdx = 0; nExistingIdx < nVesicleIdx; nExistingIdx++)
        {
            double dExistingX = aryVesicleInfo[nExistingIdx].dPosX;
            double dExistingY = aryVesicleInfo[nExistingIdx].dPosY;
            double dExistingZ = aryVesicleInfo[nExistingIdx].dPosZ;

            double dx = dExistingX - dVesicleX;
            double dy = dExistingY - dVesicleY;
            double dz = dExistingZ - dVesicleZ;

            double dDist = sqrt(dx*dx+dy*dy+dz*dz);
            if(dDist <= VESICLE_DIAM)
                goto retry;
        }

        // OK, this position is good!
        printf("%d (%le, %le, %le)\n", nVesicleIdx, dVesicleX, dVesicleY, dVesicleZ);




        ///////////////////////Visualization of cluster
        G4Sphere* pClusterSphere = new G4Sphere("Cluster", 0, (CLUSTER_SIDE/2), 0.*deg, 360.*deg, 0.*deg, 180.*deg);
        G4LogicalVolume* pClusterLog = new G4LogicalVolume(pClusterSphere, pMaterialWater, "Cluster");

        G4RotationMatrix* rotm = new G4RotationMatrix;
        rotm->rotateX(90*deg);

        new G4PVPlacement(rotm, G4ThreeVector(0, 0, 0), pClusterLog, "Cluster", pCellLog, false, 0);

        pClusterLog->SetVisAttributes(pVisClusterAttributes);
        ///////////////////////Visualization of cluster

        ///////////////////////Visualization of vesicles
        G4Sphere* pVesicleSphere = new G4Sphere("Vesicle", 0, (VESICLE_DIAM/2), 0.*deg, 360.*deg, 0.*deg, 180.*deg);
        G4LogicalVolume* pVesicleLog = new G4LogicalVolume(pVesicleSphere, pMaterialWater, "Vesicle");

        int nCopyNumber = nVesicleIdx;
        //G4RotationMatrix* rotm = new G4RotationMatrix;
        rotm->rotateX(90*deg);

        new G4PVPlacement(rotm, G4ThreeVector(dVesicleX, dVesicleY, dVesicleZ), pVesicleLog, "Vesicle", pCellLog, false, nCopyNumber);

        pVesicleLog->SetVisAttributes(pVisVesicleAttributes);
        ///////////////////////Visualization of vesicles


        aryVesicleInfo[nVesicleIdx].dPosX = dVesicleX; //Pass the Vesicle Position to the array and in order to access from other functions.
        aryVesicleInfo[nVesicleIdx].dPosY = dVesicleY;
        aryVesicleInfo[nVesicleIdx].dPosZ = dVesicleZ;
    }

}

void BGMSCDetectorConstruction::DistributeGnpsInVesicles(G4LogicalVolume *pCellLog, SVesicleInfo aryVesicleInfo[])
{
    struct SGnpInfo
    {
        double dPosX;
        double dPosY;
        double dPosZ;
        int nVesicleIdx;
    };

    SGnpInfo *aryGnpInfo = new SGnpInfo [m_nGnpCount];

    G4Material *pMaterialGold = G4NistManager::Instance()->FindOrBuildMaterial("G4_Au");
    assert(pMaterialGold != NULL);

    // Set visible attributes
    G4VisAttributes* pVisAttributes = new G4VisAttributes;
    pVisAttributes->SetForceWireframe(true);
    pVisAttributes->SetForceAuxEdgeVisible(true);
    pVisAttributes->SetForceSolid(false);
    pVisAttributes->SetVisibility(true);
    pVisAttributes->SetColor(255. / 255., 215. / 255., 0.); // gold

    for (int nGnpIdx = 0; nGnpIdx < m_nGnpCount; nGnpIdx++)
    {
        retry:
        // Select a vesicle at random
        int nVesicleIdx = int(G4UniformRand() * VESICLE_COUNT);
        double dVesicleX = aryVesicleInfo[nVesicleIdx].dPosX;
        double dVesicleY = aryVesicleInfo[nVesicleIdx].dPosY;
        double dVesicleZ = aryVesicleInfo[nVesicleIdx].dPosZ;

        // Generate a random on the vesicle
        // NOTE: Distribute throughout the vessicle, not only on surface.
        double dTheta = G4UniformRand() * 360.*deg;
        double dPhi = (180.*deg * G4UniformRand());
        double dVesicleRadius = VESICLE_DIAM / 2. - GNP_DIAM / 2.; //Make sure the GNPs don't exeed the vesicle surface (GNP_DIAM/2)
        double dGnpX = dVesicleX + dVesicleRadius*cos(dTheta)*sin(dPhi);
        double dGnpY = dVesicleY + dVesicleRadius*sin(dTheta)*sin(dPhi);
        double dGnpZ = dVesicleZ + dVesicleRadius*cos(dPhi);

        // Check if this GNP is over-lapping an exisitng GNP
        for (int nExistingGnpIdx = 0; nExistingGnpIdx < nGnpIdx; nExistingGnpIdx++)
        {
            // Focus on the same vesicle
            int nExistingVesicleIdx = aryGnpInfo[nExistingGnpIdx].nVesicleIdx;
            if (nExistingVesicleIdx == nVesicleIdx)
            {
                double dExistingX = aryGnpInfo[nExistingGnpIdx].dPosX;
                double dExistingY = aryGnpInfo[nExistingGnpIdx].dPosY;
                double dExistingZ = aryGnpInfo[nExistingGnpIdx].dPosZ;

                double dx = dExistingX - dGnpX;
                double dy = dExistingY - dGnpY;
                double dz = dExistingZ - dGnpZ;

                // Don't bother computing sqrt if the distance is large on any axis.
                if (fabs(dx) > GNP_DIAM) continue;
                if (fabs(dy) > GNP_DIAM) continue;
                if (fabs(dz) > GNP_DIAM) continue;

                double dDist = sqrt(dx*dx+dy*dy+dz*dz);
                if (dDist <= GNP_DIAM)
                    goto retry;
            }

        }
        printf("%d %d (%le, %le, %le)\n", nGnpIdx, nVesicleIdx, dGnpX, dGnpY, dGnpZ);

        G4Sphere* pGnpSphere = new G4Sphere("GNP", 0., GNP_DIAM/2, 0*deg, 360*deg, 0*deg, 180*deg);
        G4LogicalVolume *pGnpLog = new G4LogicalVolume(pGnpSphere, pMaterialGold, "GNPLogic");

        int nCopyNumber = nGnpIdx;

        G4RotationMatrix* rotm = new G4RotationMatrix;
        rotm->rotateX(180*deg);

        new G4PVPlacement(0, G4ThreeVector(dGnpX, dGnpY, dGnpZ), pGnpLog, "GnpPhys", pCellLog, false, nCopyNumber);

        pGnpLog->SetVisAttributes(pVisAttributes);

        aryGnpInfo[nGnpIdx].dPosX = dGnpX;
        aryGnpInfo[nGnpIdx].dPosY = dGnpY;
        aryGnpInfo[nGnpIdx].dPosZ = dGnpZ;
        aryGnpInfo[nGnpIdx].nVesicleIdx = nVesicleIdx;
    }
    delete [] aryGnpInfo;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BGMSCDetectorConstruction::SetMaxStep(G4double maxStep)
{
    if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}
