#include "Bullet.h"
#include "Drawer.h"
#include "GameManager.h"
#include <cmath>
#include "random.h"
#include <iostream>

bool isNear(Pos p1, Pos p2, float target);
bool isRectCollision(Pos center1, float width1, float height1, Pos center2, float width2, float height2);
float angle_set[5] = { 0, 0.0006, -0.0006, 0.0012, -0.0012 };

Bullet::Bullet(Team team, Type type, int angle) : speed(0.005)
{
	this->team = team;
	this->type = type;
	this->angle = angle;

	Color temp;
	if (team == Team::PLAYER)
	{
		position = GameManager::getInstance()->getPlayer()->getPos();
		position.y += 1;
		temp.r = 0;
		temp.g = 1;
		temp.b = 0;
	}
	else if (type == Type::ITEM)
	{
		position = GameManager::getInstance()->getEnemy()->getPos();
		temp.r = 1;
		temp.g = 1;
		temp.b = 0;
	}
	else
	{
		position = GameManager::getInstance()->getEnemy()->getPos();
		position.y -= 1;
		temp.r = 1;
		temp.g = 0;
		temp.b = 0;
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
		return (isHit || position.y < -7.0);
	else
		return (isHit || position.y > 7.0);
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
		Pos enemyPos = enemy->getPos();
		Pos enemyWingPos = enemyPos;
		enemyWingPos.y += 0.09;
		if (isRectCollision(enemyPos, 0.70, 0.70, position, 0.45, 0.45)) //몸체 충돌검사
			//|| isRectCollision(enemyWingPos, 0.3, 0.04, position, 0.06, 0.06)) //날개 충돌검사
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
		Pos playerPos = player->getPos();
		Pos playerWingPos = playerPos;
		playerWingPos.y -= 0.09;
		if (isRectCollision(playerPos, 0.70, 0.70, position, 0.45, 0.45)) //몸체 충돌검사
		    //|| isRectCollision(playerWingPos, 0.3, 0.04, position, 0.06, 0.06)) // 날개 충돌검사
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

bool isRectCollision(Pos center1, float width1, float height1, Pos center2, float width2, float height2)
{
	Pos rect1LeftDown = { center1.x - 0.5 * width1, center1.y - 0.5 * height1 };
	Pos rect1RightUp = { center1.x + 0.5 * width1, center1.y + 0.5 * height1 };

	Pos rect2Positions[4] = { {center2.x - 0.5 * width2, center2.y - 0.5 * height2},
							  {center2.x - 0.5 * width2, center2.y + 0.5 * height2},
							  {center2.x + 0.5 * width2, center2.y - 0.5 * height2},
							  {center2.x + 0.5 * width2, center2.y + 0.5 * height2}
							};

	for (int i = 0; i < 4; i++)
	{
		Pos cur = rect2Positions[i];
		if (cur.x > rect1LeftDown.x && cur.x < rect1RightUp.x && cur.y > rect1LeftDown.y && cur.y < rect1RightUp.y)
			return true;
	}
	return false;
}

Team Bullet::getTeam()
{
	return team;
}

Type Bullet::getType()
{
	return type;
}