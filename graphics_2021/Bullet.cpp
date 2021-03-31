#include "Bullet.h"
#include "Drawer.h"
#include "GameManager.h"
#include <cmath>
#include "random.h"

bool isNear(Pos p1, Pos p2, float target);
float angle_set[5] = { 0, 0.0001, -0.0001, 0.0002, -0.0002 };

Bullet::Bullet(Team team, Type type, int angle) : speed(0.0005)
{
	this->team = team;
	this->type = type;
	this->angle = angle;

	Color temp;
	if (team == Team::PLAYER)
	{
		position = GameManager::getInstance()->getPlayer()->getPos();
		temp.r = 0;
		temp.g = 1;
		temp.b = 0;
	}
	else if (type == Type::ITEM)
	{
		position.x = Random::getRandomFloat(-0.7, 0.7);
		position.y = 0.7;
		temp.r = 1;
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
		position.x += angle_set[angle];
		
	}
	else if (team == Team::ENEMY)
	{
		position.y -= speed;
		position.x += angle_set[angle];
	}
}

bool Bullet::isExpired()
{
	if (team == Team::ENEMY)
		return (isHit || position.y < -1.0);
	else
		return (isHit || position.y > 1.0);
}

bool Bullet::isEnemyBullet()
{
	if (team == Team::ENEMY)
		return true;
	else return false;
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
	if (team == Team::PLAYER)								//적이 맞았을 때
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
	{														//플레이어가 맞았을 때
		Player* player = GameManager::getInstance()->getPlayer();
		if (player == NULL)
			return;
		if (isNear(position, player->getPos(), 0.1))
		{
			GameManager::getInstance()->onPlayerHit(type);
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

Team Bullet::getTeam()
{
	return team;
}

Type Bullet::getType()
{
	return type;
}