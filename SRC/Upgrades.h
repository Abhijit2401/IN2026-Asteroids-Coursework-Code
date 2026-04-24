#pragma once
#ifndef __UPGRADES_H__
#define __UPGRADES_H__

#include "GameObject.h"

class SpeedPickup : public GameObject {
public:
	SpeedPickup(GLVector3f p) : GameObject("SpeedPickup") { mPosition = p; mPickedUp = false; }
	bool mPickedUp;
};
class RotationPickup : public GameObject {
public:
	RotationPickup(GLVector3f p) : GameObject("RotationPickup") { mPosition = p; mPickedUp = false; }
	bool mPickedUp;
};
class FireRatePickup : public GameObject {
public:
	FireRatePickup(GLVector3f p) : GameObject("FireRatePickup") { mPosition = p; mPickedUp = false; }
	bool mPickedUp;
};
class ShieldPickup : public GameObject {
public:
	ShieldPickup(GLVector3f p) : GameObject("ShieldPickup") { mPosition = p; mPickedUp = false; }
	bool mPickedUp;
};

#endif