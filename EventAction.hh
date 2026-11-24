#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class RunAction;

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void SetPassedSoller1(G4bool val) { fPassedSoller1 = val; }
    void SetPassedSoller2(G4bool val) { fPassedSoller2 = val; }
    G4bool GetPassedSoller1() const { return fPassedSoller1; }
    G4bool GetPassedSoller2() const { return fPassedSoller2; }

    void AddEdep(G4double edep) { fEdep += edep; }

    // 新增：记录是否打到 LiF 晶体
    void SetHitLiF(G4bool val) { fHitLiF = val; }
    G4bool GetHitLiF() const { return fHitLiF; }

    G4double findtheta(int id);
    
    int tid;
    G4double g_theta;

private:
    RunAction* fRunAction;
    G4double fEdep;
    G4double theta;
    G4bool fPassedSoller1;
    G4bool fPassedSoller2;
    G4bool fHitLiF;  // 新增
};

#endif