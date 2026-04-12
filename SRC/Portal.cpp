#include "Portal.h"
#include "GameWorld.h"



// Initialises the portal specific coordinates
Portal::Portal(GLVector3f position) : GameObject("Portal"), mTimeToLive(1000) { 
	mPosition = position; // mTimeToLive is set to 1000 giving it enough time to show the full animation before ending and afterwards leading to the spawning of a massive asteroid
}

Portal::~Portal(void) { } 

// Overrides the default update to act as a self destructing timer
void Portal::Update(int t) { 
	GameObject::Update(t);
	mTimeToLive -= t; // Subtracts the elapsed frame time from the portals lifespan or duration left
	if (mTimeToLive <= 0 && mWorld) {
		mWorld->FlagForRemoval(GetThisPtr());
	}
}