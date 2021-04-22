#pragma once
#include <vector>
#include <list>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Planetary.h"

class GameManager
{
public:
	~GameManager();
	static GameManager* getInstance();
	Player* getPlayer();
	Enemy* getEnemy();
	int getLevel();
	std::list<Bullet*> getBullets();
	std::list<Planetary*> getPlanetaries();
	bool isAllPass();
	bool isAllFail();
	bool isFirstViewing();
	void onKeyInput(char key);
	void onSpecialInput(int key);
	void onPlayerHit(Type type);
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
	std::list<Planetary*> planetaries;
	int level;
	bool allPass;
	bool allFail;
	bool firstViewing;
};

