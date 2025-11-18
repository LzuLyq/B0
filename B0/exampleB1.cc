//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmParameters.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4PhysListFactory.hh"

#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"
using namespace B1;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  // Set verbose level for stepping (GEANT4 10.7 compatible)
  G4VSteppingVerbose::SetInstance(new G4SteppingVerbose);

  // 设置随机数种子
  G4Random::setTheSeed(time(NULL));

  // Construct the default run manager
  //
  auto* runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager->SetUserInitialization(new DetectorConstruction());

  // Physics list
  // G4VModularPhysicsList* physicsList = new QBBC;
  // G4PhysListFactory factory;
  // G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("FTFP_BERT");
  // physicsList->ReplacePhysics(new G4EmPenelopePhysics());
  //  G4PhysListFactory factory;
  //  G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("FTFP_BERT_EMV"); 
  //  physicsList->ReplacePhysics(new G4EmLivermorePhysics());
  //  runManager->SetUserInitialization(physicsList);
  auto physicsList = new G4VModularPhysicsList();
  physicsList->RegisterPhysics(new G4EmLivermorePhysics());
  runManager->SetUserInitialization(physicsList);

  // 原子驰豫
  auto* emParams = G4EmParameters::Instance();
  emParams->SetAugerCascade(true); // 开启原子驰豫
  emParams->SetFluo(true);      // 开启荧光
  emParams->SetPixe(true);    // 开启X射线光电子
  emParams->SetMinEnergy(100*eV);
  emParams->SetMaxEnergy(1*GeV);
  emParams->SetLowestElectronEnergy(100*eV);

  // 注册原子去激发
  auto* deexcitation = new G4UAtomicDeexcitation();
  deexcitation->SetFluo(true);
  deexcitation->SetAuger(true);
  deexcitation->SetPIXE(true);
  G4LossTableManager::Instance()->SetAtomDeexcitation(deexcitation);

  // 设置极小的production cut
  G4double cutValue = 10*nanometer;
  physicsList->SetDefaultCutValue(cutValue);
  
  //
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);



  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization());

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
