#pragma once
#include "Bullet.h"
#include "GameManager.h"
#include "Plane.h"
#include "Planetary.h"
#include "default.h"

class Drawer3D
{
public:
	static void drawGame(GameManager* gameManager);
	static void changeSize(int w, int h);
	static void initScene();
};

