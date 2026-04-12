#pragma once
#ifndef __STAR_H__
#define __STAR_H__

#include "GameObject.h"
#include "Spaceship.h"

class Star : public GameObject
{
public:
	Star(int layer, shared_ptr<Spaceship> player);
	~Star(void);

	virtual void Update(int t);
	virtual void Render(void);

private:
	int mLayer;
	shared_ptr<Spaceship> mPlayer;
};

#endif