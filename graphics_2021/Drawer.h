#pragma once
#include "Bullet.h"
#include "GameManager.h"
#include "Plane.h"
#include "Planetary.h"
#include "default.h"

class Drawer
{
public:
	static void drawGame(GameManager* gameManager);
	static Color getRandomColor();
private:
	bool type;
	static void drawPlanetary(Planetary* planetary);
	static void drawPlayer(Player* player);
	static void drawEnemy(Enemy* enemy);
	static void drawBullet(Bullet* bullet);
	static void drawUI(Player* player, Enemy* enemy, bool allPass, bool allFail);
	static void drawGrid();
	static void setView(bool isFirst, Player* player);
};