#include "BGMSCPrimaryGeneratorAction.hh"

#include "G4Proton.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"

#include <G4SingleParticleSource.hh>

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#define radius 5 // Half of diameter!!

using namespace CLHEP;

BGMSCPrimaryGeneratorAction::BGMSCPrimaryGeneratorAction()
{
//    G4String fileName = "PSF_-125"; //Set this to your phsp file
//    IAEAReader = new G4IAEAphspReader(fileName);

//    //G4ThreeVector isoPos(-10.0*nm, 0*nm, 0*nm);
//    //IAEAReader->SetIsocenterPosition(isoPos);
//    IAEAReader->SetGlobalPhspTranslation(G4ThreeVector(0, 0, 125*mm-10*nm));
//    IAEAReader->SetTimesRecycled(0);


        CircleSource = new G4SingleParticleSource;
        CircleSource->SetParticleDefinition(G4Electron::Definition());
        this->setEnergyToBeta();

//        G4SPSEneDistribution* energy = CircleSource->GetEneDist();
//        energy->SetMonoEnergy(10*MeV);

        G4SPSPosDistribution* position = CircleSource->GetPosDist();
        position->SetPosDisType("Plane");
        position->SetPosDisShape("Circle");
        position->SetRadius(radius*nm);
        position->SetCentreCoords(G4ThreeVector(0*m, 0*m,  -radius*nm));

        G4SPSAngDistribution* angular = CircleSource->GetAngDist();
        angular->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
        angular->DefineAngRefAxes("angref1", G4ThreeVector(-1.0,0.0,0.0));
        CircleSource->SetNumberOfParticles(1);

//        //Psudo Gamma
//        CircleSourceG = new G4SingleParticleSource;
//        CircleSourceG->SetParticleDefinition(G4Gamma::Definition());
//        this->setEnergyToGamma();

////        G4SPSEneDistribution* energyG = CircleSourceG->GetEneDist();
////        energyG->SetMonoEnergy(10*keV);

//        G4SPSPosDistribution* positionG = CircleSourceG->GetPosDist();
//        positionG->SetPosDisType("Plane");
//        positionG->SetPosDisShape("Circle");
//        positionG->SetRadius(radius*nm);
//        positionG->SetCentreCoords(G4ThreeVector(0*m, 0*m,  -radius*nm));

//        G4SPSAngDistribution* angularG = CircleSourceG->GetAngDist();
//        angularG->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
//        angularG->DefineAngRefAxes("angref1", G4ThreeVector(-1.0,0.0,0.0));
//        CircleSourceG->SetNumberOfParticles(1);

}

BGMSCPrimaryGeneratorAction::~BGMSCPrimaryGeneratorAction()
{
    delete CircleSource;
    //delete CircleSourceG;
//    if (IAEAReader)
//        delete IAEAReader;
    //delete fParticleGun;
}

void BGMSCPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    //CircleSource->SetNumberOfParticles(3);

        CircleSource->GeneratePrimaryVertex(event);   //Changed
        //CircleSourceG->GeneratePrimaryVertex(event);
        //fParticleGun->GeneratePrimaryVertex(event);
//    IAEAReader->GeneratePrimaryVertex(event);
//    std::vector<G4ThreeVector>* pos = IAEAReader->GetPositionVector();
//    G4ThreeVector position = (*pos)[0];
//    std::ofstream outfile;

}


G4SPSEneDistribution* BGMSCPrimaryGeneratorAction::setEnergyToBeta() {
    G4SPSEneDistribution* eneDist = CircleSource->GetEneDist();
    eneDist->SetEnergyDisType("Arb");
    eneDist->ArbEnergyHistoFile("Electron_EnergyDistribution_allon_10nm_100keVX-ray_2.115_10^9.txt");
    eneDist->ArbInterpolate("Lin");
    return eneDist;
}

//G4SPSEneDistribution* BGMSCPrimaryGeneratorAction::setEnergyToGamma() {
//    G4SPSEneDistribution* eneDistG = CircleSourceG->GetEneDist();
//    eneDistG->SetEnergyDisType("Arb");
//    eneDistG->ArbEnergyHistoFile("Gamma_EnergyDistribution_20nm_100keVX-ray_allon_2115_10^6.txt");
//    eneDistG->ArbInterpolate("Lin");
//    return eneDistG;
//}




