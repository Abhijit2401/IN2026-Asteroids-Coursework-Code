#ifndef __TURRET_H__
#define __TURRET_H__

#include "GameObject.h"
#include "Sprite.h"
#include "Shape.h"

class Turret : public GameObject
{
public:
	Turret(GLVector3f p);
	virtual ~Turret(void);

	virtual void Update(int t) override;
	virtual void Render(void) override;

	void SetBulletShape(shared_ptr<Shape> shape) { mBulletShape = shape; }
	void SetTargetAsteroid(GLVector3f target, bool valid);

private:
	shared_ptr<Shape> mBulletShape;

	int mLifeTime;
	int mMaxLifeTime;
	int mShootCooldown;

	bool mHasTargetAsteroid;
	GLVector3f mTargetAsteroid;
};

#endif