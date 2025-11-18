#ifndef SiSD_h
#define SiSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"

namespace B1
{
    
class SiHit : public G4VHit
{
public:
    SiHit() : fEdep(0.), fPos(0.) {}
    virtual ~SiHit() {}
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void AddEdep(G4double edep) { fEdep += edep; }
    G4double GetEdep() const { return fEdep; }

    void SetPos(const G4ThreeVector& pos) { fPos = pos; }
    G4ThreeVector GetPos() const { return fPos; }
private:
    G4double fEdep;
    G4ThreeVector fPos ;
};
typedef G4THitsCollection<SiHit> SiHitsCollection;

extern G4ThreadLocal G4Allocator<SiHit>* SiHitAllocator;

inline void* SiHit::operator new(size_t) {
    if (!SiHitAllocator) SiHitAllocator = new G4Allocator<SiHit>;
    return (void*) SiHitAllocator->MallocSingle();
}

inline void SiHit::operator delete(void* hit) {
    SiHitAllocator->FreeSingle((SiHit*) hit);
}

class SiSD : public G4VSensitiveDetector {
public:
    SiSD(const G4String& name, const G4String& hcName);
    virtual ~SiSD() {}

    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* hist) override;
    virtual void EndOfEvent(G4HCofThisEvent* hce) override;

private:
    SiHitsCollection* fHitsCollection;
    G4int fHCID;
};
} // namespace name
#endif
