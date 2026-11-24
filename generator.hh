#ifndef generator_h
#define generator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class G4Event;

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    MyPrimaryGenerator();
    virtual ~MyPrimaryGenerator();
    
    virtual void GeneratePrimaries(G4Event*);
    
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
private:
    G4ParticleGun* fParticleGun;
    
    // Soller 狭缝参数
    G4ThreeVector fSollerCenter;
    G4ThreeVector fSollerDirection;
    G4ThreeVector fPerp1;              // 垂直于 Soller 方向的基向量1
    G4ThreeVector fPerp2;              // 垂直于 Soller 方向的基向量2
    G4double fSollerR;
    G4double fSollerAcceptAngle;
    
    // 光源参数
    G4ThreeVector fSourcePos;
    G4double fSourceR;
};

#endif