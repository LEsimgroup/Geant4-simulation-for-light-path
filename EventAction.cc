#include "EventAction.hh"
#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"

EventAction::EventAction(RunAction* runAction)
 : G4UserEventAction(),
   fRunAction(runAction),
   fEdep(0.),
   theta(0.),
   fPassedSoller1(false),
   fPassedSoller2(false)
{
    tid = 0;
    g_theta = 0.;
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fEdep = 0.;
    fPassedSoller1 = false;
    fPassedSoller2 = false;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, fEdep);
    man->FillNtupleDColumn(1, theta);
    man->FillNtupleDColumn(2, tid);
    man->FillNtupleDColumn(3, g_theta);
    man->AddNtupleRow(0);
}

G4double EventAction::findtheta(int id)
{
    if (id < 5) theta = 157.6;
    else if (id < 15) theta = 133.57;
    else if (id < 25) theta = 107.94;
    else if (id < 30) theta = 94.16;
    else if (id < 35) theta = 85.84;
    else theta = 72.05;
    
    return theta;
}