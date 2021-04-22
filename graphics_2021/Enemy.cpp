#include "Enemy.h"
#include "GameManager.h"
#include "random.h"
#include <iostream>

Enemy::Enemy(int level)
{
	team = Team::ENEMY;
	health = level;
	position.x = Random::getRandomFloat(-3, 3);
	position.y = 6;
}

Enemy::~Enemy() 
{

}

void Enemy::onHit()
{
	health--;
	std::cout << "Enemy health: " << health << std::endl;
	if (health <= 0)
		GameManager::getInstance()->onEnemyDeath();
}