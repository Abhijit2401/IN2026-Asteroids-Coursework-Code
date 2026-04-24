#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"
#include "HomingMissile.h"
#include "HomingPickup.h"
#include "TurretPickup.h"
#include "SpiralPickup.h"
#include "SpiralBullet.h"
#include "Turret.h"
#include "AnimationManager.h"
#include "Animation.h"
#include "Upgrades.h"

using namespace std;

// Constructor initialises the base GameObject with the name Spaceship so it can be identified accurately
Spaceship::Spaceship() : GameObject("Spaceship"), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false) {
	ResetStats();
}

Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r) : GameObject("Spaceship", p, v, a, h, r), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false) {
	ResetStats();
}

Spaceship::Spaceship(const Spaceship& s) : GameObject(s), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false) {
	ResetStats();
}

Spaceship::~Spaceship(void) { }

// Stats decided for when stat resetting is called for
void Spaceship::ResetStats() {
	mSpeedStat = 15.0f;
	mRotationStat = 180.0f;
	mFireRateStat = 150;
}
// Lab 05: Overridess the Update method which is called every frame in the game loop
void Spaceship::Update(int t) {
	if (mBulletCooldown > 0) mBulletCooldown -= t; // Decreases cooldown timerse by the elapsed time t to control the weapon fire rates and invulnerability windows
	if (mMissileCooldown > 0) mMissileCooldown -= t;
	if (mInvulnerableTimer > 0) mInvulnerableTimer -= t;
	if (mSpiralTimer > 0) {
		mSpiralTimer -= t;
	}
	// Braking applies friction that takes away 2% of velocity every frame and stops the speed entierlly after its velocity length is less than 0.5 to make the ship come to a halt
	if (mBraking) {
		mVelocity.x *= 0.98f;
		mVelocity.y *= 0.98f;
		if (mVelocity.length() < 0.5f) {
			mVelocity.x = 0;
			mVelocity.y = 0;
		}
	}
	GameObject::Update(t); // Processes the standard physics
	// Dynamic sprite animation where velocity impacts the loop animation speed
	if (mSprite) {
		float speed = mVelocity.length();
		if (speed > 0.5f) {
			mSprite->SetLoopAnimation(true);
			float extraSpeed = speed / 15.0f;
			if (extraSpeed > 3.0f) extraSpeed = 3.0f; // Caps the animation speed to 3
			mSprite->Update((int)(t * extraSpeed));
		}
		else {
			mSprite->SetLoopAnimation(false);
		}
	}
}
// Render method to add visual effects like skipping its rendering every 100ms to create a flashing effect
void Spaceship::Render(void) {
	if (mInvulnerableTimer > 0) {
		if ((mInvulnerableTimer / 100) % 2 == 0) {
			return;
		}
	}
	// Renders the physical vector shape of the ship then base sprite rendering
	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();
	GameObject::Render();
}
// Lab 01 and Lecture 08: Applies the thrust to calculate acceleration amount
void Spaceship::Thrust(float t) {
	mThrust = t;
	// Lecture 08: Trigonometry is used to calculate the 2D vector components of the ship's acceleration by multiplying the thrust scalar by the cosine and sine of the ships current angle
	mAcceleration.x = mThrust * cos(DEG2RAD * mAngle);
	mAcceleration.y = mThrust * sin(DEG2RAD * mAngle);
}

void Spaceship::Rotate(float r) {
	mRotation = r;
}

void Spaceship::Shoot(void) {
	if (!mWorld) return;
	if (mBulletCooldown > 0) return; // Prevents shooting if the cooldown tiimer is still active
	float finalAngle = mAngle; // Calculates where the ship is pointing towards using vector normalisation
	GLVector3f spaceshipDirection(cos(DEG2RAD * finalAngle), sin(DEG2RAD * finalAngle), 0);
	spaceshipDirection.normalize();
	GLVector3f bulletPos = mPosition + (spaceshipDirection * 4); // Spawns the bullet ahead of hte player slightly to prevent them from clipping or glitching
	float bulletSpeed = 50.0f;
	GLVector3f bulletVel = mVelocity + spaceshipDirection * bulletSpeed; // Inherits the ships current velocity as taught in the Lectures and Labs
	// Checks if the player has the spiral upgrade active
	if (mSpiralTimer > 0) {
		// Spawns 2 spiral bullets with 180 degrees offset (Pi radians)
		shared_ptr<GameObject> p1 = make_shared<SpiralBullet>(bulletPos, bulletVel, mAcceleration, mAngle, 0, 4000, 0.0f);
		p1->SetShape(mBulletShape);
		p1->SetScale(3.0f);
		p1->SetBoundingShape(make_shared<BoundingSphere>(p1->GetThisPtr(), 4.8f));
		mWorld->AddObject(p1);
		shared_ptr<GameObject> p2 = make_shared<SpiralBullet>(bulletPos, bulletVel, mAcceleration, mAngle, 0, 4000, 3.1415f);
		p2->SetShape(mBulletShape);
		p2->SetScale(3.0f);
		p2->SetBoundingShape(make_shared<BoundingSphere>(p2->GetThisPtr(), 4.8f));
		mWorld->AddObject(p2);
	}
	else {
		// Otherwise its single bullet simple straight line firing 
		shared_ptr<GameObject> projectile = make_shared<Bullet>(bulletPos, bulletVel, mAcceleration, mAngle, 0, 4000);
		projectile->SetShape(mBulletShape);
		projectile->SetScale(3.0f);
		projectile->SetBoundingShape(make_shared<BoundingSphere>(projectile->GetThisPtr(), 4.8f));
		mWorld->AddObject(projectile);
	}
	mBulletCooldown = mFireRateStat; // Fire rate determines hte bullet cooldown and how fast the player can spam click spacebar or hold it
}
// Missile shooting
void Spaceship::ShootMissile(void) {
	if (!mWorld || mHomingAmmo <= 0) return;
	if (mMissileCooldown > 0) return;
	GLVector3f spaceshipDirection(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	spaceshipDirection.normalize();
	GLVector3f bulletPos = mPosition + (spaceshipDirection * 4);
	float bulletSpeed = 5.0f; // Slower starting speed since the homing missile logic is that it speeds up over time anyways
	GLVector3f bullet_velocity = mVelocity + spaceshipDirection * bulletSpeed;
	shared_ptr<GameObject> projectile = make_shared<HomingMissile>(bulletPos, bullet_velocity, mAcceleration, mAngle, 0, 8000);
	if (mMissileSprite) projectile->SetSprite(mMissileSprite);
	projectile->SetBoundingShape(make_shared<BoundingSphere>(projectile->GetThisPtr(), 2.0f));
	mWorld->AddObject(projectile);
	mHomingAmmo--; // Consumes the homing missile ammo
	mMissileCooldown = 350; // Slower fire rate than the bullets
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o) {
	if (mIsDead) return false; // Ensures that the dead ships dont havew hitboxes
	// Asteroid collision check
	if (o->GetType() == GameObjectType("Asteroid")) {
		if (mInvulnerableTimer > 0) return false;
		if (mBoundingShape.get() == NULL) return false;
		if (o->GetBoundingShape().get() == NULL) return false;
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	// Pikcup collision check and ensures that invulnerability doesnt prevent the player from picking up items
	if (o->GetType() == GameObjectType("HomingPickup") || o->GetType() == GameObjectType("TurretPickup") || o->GetType() == GameObjectType("SpiralPickup") ||
		o->GetType() == GameObjectType("SpeedPickup") || o->GetType() == GameObjectType("RotationPickup") || o->GetType() == GameObjectType("FireRatePickup") ||
		o->GetType() == GameObjectType("ShieldPickup")) {
		if (mBoundingShape.get() == NULL) return false;
		if (o->GetBoundingShape().get() == NULL) return false;
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	return false;
}

void Spaceship::OnCollision(const GameObjectList& objects) {
	if (mIsDead) return;
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		shared_ptr<GameObject> o = *it;
		if (o->GetType() == GameObjectType("SpeedPickup")) {
			shared_ptr<SpeedPickup> pickup = static_pointer_cast<SpeedPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mSpeedStat += 20.0f; // Permanently increases speed stat by 20 until death and respawn for recalcing stats
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("RotationPickup")) {
			shared_ptr<RotationPickup> pickup = static_pointer_cast<RotationPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mRotationStat += 20.0f; // Rotation speed
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("FireRatePickup")) {
			shared_ptr<FireRatePickup> pickup = static_pointer_cast<FireRatePickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mFireRateStat -= 10; // Fire rate
				if (mFireRateStat < 20) mFireRateStat = 20;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("HomingPickup")) {
			shared_ptr<HomingPickup> pickup = static_pointer_cast<HomingPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mHomingAmmo += 5; // Increases homing ammo by 5 when picked up
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("ShieldPickup")) {
			shared_ptr<ShieldPickup> pickup = static_pointer_cast<ShieldPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mInvulnerableTimer += 10000; // Invulnerability timer incremented for 10 seconds
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("TurretPickup")) {
			shared_ptr<TurretPickup> pickup = static_pointer_cast<TurretPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				GLVector3f spawnPos = pickup->GetPosition();
				// Spawns a automated turret object on the pickup spot and auto aims
				shared_ptr<Turret> turret = make_shared<Turret>(spawnPos);
				turret->SetScale(0.5f);
				Animation* t_anim = AnimationManager::GetInstance().GetAnimationByName("turret_anim");
				if (t_anim) {
					shared_ptr<Sprite> t_sprite = make_shared<Sprite>(t_anim->GetWidth(), t_anim->GetHeight(), t_anim);
					t_sprite->SetLoopAnimation(true);
					turret->SetSprite(t_sprite);
				}
				turret->SetBulletShape(make_shared<Shape>("bullet.shape"));
				mWorld->AddObject(turret);
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("SpiralPickup")) {
			shared_ptr<SpiralPickup> pickup = static_pointer_cast<SpiralPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mSpiralTimer += 10000; // Spiral weapon timer incremented for 10 seconds
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("Asteroid")) {
			float asteroidRadius = 10.0f;
			shared_ptr<BoundingSphere> bs = dynamic_pointer_cast<BoundingSphere>(o->GetBoundingShape());
			if (bs) {
				asteroidRadius = bs->GetRadius();
			}
			// Checks invulnerability timer 
			if (mInvulnerableTimer > 0) {
				mWorld->FlagForRemoval(o); // Destroys the asteroid upon collision if timer is active
				continue;
			}
			// If asteroid radius is 12 or above, the ship explodes
			if (asteroidRadius >= 12.0f) {
				mIsDead = true;
				mWorld->FlagForRemoval(GetThisPtr());
				mWorld->FlagForRemoval(o);
				break;
			}
			// If asteroid is small enough then calc the collision normal and the bounce 
			GLVector3f diff = mPosition - o->GetPosition();
			float dist = diff.length();
			if (dist == 0.0f) {
				diff.x = 1.0f; dist = 1.0f;  // Ensures that there is no division by 0 to prevent crashes
			}
			GLVector3f normal = diff / dist;
			GLVector3f relativeVel = mVelocity - o->GetVelocity();
			float velAlongNormal = (relativeVel.x * normal.x) + (relativeVel.y * normal.y);
			if (velAlongNormal > 0) continue; // If they are already moving away from each other ignore the physics
			// Mass of the ship and asteeroid mass based on radius size
			float massShip = 0.3f;
			float massAsteroid = asteroidRadius * 0.2f;
			float bounciness = 1.5f - (asteroidRadius / 25.0f); // This means smaller asteroids are bouncier for a more dynamic physics approach to the CW
			float j = -(1.0f + bounciness) * velAlongNormal; // Calcs the impulse or the instantaneous force
			j /= (1.0f / massShip) + (1.0f / massAsteroid);
			GLVector3f impulse = normal * j;
			mVelocity.x += (impulse.x / massShip);
			mVelocity.y += (impulse.y / massShip);
			GLVector3f asteroidVel = o->GetVelocity();
			asteroidVel.x -= (impulse.x / massAsteroid);
			asteroidVel.y -= (impulse.y / massAsteroid);
			o->SetVelocity(asteroidVel);
			// Forces objects to unbind from each otehr so they dont get stuck or trapped inside one another
			float shipRadius = 6.0f;
			float boundTogether = (asteroidRadius + shipRadius) - dist;
			if (boundTogether > 0) {
				mPosition.x += normal.x * (boundTogether * 0.5f);
				mPosition.y += normal.y * (boundTogether * 0.5f);
				GLVector3f asteroidPos = o->GetPosition();
				asteroidPos.x -= normal.x * (boundTogether * 0.5f);
				asteroidPos.y -= normal.y * (boundTogether * 0.5f);
				o->SetPosition(asteroidPos);
			}
		}
	}
}