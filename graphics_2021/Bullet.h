#pragma once
#include "default.h"

class Bullet
{
public:
	Bullet(Team team);
	void move();
	void setColor(Color color);
	Pos getPos();
	Color getColor();
	bool isExpired();
	void checkBulletHit();

private:
	Pos position;
	Color color;
	Team team;
	const float speed;
	bool isHit;
};

