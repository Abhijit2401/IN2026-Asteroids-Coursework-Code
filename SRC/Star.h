#pragma once
#ifndef __STAR_H__
#define __STAR_H__

#include "GameObject.h"
#include "Spaceship.h"



// Inherits GameObject
class Star : public GameObject {
public:
	Star(int layer, shared_ptr<Spaceship> player);
	~Star(void);
	virtual void Update(int t);
	virtual void Render(void);
private:
	int mLayer; // This decides depth, colour, brihgtness and size and the speed of the stars basedon layer
	shared_ptr<Spaceship> mPlayer; // Uses smart pointer to track the player
};

#endif