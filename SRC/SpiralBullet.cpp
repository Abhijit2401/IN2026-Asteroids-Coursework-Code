#include "SpiralBullet.h"



// Inherits from base bullet class and adds 2 parameters
SpiralBullet::SpiralBullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl, float phase) : Bullet(p, v, a, h, r, ttl), mAge(0), mPhase(phase) { }

void SpiralBullet::Update(int t) {
	// Only applies the calcs if the bullet is moving
	if (mVelocity.length() > 0) {
		// This works by calculating the perpendicular vector then undoing the previous frames wave offset then incrementing the mAge tracker then applying hte movement and then calculating and appliying the new wave offset
		GLVector3f perp(-mVelocity.y, mVelocity.x, 0);
		perp.normalize();
		float prevWave = sin(mAge * 0.008f + mPhase) * 15.0f;
		mPosition = mPosition - (perp * prevWave);
		mAge += t;
		Bullet::Update(t);
		// mAge * 0.008f controls the frequency of the spiral
		float newWave = sin(mAge * 0.008f + mPhase) * 15.0f;
		mPosition = mPosition + (perp * newWave);
	}
	else {
		Bullet::Update(t);
	}
}