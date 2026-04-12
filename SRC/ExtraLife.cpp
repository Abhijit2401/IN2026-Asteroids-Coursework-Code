// Lab 02: Included to use the rand() function for the random spawning 
#include <stdlib.h>
#include "GameUtil.h"
#include "ExtraLife.h"
#include "BoundingSphere.h"

// Lab 02: Initialises the base GameObject with a specific name for the RTTI identification
ExtraLife::ExtraLife(void) : GameObject("ExtraLife") {
	// Lab 02: Uses rand() from stdlib.h to spawn extra lives at a random positions
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 2.0 * cos(DEG2RAD * (rand() % 360));
	mVelocity.y = 2.0 * sin(DEG2RAD * (rand() % 360));
	mVelocity.z = 0.0;
	mPickedUp = false; // Flag used by the asteroids game manager to know when to give an extra life to the player
}

ExtraLife::~ExtraLife(void) { }

bool ExtraLife::CollisionTest(shared_ptr<GameObject> o) {
	// Lab 03: Uses Run Time Type Information (RTTI) to ensure the extra life only triggers a collision if the object touching it is the Spaceship
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void ExtraLife::OnCollision(const GameObjectList& objects) {
	// Iterates through all the objects collided with
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		// Lecture 02: Uses shared_ptr smart pointers to safely reference the collided object without memtory leaks
		shared_ptr<GameObject> o = *it;
		if (o->GetType() == GameObjectType("Spaceship")) {
			mPickedUp = true;
			mWorld->FlagForRemoval(GetThisPtr()); // Safely removes the heart from the active game world so it can't be picked up twice or glitches
		}
	}
}