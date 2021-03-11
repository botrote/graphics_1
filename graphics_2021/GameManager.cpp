#include "GameManager.h"
#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include <iostream>
#include "random.h"

GameManager* GameManager::instance = NULL;

GameManager* GameManager::getInstance()
{
	if (GameManager::instance == NULL)
		GameManager::instance = new GameManager();
	return GameManager::instance;
}

GameManager::GameManager()
{
	level = 1;
	player = new Player();
	enemy = new Enemy(level);
	allPass = false;
	allFail = false;
	srand(time(0));
}

GameManager::~GameManager()
{

}

void GameManager::onKeyInput(char key)
{
	if (level >= 6 || player == NULL)
		return;

	switch (key) 
	{
	case ' ':		
		{
		if (allFail)
			return;
		Bullet* temp = new Bullet(Team::PLAYER);
		bullets.push_back(temp);
		break;
		}
	case 'c':
		allPass = !allPass;
		break;
	case 'f':
		allFail = !allFail;
		break;
	}
	
	glutPostRedisplay();
}

void GameManager::onSpecialInput(int key)
{
	if (level >= 6 || player == NULL)
		return;

	switch (key) 
	{									//플레이어 상하좌우 움직이는 부분
	case GLUT_KEY_DOWN:
		player->move(Direction::DOWN);
		break;
	case GLUT_KEY_UP:
		player->move(Direction::UP);
		break;
	case GLUT_KEY_RIGHT:
		player->move(Direction::RIGHT);
		break;
	case GLUT_KEY_LEFT:
		player->move(Direction::LEFT);
		break;
	}
	glutPostRedisplay();
}

void GameManager::onPlayerHit()
{
	if (allPass)
	{
		return;
	}
	if (allFail)
	{
		onPlayerDeath();
		return;
	}
	player->onHit();
}

void GameManager::onPlayerDeath()
{
	std::cout << "You lose...\n";
	delete(player);
	player = NULL;
	//exit(0);
}

void GameManager::onEnemyHit()
{
	if (allPass)
	{
		onEnemyDeath();
		return;
	}
	enemy->onHit();
}

void GameManager::onEnemyDeath()
{
	std::cout << "Enemy " << level << " death!\n";
	delete(enemy);
	enemy = NULL;
	level++;
	if (level >= 6)
	{
		std::cout << "You win!\n";
		//exit(0);
	}
}

bool bulletExpired(Bullet* b)
{
	return b->isExpired();
}

void GameManager::updateState()
{
	if (level >= 6 || player == NULL)
		return;

	static int enemyShootTimer = 0;
	static int enemyRespawnTimer = 0;
	static int enemyMoveTimer = 0;

	enemyShootTimer++;
	enemyMoveTimer++;

	if (enemy == NULL)
	{
		enemyRespawnTimer++;
		if (enemyRespawnTimer > 4000)
		{
			enemyRespawnTimer = 0;
			enemy = new Enemy(level);
		}
	}

	for (Bullet* b : bullets)
	{
		b->move();
		b->checkBulletHit();
	}
	bullets.remove_if(bulletExpired);

	if (enemyShootTimer > 3400 - 200 * level)
	{
		enemyShootTimer = 0;
		if (enemy != NULL)
		{
			Bullet* temp = new Bullet(Team::ENEMY);
			bullets.push_back(temp);
		}
	}

	if (enemyMoveTimer > 1500 - 100 * level)
	{
		enemyMoveTimer = 0;
		if(enemy != NULL)
		{
			int rand = Random::getRandomInt(0, 10);
			if (rand < 4)
				enemy->move(Direction::LEFT);
			else if(rand > 5)
				enemy->move(Direction::RIGHT);
		}
	}

	glutPostRedisplay();
}

Player* GameManager::getPlayer()
{
	return player;
}

Enemy* GameManager::getEnemy()
{
	return enemy;
}

std::list<Bullet*> GameManager::getBullets()
{
	return bullets;
}

bool GameManager::isAllPass()
{
	return allPass;
}

bool GameManager::isAllFail()
{
	return allFail;
}