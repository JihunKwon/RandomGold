#ifndef BGMSCSTEPPINGACTION_H
#define BGMSCSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

class PrimaryGeneratorAction;

class BGMSCSteppingAction : public G4UserSteppingAction {

public:
    BGMSCSteppingAction()
    {
        i = 0;
        Edep = 0;
    }

    ~BGMSCSteppingAction()
    {
        G4cout << "Number of ionization is " << i << G4endl;
        G4cout << "Total energy deposition is " << Edep/MeV << G4endl;
    }

    void UserSteppingAction(const G4Step* aStep);

private:
    G4int i;
    G4double Edep;
};

#endif
