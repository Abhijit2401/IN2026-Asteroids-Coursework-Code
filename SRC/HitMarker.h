#ifndef __HITMARKER_H__
#define __HITMARKER_H__

#include "GameObject.h"



// Lecture 03 and Lab 01: Inherits from GameObject so that the HitMarker can be automatically updated and rendered by the GameWorld
class HitMarker : public GameObject {
public:
	HitMarker(GLVector3f p);
	virtual ~HitMarker(void);
	virtual void Update(int t);
private:
	// Lab 01: Variable to track duration or lifespan of the hitmarker
	int mTimeLeft;
};

#endif