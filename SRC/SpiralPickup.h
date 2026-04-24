#pragma once
#ifndef __SPIRALPICKUP_H__
#define __SPIRALPICKUP_H__

#include "GameObject.h"

// Inherits GameObject class
class SpiralPickup : public GameObject {
public:
	SpiralPickup(GLVector3f p);
	bool mPickedUp;
};

#endif