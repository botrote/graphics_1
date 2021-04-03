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

	std::list<Planetary*> planetaries = gameManager->getPlanetaries();
	for (Planetary* p : planetaries)
		drawPlanetary(p);

	std::list<Bullet*> bullets = gameManager->getBullets();
	for (Bullet* b : bullets)
		drawBullet(b);

	Player* player = gameManager->getPlayer();
	if (player != NULL)
		drawPlayer(player);

	Enemy* enemy = gameManager->getEnemy();
	if (enemy != NULL)
		drawEnemy(enemy);

	glLoadIdentity();
	drawUI(player, enemy, gameManager->isAllPass(), gameManager->isAllFail());

	glutSwapBuffers();
}

void drawCircle(float radius)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i += 2)
	{
		double angle = i * 3.141592 / 180;
		double x = radius * cos(angle);
		double y = radius * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glFinish();


//출처: https://blog.amaorche.com/25 [팬더노트]
}

void Drawer::drawPlanetary(Planetary* planetary)
{
	glLoadIdentity();

	//항성
	glColor3f(planetary->starColor.r, planetary->starColor.g, planetary->starColor.b);
	glPushMatrix();
	glTranslatef(planetary->starPosition.x, planetary->starPosition.y, 0);
	drawCircle(planetary->starRadius);
	
	//행성
	glColor3f(planetary->planetColor.r, planetary->planetColor.g, planetary->planetColor.b);
	glPushMatrix();
	glRotatef(planetary->planetAngle, 0, 0, 1);
	glTranslatef(planetary->planetRevoRadius, 0, 0);
	drawCircle(planetary->planetRadius);

	//위성
	glColor3f(planetary->satelliteColor.r, planetary->satelliteColor.g, planetary->satelliteColor.b);
	glPushMatrix();
	glRotatef(planetary->satelliteAngle, 0, 0, 1);
	glTranslatef(0, planetary->satelliteRevoRadius, 0);
	drawCircle(planetary->satelliteRadius);
	
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void Drawer::drawPlayer(Player* player)
{
	Pos playerPos = player->getPos();
	Color playerColor = player->getColor();
	glColor3f(playerColor.r, playerColor.g, playerColor.b);

	//몸체
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(playerPos.x, playerPos.y, 0);
	glRectf(-0.1, -0.1, 0.1, 0.1);

	//머리
	glPushMatrix();
	glTranslatef(0, 0.09, 0);
	glRectf(-0.04, 0, 0.04, 0.1);
	glPopMatrix();

	//우측날개
	glPushMatrix();
	glTranslatef(0.03f, -0.1f, 0);
	glRotatef(-(player->getWingAngle()), 0, 0, 1.0);
	glRectf(0, 0, 0.15f, 0.070);

	//우측 캐논
	glPushMatrix();
	glTranslatef(0.15f, 0.0f, 0);
	glRotatef(+(player->getCannonAngle()), 0, 0, 1.0);
	glRectf(0.0f, 0.0f, 0.10f, 0.05f);
	glPopMatrix();
	glPopMatrix();

	//좌측날개
	glPushMatrix();
	glTranslatef(-0.03f, -0.1f, 0);
	glRotatef(+(player->getWingAngle()), 0, 0, 1.0);
	glRectf(0, 0, -0.15f, 0.070);

	//좌측캐논
	glPushMatrix();
	glTranslatef(-0.15f, 0.0f, 0);
	glRotatef(-(player->getCannonAngle()), 0, 0, 1.0);
	glRectf(-0.0f, 0.0f, -0.10f, 0.05f);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();

}

void Drawer::drawEnemy(Enemy* enemy)
{
	Pos enemyPos = enemy->getPos();
	Color enemyColor = enemy->getColor();
	glColor3f(enemyColor.r, enemyColor.g, enemyColor.b);

	//몸체
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(enemyPos.x, enemyPos.y, 0);
	glRectf(-0.1, -0.1, 0.1, 0.1);

	//머리
	glPushMatrix();
	glTranslatef(0, -0.09, 0);
	glRectf(-0.04, 0, 0.04, -0.1);
	glPopMatrix();

	//우측날개
	glPushMatrix();
	glTranslatef(0.03f, 0.1f, 0);
	glRotatef(+(enemy->getWingAngle()), 0, 0, 1.0);
	glRectf(0, 0, 0.15f, -0.070);

	//우측 캐논
	glPushMatrix();
	glTranslatef(0.15f, 0.0f, 0);
	glRotatef(-(enemy->getCannonAngle()), 0, 0, 1.0);
	glRectf(0.0f, 0.0f, 0.10f, -0.05f);
	glPopMatrix();
	glPopMatrix();

	//좌측날개
	glPushMatrix();
	glTranslatef(-0.03f, 0.1f, 0);
	glRotatef(-(enemy->getWingAngle()), 0, 0, 1.0);
	glRectf(0, 0, -0.15f, -0.070);

	//좌측캐논
	glPushMatrix();
	glTranslatef(-0.15f, 0.0f, 0);
	glRotatef(+(enemy->getCannonAngle()), 0, 0, 1.0);
	glRectf(-0.0f, 0.0f, -0.10f, -0.05f);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void Drawer::drawBullet(Bullet* bullet)
{
	glLoadIdentity();
	Pos bulletPos = bullet->getPos();
	Color bulletColor = bullet->getColor();
	glColor3f(bulletColor.r, bulletColor.g, bulletColor.b);
	glTranslatef(bulletPos.x, bulletPos.y, 0);
	if (bullet->getType() == Type::ITEM)
	{
		glRotatef(45, 0, 0, 1);
		glRectf(-0.03, -0.03, 0.03, 0.03);
	}
	else
		glRectf(-0.03, -0.03, 0.03, 0.03);
}

void Drawer::drawUI(Player* player, Enemy* enemy, bool allPass, bool allFail)
{
	if (player != NULL)
	{
		if (allFail)
		{
			glColor3f(1, 1, 1);
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
			glColor3f(1, 1, 1);
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
