#include "BGMSCSteppingAction.hh"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4CsvAnalysisManager.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include "G4CsvAnalysisManager.hh"

void BGMSCSteppingAction::UserSteppingAction(const G4Step* aStep)
{
    G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();

//    for (G4int i = 0; i < 200; i++){
//        InEnergy[i] = {0};
//        OutEnergy[i] = {0};
//    }

    if (aStep->GetPostStepPoint()->GetMaterial())
    {
        if ((aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
                (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni"))
            i = i+1;

        if (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_WATER")
        {
            Edep = Edep + aStep->GetTotalEnergyDeposit();

            if ((aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-") &&
                    (aStep->GetTrack()->GetParentID() != 0))
            {
                Edepout = Edepout + aStep->GetTotalEnergyDeposit();
            }
        }

//        if ((aStep->GetPreStepPoint()->GetMaterial()->GetName() == "G4_WATER") &&
//                (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
//                (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-"))    //if electron come into the GNP
//        {
//            G4double eEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
//            //G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
//            analysisManager->FillH1(0, eEnergy);
//            for (G4int j = 0; j < 200; j++){
//                if ((j < eEnergy) && (eEnergy <= (j+1)))
//                {
//                    InEnergy[j] = InEnergy[j] + 1;
//                    break;
//                }
//            }
//        }

//        if ((aStep->GetPreStepPoint()->GetMaterial()->GetName() == "G4_Au") &&
//                (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_WATER") &&
//                (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-"))    // if electron escape GNP
//        {
//            G4double eOutEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
//            //G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
//            //analysisManager->FillH1(1, eOutEnergy);
//            for (G4int j = 0; j < 200; j++){
//                if ((j < eOutEnergy) && (eOutEnergy <= (j+1)))
//                {
//                    OutEnergy[j] = OutEnergy[j] + 1;
//                    break;
//                }
//            }
//        }

        if (aStep->GetPostStepPoint()->GetMaterial()->GetName() == "G4_Au")
        {
            if (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-")
                SelfAbsE = SelfAbsE + aStep->GetTotalEnergyDeposit();
            if (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "gamma")
                SelfAbsG = SelfAbsG + aStep->GetTotalEnergyDeposit();
        }
    }

    if (aStep->GetTrack()->GetVolume()->GetName() == "NanoTubsPhys")
    {
        if (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-")
        {
            if (aStep->IsFirstStepInVolume())
            {
                G4double eEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
                analysisManager->FillH1(0, eEnergy);
            }
            if (aStep->IsLastStepInVolume())
            {
                G4double eOutEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
                analysisManager->FillH1(1, eOutEnergy);
            }
        }
    }
//    std::ofstream WriteInEnergy("Electron_InEdist.txt");
//    for (G4int k = 0; k < 200; k++){
//        WriteInEnergy << k << " " << InEnergy[k] << "\n";
//    }
//    std::ofstream WriteOutEnergy("Electron_OutEdist.txt");
//    for (int k = 0; k < 200; k++){
//        WriteOutEnergy << k << " " << OutEnergy[k] << "\n";
//    }

}
