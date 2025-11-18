/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4SDManager.hh"
#include "SiSD.hh"


#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4Region.hh"
#include "G4ProductionCuts.hh"
namespace B1
{
void DetectorConstruction::ConstructSDandField() {
    auto sdManager = G4SDManager::GetSDMpointer();
    auto siSD = new SiSD("SiSD" , "SiHitsCollection");
    sdManager->AddNewDetector(siSD);

    SetSensitiveDetector("Detector", siSD, true);  // 假设逻辑体名字是 Detector
}

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();

    // Define elements
    G4double a, z;
    G4double density;
    // Fluorine
    a = 18.998*g/mole;
    z = 9;
    G4Element* elF = new G4Element("Fluorine", "F", z, a);
    // Uranium
    a = 235.0*g/mole;  // Can choose 235U or 238U
    z = 92;
    G4Element* elU = new G4Element("Uranium", "U", z, a);
    // Boron
    a = 10.81*g/mole;
    z = 5;
    G4Element* elB = new G4Element("Boron", "B", z, a);
    // Silicon
    a = 28.0855*g/mole; z = 14;
    G4Element* elSi = new G4Element("Silicon", "Si", z, a);
    // Chlorine
    a = 35.45*g/mole; z = 17;
    G4Element* elCl = new G4Element("Chlorine", "Cl", z, a);

    // Create materials
    // 定义六氟化铀
    density = 1.5*kg/m3;  // Density of UF6 at room temperature and pressure
    G4Material* UF6 = new G4Material("UF6", density, 2);
    UF6->AddElement(elU, 1);
    UF6->AddElement(elF, 6);

    // 定义纯氯
    //density = 3.2*kg/m3;  // Density of Cl2 gas at room temperature and pressure
    // G4Material* Cl2 = new G4Material("Cl2", density, 1);
    // Cl2->AddElement(elCl, 2);
    // 取六氟化铀的密度为混合物的近似密度
    // density = 1.5*kg/m3;  // Approximate density of UF6 -

    // 定义含硼的六氟化铀
    // G4Material* UF6_B = new G4Material("UF6_B", density, 2);
    // Calculate molar fraction of Boron
    // 5 µg/g = 5e-6 g B / 1 g UF6
    // Assume molar mass of UF6 = 235 + 6*18.998 ≈ 349 g/mol
    // G4double massFraction_B = 3e-06;       // 4 µg/g
    // G4double massFractionUF6 = 1.0 - massFraction_B;
    
    // 定义含硼硅氯的六氟化铀
    G4double massFraction_B = 1e-06;   //   1 µg/g
    G4double massFraction_Si = 0.00010000000000000003;   // 100 µg/g
    G4double massFraction_Cl = 0.00010000000000000003;   // 100 µg/g
    G4double massFraction_UF6 = 1.0 - (massFraction_B + massFraction_Si + massFraction_Cl);
    G4Material* UF6_B_Si_Cl = new G4Material("UF6_B_Si_Cl", density, 4);
    UF6_B_Si_Cl->AddMaterial(UF6, massFraction_UF6);
    UF6_B_Si_Cl->AddElement(elB,  massFraction_B);
    UF6_B_Si_Cl->AddElement(elSi, massFraction_Si);
    UF6_B_Si_Cl->AddElement(elCl, massFraction_Cl);

    //定义纯硼
    //G4Material* pureB = new G4Material("PureB", 150*kg/m3, 1);
    //pureB->AddElement(elB, 1);

    // Output check
    //G4cout << "Defined material: " << UF6_B->GetName() << G4endl;
    // G4Material::DumpTable(); // Removed: No such member function
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  DefineMaterials();

  // 样品材料选择   
  //G4Material* sam_mat = G4Material::GetMaterial("PureB");
  //G4Material* sam_mat = G4Material::GetMaterial("UF6_B");
  //G4Material* sam_mat = G4Material::GetMaterial("UF6");
  //G4Material* sam_mat = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material* sam_mat = G4Material::GetMaterial("UF6_B_Si_Cl");
  //G4Material* sam_mat = G4Material::GetMaterial("Cl2");
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXYZ = 1.0 * cm;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");
  auto solidWorld = new G4Box("World", 0.5 * world_sizeXYZ, 0.5 * world_sizeXYZ, 0.5 * world_sizeXYZ);
  auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  auto physWorld =
      new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, checkOverlaps);
  
  // Shape collimator
  //   auto collimator_mat = nist->FindOrBuildMaterial("G4_Pb");
  //   auto solidCollimator = new G4Box("Collimator", 0.75*cm, 0.75*cm, 1.5*cm);
  //   auto logicCollimator = new G4LogicalVolume(solidCollimator, collimator_mat, "Collimator");
  //   new G4PVPlacement(nullptr,  // no rotation
  //     G4ThreeVector(0, 0, -12*cm),   // at (0,0,-12)
  //     logicCollimator,          // its logical volume
  //     "Collimator",            // its name
  //     logicEnv,                // its mother  volume
  //     false,                   // no boolean operation
  //     0,                       // copy number
  //     checkOverlaps);          // overlaps checking
  //  auto solidHole = new G4Tubs("Hole", 0, 0.5*cm, 1.4*cm, 0., 360.*deg);
  //  auto logicHole = new G4LogicalVolume(solidHole, env_mat, "Hole");
  //  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0.1*cm), logicHole, "Hole", logicCollimator, false, 0, checkOverlaps);
  
  // sample target
  G4double sampleXY = 1.0 * cm;
  G4double sampleThickness = 2000 * um; // 薄靶
  auto solidSample =
      new G4Box("Sample", 0.5 * sampleXY, 0.5 * sampleXY, 0.5 * sampleThickness);
  auto logicSample = new G4LogicalVolume(solidSample, sam_mat, "SampleLogic");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicSample, "Sample", logicWorld, false, 0, checkOverlaps);
  //
  // Be window
  auto window_mat = nist->FindOrBuildMaterial("G4_Be");
    G4double windowThickness = 0.008 * mm;
    auto solidWindow = new G4Tubs("Window", 0, 0.9 * cm, 0.5 * windowThickness, 0., 360. * deg);
    G4double windowPosZ = 0.5 * sampleThickness + 0.5 * windowThickness;
    auto logicWindow = new G4LogicalVolume(solidWindow, window_mat, "Window");
    new G4PVPlacement(nullptr,  G4ThreeVector(0, 0, windowPosZ),  logicWindow, "Window", logicWorld,  false, 0, checkOverlaps);       
  
  // Detector
  auto detector_mat = nist->FindOrBuildMaterial("G4_Si");
  G4double detThickness = 0.2 * cm;
  auto solidDetector = new G4Tubs("Detector", 0, 0.9 * cm, 0.5 * detThickness, 0., 360. * deg);
  // 与样品表面间隔 0.1 mm 真空
  G4double detPosZ = 0.5 * sampleThickness + 0.1 * mm + 0.5 * detThickness;
  auto logicDetector = new G4LogicalVolume(solidDetector, detector_mat, "Detector");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, detPosZ),
                    logicDetector, "Detector", logicWorld, false, 0, checkOverlaps);
  //
  // Regions and Production Cuts
  //
  G4Region* sampleRegion = new G4Region("SampleRegion");
  G4ProductionCuts* sampleCuts = new G4ProductionCuts();
  sampleCuts->SetProductionCut(1 * nm, "gamma");
  sampleCuts->SetProductionCut(1 * nm, "e-");
  sampleRegion->SetProductionCuts(sampleCuts);
  logicSample->SetRegion(sampleRegion);
  sampleRegion->AddRootLogicalVolume(logicSample);

  G4Region* detRegion = new G4Region("DetectorRegion");
  G4ProductionCuts* detCuts = new G4ProductionCuts();
  detCuts->SetProductionCut(10 * nm, "gamma");
  detCuts->SetProductionCut(10 * nm, "e-");
  detRegion->SetProductionCuts(detCuts);
  logicDetector->SetRegion(detRegion);
  detRegion->AddRootLogicalVolume(logicDetector);                  
  
  // Set logicDetector as scoring volume
  fScoringVolume = logicDetector;
  
  return physWorld;
}

}
