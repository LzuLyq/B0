#include "SiSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

namespace B1{

G4ThreadLocal G4Allocator<SiHit>* SiHitAllocator = nullptr;

SiSD::SiSD(const G4String& name, const G4String& hcName)
 : G4VSensitiveDetector(name), fHitsCollection(nullptr), fHCID(-1)
{
    collectionName.insert(hcName);
}

void SiSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new SiHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool SiSD::ProcessHits(G4Step* step, G4TouchableHistory* /*hist*/)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    auto hit = new SiHit();
    hit->AddEdep(edep);
    hit->SetPos(step->GetPostStepPoint()->GetPosition());

    fHitsCollection->insert(hit);

    return true;
}

void SiSD::EndOfEvent(G4HCofThisEvent*) {
    if (verboseLevel > 1) {
        G4cout << "SiSD: " << fHitsCollection->entries()
               << " hits in this event" << G4endl;
    }
}

} // namespace B1