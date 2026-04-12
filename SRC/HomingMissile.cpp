#include "HomingMissile.h"



// Lecture 03 and Lab 01: This constructor calls the base Bullet class constructor to automatically handle standard properties like position, velocity and duration or lifespan
HomingMissile::HomingMissile(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl) : Bullet(p, v, a, h, r, ttl), mHasTarget(false) { }

// A custom setter to assign a target for the missile to chase.
void HomingMissile::SetTarget(GLVector3f target) {
	mTarget = target;
	mHasTarget = true;
}

void HomingMissile::Update(int t) {
	// Makes the missile speed up over time with a max speed of 80
	float currentSpeed = mVelocity.length();
	float maxSpeed = 80.0f;
	// If statement to get the current unit vectorand apply a thrust based on the elapsed time whilst capping acceleration and finally applying increased speed magnitude back to directcional vector
	if (currentSpeed > 0.0f && currentSpeed < maxSpeed) {
		GLVector3f direction = mVelocity / currentSpeed;
		float newSpeed = currentSpeed + (0.05f * t);
		if (newSpeed > maxSpeed) newSpeed = maxSpeed;
		mVelocity = direction * newSpeed;
	}
	// Homing steering logic
	if (mHasTarget) {
		GLVector3f dir = mTarget - mPosition;
		dir.normalize(); // This converts it to a unit vector to ensure consistent rotational speed when turning towards target
		mVelocity.x += dir.x * 2.0f;
		mVelocity.y += dir.y * 2.0f;
		if (mVelocity.length() > maxSpeed) {
			mVelocity.normalize();
			mVelocity = mVelocity * maxSpeed;
		}
		mAngle = atan2(mVelocity.y, mVelocity.x) * (180.0f / 3.14159265f) + 90.0f;
		mHasTarget = false; // Resets the target flag so the game manager has to actively give it a new target next frame
	}
	Bullet::Update(t);
}