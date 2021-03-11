#include "Bullet.h"
#include "Drawer.h"
#include "GameManager.h"
#include <cmath>

bool isNear(Pos p1, Pos p2, float target);

Bullet::Bullet(Team team) : speed(0.0005)
{
	this->team = team;
	Color temp;
	if (team == Team::PLAYER)
	{
		position = GameManager::getInstance()->getPlayer()->getPos();
		temp.r = 0;
		temp.g = 1;
		temp.b = 0;
	}
	else
	{
		position = GameManager::getInstance()->getEnemy()->getPos();
		temp.r = 0;
		temp.g = 0;
		temp.b = 1;
	}
	
	setColor(temp);
	isHit = false;
}

void Bullet::setColor(Color color)
{
	this->color = color;
}

void Bullet::move()
{
	if (team == Team::PLAYER)
	{
		position.y += speed;
	}
	else if (team == Team::ENEMY)
	{
		position.y -= speed;
	}
}

bool Bullet::isExpired()
{
	if (team == Team::ENEMY)
		return (isHit || position.y < -1.0);
	else
		return (isHit || position.y > 1.0);
}

Pos Bullet::getPos()
{
	return position;
}

Color Bullet::getColor()
{
	return color;
}

void Bullet::checkBulletHit()
{
	if (team == Team::PLAYER)
	{
		Enemy* enemy = GameManager::getInstance()->getEnemy();
		if (enemy == NULL)
			return;
		if (isNear(position, enemy->getPos(), 0.1))
		{
			GameManager::getInstance()->onEnemyHit();
			isHit = true;
		}
	}
	else
	{
		Player* player = GameManager::getInstance()->getPlayer();
		if (player == NULL)
			return;
		if (isNear(position, player->getPos(), 0.1))
		{
			GameManager::getInstance()->onPlayerHit();
			isHit = true;
		}
	}
}

bool isNear(Pos p1, Pos p2, float target)
{
	float distX = p1.x - p2.x;
	float distY = p1.y - p2.y;

	return (pow(distX, 2) + pow(distY, 2)) < pow(target, 2);
}