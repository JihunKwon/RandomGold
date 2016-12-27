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
                (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "e-"))
        {
            G4double eEnergy = aStep->GetPostStepPoint()->GetKineticEnergy()/keV;
            G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
            analysisManager->FillH1(0, eEnergy);
        }

    }

    G4IAEAphspWriter::GetInstance()->UserSteppingAction(aStep);

}
