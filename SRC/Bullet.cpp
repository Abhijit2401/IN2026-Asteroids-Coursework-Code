#include "GameWorld.h"
#include "Bullet.h"
// Lab 03: Included so circular hitboxes can be used for the bullets for collision detection
#include "BoundingSphere.h"
#include "HitMarker.h"
#include "AnimationManager.h"
#include "Sprite.h"
#include "Animation.h"

// Lab 01: Initialises mTimeToLive to 2000ms so bullets dont last forever
Bullet::Bullet() : GameObject("Bullet"), mTimeToLive(2000) { }

Bullet::Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl) : GameObject("Bullet", p, v, a, h, r), mTimeToLive(ttl) { }

Bullet::Bullet(const Bullet& b) : GameObject(b), mTimeToLive(b.mTimeToLive) { }

Bullet::~Bullet(void) { }

// Lab 01: Overrides the Update method to reduce the time every frame
void Bullet::Update(int t) {
	GameObject::Update(t);
	// Lab 01: Reduces the bullets lifespanby the elapsed time 't'
	mTimeToLive = mTimeToLive - t;
	if (mTimeToLive < 0) { 
		mTimeToLive = 0; 
	}
	if (mTimeToLive == 0) {
		// Lab 02: Safely flags the bullet for removal from the GameWorld when its timer is finisihed
		if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
	}
}
// Collision detection using BoundingSpheres
bool Bullet::CollisionTest(shared_ptr<GameObject> o) {
	// Uses Run Time Type Information (RTTI) to ensure bullets only test for collisions against all types of asteroids
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	// Lab 03: Null pointer checks before testing shapes
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
// Triggers on the frame in which the bullet intersects with another object
void Bullet::OnCollision(const GameObjectList& objects) {
	bool hitAsteroid = false;
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->GetType() == GameObjectType("Asteroid")) {
			hitAsteroid = true;
			break; //
		}
	}
	// A visual hitmarker is shown at the exact coordinate of impact when an asteroid is shot
	if (hitAsteroid && mWorld) {
		shared_ptr<HitMarker> marker = make_shared<HitMarker>(mPosition);
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("crosshair");
		if (anim_ptr) {
			shared_ptr<Sprite> marker_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			marker->SetSprite(marker_sprite);
			mWorld->AddObject(marker);
		}
	}
	// On collision the bullet is flagged to be destroyed
	mWorld->FlagForRemoval(GetThisPtr());
}