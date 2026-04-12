#include "HitMarker.h"
#include "GameWorld.h"



// Lab 02: Initialises base GameObject with a specific name "HitMarker" for RTTI type checking.
// Lab 01: Initialises mTimeLeft to 150ms so the visual marker disappears, using the same lifespan logic for teh bullets
HitMarker::HitMarker(GLVector3f p) : GameObject("HitMarker"), mTimeLeft(150) {
	mPosition = p;
}

HitMarker::~HitMarker(void) { }
// Lab 01 and Lab 05: Overrides the Update method called every frame in the gameloop
void HitMarker::Update(int t) {
	mTimeLeft -= t;
	if (mTimeLeft <= 0 && mWorld) {
		mWorld->FlagForRemoval(GetThisPtr());
	}
}