#include "SpiralBullet.h"

SpiralBullet::SpiralBullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl, float phase)
	: Bullet(p, v, a, h, r, ttl), mAge(0), mPhase(phase)
{
}

void SpiralBullet::Update(int t)
{
	if (mVelocity.length() > 0) {
		GLVector3f perp(-mVelocity.y, mVelocity.x, 0);
		perp.normalize();
		float prevWave = sin(mAge * 0.008f + mPhase) * 15.0f;
		mPosition = mPosition - (perp * prevWave);

		mAge += t;

		Bullet::Update(t);
		float newWave = sin(mAge * 0.008f + mPhase) * 15.0f;
		mPosition = mPosition + (perp * newWave);
	}
	else {
		Bullet::Update(t);
	}
}