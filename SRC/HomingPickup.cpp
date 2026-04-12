#include "HomingPickup.h"

HomingPickup::HomingPickup(GLVector3f p) : GameObject("HomingPickup") {
	mPosition = p;
	mPickedUp = false;
	mAngle = 180.0f;
}