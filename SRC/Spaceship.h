#ifndef __SPACESHIP_H__
#define __SPACESHIP_H__

#include "GameUtil.h"
#include "GameObject.h"
#include "Shape.h"
#include "Sprite.h"



// Spaceship publicly inherits from GameObject so that it can be managed by the game world and be automatically updated and rendered
class Spaceship : public GameObject {
public:
	Spaceship();
	Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r);
	Spaceship(const Spaceship& s);
	// Virtual overrides
	virtual ~Spaceship(void);
	virtual void Update(int t);
	virtual void Render(void);
	virtual void Thrust(float t);
	virtual void Rotate(float r);
	virtual void Shoot(void);
	void ShootMissile(void);
	// Encapsulated to allow outside classes to read the below variables
	int GetAmmo() {
		return mHomingAmmo;
	}
	int GetSpiralTimer() {
		return mSpiralTimer;
	}
	int GetInvulnerableTimer() {
		return mInvulnerableTimer;
	}
	void SetInvulnerable(int t) {
		mInvulnerableTimer = t;
	}
	void SetDead(bool d) { mIsDead = d; }
	void SetSpaceshipShape(shared_ptr<Shape> spaceship_shape) { mSpaceshipShape = spaceship_shape; }
	void SetBulletShape(shared_ptr<Shape> bullet_shape) { mBulletShape = bullet_shape; }
	void SetMissileSprite(shared_ptr<Sprite> missile_sprite) { mMissileSprite = missile_sprite; }
	void SetTurretSprite(shared_ptr<Sprite> turret_sprite) { mTurretSprite = turret_sprite; }
	bool CollisionTest(shared_ptr<GameObject> o); // Collision logic
	void OnCollision(const GameObjectList& objects);
	void ResetStats();
	float GetSpeedStat() {
		return mSpeedStat;
	}
	float GetRotationStat() {
		return mRotationStat;
	}
	int GetFireRateStat() {
		return mFireRateStat;
	}
	void SetBraking(bool b) {
		mBraking = b;
	}
private:
	float mThrust;
	shared_ptr<Shape> mSpaceshipShape;
	shared_ptr<Shape> mBulletShape;
	shared_ptr<Sprite> mMissileSprite;
	shared_ptr<Sprite> mTurretSprite;
	int mHomingAmmo;
	int mBulletCooldown;
	int mMissileCooldown;
	int mInvulnerableTimer;
	bool mIsDead;
	int mSpiralTimer;
	float mSpeedStat;
	float mRotationStat;
	int mFireRateStat;
	bool mBraking; // Flag to trigger the inertia friction
};

#endif