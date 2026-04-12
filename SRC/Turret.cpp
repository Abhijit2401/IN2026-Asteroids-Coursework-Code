#include "Turret.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "BoundingSphere.h"

Turret::Turret(GLVector3f p)
	: GameObject("Turret"), mLifeTime(10000), mMaxLifeTime(10000), mShootCooldown(0), mHasTargetAsteroid(false)
{
	mPosition = p;
}

Turret::~Turret(void)
{
}

void Turret::SetTargetAsteroid(GLVector3f target, bool valid)
{
	mTargetAsteroid = target;
	mHasTargetAsteroid = valid;
}

void Turret::Update(int t)
{
	mLifeTime -= t;
	if (mLifeTime <= 0 && mWorld) {
		mWorld->FlagForRemoval(GetThisPtr());
		return;
	}

	if (mShootCooldown > 0) mShootCooldown -= t;

	if (mWorld && mShootCooldown <= 0 && mHasTargetAsteroid) {
		GLVector3f dir = mTargetAsteroid - mPosition;
		dir.normalize();

		float aimAngle = atan2(dir.y, dir.x) * (180.0f / 3.14159265f);
		mAngle = aimAngle + 90.0f;

		GLVector3f bVel = dir * 50.0f;
		shared_ptr<GameObject> tb = make_shared<Bullet>(mPosition, bVel, GLVector3f(0, 0, 0), aimAngle, 0, 2000);
		tb->SetBoundingShape(make_shared<BoundingSphere>(tb->GetThisPtr(), 2.0f));
		if (mBulletShape) tb->SetShape(mBulletShape);
		mWorld->AddObject(tb);

		mShootCooldown = 400;
	}

	GameObject::Update(t);
}

void Turret::Render(void)
{

	if (mLifeTime <= 2000) {
		if ((mLifeTime / 100) % 2 == 0) {
			return;
		}
	}

	GameObject::Render();
}