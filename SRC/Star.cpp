#include <stdlib.h>
#include "GameUtil.h"
#include "Star.h"

Star::Star(int layer, shared_ptr<Spaceship> player) : GameObject("Star"), mLayer(layer), mPlayer(player)
{
	mPosition.x = (rand() % 1200) - 600;
	mPosition.y = (rand() % 800) - 400;
	mPosition.z = -10.0f * layer;
}

Star::~Star(void)
{
}

void Star::Update(int t)
{
	float driftSpeed = -1.0f * mLayer;
	float playerParallaxX = 0;
	float playerParallaxY = 0;

	if (mPlayer) {
		GLVector3f shipVel = mPlayer->GetVelocity();
		playerParallaxX = -shipVel.x * (0.02f * mLayer);
		playerParallaxY = -shipVel.y * (0.02f * mLayer);
	}
	mVelocity.x = driftSpeed + playerParallaxX;
	mVelocity.y = playerParallaxY;
	GameObject::Update(t);
}

void Star::Render(void)
{
	glDisable(GL_LIGHTING);
	if (mLayer == 1) glColor3f(0.3f, 0.3f, 0.3f);
	if (mLayer == 2) glColor3f(0.6f, 0.6f, 0.6f);
	if (mLayer == 3) glColor3f(1.0f, 1.0f, 1.0f);

	glPointSize((float)mLayer);

	glBegin(GL_POINTS);
	glVertex2f(0, 0);
	glEnd();

	glEnable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
}