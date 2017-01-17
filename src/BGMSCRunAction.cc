#include "BGMSCRunAction.hh"

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4TrackingManager.hh"

#include "G4CsvAnalysisManager.hh"
#include "G4IAEAphspWriter.hh"

#include "G4CsvAnalysisManager.hh"

#include <fstream>
#include <sstream>
#include <iostream>

#define _USE_MATH_DEFINES

BGMSCRunAction::BGMSCRunAction() : G4UserRunAction()
{}

BGMSCRunAction::~BGMSCRunAction()
{}

void BGMSCRunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
    G4IAEAphspWriter* Writer = G4IAEAphspWriter::GetInstance();
    //Writer->SetZStop(60*nm);
    Writer->SetRadius(25*nm);
    std::vector<double>* a = Writer->GetZStopVector();
    Writer->BeginOfRunAction(aRun);

    G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
    G4cout << " Histogram id is " <<
              analysisManager->CreateH1("Stokes", "Energy Eistribution of electron", 2500., 0., 250.) << G4endl;
}

void BGMSCRunAction::EndOfRunAction(const G4Run* aRun)
{
    G4IAEAphspWriter::GetInstance()->EndOfRunAction(aRun);
    G4CsvAnalysisManager* analysisManager = G4CsvAnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}
