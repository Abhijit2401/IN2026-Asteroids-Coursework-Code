#ifndef __GALAXY_H__
#define __GALAXY_H__

#include "GameObject.h"
#include "Sprite.h"
#include "Spaceship.h"


// Lecture 03: Inherits from GameObject so Galaxy can be managed by GameWorld
class Galaxy : public GameObject {
public:
	Galaxy(shared_ptr<Spaceship> ship);
	virtual ~Galaxy(void);
	virtual void Render(void) override; // Lab 05: Overrides the core methods to implement rendering and parallax updates
	virtual void Update(int t) override;
	void SetSprite(shared_ptr<Sprite> sprite) {
		mSprite = sprite; 
	}
private:
	shared_ptr<Sprite> mSprite; // Lecture 02: Uses smart pointers (shared_ptr) for resources and object references to prevent memory leaks
	shared_ptr<Spaceship> mSpaceship;
};

#endif