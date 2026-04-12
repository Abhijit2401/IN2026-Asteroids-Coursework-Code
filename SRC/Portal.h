#pragma once
#ifndef __PORTAL_H__
#define __PORTAL_H__

#include "GameObject.h"

class Portal : public GameObject {
public:
	Portal(GLVector3f position);
	~Portal(void);
	virtual void Update(int t);
private:
	int mTimeToLive;
};

#endif