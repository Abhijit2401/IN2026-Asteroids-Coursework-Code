#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h" // Lab 04: Includes the listener interfaces
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <list>
#include <string>

// Struct used for managing UI queue system
struct ScoreEvent {
	std::string name;
	int pointsLeft;
};
class GameObject;
class Spaceship;
class GUILabel;
class GUIIcon;

// Lecture 04: Implements the Observer pattern and asteroids inherits from multiple listener interfaces, meaning it responds to events from the keyboard, game world, score and the player
class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener {
public:
	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);
	virtual void Start(void);
	virtual void Stop(void);
	virtual void OnKeyPressed(uchar key, int x, int y);
	virtual void OnKeyReleased(uchar key, int x, int y);
	virtual void OnSpecialKeyPressed(int key, int x, int y);
	virtual void OnSpecialKeyReleased(int key, int x, int y);
	virtual void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object);
	virtual void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);
	virtual void OnWorldUpdated(GameWorld* world);
	virtual void OnScoreChanged(int score);
	virtual void OnPlayerKilled(int lives_left);
	virtual void OnTimer(int value);

private:
	// Lecture 02: Uses shared_ptr to manage memory without memory leaks
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel; // Lab 04: Uses the GUILabel class to display the Heads Up Display (HUD)
	shared_ptr<GUIIcon> mLifeIcons[10];
	void UpdateLivesUI();
	shared_ptr<GUILabel> mMissileCountLabel;
	shared_ptr<GUILabel> mSpiralTimerLabel;
	shared_ptr<GUILabel> mStatsLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	std::list<ScoreEvent> mScoreEvents;
	int mDisplayScore;
	shared_ptr<GUILabel> mEventLabels[5];
	void UpdateScoreUI();
	shared_ptr<GUILabel> mShieldTimerLabel;
	shared_ptr<GUILabel> mControlLabels[5];
	uint mLevel;
	uint mAsteroidCount;
	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateAsteroids(const uint num_asteroids);
	void CreateGUI();
	void CreateStars();
	shared_ptr<GameObject> CreateExplosion();
	void CreateExtraLife();
	const static uint SHOW_GAME_OVER = 0; // IDs used for identifying different timer events
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint SPAWN_ASTEROID = 3;
	ScoreKeeper mScoreKeeper;
	Player mPlayer;
	std::list<shared_ptr<GameObject>> mActiveAsteroids;
	std::list<shared_ptr<GameObject>> mActiveMissiles;
	std::list<shared_ptr<GameObject>> mActiveTurrets;
	bool mShipDead; // Flag to prevent mutliple explosions or other logic from occurring when spaceship dies
};

#endif