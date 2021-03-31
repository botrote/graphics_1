#include <windows.h>
#include <GL/GL.h>
#include <GL/glut.h>
#include "Drawer.h"
#include "random.h"
#include <iostream>

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
	glMatrixMode(GL_MODELVIEW);

	Player* player = gameManager->getPlayer();
	if (player != NULL)
	{
		glLoadIdentity();
		Pos playerPos = player->getPos();
		Color playerColor = player->getColor();
		glColor3f(playerColor.r, playerColor.g, playerColor.b);
		glTranslatef(playerPos.x, playerPos.y, 0);
		glRectf(-0.1, -0.1, 0.1, 0.1);
	}

	Enemy* enemy = gameManager->getEnemy();
	if (enemy != NULL)
	{
		glLoadIdentity();
		Pos enemyPos = enemy->getPos();
		Color enemyColor = enemy->getColor();
		glColor3f(enemyColor.r, enemyColor.g, enemyColor.b);
		glTranslatef(enemyPos.x, enemyPos.y, 0);
		glRectf(-0.1, -0.1, 0.1, 0.1);
	}

	std::list<Bullet*> bullets = gameManager->getBullets();
	for (Bullet* b : bullets)
	{
		glLoadIdentity();
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();
		glColor3f(bulletColor.r, bulletColor.g, bulletColor.b);
		glTranslatef(bulletPos.x, bulletPos.y, 0);
		glRectf(-0.03, -0.03, 0.03, 0.03);
	}

	glLoadIdentity();
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
