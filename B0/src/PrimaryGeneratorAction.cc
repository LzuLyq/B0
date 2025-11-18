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
/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="gamma");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(6.*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of ecah event
  // 点源位置
  // G4ThreeVector sourcePos = G4ThreeVector(0., 0., -13.*cm); 
  // fParticleGun->SetParticlePosition(sourcePos);

  // 面源位置
  G4double radius = 1*mm;
  G4double zPos = -1*mm;
  // 面上随机取点
  G4double r = radius * std::sqrt(G4UniformRand());
  G4double phi = 2.*CLHEP::pi*G4UniformRand();
  G4double xPos = r * std::cos(phi);
  G4double yPos = r * std::sin(phi);
  G4ThreeVector sourcePos = G4ThreeVector(xPos, yPos, zPos);
  fParticleGun->SetParticlePosition(sourcePos);

  // //
  // auto nav = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  // G4VPhysicalVolume* physVol = nav->LocateGlobalPointAndSetup(sourcePos);
  // G4Material* mat = nullptr;
  // if (physVol) {
  //   G4LogicalVolume* logVol = physVol->GetLogicalVolume();
  //   if (logVol) mat = logVol->GetMaterial();
  // }
  // G4cout << "DEBUG: Source pos = " << sourcePos
  //      << "  Material at source = " << (mat ? mat->GetName() : G4String("NULL")) << G4endl;

  // // Also print the configured momentum direction
  //  G4cout << "DEBUG: ParticleGun momentum direction = " 
  //      << fParticleGun->GetParticleMomentumDirection() << G4endl;



  // 随机方向
  // G4double costheta = 2.*G4UniformRand() - 1.;   // cosθ ∈ [-1,1]
  // G4double sintheta = std::sqrt(1. - costheta*costheta);
  // G4double phi = 2.*CLHEP::pi*G4UniformRand();
  // G4double x = sintheta * std::cos(phi);
  // G4double y = sintheta * std::sin(phi);
  // G4double z = costheta;

  // z方向发射
  G4ThreeVector direction(0,0,1);
    fParticleGun->SetParticleMomentumDirection(direction);

  // 真实情况随机能量: 59.5keV(97%), 26.3keV(2.4%), 33.2keV(0.6%)
  // G4double rnd = G4UniformRand();
  // G4double energy = 30.0;
  // if (rnd < 0.024) {
  //   energy = 26.3*keV;
  // } else if (rnd < 0.024 + 0.006) {
  //   energy = 33.2*keV;
  // } else {
  //   energy = 59.5*keV;
  // }
  
  // 26.3keV单能量
  //G4double energy = 30.0*keV;
  G4double energy = 2.9*keV;
  fParticleGun->SetParticleEnergy(energy);
  //create vertex
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}


