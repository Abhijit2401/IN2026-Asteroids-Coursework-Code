// Lab 02: Includes guards to prevent the compiler from including this header multiple times
#ifndef __ASTEROID_H__
#define __ASTEROID_H__
#include "GameObject.h"



// Lecture 03: Asteroid publicly inherits from the GameObject base class so it can exist in the GameWorld
class Asteroid : public GameObject
{
public:
	// Constructor and Destructor
	Asteroid(void);
	Asteroid(int size, GLVector3f position, GLVector3f velocity);
	~Asteroid(void);
	// Getter used to track if player should be awarded points or not
	bool GetHitByBullet() { 
		return mHitByBullet; 
	}
	// Virtual overrides to ensure the engine calls teh asteroids logic 
	virtual void Update(int t);
	virtual void Render(void);
	// Physics and Collision overrides from Lab 03
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
	// Asteroid properties
	int mSize;
	bool mHitByBullet;
	int mHitPoints;
	int mFlashTime;
	int mInvulnerableTime;
};

#endif
