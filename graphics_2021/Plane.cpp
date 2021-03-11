#include "Plane.h"
#include "Drawer.h"


Plane::Plane() : speed(0.05)
{
	setColor(Drawer::getRandomColor());
}

Plane::~Plane()
{

}

void Plane::move(Direction dir)
{
	if (dir == Direction::DOWN)
	{
		position.y -= speed;
		if (position.y < -0.9)
			position.y = -0.9;
	}
	else if (dir == Direction::UP)
	{
		position.y += speed;
		if (position.y > 0.9)
			position.y = 0.9;
	}
	else if (dir == Direction::LEFT)
	{
		position.x -= speed;
		if (position.x < -0.9)
			position.x = -0.9;
	}
	else if (dir == Direction::RIGHT)
	{
		position.x += speed;
		if (position.x > 0.9)
			position.x = 0.9;
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