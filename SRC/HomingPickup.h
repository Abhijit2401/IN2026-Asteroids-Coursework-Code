#ifndef __HOMINGPICKUP_H__
#define __HOMINGPICKUP_H__

#include "GameObject.h"

class HomingPickup : public GameObject {
public:
	HomingPickup(GLVector3f p);
	bool mPickedUp;
};

#endif