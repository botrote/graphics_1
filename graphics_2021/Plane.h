#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "default.h"


class Plane
{
public:
	Plane();
	~Plane();
	void move(Direction dir);
	void setColor(Color color);
	Color getColor();
	Pos getPos();
	int getHealth();
	virtual void onHit() = 0;
	void updateParts();
	float getWingAngle();
	float getCannonAngle();
	
protected:
	int health;
	Team team;
	Pos position;

private:
	Color color;
	const float speed;
	float wingAngle;
	bool wingGetsWider;
	float cannonAngle;
	bool cannonGetsWider;
	
};

