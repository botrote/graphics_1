#include "Player.h"
#include "GameManager.h"
#include "Drawer.h"
#include <iostream>


Player::Player()
{
	team = Team::PLAYER;
	health = 3;
	setColor(Drawer::getRandomColor());
	position.x = 0.0;
	position.y = -0.7;
	bullet_level = 1;
}

Player::~Player()
{

}

void Player::onHit()
{
	setColor(Drawer::getRandomColor());
	health--;
	std::cout << "Player health: " << health << std::endl;
	if (health <= 0)
		GameManager::getInstance()->onPlayerDeath();
}

void Player::onItemHit()
{
	bullet_level++;
	std::cout << "Player bullet_level: " << bullet_level << std::endl;
}

int Player::getBulletLevel()
{
	return bullet_level;
}