#pragma once
#include "Plane.h"

class Enemy : public Plane 
{
public:
	Enemy(int level);
	~Enemy();
	void onHit();

private:

};

