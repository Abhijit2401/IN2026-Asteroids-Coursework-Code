#include "TurretPickup.h"

TurretPickup::TurretPickup(GLVector3f p) : GameObject("TurretPickup") {
	mPosition = p;
	mPickedUp = false;
	mAngle = 180.0f;
}