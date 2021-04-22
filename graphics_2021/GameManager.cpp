#include "GameManager.h"
#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include <iostream>
#include "random.h"
#include "Drawer3D.h"

GameManager* GameManager::instance = NULL;
int bulletRefreshTimer = 0;

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
	firstViewing = false;
	srand(time(0));

	Pos plaPos1 = { Random::getRandomFloat(3, 7), Random::getRandomFloat(4, 9) };
	Planetary* planetary1 = new Planetary(plaPos1, 1, 0.5, 0.25);
	planetaries.push_back(planetary1);

	Pos plaPos2 = { Random::getRandomFloat(-7, -3), Random::getRandomFloat(4, 9) };
	Planetary* planetary2 = new Planetary(plaPos2, 0.8, 0.4, 0.2);
	planetaries.push_back(planetary2);

	/*
	Pos plaPos3 = { Random::getRandomFloat(-1, 1), Random::getRandomFloat(-1, 1) };
	Planetary* planetary3 = new Planetary(plaPos3, 0.15, 0.075, 0.05);
	planetaries.push_back(planetary3);
	*/
}

GameManager::~GameManager()
{

}

void GameManager::onKeyInput(char key)
{
	if (level >= 6 || player == NULL)
		return;

	int bullet_level = player->getBulletLevel();

	switch (key) 
	{
	case ' ':		
		{
		if (allFail)
			return;
		for (int i = 0; i < bullet_level; i++)
		{
			if (bulletRefreshTimer < 0)
			{
				Bullet* temp = new Bullet(Team::PLAYER, Type::BULLET, i);
				bullets.push_back(temp);
			}
		}
		if (bulletRefreshTimer < 0)
			bulletRefreshTimer = 2500;
		break;
		}
	case 'c':
		allPass = !allPass;
		break;
	case 'f':
		allFail = !allFail;
		break;
	case 'v':
		firstViewing = !firstViewing;
		Drawer3D::updateViewing();
		break;
	}
	
	glutPostRedisplay();
}

void GameManager::onSpecialInput(int key)
{
	if (level >= 6 || player == NULL)
		return;

	switch (key) 
	{									//�÷��̾� �����¿� �����̴� �κ�
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

	Drawer3D::updateViewing();

}

void GameManager::onPlayerHit(Type type)
{
	if (type == Type::BULLET)
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
	else
	{
		player->onItemHit();
	}
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

bool bulletExpired(Bullet* b)
{
	return b->isExpired();
}

bool bulletEnemy(Bullet* b)
{
	return b->isEnemyBullet();
}


void GameManager::onEnemyDeath()
{
	std::cout << "Enemy " << level << " death!\n";
	
	bullets.remove_if(bulletEnemy);	//ȭ�鿡�� ���� bullet �˻�.

	Bullet* temp = new Bullet(Team::ENEMY, Type::ITEM, 0);
	bullets.push_back(temp);

	delete(enemy);
	enemy = NULL;

	level++;
	if (level >= 6)
	{
		std::cout << "You win!\n";
		//exit(0);
	}
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
	bulletRefreshTimer -= 3 + (player->getBulletLevel());

	if (player != NULL)
		player->updateParts();
	if (enemy != NULL)
		enemy->updateParts();

	if (enemy == NULL)				//���� ���� enemy�� ��Ÿ�� ������ ���ð�
	{
		enemyRespawnTimer++;
		if (enemyRespawnTimer > 1000)
		{
			enemyRespawnTimer = 0;
			enemy = new Enemy(level);
		}
	}

	for (Bullet* b : bullets)		//��� bullet�� �����̰�, �浹�˻���.
	{
		b->move();
		b->checkBulletHit();
	}
	bullets.remove_if(bulletExpired);	//ȭ�鿡�� ���� bullet �˻�.
	
	if (enemyShootTimer > 3000 - 200 * level)		//�������� enemy�� ���ݼӵ��� �ٸ�
	{
		enemyShootTimer = 0;
		if (enemy != NULL)
		{
			for (int i = 0; i < level; i++) {
				Bullet* temp = new Bullet(Team::ENEMY, Type::BULLET, i);
				bullets.push_back(temp);
			}
		}
	}

	if (enemyMoveTimer > 450 - 30 * level)		//�������� enemy�� �̵��ӵ��� �ٸ�
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

	for (Planetary* p : planetaries)
		p->update();

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

int GameManager::getLevel()
{
	return level;
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

bool GameManager::isFirstViewing()
{
	return firstViewing;
}

std::list<Planetary*> GameManager::getPlanetaries()
{
	return planetaries;
}