#include "Galaxy.h"


// Lecture 02: Passes shared_ptr to safely maintain a reference to the player's ship without memory leaks
// Lab 02: Initialises base GameObject with a string name for RTTI type checking
Galaxy::Galaxy(shared_ptr<Spaceship> ship) : GameObject("Galaxy"), mSpaceship(ship) {
	mPosition = GLVector3f(0.0f, 0.0f, 0.0f);
}

Galaxy::~Galaxy(void) { }
// Lab 05: Overrides the Update method
void Galaxy::Update(int t) {
	if (mSpaceship) {
		// Lecture 06: Implements parallax scrolling by making the lower background layers move slower than the foregroudn and front layers
		float targetX = -mSpaceship->GetPosition().x * 0.08f;
		float targetY = -mSpaceship->GetPosition().y * 0.08f;

		float smoothSpeed = 0.005f * t;
		if (smoothSpeed > 1.0f) smoothSpeed = 1.0f;
		mPosition.x += (targetX - mPosition.x) * smoothSpeed;
		mPosition.y += (targetY - mPosition.y) * smoothSpeed;
	}
	GameObject::Update(t);
}
// Lab 01 and 02: Overrides Render to use OpenGL matrix transformations
void Galaxy::Render(void) {
	if (mSprite) {
		// Lab 02: Uses glPushMatrix() to isolate these specific transformations so that it only affects the Galaxy sprite and not the rest of the game world
		glPushMatrix();
		glTranslatef(mPosition.x, mPosition.y, 0.0f);
		glScalef(0.5f, 0.5f, 1.0f);
		glDisable(GL_LIGHTING);
		mSprite->Render();
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
}