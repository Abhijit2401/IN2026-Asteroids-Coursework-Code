#include <stdlib.h>
#include "GameUtil.h"
#include "Star.h"

Star::Star(int layer, shared_ptr<Spaceship> player) : GameObject("Star"), mLayer(layer), mPlayer(player) {
	//Procedurally generates the coordinates for stars randomly within teh screen range
	mPosition.x = (rand() % 1200) - 600;
	mPosition.y = (rand() % 800) - 400;
	// Pushes the stars back in teh z layer to ensure it stays in the background
	mPosition.z = -10.0f * layer;
}
Star::~Star(void) { }
void Star::Update(int t) {
	float driftSpeed = -1.0f * mLayer; // Slow movement where higher layer levels lead to faster movement
	float playerParallaxX = 0;
	float playerParallaxY = 0;
	if (mPlayer) {
		GLVector3f shipVel = mPlayer->GetVelocity();
		// Stars move in opposite direction to player
		playerParallaxX = -shipVel.x * (0.02f * mLayer);
		playerParallaxY = -shipVel.y * (0.02f * mLayer);
	}
	mVelocity.x = driftSpeed + playerParallaxX;
	mVelocity.y = playerParallaxY;
	GameObject::Update(t);
}
void Star::Render(void) {
	glDisable(GL_LIGHTING); // Disables lighting so that the stars produce their own light
	if (mLayer == 1) glColor3f(0.3f, 0.3f, 0.3f);
	if (mLayer == 2) glColor3f(0.6f, 0.6f, 0.6f);
	if (mLayer == 3) glColor3f(1.0f, 1.0f, 1.0f); // 3rd layer produces the most lighting and is the closest
	glPointSize((float)mLayer);
	glBegin(GL_POINTS); // Draws single open GL points
	glVertex2f(0, 0);
	glEnd();
	glEnable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f); // Resets the global colour to white
}