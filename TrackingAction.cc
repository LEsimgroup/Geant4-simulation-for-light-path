#include "TrackingAction.hh"
#include "EventAction.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4Trajectory.hh"

TrackingAction::TrackingAction(EventAction* eventAction)
: fEventAction(eventAction)
{}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // 每条轨迹开始时重置标志
    fEventAction->SetHitLiF(false);
    
    // 创建并存储轨迹
    fpTrackingManager->SetStoreTrajectory(true);
    fpTrackingManager->SetTrajectory(new G4Trajectory(track));
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    // 如果光子没有打到 LiF 晶体，删除这条轨迹
    if (!fEventAction->GetHitLiF())
    {
        G4VTrajectory* traj = fpTrackingManager->GetTrajectory();
        if (traj)
        {
            delete traj;
            fpTrackingManager->SetTrajectory(nullptr);
        }
    }
}