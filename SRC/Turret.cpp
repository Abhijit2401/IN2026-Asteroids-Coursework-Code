#include "Turret.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "BoundingSphere.h"

Turret::Turret(GLVector3f p) : GameObject("Turret"), mLifeTime(10000), mMaxLifeTime(10000), mShootCooldown(0), mHasTargetAsteroid(false) {
	mPosition = p;
}

Turret::~Turret(void) { }

void Turret::SetTargetAsteroid(GLVector3f target, bool valid) {
	mTargetAsteroid = target;
	mHasTargetAsteroid = valid;
}
void Turret::Update(int t) {
	// Life time for the turret
	mLifeTime -= t;
	if (mLifeTime <= 0 && mWorld) {
		mWorld->FlagForRemoval(GetThisPtr()); // Safely removes turret after timer is finished (10 seconds)
		return;
	}
	if (mShootCooldown > 0) mShootCooldown -= t; // Weapon ccooldown timer
	if (mWorld && mShootCooldown <= 0 && mHasTargetAsteroid) {
		GLVector3f dir = mTargetAsteroid - mPosition; // Calcs the directional vector from turret to the asteroid
		dir.normalize(); // Converts it to a unit vector
		float aimAngle = atan2(dir.y, dir.x) * (180.0f / 3.1415f); // Calcs the angle required to look in the asteroids direction using atan2 and converts radians to degrees
		mAngle = aimAngle + 90.0f; // Rotates the sprite 
		GLVector3f bVel = dir * 50.0f; // Bullet speed is 50 in the direction making it a vector velocity
		// Spawns the bullet with 0 acclereation
		shared_ptr<GameObject> tb = make_shared<Bullet>(mPosition, bVel, GLVector3f(0, 0, 0), aimAngle, 0, 2000);
		tb->SetBoundingShape(make_shared<BoundingSphere>(tb->GetThisPtr(), 2.0f));
		if (mBulletShape) tb->SetShape(mBulletShape);
		mWorld->AddObject(tb);
		mShootCooldown = 400; // Resets the shooting cooldown
	}
	GameObject::Update(t);
}
void Turret::Render(void) {
	// If the turret has less than 2 seconds left, it will start flashing (same logic as the spacship invulnerability flashing rendering)
	if (mLifeTime <= 2000) {
		if ((mLifeTime / 100) % 2 == 0) {
			return;
		}
	}
	GameObject::Render(); // If it isnt flashing hten it is rendered normally
}