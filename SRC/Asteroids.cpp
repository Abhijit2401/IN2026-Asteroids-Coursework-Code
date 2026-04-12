#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h" // Lab 05: Includes for handling VFX and animations
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h" // Lab 03 and 04: Includes for the RTTI type checking and HUD Labels
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "ImageManager.h"
#include "ExtraLife.h"
#include "GUIIcon.h"
#include "Star.h"
#include "Portal.h"
#include "HomingPickup.h"
#include "HomingMissile.h"
#include "TurretPickup.h"
#include "SpiralPickup.h"
#include "Turret.h"
#include "Galaxy.h"
#include "Upgrades.h"



//Custom timer ID for scoring animation
const int ANIMATE_SCORE = 99;

Asteroids::Asteroids(int argc, char* argv[]) : GameSession(argc, argv) {
	mLevel = 0;
	mAsteroidCount = 0;
	mShipDead = false;
	mDisplayScore = 0;
}

Asteroids::~Asteroids(void) {
}
// Start() initialises the game session, loads assets, and sets up the initial game state
void Asteroids::Start() {
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);
	mGameWorld->AddListener(thisPtr.get()); // Registers this class as an observer/listener for game events
	mGameWindow->AddKeyboardListener(thisPtr);
	mGameWorld->AddListener(&mScoreKeeper);
	mScoreKeeper.AddListener(thisPtr);
	glutReshapeWindow(1280, 720); // Sets the intiial window resolution
	// Sets up OpenGL lighting
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);
	// Loads all sprite filmstrips and UI icons into memory
	Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 32, 256, 32, 32, "spaceship.png");
	Animation* meteor_anim = AnimationManager::GetInstance().CreateAnimationFromFile("portal", 32, 384, 32, 32, "meteor_strip.png");
	ImageManager::GetInstance().CreateImageFromFile("heart_icon", 16, 16, "heart.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("crosshair", 32, 32, 32, 32, "crosshair.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("missile", 32, 32, 32, 32, "missiles.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("missile_pickup", 32, 32, 32, 32, "missiles_icon.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("turret_anim", 32, 256, 32, 32, "turret_strip.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("turret_icon", 32, 32, 32, 32, "turret.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("spiral_icon", 32, 32, 32, 32, "spiral.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("shield_icon", 32, 32, 32, 32, "shield.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("speed_icon", 32, 32, 32, 32, "speed.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("rotation_icon", 32, 32, 32, 32, "rotation_speed.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("firing_icon", 32, 32, 32, 32, "firing_speed.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("heart", 96, 16, 16, 16, "heart_animated.png");
	shared_ptr<GameObject> ship = CreateSpaceship();
	// Initialises the parallax background galaxy
	Animation* galaxy_anim = AnimationManager::GetInstance().CreateAnimationFromFile("galaxy", 2048, 2048, 2048, 2048, "galaxy.png");
	shared_ptr<Galaxy> galaxy_bg = make_shared<Galaxy>(mSpaceship);
	if (galaxy_anim) {
		shared_ptr<Sprite> gal_sprite = make_shared<Sprite>(galaxy_anim->GetWidth(), galaxy_anim->GetHeight(), galaxy_anim);
		galaxy_bg->SetSprite(gal_sprite);
	}
	mGameWorld->AddObject(galaxy_bg);
	CreateStars();
	mGameWorld->AddObject(ship);
	CreateAsteroids(4); // Starts with 4 asteroids
	CreateGUI();
	mGameWorld->AddListener(&mPlayer);
	mPlayer.AddListener(thisPtr);
	// Sets engine timers for spawning, levels and UI score animation
	SetTimer(2000, SPAWN_ASTEROID);
	SetTimer(10000, START_NEXT_LEVEL);
	SetTimer(50, ANIMATE_SCORE);
	GameSession::Start();
}
void Asteroids::Stop() {
	GameSession::Stop();
}
// Lab 01: Handles standard keyboard input such as spacebar to shoot
void Asteroids::OnKeyPressed(uchar key, int x, int y) {
	switch (tolower(key)) {
	case ' ': mSpaceship->Shoot(); break;
	case 'q': mSpaceship->ShootMissile(); break;
	default: break;
	}
}
void Asteroids::OnKeyReleased(uchar key, int x, int y) {
	switch (tolower(key)) {
	default: break;
	}
}
// Lab 01: Handles special keys like the arrow keys for movement
void Asteroids::OnSpecialKeyPressed(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: mSpaceship->Thrust(mSpaceship->GetSpeedStat()); break;
	case GLUT_KEY_DOWN: mSpaceship->SetBraking(true); break; // Activates brakes
	case GLUT_KEY_LEFT: mSpaceship->Rotate(mSpaceship->GetRotationStat()); break;
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-mSpaceship->GetRotationStat()); break;
	default: break;
	}
}
void Asteroids::OnSpecialKeyReleased(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: mSpaceship->Thrust(0.0f); break;
	case GLUT_KEY_DOWN: mSpaceship->SetBraking(false); break; // Deactivates brakes
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0.0f); break;
	default: break;
	}
}
void Asteroids::OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {
	if (object->GetType() == GameObjectType("Asteroid")) {
		mAsteroidCount++;
		mActiveAsteroids.push_back(object);
	}
	else if (dynamic_pointer_cast<HomingMissile>(object)) {
		mActiveMissiles.push_back(object);
	}
	else if (dynamic_pointer_cast<Turret>(object)) {
		mActiveTurrets.push_back(object);
	}
}
// Lab 04: Callback method from the Observer Pattern when an object is removed from the GameWorld
void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object) {
	// Lab 03: Uses GetType() (RTTI) to identify if the removed object is an asteroid
	if (object->GetType() == GameObjectType("Asteroid")) {
		mActiveAsteroids.remove(object);
		// Spawns explosion VFX exactly where the asteroid is
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		// Safely downcasts the GameObject to an asteroid to access its properties
		shared_ptr<Asteroid> ast = dynamic_pointer_cast<Asteroid>(object);
		float radius = 10.0f;
		shared_ptr<BoundingSphere> bs = dynamic_pointer_cast<BoundingSphere>(object->GetBoundingShape());
		if (bs) radius = bs->GetRadius();
		// If destroyed by player, adds to UI queue based on size
		if (ast && ast->GetHitByBullet()) {
			ScoreEvent ev;
			if (radius > 20.0f) { ev.name = "Massive Asteroid"; ev.pointsLeft = 200; }
			else if (radius > 15.0f) { ev.name = "Large Asteroid"; ev.pointsLeft = 100; }
			else if (radius > 10.0f) { ev.name = "Medium Asteroid"; ev.pointsLeft = 50; }
			else { ev.name = "Small Asteroid"; ev.pointsLeft = 20; }
			mScoreEvents.push_back(ev);
			UpdateScoreUI();
		}
		float childRadius = 0.0f; // Asteroid splitting logic
		float childScale = 0.0f;
		float childSpeed = 15.0f;
		// Determines size of new children asteroids based on parent's radius
		if (radius > 20.0f) {
			childRadius = 17.5f;
			childScale = 0.35f;
			childSpeed = 6.0f;
		}
		else if (radius > 15.0f) {
			childRadius = 12.5f;
			childScale = 0.25f;
			childSpeed = 10.0f;
		}
		else if (radius > 10.0f) {
			childRadius = 7.5f;
			childScale = 0.15f;
			childSpeed = 18.0f;
		}
		// If it is big enough to split, then it spawns 2 smaller child rocks moving at random angles
		if (childRadius > 0) {
			for (int i = 0; i < 2; i++) {
				float angle = (rand() % 360) * DEG2RAD;
				GLVector3f childVel(childSpeed * cos(angle), childSpeed * sin(angle), 0);
				shared_ptr<Asteroid> child = make_shared<Asteroid>(1, object->GetPosition(), childVel);
				Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
				shared_ptr<Sprite> ast_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
				ast_sprite->SetLoopAnimation(true);
				child->SetSprite(ast_sprite);
				child->SetScale(childScale);
				child->SetBoundingShape(make_shared<BoundingSphere>(child->GetThisPtr(), childRadius));
				mGameWorld->AddObject(child);
			}
		}
		// Random chance table from 0 to 49 for asteroid drops
		int dropRoll = rand() % 50;
		if (dropRoll == 0) {
			shared_ptr<HomingPickup> pickup = make_shared<HomingPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("missile_pickup");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 1) {
			shared_ptr<TurretPickup> pickup = make_shared<TurretPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("turret_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 2) {
			shared_ptr<SpiralPickup> pickup = make_shared<SpiralPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spiral_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 3) {
			shared_ptr<SpeedPickup> pickup = make_shared<SpeedPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("speed_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 4) {
			shared_ptr<RotationPickup> pickup = make_shared<RotationPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("rotation_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 5) {
			shared_ptr<FireRatePickup> pickup = make_shared<FireRatePickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("firing_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 6) {
			shared_ptr<ShieldPickup> pickup = make_shared<ShieldPickup>(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("shield_icon");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			pickup->SetSprite(sprite);
			pickup->SetScale(0.5f);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 10.0f));
			mGameWorld->AddObject(pickup);
		}
		else if (dropRoll == 7) {
			shared_ptr<ExtraLife> pickup = make_shared<ExtraLife>();
			pickup->SetPosition(object->GetPosition());
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("heart");
			shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			sprite->SetLoopAnimation(true);
			pickup->SetSprite(sprite);
			pickup->SetBoundingShape(make_shared<BoundingSphere>(pickup->GetThisPtr(), 8.0f));
			mGameWorld->AddObject(pickup);
		}
	}
	else if (dynamic_pointer_cast<HomingMissile>(object)) {
		mActiveMissiles.remove(object);
	}
	else if (dynamic_pointer_cast<Turret>(object)) {
		mActiveTurrets.remove(object);
	}
	// Updates the players life total visually when they collect a heart
	if (object->GetType() == GameObjectType("ExtraLife")) {
		shared_ptr<ExtraLife> extraLife = static_pointer_cast<ExtraLife>(object);
		if (extraLife->mPickedUp) {
			mPlayer.AddLife();
			UpdateLivesUI();
		}
	}
	// Handles portal spawns for the largest level of asteroids 
	if (object->GetType() == GameObjectType("Portal")) {
		GLVector3f pos = object->GetPosition();
		float angle = rand() % 360;
		GLVector3f vel(5.0f * cos(DEG2RAD * angle), 5.0f * sin(DEG2RAD * angle), 0);
		shared_ptr<Asteroid> massiveAst = make_shared<Asteroid>(2, pos, vel);
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> ast_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		ast_sprite->SetLoopAnimation(true);
		massiveAst->SetSprite(ast_sprite);
		massiveAst->SetScale(0.4f);
		massiveAst->SetBoundingShape(make_shared<BoundingSphere>(massiveAst->GetThisPtr(), 20.0f));
		mGameWorld->AddObject(massiveAst);
	}
}
// Called every frame to process UI logic and tracking algorithms
void Asteroids::OnWorldUpdated(GameWorld* world) {
	// Dynamically updates the HUD based on the players current stats or ammo
	if (mSpaceship) {
		std::ostringstream ms;
		ms << "Missiles: " << mSpaceship->GetAmmo();
		if (mMissileCountLabel) mMissileCountLabel->SetText(ms.str());
		int sSecs = mSpaceship->GetSpiralTimer() / 1000;
		if (sSecs > 0) {
			std::ostringstream ss;
			ss << "Spiral: " << sSecs << "s";
			mSpiralTimerLabel->SetText(ss.str());
			mSpiralTimerLabel->SetVisible(true);
		}
		else {
			mSpiralTimerLabel->SetVisible(false);
		}
		int shSecs = mSpaceship->GetInvulnerableTimer() / 1000;
		if (shSecs > 0) {
			std::ostringstream shs;
			shs << "Shield: " << shSecs << "s";
			mShieldTimerLabel->SetText(shs.str());
			mShieldTimerLabel->SetVisible(true);
		}
		else {
			mShieldTimerLabel->SetVisible(false);
		}
		std::ostringstream stats;
		stats << "Move Speed: " << mSpaceship->GetSpeedStat() << "  |  Rotation Speed: " << mSpaceship->GetRotationStat() << "  |  Firing Rate: " << mSpaceship->GetFireRateStat();
		if (mStatsLabel) mStatsLabel->SetText(stats.str());
	}
	// Iterates through active missiles to find the closest asteroid to home towards
	for (auto it = mActiveMissiles.begin(); it != mActiveMissiles.end(); ++it) {
		shared_ptr<GameObject> nearest;
		float minDist = 999999.0f;
		for (auto astIt = mActiveAsteroids.begin(); astIt != mActiveAsteroids.end(); ++astIt) {
			float dist = ((*astIt)->GetPosition() - (*it)->GetPosition()).length();
			if (dist < minDist) {
				minDist = dist;
				nearest = *astIt;
			}
		}
		if (nearest) {
			shared_ptr<HomingMissile> missile = dynamic_pointer_cast<HomingMissile>(*it);
			if (missile) {
				missile->SetTarget(nearest->GetPosition()); // Feeds the target to the missiles update loop
			}
		}
	}
	// Turrets use similar logic to aim at the nearest asteroid independently
	for (auto it = mActiveTurrets.begin(); it != mActiveTurrets.end(); ++it) {
		shared_ptr<GameObject> nearest;
		float minDist = 999999.0f;
		for (auto astIt = mActiveAsteroids.begin(); astIt != mActiveAsteroids.end(); ++astIt) {
			float dist = ((*astIt)->GetPosition() - (*it)->GetPosition()).length();
			if (dist < minDist) {
				minDist = dist;
				nearest = *astIt;
			}
		}
		shared_ptr<Turret> turret = dynamic_pointer_cast<Turret>(*it);
		if (turret) {
			if (nearest) {
				turret->SetTargetAsteroid(nearest->GetPosition(), true);
			}
			else {
				turret->SetTargetAsteroid(GLVector3f(), false);
			}
		}
	}
}
// Builds the GUI usinh standard normalised coordinates from 0.0 to 1.0
void Asteroids::CreateGUI() {
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Sets up main score
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> score_component = static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));
	// Sets up labels for score queue
	for (int i = 0; i < 5; i++) {
		mEventLabels[i] = make_shared<GUILabel>("");
		mEventLabels[i]->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mEventLabels[i]->SetVisible(false);
		shared_ptr<GUIComponent> lbl = static_pointer_cast<GUIComponent>(mEventLabels[i]);
		// Increments the Y offset to stack them
		mGameDisplay->GetContainer()->AddComponent(lbl, GLVector2f(0.0f, 0.95f - (i * 0.03f)));
	}
	// Sets up row of hearts as a health bar
	Image* heart_image = ImageManager::GetInstance().GetImageByName("heart_icon");
	for (int i = 0; i < 10; i++) {
		mLifeIcons[i] = make_shared<GUIIcon>(heart_image);
		shared_ptr<GUIComponent> heart_comp = static_pointer_cast<GUIComponent>(mLifeIcons[i]);
		mGameDisplay->GetContainer()->AddComponent(heart_comp, GLVector2f(0.005f + (i * 0.025f), 0.015f));
		mLifeIcons[i]->SetVisible(false);
	}
	UpdateLivesUI();
	mSpiralTimerLabel = make_shared<GUILabel>("Spiral: 0s");
	mSpiralTimerLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	shared_ptr<GUIComponent> spiral_comp = static_pointer_cast<GUIComponent>(mSpiralTimerLabel);
	mGameDisplay->GetContainer()->AddComponent(spiral_comp, GLVector2f(0.005f, 0.06f));
	mShieldTimerLabel = make_shared<GUILabel>("Shield: 0s");
	mShieldTimerLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	shared_ptr<GUIComponent> shield_comp = static_pointer_cast<GUIComponent>(mShieldTimerLabel);
	mGameDisplay->GetContainer()->AddComponent(shield_comp, GLVector2f(0.005f, 0.10f));
	std::string controls[] = { "SPACEBAR: Shoot", "Q: Homing Missiles", "LEFT / RIGHT: Turn left / right", "DOWN: Brake", "UP: Thrust" };
	for (int i = 0; i < 5; i++) {
		mControlLabels[i] = make_shared<GUILabel>(controls[i]);
		mControlLabels[i]->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
		mControlLabels[i]->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
		shared_ptr<GUIComponent> ctrl_comp = static_pointer_cast<GUIComponent>(mControlLabels[i]);
		mGameDisplay->GetContainer()->AddComponent(ctrl_comp, GLVector2f(1.0f, 0.015f + (i * 0.04f)));
	}
	mMissileCountLabel = make_shared<GUILabel>("Missiles: 0");
	mMissileCountLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mMissileCountLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	shared_ptr<GUIComponent> missile_count_comp = static_pointer_cast<GUIComponent>(mMissileCountLabel);
	mGameDisplay->GetContainer()->AddComponent(missile_count_comp, GLVector2f(1.0f, 0.95f));
	mStatsLabel = make_shared<GUILabel>("SPD: 15 | TRN: 180 | FIRE: 150");
	mStatsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mStatsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	shared_ptr<GUIComponent> stats_comp = static_pointer_cast<GUIComponent>(mStatsLabel);
	mGameDisplay->GetContainer()->AddComponent(stats_comp, GLVector2f(1.0f, 1.0f));
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameOverLabel->SetVisible(false);
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
}
// Lab 05: Uses timers to handle events like respawning the player safely
void Asteroids::OnTimer(int value) {
	if (value == CREATE_NEW_PLAYER) {
		mShipDead = false;
		ResetSpaceship();
		mGameWorld->AddObject(mSpaceship);
	}
	if (value == START_NEXT_LEVEL) {
		mLevel++;
		SetTimer(10000, START_NEXT_LEVEL);
	}
	if (value == SPAWN_ASTEROID) {
		if (mAsteroidCount < 10 + 2 * mLevel) {
			CreateAsteroids(1);
		}
		int spawnDelay = 3000 - (mLevel * 200); // Spawn rate speeds up as the level increases
		if (spawnDelay < 500) spawnDelay = 500;
		SetTimer(spawnDelay, SPAWN_ASTEROID);
	}
	if (value == SHOW_GAME_OVER) {
		mGameOverLabel->SetVisible(true);
	}
	// Custom timer to create the score animation
	if (value == ANIMATE_SCORE) {
		if (!mScoreEvents.empty()) {
			ScoreEvent& ev = mScoreEvents.front();
			int chunk = 1; // Number of points added per frame
			if (ev.pointsLeft < chunk) chunk = ev.pointsLeft;
			mDisplayScore += chunk;
			ev.pointsLeft -= chunk;
			std::ostringstream ss;
			ss << "Score: " << mDisplayScore;
			mScoreLabel->SetText(ss.str());
			if (ev.pointsLeft <= 0) {
				mScoreEvents.pop_front();
			}
			UpdateScoreUI();
		}
		SetTimer(50, ANIMATE_SCORE); // Recalls the timer for the loop
	}
}
void Asteroids::OnScoreChanged(int score) {}
void Asteroids::OnPlayerKilled(int lives_left) {
	if (mShipDead) return;
	mShipDead = true;
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);
	UpdateLivesUI();
	if (lives_left > 0) {
		SetTimer(1000, CREATE_NEW_PLAYER);
	}
	else {
		SetTimer(500, SHOW_GAME_OVER);
	}
}
// Iterates through the active score queue and displays up to 5 events under the score to display the killed asteroid types and teh score for it
void Asteroids::UpdateScoreUI() {
	int i = 0;
	for (auto it = mScoreEvents.begin(); it != mScoreEvents.end() && i < 5; ++it, ++i) {
		std::ostringstream ss;
		ss << it->name << " +" << it->pointsLeft;
		mEventLabels[i]->SetText(ss.str());
		mEventLabels[i]->SetVisible(true);
	}
	for (; i < 5; i++) {
		if (mEventLabels[i]) mEventLabels[i]->SetVisible(false);
	}
}
// Synchonises the visual heart icons with the player lives
void Asteroids::UpdateLivesUI() {
	int lives = mPlayer.GetLives();
	if (lives < 0) lives = 0;
	if (lives > 10) lives = 10;
	for (int i = 0; i < 10; i++) {
		if (mLifeIcons[i]) {
			if (i < lives) {
				mLifeIcons[i]->SetVisible(true);
			}
			else {
				mLifeIcons[i]->SetVisible(false);
			}
		}
	}
}
void Asteroids::CreateExtraLife() {}
// Lab 05: Uses AnimationManager to load and play a filmstrip sprite for explosions
shared_ptr<GameObject> Asteroids::CreateExplosion() {
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}
void Asteroids::ResetSpaceship() {
	if (!mSpaceship) return;
	mSpaceship->SetPosition(GLVector3f(0.0f, 0.0f, 0.0f));
	mSpaceship->SetVelocity(GLVector3f(0.0f, 0.0f, 0.0f));
	mSpaceship->SetAcceleration(GLVector3f(0.0f, 0.0f, 0.0f));
	mSpaceship->SetAngle(0.0f);
	mSpaceship->Rotate(0.0f);
	mSpaceship->Thrust(0.0f);
	mSpaceship->SetDead(false);
	mSpaceship->SetInvulnerable(3000);
	mSpaceship->ResetStats();
}
shared_ptr<GameObject> Asteroids::CreateSpaceship() {
	mSpaceship = make_shared<Spaceship>();
	// Initialises the physical collision space and graphics for the player
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	Animation* missile_anim = AnimationManager::GetInstance().GetAnimationByName("missile");
	if (missile_anim) {
		shared_ptr<Sprite> missile_sprite = make_shared<Sprite>(missile_anim->GetWidth(), missile_anim->GetHeight(), missile_anim);
		mSpaceship->SetMissileSprite(missile_sprite);
	}
	Animation* turret_anim = AnimationManager::GetInstance().GetAnimationByName("turret_anim");
	if (turret_anim) {
		shared_ptr<Sprite> turret_sprite = make_shared<Sprite>(turret_anim->GetWidth(), turret_anim->GetHeight(), turret_anim);
		turret_sprite->SetLoopAnimation(true);
		mSpaceship->SetTurretSprite(turret_sprite);
	}
	mSpaceship->SetScale(0.6f);
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 6.0f));
	ResetSpaceship();
	return mSpaceship;
}
void Asteroids::CreateAsteroids(const uint num_asteroids) {
	for (uint i = 0; i < num_asteroids; i++) {
		GLVector3f spawnPos;
		GLVector3f shipPos = mSpaceship ? mSpaceship->GetPosition() : GLVector3f(0, 0, 0);
		// Uses a do and while loop to ensure asteroids dont spawn on top of the player
		do {
			spawnPos.x = (rand() % 1280) - 640;
			spawnPos.y = (rand() % 720) - 360;
			spawnPos.z = 0;
		} 
		while ((spawnPos - shipPos).length() < 450.0f);
		int spawnType = rand() % 100;
		if (spawnType < 10) {
			shared_ptr<GameObject> portal = make_shared<Portal>(spawnPos);
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("portal");
			shared_ptr<Sprite> portal_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			portal_sprite->SetLoopAnimation(false);
			portal->SetSprite(portal_sprite);
			portal->SetScale(1.0f);
			mGameWorld->AddObject(portal);
		}
		else {
			float angle = rand() % 360;
			float speed = 10.0f;
			float scale = 0.2f;
			float radius = 10.0f;
			// Adjusts the speed and scale based on the asteroid type 
			if (spawnType >= 10 && spawnType < 40) {
				speed = 8.0f;
				scale = 0.25f;
				radius = 12.5f;
			}
			else if (spawnType >= 40 && spawnType < 80) {
				speed = 15.0f;
				scale = 0.15f;
				radius = 7.5f;
			}
			else {
				speed = 4.0f;
				scale = 0.35f;
				radius = 17.5f;
			}
			GLVector3f velocity(speed * cos(DEG2RAD * angle), speed * sin(DEG2RAD * angle), 0);
			shared_ptr<Asteroid> asteroid = make_shared<Asteroid>(1, spawnPos, velocity);
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
			shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			asteroid_sprite->SetLoopAnimation(true);
			asteroid->SetSprite(asteroid_sprite);
			asteroid->SetScale(scale);
			asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), radius)); // Bounding spheres from Labs again
			mGameWorld->AddObject(asteroid);
		}
	}
}
// Procedurally generates parallax moving stars in the bakcground
void Asteroids::CreateStars() {
	for (int i = 0; i < 100; i++) {
		int layer = (rand() % 3) + 1;
		shared_ptr<GameObject> star = make_shared<Star>(layer, mSpaceship);
		mGameWorld->AddObject(star);
	}
}