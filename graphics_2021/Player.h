#pragma once
#include "Plane.h"


class Player : public Plane
{
public:
	Player();
	~Player();
	void onHit();
	void onItemHit();
	int getBulletLevel();

private:
	int bullet_level;
};




