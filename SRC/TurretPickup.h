#pragma once
#ifndef __TURRETPICKUP_H__
#define __TURRETPICKUP_H__

#include "GameObject.h"

class TurretPickup : public GameObject
{
public:
	TurretPickup(GLVector3f p);
	bool mPickedUp;
};

#endif