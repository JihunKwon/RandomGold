#include "BGMSCPrimaryGeneratorAction.hh"

#include "G4Proton.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"

#include <G4SingleParticleSource.hh>

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

        G4SPSEneDistribution* energy = CircleSource->GetEneDist();
        //energy->SetEnergyDisType("Gauss");
        //energy->SetBeamSigmaInE(0 *MeV);
        //energy->SetMonoEnergy(20*keV);

        G4SPSPosDistribution* position = CircleSource->GetPosDist();
        position->SetPosDisType("Plane");
        position->SetPosDisShape("Circle");
        position->SetRadius(10*nm);
        position->SetCentreCoords(G4ThreeVector(0*m, 0*m,  -60*nm));

        G4SPSAngDistribution* angular = CircleSource->GetAngDist();
        angular->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
        angular->DefineAngRefAxes("angref1", G4ThreeVector(-1.0,0.0,0.0));
        CircleSource->SetNumberOfParticles(1);
}

BGMSCPrimaryGeneratorAction::~BGMSCPrimaryGeneratorAction()
{
    delete CircleSource;
//    if (IAEAReader)
//        delete IAEAReader;
}

void BGMSCPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    //CircleSource->SetNumberOfParticles(3);

        CircleSource->GeneratePrimaryVertex(event);
//    IAEAReader->GeneratePrimaryVertex(event);
//    std::vector<G4ThreeVector>* pos = IAEAReader->GetPositionVector();
//    G4ThreeVector position = (*pos)[0];
//    std::ofstream outfile;

}


G4SPSEneDistribution* BGMSCPrimaryGeneratorAction::setEnergyToBeta() {
    G4SPSEneDistribution* eneDist = CircleSource->GetEneDist();
    eneDist->SetEnergyDisType("Arb");
    eneDist->ArbEnergyHistoFile("Electron_EnergyDistribution.txt");
    eneDist->ArbInterpolate("Lin");
    return eneDist;
}
