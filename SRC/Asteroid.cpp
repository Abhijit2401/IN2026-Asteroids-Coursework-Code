#include <stdlib.h>  // Learned in Lab 02 and is included to use the rand() function for random numbers
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h" // Learned in Lab 03 as a base class for object collision shapes
#include "BoundingSphere.h" // Learned in Lab 02/03 and is used to give game objects a physical hit-box




// Constructor to accept size, position, and velocity for CW
Asteroid::Asteroid(int size, GLVector3f position, GLVector3f velocity) : GameObject("Asteroid") {
	mSize = size;
	mAngle = rand() % 360; // Lab 02: Uses rand() to generate a random starting rotation angle
	mRotation = 0;
	mPosition = position;
	mVelocity = velocity;

	mHitByBullet = false;
	mFlashTime = 0;
	mInvulnerableTime = 0;
	// Sets health and speed multipliers based on the size tier of hte asteroid
	if (mSize == 2) {
		mHitPoints = 4; // Largest asteroids take 4 hits to destroy
		mVelocity.x *= 0.5f;
		mVelocity.y *= 0.5f;
	}
	else if (mSize == 1) {
		mHitPoints = 3;
	}
	else {
		mHitPoints = 1; // Smallest destroy in 1 hit
	}
}


// Destructor
Asteroid::~Asteroid(void) {
}

// Called every frame to update asteroid
void Asteroid::Update(int t) {
	GameObject::Update(t); // Base class update always called first to handle standard physics
	if (mFlashTime > 0) mFlashTime -= t; // Timer mechanics inspired by Lab 05 timers to handle flash and invulnerability states
	if (mInvulnerableTime > 0) mInvulnerableTime -= t;
}


// Overrides the Render method to add visual effects taught in Lab 01 and Lab 02
void Asteroid::Render(void) {
	if (mFlashTime > 0) {
		glDisable(GL_LIGHTING); // Lab 01/02: Disable lighting to draw solid, unshaded colors
		glColor3f(1.0f, 0.0f, 0.0f); // Temporarily set the asteroid color to red when flashing
	}
	GameObject::Render();
	if (mFlashTime > 0) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_LIGHTING);
	}
}

// CollisionTest overrides the default to use Bounding Spheres which is learnt from Labs 02 and 03
bool Asteroid::CollisionTest(shared_ptr<GameObject> o) {
	// If both objects lack a hitbox they cant collide
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}


// OnCollision handles the logic when a crash happens which is learnt and taken from Lab 03 and CW part 1
void Asteroid::OnCollision(const GameObjectList& objects) {
	// Iterates through all objects collided with in this frame
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		// Lecture 02: Uses shared_ptr to manage memory safely
		shared_ptr<GameObject> o = *it;
		// Lab 03: Uses the engine's Run-Time Type Information (RTTI) to identify if hit by a bulket
		if (o->GetType() == GameObjectType("Bullet")) {
			mWorld->FlagForRemoval(o);
			if (mInvulnerableTime <= 0) {
				mHitPoints -= 1;
				mInvulnerableTime = 200;
				mFlashTime = 200;
				// If health drops below 0, destroy the asteroid
				if (mHitPoints <= 0) {
					mHitByBullet = true; // Flag used later to award points to the player
					mWorld->FlagForRemoval(GetThisPtr());
					// If the asteroid is big enough, meaning it is size 1 or 2, it should split into smaller pieces
					if (mSize > 0) {
						// Calculates the new velocity vectors to make pieces fly in opposite directions diagonally.
						GLVector3f vel1(mVelocity.x + 15, mVelocity.y + 15, 0);
						GLVector3f vel2(mVelocity.x - 15, mVelocity.y - 15, 0);
						// Creates 2 child asteroids with "size -1"
						shared_ptr<Asteroid> ast1 = make_shared<Asteroid>(mSize - 1, mPosition, vel1);
						shared_ptr<Asteroid> ast2 = make_shared<Asteroid>(mSize - 1, mPosition, vel2);
						// Inherits its visual graphics from parent asteroid
						ast1->SetSprite(mSprite);
						ast1->SetShape(mShape);
						ast2->SetSprite(mSprite);
						ast2->SetShape(mShape);
						// Scales image and hitbox down to new size
						float newScale = (mSize - 1 == 1) ? 0.2f : 0.1f;
						float newRadius = (mSize - 1 == 1) ? 10.0f : 5.0f;
						ast1->SetScale(newScale);
						ast2->SetScale(newScale);
						ast1->SetBoundingShape(make_shared<BoundingSphere>(ast1->GetThisPtr(), newRadius));
						ast2->SetBoundingShape(make_shared<BoundingSphere>(ast2->GetThisPtr(), newRadius));
						// Spawns them into the world
						mWorld->AddObject(ast1);
						mWorld->AddObject(ast2);
					}
				}
			}

		}

		else if (o->GetType() == GameObjectType("Asteroid")) {
			// Calculates direction vector from pushing away from another asteroid
			GLVector3f n = mPosition - o->GetPosition();
			float dist = n.length();
			if (dist > 0.0f) {
				n.normalize(); // Converts to a Unit Vector of length 1
				GLVector3f relVel = mVelocity - o->GetVelocity();
				float speedAlongNormal = (relVel.x * n.x) + (relVel.y * n.y);
				if (speedAlongNormal < 0) { // If moving towards each other, make them bounce with one another to avoid phasing through or getting stuck
					mVelocity.x -= speedAlongNormal * n.x;
					mVelocity.y -= speedAlongNormal * n.y;
				}
			}
		}

	}
}