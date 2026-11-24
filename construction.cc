#include "construction.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"
#include "G4Colour.hh"
#include "G4IntersectionSolid.hh"


MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    // Get nist material manager to Get material
    G4NistManager* nist = G4NistManager::Instance();
    
    // World
    G4Material* Air_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4double world_sizeX = 500*mm, world_sizeY = 500*mm, world_sizeZ = 500*mm;    
    G4Box* solidWorld = new G4Box("World",
                                   0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);

    G4double densityVacuum = 1e-25*g/cm3;
    G4double moleMassVacuum = 1.01 * g/mole;
    G4double pressureVacuum = 3.e-18 * pascal;
    G4double temperatureVacuum = 2.73 * kelvin;

    G4Material* vacuum = new G4Material(
        "Vacuum", 1., moleMassVacuum,
        densityVacuum, kStateGas,
        temperatureVacuum, pressureVacuum
    );                           
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");    
    
    G4VPhysicalVolume* physWorld = 
        new G4PVPlacement(0,
                          G4ThreeVector(),
                          logicWorld,
                          "World",
                          0,
                          false,
                          0,
                          true);

    // ========== Cu 针孔圆台 ==========
    G4Material* Cu = nist->FindOrBuildMaterial("G4_Cu");

    G4double rUpperOuter = 20.0 * mm;
    G4double rLowerOuter = 10.0 * mm;
    G4double rHole       = 7.5 * mm;
    G4double halfHeight  = 3.0 * mm;

    G4Cons* solidCuFrustum = new G4Cons(
        "CuFrustum",
        rHole, rLowerOuter,
        rHole, rUpperOuter,
        halfHeight,
        0., 360.*deg
    );

    G4LogicalVolume* logicCuFrustum = new G4LogicalVolume(solidCuFrustum, Cu, "CuFrustum");

    G4VPhysicalVolume *pinhole = new G4PVPlacement(
        nullptr,              
        G4ThreeVector(0, 0, 30 * mm),
        logicCuFrustum,
        "CuFrustum",
        logicWorld,    
        false, 0, true
    );

    // ========== Si 探测器 ==========
    G4Material* Si = nist->FindOrBuildMaterial("G4_Si");

    G4double rSi     = 150.0 * mm;
    G4double halfZSi = 1.0 * mm;

    G4Tubs* solidSiPlate = new G4Tubs("SiPlate", 0., rSi, halfZSi, 0., 360.*deg);

    G4LogicalVolume* logicSiPlate = new G4LogicalVolume(solidSiPlate, Si, "SiPlate");

    G4VPhysicalVolume *BareSi = new G4PVPlacement(
        nullptr,
        G4ThreeVector(0, 0, 0),
        logicSiPlate,
        "SiPlate",
        logicWorld,
        false, 0, true
    );

    // ========== Bragg 几何参数定义 ==========
    G4double thetaBragg = 41.47 * deg;    // Bragg 角 (41.47°)
    
    // 第一个索拉狭缝入射方向（与 +X 轴成 30°）
    G4double angleIn = 30.0 * deg;
    G4ThreeVector dirIn(std::cos(angleIn), 0., std::sin(angleIn));
    
    // LiF 晶体长边方向与 +X 轴成 30° + 41.47° = 71.47°
    G4double tiltLiF = angleIn + thetaBragg;  // 71.47°

    // 第二个索拉狭缝出射方向（满足 Bragg 对称条件：71.47° + 41.47° = 112.94°）
    G4double angleOut = tiltLiF + thetaBragg; // 112.94°
    G4ThreeVector dirOut(std::cos(angleOut), 0., std::sin(angleOut));
    
    // 第一个索拉狭缝到原点的距离
    G4double R_soller = 130.0 * mm;

    // ========== 第一个索拉狭缝（保持位置和方向不变） ==========
    G4Material* Mo = nist->FindOrBuildMaterial("G4_Mo");

    G4double plateThick  = 0.05 * mm;
    G4double plateWidth  = 20.0 * mm;
    G4double plateLength = 100.0 * mm;

    G4double gap     = 1.5 * mm;
    G4double period  = plateThick + gap;
    G4int    nPlates = 13;

    G4Box* solidPlate = new G4Box("SollerPlate1",
                                   plateThick/2.0,
                                   plateWidth/2.0,
                                   plateLength/2.0);
    G4LogicalVolume* logicPlate = new G4LogicalVolume(solidPlate, Mo, "SollerPlate1");

    // 旋转：局部 z 轴沿入射方向 dirIn (30°)
    G4RotationMatrix* rotSlit1 = new G4RotationMatrix();
    rotSlit1->rotateY(angleIn - 90.0 * deg);  // -60°

    // 第一个索拉狭缝位置：在入射方向上，距原点 R_soller
    G4ThreeVector centerSoller1(R_soller * std::cos(angleIn), 0., R_soller * std::sin(angleIn));

    // 第一个狭缝排布方向向量
    G4ThreeVector ez1(-0.75*mm, 0., 1.3*mm);

    for (G4int i = 0; i < nPlates; ++i)
    {
        G4double zOffset = (i - 6) * period;
        G4ThreeVector pos = centerSoller1 + zOffset * ez1;

        new G4PVPlacement(
            rotSlit1,
            pos,
            logicPlate,
            "SollerPlate1",
            logicWorld,
            false, i, true);
    }

    // ---------- 索拉狭缝套筒几何（供两个索拉狭缝共用） ----------
    G4Material* StainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

    G4double sleeveInX = 20.0 * mm;
    G4double sleeveInY = 20.0 * mm;
    G4double wallThick = 4.0 * mm;
    G4double sleeveOutX = sleeveInX + 2*wallThick;
    G4double sleeveOutY = sleeveInY + 2*wallThick;
    G4double sleeveOutZ = 100.0 * mm;

    G4Box* solidSleeveOuter = new G4Box("SleeveOuter",
                                         sleeveOutX/2.0,
                                         sleeveOutY/2.0,
                                         sleeveOutZ/2.0);

    G4double innerHalfZ = sleeveOutZ;
    G4Box* solidSleeveInner = new G4Box("SleeveInner",
                                         sleeveInX/2.0,
                                         sleeveInY/2.0,
                                         innerHalfZ);

    G4SubtractionSolid* solidSleeve = new G4SubtractionSolid(
        "Sleeve",
        solidSleeveOuter,
        solidSleeveInner,
        nullptr,
        G4ThreeVector(0., 0., 0.)
    );

    G4LogicalVolume* logicSleeve = new G4LogicalVolume(
        solidSleeve, StainlessSteel, "SleeveLV"
    );

    G4VPhysicalVolume* Sleeve1 = new G4PVPlacement(
        rotSlit1,
        centerSoller1,
        logicSleeve,
        "Sleeve1",
        logicWorld,
        false, 0, true);

    // ========== LiF 晶体 ==========
    G4Material* LiF = new G4Material("LiF", 2.64 * g/cm3, 2);
    LiF->AddElement(nist->FindOrBuildElement("Li"), 0.2676);
    LiF->AddElement(nist->FindOrBuildElement("F"),  0.7324);

    G4double crystX = 75.0 * mm;  // 长边
    G4double crystY = 30.0 * mm;  // 宽度（沿 Y 轴）
    G4double crystZ = 3.5 * mm;   // 厚度

    G4Box* solidLiF = new G4Box("LiFCrystal", crystX/2.0, crystY/2.0, crystZ/2.0);

    // LiF 晶体及圆柱中心位置：位于第一个索拉狭缝长边中心延长线上
    G4double distSollerToLiF = 95.0 * mm;
    G4ThreeVector posLiF = centerSoller1 + distSollerToLiF * dirIn;

    // LiF 绕 Y 轴旋转，使其长边切线方向与索拉狭缝长边方向成 41.47°
    G4RotationMatrix* rotLiF = new G4RotationMatrix();
    rotLiF->rotateY(tiltLiF);
    
    G4LogicalVolume* logicLiF = new G4LogicalVolume(solidLiF, LiF, "LiFCrystal");
    G4VisAttributes* LiFVisAtt = new G4VisAttributes(G4Colour(0., 1., 1., 1.));
    logicLiF->SetVisAttributes(LiFVisAtt);
    
    G4VPhysicalVolume *LiF_Crystal = new G4PVPlacement(
        rotLiF,
        posLiF,
        logicLiF,
        "LiF_Crystal",
        logicWorld,
        false, 0, true
    );

    // ========== 第二个索拉狭缝（对准圆柱第二个孔及 LiF 晶体中心） ==========
    // 旋转：使局部 z 轴（狭缝长边）沿 dirOut 方向，延长线经过圆孔中心和 LiF 中心
    G4RotationMatrix* rotSlit2 = new G4RotationMatrix();
    rotSlit2->rotateY(angleOut - 90.0 * deg);

    // 第二个索拉狭缝中心位置：从 LiF 中心沿 dirOut 延伸相同距离
    G4ThreeVector centerSoller2 = posLiF + distSollerToLiF * dirOut;

    // 第二个狭缝叶片排布方向（垂直于 dirOut，且保持与第一个狭缝相同的间距比例）
    G4double ezNorm = ez1.mag();
    G4ThreeVector ez2 = ezNorm * G4ThreeVector(-std::sin(angleOut), 0., std::cos(angleOut));

    for (G4int i = 0; i < nPlates; ++i)
    {
        G4double zOffset = (i - 6) * period;
        G4ThreeVector pos2 = centerSoller2 + zOffset * ez2;

        new G4PVPlacement(
            rotSlit2,
            pos2,
            logicPlate,
            "SollerPlate2",
            logicWorld,
            false, i, true);
    }

    // 放置第二个索拉狭缝套筒
    G4VPhysicalVolume* Sleeve2 = new G4PVPlacement(
        rotSlit2,
        centerSoller2,
        logicSleeve,
        "Sleeve2",
        logicWorld,
        false, 1, true);

    // ========== 空心圆柱（圆形底面与 Z 轴平行，开两个孔分别对应两个索拉狭缝） ==========
    G4double cylRmin = 44.0 * mm;
    G4double cylRmax = 50.0 * mm;
    G4double cylHalfH = 15.0 * mm;

    G4Tubs* solidRing = new G4Tubs("Ring", cylRmin, cylRmax, cylHalfH, 0., 360.*deg);
    G4Material* RingMat = nist->FindOrBuildMaterial("G4_Fe");

    // 圆柱绕 X 轴旋转 90°，使其轴线沿 Y 轴，圆形底面平行于 XZ 平面（与 Z 轴平行）
    G4RotationMatrix* rotRing = new G4RotationMatrix();
    rotRing->rotateX(90.0 * deg);

    // 定义圆孔固体
    G4double holeR = 15.0 * mm;
    G4double holeHalfLen = 10.0 * mm;
    G4Tubs* solidHole = new G4Tubs("SleeveHole", 0., holeR, holeHalfLen, 0., 360.*deg);

    G4double Rmid = 0.5 * (cylRmin + cylRmax);

    // --- 第 1 个孔（对应第一个索拉狭缝，方向 -dirIn） ---
    G4ThreeVector hole1LocalPos(-Rmid * std::cos(angleIn), -Rmid * std::sin(angleIn), 0.);
    G4RotationMatrix* rotHole1 = new G4RotationMatrix();
    rotHole1->rotateZ(-angleIn);
    rotHole1->rotateY(90.0 * deg);

    G4SubtractionSolid* solidRingWithOneHole = new G4SubtractionSolid(
        "RingWithOneHole",
        solidRing,
        solidHole,
        rotHole1,
        hole1LocalPos
    );

  

    // ========== 圆柱顶部和底部盖板 ==========
    G4double capR     = cylRmax;
    G4double capHalfT = 2.5 * mm;

    G4Tubs* solidCap = new G4Tubs("RingCap", 0., capR, capHalfT, 0., 360.*deg);

    G4LogicalVolume* logicCap = new G4LogicalVolume(
        solidCap, nist->FindOrBuildMaterial("G4_STAINLESS-STEEL"), "RingCapLV"
    );

    // 圆柱轴线方向为 Y 轴
    G4ThreeVector cylAxis(0., 1., 0.);
    G4ThreeVector capTopCenter = posLiF + (cylHalfH + capHalfT) * cylAxis;
    G4ThreeVector capBotCenter = posLiF - (cylHalfH + capHalfT) * cylAxis;

    G4VPhysicalVolume* topcap = new G4PVPlacement(
        rotRing, capTopCenter, logicCap, "RingTopCap", logicWorld, false, 0, true);

    G4VPhysicalVolume* bottomcap = new G4PVPlacement(
        rotRing, capBotCenter, logicCap, "RingBottomCap", logicWorld, false, 1, true);

    return physWorld;
}