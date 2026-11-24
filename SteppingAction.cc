#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particle = track->GetDefinition();
    
    // 只处理光子
    if (particle != G4Gamma::GammaDefinition())
        return;

    // 检测是否在 LiF 晶体中
    auto volume = track->GetVolume();
    if (volume && volume->GetName() == "LiF_Crystal")
    {
        // 光子打到 LiF 晶体
        fEventAction->SetHitLiF(true);
        
        // 记录能量沉积
        G4double edep = step->GetTotalEnergyDeposit();
        fEventAction->AddEdep(edep);
    }

    // 记录光子方向角（如果需要）
    G4ThreeVector momDir = track->GetMomentumDirection();
    fEventAction->g_theta = momDir.theta();
}