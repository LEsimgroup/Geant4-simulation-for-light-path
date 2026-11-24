#include "generator.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1);
    
    // ========== Soller 狭缝几何参数 ==========
    // Soller 狭缝方向：与 x 轴成 30°，在 xz 平面内，z > 0
    G4double sollerAngle = 30.0 * deg;  // 与 +x 轴的夹角
    
    // 方向向量：在 xz 平面内，与 +x 成 30°，向 +z 方向
    fSollerDirection = G4ThreeVector(
        std::cos(sollerAngle),
        0.0,
        std::sin(sollerAngle)
    ).unit();
    
    // Soller 狭缝入口中心位置
    G4double sollerEntranceDistance = 50.0 * mm;
    fSollerCenter = fSollerDirection * sollerEntranceDistance;
    
    fSollerR = 3.0 * mm;
    fSollerAcceptAngle = 0.5 * deg;
    
    fSourcePos = G4ThreeVector(0.0, 0.0, 0.0);
    fSourceR = 25.0 * mm;
    
    // 计算垂直于 Soller 方向的两个正交向量
    G4ThreeVector axisY(0.0, 1.0, 0.0);
    fPerp1 = axisY.cross(fSollerDirection).unit();
    fPerp2 = fSollerDirection.cross(fPerp1).unit();
    
    G4cout << "Soller 狭缝方向: " << fSollerDirection << G4endl;
    G4cout << "Soller 入口中心: " << fSollerCenter << G4endl;
    G4cout << "接受角: " << fSollerAcceptAngle/deg << " deg" << G4endl;
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");

    // ============================================================
    // 位置采样：在垂直于 Soller 轴的平面内，圆形均匀分布
    // ============================================================
    G4double u = G4UniformRand();
    G4double r = fSourceR * std::sqrt(u);
    G4double phiPos = 2.0 * pi * G4UniformRand();
    
    G4ThreeVector sourcePos = fSourcePos 
                             + r * std::cos(phiPos) * fPerp1 
                             + r * std::sin(phiPos) * fPerp2;
    
    fParticleGun->SetParticlePosition(sourcePos);

    // ============================================================
    // 方向采样：沿 Soller 狭缝方向，小角度范围内，确保 dz > 0
    // ============================================================
    
    // 使用拒绝采样法：采样直到满足 dz > 0
    G4ThreeVector direction;
    G4double maxAngle = fSollerAcceptAngle;
    
    do {
        // 在以 fSollerDirection 为中心的锥角内均匀采样
        // 使用球面锥角均匀分布
        G4double cosTheta = 1.0 - G4UniformRand() * (1.0 - std::cos(maxAngle));
        G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
        G4double phiDir = 2.0 * pi * G4UniformRand();
        
        // 局部坐标系 -> 全局坐标系
        direction = cosTheta * fSollerDirection 
                  + sinTheta * std::cos(phiDir) * fPerp1 
                  + sinTheta * std::sin(phiDir) * fPerp2;
        
        direction = direction.unit();
        
    } while (direction.z() <= 0.0);  // 确保 dz > 0
    
    fParticleGun->SetParticleMomentumDirection(direction);

    // ============================================================
    // 能量设置
    // ============================================================
    fParticleGun->SetParticleEnergy(4.65 * keV);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}