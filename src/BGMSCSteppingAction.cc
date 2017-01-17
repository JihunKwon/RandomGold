#include "BGMSCSteppingAction.hh"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4CsvAnalysisManager.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"
#include "G4IAEAphspWriter.hh"

#include "G4CsvAnalysisManager.hh"

void BGMSCSteppingAction::UserSteppingAction(const G4Step* aStep)
{    
    if (aStep->GetPostStepPoint()->GetMaterial())
    {
        if ((aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
                (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni"))
            i = i+1;

        if (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_WATER")
            Edep = Edep + aStep->GetTotalEnergyDeposit();

        if ((aStep->GetPreStepPoint()->GetMaterial()->GetName() == "G4_WATER") &&
                (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
                (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-"))    //if electron come into the GNP
        {
            G4double eEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
            G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
            analysisManager->FillH1(0, eEnergy);
        }

        G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " <<
                  aStep->GetTrack()->GetOriginTouchable()->GetVolume()->GetLogicalVolume()->GetName() << G4endl;//GetOriginTouchable()->GetVolume()->GetLogicalVolume()->GetName() << G4endl;

//        if (aStep->GetTrack()->GetOriginTouchable())
//        {

//            if ((aStep->GetPreStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
//                    (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_WATER") &&
//                    (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-") &&
//                    (aStep->GetTrack()->)
//            {
//                j = j+1;  //Number of electrons escaping bystander GNP
//                Edepout = Edepout + aStep->GetTotalEnergyDeposit();   //Energy deposition by electrons
//            }
//        }
//        else
//        {
//            G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " " <<
//                      aStep->GetTrack()->GetOriginTouchable()->GetVolume()->GetLogicalVolume() << G4endl;

//        }

    }

    G4IAEAphspWriter::GetInstance()->UserSteppingAction(aStep);

}
