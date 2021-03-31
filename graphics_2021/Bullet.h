#pragma once
#include "default.h"

class Bullet
{
public:
	Bullet(Team team, Type type, int angle);
	void move();
	void setColor(Color color);
	Pos getPos();
	Color getColor();
	Team getTeam();
	Type getType();
	bool isExpired();
	bool isEnemyBullet();
	void checkBulletHit();

private:
	Pos position;
	Color color;
	Team team;
	Type type;
	const float speed;
	bool isHit;
	int angle;
};

