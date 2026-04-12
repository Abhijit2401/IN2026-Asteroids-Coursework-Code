#ifndef __HOMINGMISSILE_H__
#define __HOMINGMISSILE_H__

#include "Bullet.h"



class HomingMissile : public Bullet {
public:
	HomingMissile(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl);
	virtual void Update(int t) override;
	void SetTarget(GLVector3f target);

private:
	// Variables to track the lock on state and target coordinates
	bool mHasTarget;
	GLVector3f mTarget;
};

#endif