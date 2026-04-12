#ifndef _EXTRALIFE_H_
#define _EXTRALIFE_H_

#include "GameObject.h"


// Lecture 03: ExtraLife publicly inherits from GameObject so it can be managed by the GameWorld and interact with the physics
class ExtraLife : public GameObject {
public:
	ExtraLife(void);
	~ExtraLife(void);
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
	bool mPickedUp;
};

#endif