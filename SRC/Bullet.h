#ifndef __BULLET_H__ // Lab 02: Include guards to prevent multiple inclusions of this header
#define __BULLET_H__

#include "GameUtil.h"
#include "GameObject.h"

// Lab 01: Bullet inherits from GameObject
class Bullet : public GameObject
{
public:
	Bullet();
	Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl);
	Bullet(const Bullet& b);
	virtual ~Bullet(void);
	virtual void Update(int t);
	void SetTimeToLive(int ttl) { 
		mTimeToLive = ttl; 
	}
	int GetTimeToLive(void) { 
		return mTimeToLive; 
	}
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
protected:
	// Lab 01: Variable to track how much long the bullet lasts
	int mTimeToLive;
};

#endif