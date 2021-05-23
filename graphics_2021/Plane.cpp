#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include "Plane.h"
#include "Drawer.h"
#include <iostream>

Plane::Plane() : speed(0.075)
{
	setColor(Drawer::getRandomColor());
	wingAngle = 0;
	cannonAngle = 45;
	wingGetsWider = true;
	cannonGetsWider = true;
}

Plane::~Plane()
{

}

void Plane::move(Direction dir)
{
	if (dir == Direction::DOWN)
	{
		position.y -= speed;
		if (position.y < -17.0)
			position.y = -17.0;
	}
	else if (dir == Direction::UP)
	{
		position.y += speed;
		if (position.y > 17.0)
			position.y = 17.0;
	}
	else if (dir == Direction::LEFT)
	{
		position.x -= speed;
		if (position.x < -5.0)
			position.x = -5.0;
	}
	else if (dir == Direction::RIGHT)
	{
		position.x += speed;
		if (position.x > 5.0)
			position.x = 5.0;
	}
}

void Plane::updateParts()
{
	if (wingGetsWider == true)
	{
		wingAngle += 0.1;
		if (wingAngle >= 30)
			wingGetsWider = false;
	}
	else
	{
		wingAngle -= 0.1;
		if (wingAngle <= 0)
			wingGetsWider = true;
	}

	if (cannonGetsWider == true)
	{
		cannonAngle += 0.1;
		if (cannonAngle >= 90)
			cannonGetsWider = false;
	}
	else
	{
		cannonAngle -= 0.1;
		if (cannonAngle <= 45)
			cannonGetsWider = true;
	}
}

void Plane::setColor(Color color)
{
	this->color = color;
}

Pos Plane::getPos()
{
	return position;
}

Color Plane::getColor()
{
	return color;
}

int Plane::getHealth()
{
	return health;
}

float Plane::getWingAngle()
{
	return wingAngle;
}

float Plane::getCannonAngle()
{
	return cannonAngle;
}