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

Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false)
{
	ResetStats();
}

Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false)
{
	ResetStats();
}

Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0), mHomingAmmo(100), mBulletCooldown(0), mMissileCooldown(0), mInvulnerableTimer(3000), mIsDead(false), mSpiralTimer(0), mBraking(false)
{
	ResetStats();
}

Spaceship::~Spaceship(void)
{
}

void Spaceship::ResetStats()
{
	mSpeedStat = 15.0f;
	mRotationStat = 180.0f;
	mFireRateStat = 150;
}

void Spaceship::Update(int t)
{
	if (mBulletCooldown > 0) mBulletCooldown -= t;
	if (mMissileCooldown > 0) mMissileCooldown -= t;
	if (mInvulnerableTimer > 0) mInvulnerableTimer -= t;

	if (mSpiralTimer > 0) {
		mSpiralTimer -= t;
	}

	if (mBraking) {
		mVelocity.x *= 0.98f;
		mVelocity.y *= 0.98f;
		if (mVelocity.length() < 0.5f) {
			mVelocity.x = 0;
			mVelocity.y = 0;
		}
	}

	GameObject::Update(t);

	if (mSprite) {
		float speed = mVelocity.length();

		if (speed > 0.5f) {
			mSprite->SetLoopAnimation(true);
			float extraSpeed = speed / 15.0f;

			if (extraSpeed > 3.0f) extraSpeed = 3.0f;

			mSprite->Update((int)(t * extraSpeed));
		}
		else {
			mSprite->SetLoopAnimation(false);
		}
	}
}

void Spaceship::Render(void)
{
	if (mInvulnerableTimer > 0) {
		if ((mInvulnerableTimer / 100) % 2 == 0) {
			return;
		}
	}

	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

	GameObject::Render();
}

void Spaceship::Thrust(float t)
{
	mThrust = t;
	mAcceleration.x = mThrust * cos(DEG2RAD * mAngle);
	mAcceleration.y = mThrust * sin(DEG2RAD * mAngle);
}

void Spaceship::Rotate(float r)
{
	mRotation = r;
}

void Spaceship::Shoot(void)
{
	if (!mWorld) return;
	if (mBulletCooldown > 0) return;

	float finalAngle = mAngle;

	GLVector3f spaceship_heading(cos(DEG2RAD * finalAngle), sin(DEG2RAD * finalAngle), 0);
	spaceship_heading.normalize();

	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	float bullet_speed = 50.0f;
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;

	if (mSpiralTimer > 0) {
		shared_ptr<GameObject> p1 = make_shared<SpiralBullet>(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 4000, 0.0f);
		p1->SetShape(mBulletShape);
		p1->SetScale(3.0f);
		p1->SetBoundingShape(make_shared<BoundingSphere>(p1->GetThisPtr(), 4.8f));
		mWorld->AddObject(p1);

		shared_ptr<GameObject> p2 = make_shared<SpiralBullet>(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 4000, 3.14159f);
		p2->SetShape(mBulletShape);
		p2->SetScale(3.0f);
		p2->SetBoundingShape(make_shared<BoundingSphere>(p2->GetThisPtr(), 4.8f));
		mWorld->AddObject(p2);
	}
	else {
		shared_ptr<GameObject> projectile = make_shared<Bullet>(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 4000);
		projectile->SetShape(mBulletShape);
		projectile->SetScale(3.0f);
		projectile->SetBoundingShape(make_shared<BoundingSphere>(projectile->GetThisPtr(), 4.8f));
		mWorld->AddObject(projectile);
	}
	mBulletCooldown = mFireRateStat;
}

void Spaceship::ShootMissile(void)
{
	if (!mWorld || mHomingAmmo <= 0) return;
	if (mMissileCooldown > 0) return;

	GLVector3f spaceship_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	spaceship_heading.normalize();

	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	float bullet_speed = 5.0f;
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;

	shared_ptr<GameObject> projectile = make_shared<HomingMissile>(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 8000);
	if (mMissileSprite) projectile->SetSprite(mMissileSprite);
	projectile->SetBoundingShape(make_shared<BoundingSphere>(projectile->GetThisPtr(), 2.0f));
	mWorld->AddObject(projectile);

	mHomingAmmo--;
	mMissileCooldown = 350;
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (mIsDead) return false;

	if (o->GetType() == GameObjectType("Asteroid")) {
		if (mInvulnerableTimer > 0) return false;
		if (mBoundingShape.get() == NULL) return false;
		if (o->GetBoundingShape().get() == NULL) return false;
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	if (o->GetType() == GameObjectType("HomingPickup") || o->GetType() == GameObjectType("TurretPickup") || o->GetType() == GameObjectType("SpiralPickup") ||
		o->GetType() == GameObjectType("SpeedPickup") || o->GetType() == GameObjectType("RotationPickup") || o->GetType() == GameObjectType("FireRatePickup") ||
		o->GetType() == GameObjectType("ShieldPickup")) {
		if (mBoundingShape.get() == NULL) return false;
		if (o->GetBoundingShape().get() == NULL) return false;
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	return false;
}

void Spaceship::OnCollision(const GameObjectList& objects)
{
	if (mIsDead) return;

	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		shared_ptr<GameObject> o = *it;

		if (o->GetType() == GameObjectType("SpeedPickup")) {
			shared_ptr<SpeedPickup> pickup = static_pointer_cast<SpeedPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mSpeedStat += 20.0f;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("RotationPickup")) {
			shared_ptr<RotationPickup> pickup = static_pointer_cast<RotationPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mRotationStat += 20.0f;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("FireRatePickup")) {
			shared_ptr<FireRatePickup> pickup = static_pointer_cast<FireRatePickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mFireRateStat -= 10;
				if (mFireRateStat < 20) mFireRateStat = 20;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("HomingPickup")) {
			shared_ptr<HomingPickup> pickup = static_pointer_cast<HomingPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mHomingAmmo += 5;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("ShieldPickup")) {
			shared_ptr<ShieldPickup> pickup = static_pointer_cast<ShieldPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				mInvulnerableTimer += 10000;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("TurretPickup")) {
			shared_ptr<TurretPickup> pickup = static_pointer_cast<TurretPickup>(o);
			if (!pickup->mPickedUp) {
				pickup->mPickedUp = true;
				GLVector3f spawnPos = pickup->GetPosition();
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
				mSpiralTimer += 10000;
				mWorld->FlagForRemoval(pickup->GetThisPtr());
			}
		}
		else if (o->GetType() == GameObjectType("Asteroid")) {

			float asteroidRadius = 10.0f;
			shared_ptr<BoundingSphere> bs = dynamic_pointer_cast<BoundingSphere>(o->GetBoundingShape());
			if (bs) {
				asteroidRadius = bs->GetRadius();
			}
			if (mInvulnerableTimer > 0) {
				mWorld->FlagForRemoval(o);
				continue;
			}

			if (asteroidRadius >= 12.0f) {
				mIsDead = true;
				mWorld->FlagForRemoval(GetThisPtr());
				mWorld->FlagForRemoval(o);
				break;
			}

			GLVector3f diff = mPosition - o->GetPosition();
			float dist = diff.length();
			if (dist == 0.0f) { diff.x = 1.0f; dist = 1.0f; }
			GLVector3f normal = diff / dist;

			GLVector3f relVel = mVelocity - o->GetVelocity();
			float velAlongNormal = (relVel.x * normal.x) + (relVel.y * normal.y);
			if (velAlongNormal > 0) continue;
			float massShip = 0.3f;
			float massAsteroid = asteroidRadius * 0.2f;
			float bounciness = 1.5f - (asteroidRadius / 25.0f);

			float j = -(1.0f + bounciness) * velAlongNormal;
			j /= (1.0f / massShip) + (1.0f / massAsteroid);
			GLVector3f impulse = normal * j;

			mVelocity.x += (impulse.x / massShip);
			mVelocity.y += (impulse.y / massShip);

			GLVector3f astVel = o->GetVelocity();
			astVel.x -= (impulse.x / massAsteroid);
			astVel.y -= (impulse.y / massAsteroid);
			o->SetVelocity(astVel);

			float shipRadius = 6.0f;
			float penetration = (asteroidRadius + shipRadius) - dist;
			if (penetration > 0) {
				mPosition.x += normal.x * (penetration * 0.5f);
				mPosition.y += normal.y * (penetration * 0.5f);

				GLVector3f astPos = o->GetPosition();
				astPos.x -= normal.x * (penetration * 0.5f);
				astPos.y -= normal.y * (penetration * 0.5f);
				o->SetPosition(astPos);
			}
		}
	}
}