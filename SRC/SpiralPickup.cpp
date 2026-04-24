#include "SpiralPickup.h"

SpiralPickup::SpiralPickup(GLVector3f p) : GameObject("SpiralPickup") {
	mPosition = p;
	mPickedUp = false;
	mAngle = 180.0f;
}