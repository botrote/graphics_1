#pragma once
#include <vector>
#include <list>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

class GameManager
{
public:
	~GameManager();
	static GameManager* getInstance();
	Player* getPlayer();
	Enemy* getEnemy();
	std::list<Bullet*> getBullets();
	bool isAllPass();
	bool isAllFail();
	void onKeyInput(char key);
	void onSpecialInput(int key);
	void onPlayerHit();
	void onPlayerDeath();
	void onEnemyHit();
	void onEnemyDeath();
	void updateState();

private:
	GameManager();
	static GameManager* instance;
	Player* player;
	Enemy* enemy;
	std::list<Bullet*> bullets;
	int level;
	bool allPass;
	bool allFail;
};

