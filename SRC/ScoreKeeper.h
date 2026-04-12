#ifndef __SCOREKEEPER_H__
#define __SCOREKEEPER_H__

#include "GameUtil.h"

#include "GameObject.h"
#include "GameObjectType.h"
#include "IScoreListener.h"
#include "IGameWorldListener.h"
#include "Asteroid.h"
#include "BoundingSphere.h"


// ScoreKeeper inherits from IGameWorldListener so it can use the game world for events without the GameWorld needing to know how scoring works.
class ScoreKeeper : public IGameWorldListener {
public:
	ScoreKeeper() { 
		mScore = 0; 
	}
	virtual ~ScoreKeeper() { }
	void OnWorldUpdated(GameWorld* world) { }
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) { }
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object) {
		if (object->GetType() == GameObjectType("Asteroid")) {
			shared_ptr<Asteroid> asteroid = static_pointer_cast<Asteroid>(object);
			if (asteroid->mHitByBullet) {
				float radius = 10.0f;
				shared_ptr<BoundingSphere> bs = dynamic_pointer_cast<BoundingSphere>(asteroid->GetBoundingShape());
				if (bs) {
					radius = bs->GetRadius();
				}
				// Score points based on the radius of asteroids
				if (radius > 20.0f) {
					mScore += 500;
				}
				else if (radius > 15.0f) {
					mScore += 300;
				}
				else if (radius > 10.0f) {
					mScore += 200;
				}
				else {
					mScore += 100;
				}
				FireScoreChanged(); // Adds the score to the UI queue list
			}
		}
	}
	// Allows for other classes like asteroids to "subscribe" to score updates
	void AddListener(shared_ptr<IScoreListener> listener) {
		mListeners.push_back(listener);
	}
	// Loops through all "subscribed" listeners and pushes the new score to them
	void FireScoreChanged() {
		for (ScoreListenerList::iterator lit = mListeners.begin(); lit != mListeners.end(); ++lit) {
			(*lit)->OnScoreChanged(mScore);
		}
	}
private:
	int mScore; // Tracks the actual internal score
	typedef std::list< shared_ptr<IScoreListener> > ScoreListenerList;
	ScoreListenerList mListeners;
};

#endif