#include "GameManager.h"
#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include <iostream>
#include "random.h"

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
	hidden_rendering_mode = false;
	shading_mode = true;
	textured = false;
	normalMode = false;

	srand(time(0));

	Pos plaPos1 = { Random::getRandomFloat(3, 7), Random::getRandomFloat(4, 9) };
	Planetary* planetary1 = new Planetary(plaPos1, 1, 0.7, 0.5);
	planetaries.push_back(planetary1);

	Pos plaPos2 = { Random::getRandomFloat(-7, -3), Random::getRandomFloat(4, 9) };
	Planetary* planetary2 = new Planetary(plaPos2, 0.8, 0.6, 0.5);
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
		break;
	case 'r':
		hidden_rendering_mode = !hidden_rendering_mode;
		break;
	case 't':
		textured = !textured;
		break;
	case 'n':
		normalMode = !normalMode;
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
	
	bullets.remove_if(bulletEnemy);	//화면에서 지울 bullet 검사.

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

	static int enemyShootTimer = 100;
	static int enemyRespawnTimer = 0;
	static int enemyMoveTimer = 0;

	enemyShootTimer+=10;
	enemyMoveTimer+=100;
	bulletRefreshTimer -= 10 + 10*(player->getBulletLevel());

	if (player != NULL)
		player->updateParts();
	if (enemy != NULL)
		enemy->updateParts();

	if (enemy == NULL)				//다음 레벨 enemy가 나타날 때까지 대기시간
	{
		enemyRespawnTimer+=5;
		if (enemyRespawnTimer > 1000)
		{
			enemyRespawnTimer = 0;
			enemy = new Enemy(level);
		}
	}

	for (Bullet* b : bullets)		//모든 bullet을 움직이고, 충돌검사함.
	{
		b->move();
		b->checkBulletHit();
	}
	bullets.remove_if(bulletExpired);	//화면에서 지울 bullet 검사.
	
	if (enemyShootTimer > 2000 - 100 * level)		//레벨마다 enemy의 공격속도가 다름
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

	if (enemyMoveTimer > 1000 - 30 * level)		//레벨마다 enemy의 이동속도가 다름
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

bool GameManager::isHiddenRenderingMode()
{
	return hidden_rendering_mode;
}

bool GameManager::isShadingMode()
{
	return shading_mode;
}

bool GameManager::isTextured()
{
	return textured;
}

bool GameManager::isNormalMode()
{
	return normalMode;
}

std::list<Planetary*> GameManager::getPlanetaries()
{
	return planetaries;
}