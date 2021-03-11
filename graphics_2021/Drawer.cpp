#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include "Drawer.h"
#include "random.h"

Color Drawer::getRandomColor()
{
	Color c;
	c.r = Random::getRandomFloat(0, 1);
	c.g = Random::getRandomFloat(0, 1);
	c.b = Random::getRandomFloat(0, 1);
	return c;
}

void Drawer::drawGame(GameManager* gameManager)
{
	glClear(GL_COLOR_BUFFER_BIT);

	std::list<Bullet*> bullets = gameManager->getBullets();
	for (Bullet* b : bullets)
	{
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();
		glColor3f(bulletColor.r, bulletColor.g, bulletColor.b);
		glRectf(bulletPos.x - 0.03, bulletPos.y - 0.03, bulletPos.x + 0.03, bulletPos.y + 0.03);
	}

	Player* player = gameManager->getPlayer();
	if (player != NULL)
	{
		Pos playerPos = player->getPos();
		Color playerColor = player->getColor();
		glColor3f(playerColor.r, playerColor.g, playerColor.b);
		glRectf(playerPos.x - 0.1, playerPos.y - 0.1, playerPos.x + 0.1, playerPos.y + 0.1);
	}

	Enemy* enemy = gameManager->getEnemy();
	if (enemy != NULL)
	{
		Pos enemyPos = enemy->getPos();
		Color enemyColor = enemy->getColor();
		glColor3f(enemyColor.r, enemyColor.g, enemyColor.b);
		glRectf(enemyPos.x - 0.1, enemyPos.y - 0.1, enemyPos.x + 0.1, enemyPos.y + 0.1);
	}

	drawUI(player, enemy, gameManager->isAllPass(), gameManager->isAllFail());

	glutSwapBuffers();
}

void Drawer::drawUI(Player* player, Enemy* enemy, bool allPass, bool allFail)
{
	if (player != NULL)
	{
		if (allFail)
		{
			glColor3f(0, 0, 0);
			glRectf(-0.9, -0.9, -0.85, -0.85);
		}
		else
		{
			glColor3f(1, 0, 0);
			for (int i = 0; i < player->getHealth(); i++)
			{
				glRectf(-0.9 + i * (0.1), -0.9, -0.85 + i * (0.1), -0.85);
			}
		}
	}

	if (enemy != NULL)
	{
		if (allPass)
		{
			glColor3f(0, 0, 0);
			glRectf(-0.9, +0.9, -0.85, +0.85);
			return;
		}
		else
		{
			glColor3f(1, 0, 0);
			for (int i = 0; i < enemy->getHealth(); i++)
			{
				glRectf(-0.9 + i * (0.1), 0.9, -0.85 + i * (0.1), 0.85);
			}
		}
	}
}
