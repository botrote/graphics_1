#pragma once
#include "Bullet.h"
#include "GameManager.h"
#include "Plane.h"
#include "default.h"

class Drawer
{
public:
	static void drawGame(GameManager* gameManager);
	static void drawUI(Player* player, Enemy* enemy, bool allPass, bool allFail);
	static Color getRandomColor();
private:
	//static void drawThing(Drawable drawble);
	//static void drawPlane(Plane* plane);
	//static void drawBullet(Bullet* bullet);
};