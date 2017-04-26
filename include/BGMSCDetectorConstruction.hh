#ifndef BGMSCDETECTORCONSTRUCTION_H
#define BGMSCDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "tls.hh"

class BGMSCDetectorMessenger;
class DetectorMessenger;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4UserLimits;

using namespace CLHEP;

/////////////////////////////////////////////////////////////////////////////
struct SVesicleInfo
{
    double dPosX;
    double dPosY;
    double dPosZ;
    double dDiam;
};
/////////////////////////////////////////////////////////////////////////////

class BGMSCDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    BGMSCDetectorConstruction();
    ~BGMSCDetectorConstruction() {}

    G4VPhysicalVolume* Construct();

    void SetMaxStep (G4double );
    void SetDistribution(G4String& strDistribution);

 // void ConstructSDandField();
 // This method is used in multi-threaded applications to build
 // per-worker non-shared objects: SensitiveDetectors and Field managers

private:
    G4UserLimits* fStepLimit;   // pointer to user step limits

protected:
    DetectorMessenger* m_pDetectorMessenger;
    G4double m_dCellSide;
    G4double m_dClusterSide;
    G4int m_nGnpCount;
    G4String m_strDistribution;

    void DistributeGnpsRandom (G4LogicalVolume *pCubeLog);

    void DistributeVesiclesInSphereCluster(G4LogicalVolume* pCubeLog, SVesicleInfo aryVesicleInfo[]);
    void DistributeGnpsInVesicles(G4LogicalVolume* pCubeLog, SVesicleInfo aryVesicleInfo[]);
};

#endif

