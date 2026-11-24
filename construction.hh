#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
 	public:
 	MyDetectorConstruction();
		~MyDetectorConstruction();

    	virtual G4VPhysicalVolume* Construct();  //必须要有的函数，构建探测器时执行，只能修改函数的功能，不能重命名
//		inline const G4VPhysicalVolume* Getbody() const {return phys_body;};  //用于外部获取探测器的所在的位置信息
//		inline const G4VPhysicalVolume* Getbody() const;
  protected:
		G4VPhysicalVolume* SollerPlate1[13];
		G4VPhysicalVolume* SollerPlate2[13];
};

#endif
