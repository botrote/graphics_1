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

	drawGrid();

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
	//glBegin(GL_POLYGON);
	//for (int i = 0; i < 360; i += 2)
	//{
	//	double angle = i * 3.141592 / 180;
	//	double x = radius * cos(angle);
	//	double y = radius * sin(angle);
	//	glVertex2f(x, y);
	//}
	//glEnd();
	//glFinish();

	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);		//line만 그릴건지, 색으로 채울건지 등 모드 지정

	//gluSphere(obj, radius, 20, 20);
	glutWireSphere(radius, 20, 20);

}

void Drawer::drawPlanetary(Planetary* planetary)
{
	glLoadIdentity();
	glEnable(GL_POLYGON_OFFSET_FILL);		//hidden line remove
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

	glDisable(GL_POLYGON_OFFSET_FILL);
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
	//glRectf(-0.1, -0.1, 0.1, 0.1);
	glutWireCube(0.15);

	//머리
	glPushMatrix();
	glTranslatef(0, 0.09, 0);
	glScalef(1, 2, 1);
	//glRectf(-0.04, 0, 0.04, 0.1);
	glutWireCube(0.05);
	glPopMatrix();

	//우측날개
	glPushMatrix();
	//glTranslatef(0.03f, -0.1f, 0);
	//glRotatef(-(player->getWingAngle()), 0, 0, 1.0);
	//glRectf(0, 0, 0.15f, 0.070);
	glTranslatef(0.085f, -0.085f, 0);
	glRotatef(45, 0, 0, 1.0);
	glScalef(1, 1.5, 1);
	glutWireCube(0.08);
	glPopMatrix();

	//우측 캐논
	/*
	glPushMatrix();
	glTranslatef(0.15f, 0.0f, 0);
	glRotatef(+(player->getCannonAngle()), 0, 0, 1.0);
	glRectf(0.0f, 0.0f, 0.10f, 0.05f);
	glPopMatrix();
	glPopMatrix();
	*/

	//좌측날개
	glPushMatrix();
	//glTranslatef(-0.03f, -0.1f, 0);
	//glRotatef(+(player->getWingAngle()), 0, 0, 1.0);
	//glRectf(0, 0, -0.15f, 0.070);
	glTranslatef(-0.085f, -0.085f, 0);
	glRotatef(-45, 0, 0, 1.0);
	glScalef(1, 1.5, 1);
	glutWireCube(0.08);

	//좌측캐논
	/*
	glPushMatrix();
	glTranslatef(-0.15f, 0.0f, 0);
	glRotatef(-(player->getCannonAngle()), 0, 0, 1.0);
	glRectf(-0.0f, 0.0f, -0.10f, 0.05f);
	glPopMatrix();
	glPopMatrix();
	*/

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
	//glRectf(-0.1, -0.1, 0.1, 0.1);
	glutWireCube(0.15);

	//머리
	glPushMatrix();
	glTranslatef(0, -0.09, 0);
	glScalef(1, 2, 1);
	//glRectf(-0.04, 0, 0.04, 0.1);
	glutWireCube(0.05);
	glPopMatrix();

	//우측날개
	glPushMatrix();
	//glTranslatef(0.03f, -0.1f, 0);
	//glRotatef(-(player->getWingAngle()), 0, 0, 1.0);
	//glRectf(0, 0, 0.15f, 0.070);
	glTranslatef(0.085f, +0.085f, 0);
	glRotatef(-45, 0, 0, 1.0);
	glScalef(1, 1.5, 1);
	glutWireCube(0.08);
	glPopMatrix();

	//우측 캐논
	/*
	glPushMatrix();
	glTranslatef(0.15f, 0.0f, 0);
	glRotatef(-(enemy->getCannonAngle()), 0, 0, 1.0);
	glRectf(0.0f, 0.0f, 0.10f, -0.05f);
	glPopMatrix();
	glPopMatrix();
	*/

	//좌측날개
	glPushMatrix();
	//glTranslatef(-0.03f, 0.1f, 0);
	//glRotatef(-(enemy->getWingAngle()), 0, 0, 1.0);
	//glRectf(0, 0, -0.15f, -0.070);
	glTranslatef(-0.085f, +0.085f, 0);
	glRotatef(45, 0, 0, 1.0);
	glScalef(1, 1.5, 1);
	glutWireCube(0.08);

	//좌측캐논
	/*
	glPushMatrix();
	glTranslatef(-0.15f, 0.0f, 0);
	glRotatef(+(enemy->getCannonAngle()), 0, 0, 1.0);
	glRectf(-0.0f, 0.0f, -0.10f, -0.05f);
	glPopMatrix();
	glPopMatrix();
	*/

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
		//glRectf(-0.03, -0.03, 0.03, 0.03);
		glutSolidCube(0.06);
	}
	else
		glutSolidSphere(0.03, 10, 10);
		//glRectf(-0.03, -0.03, 0.03, 0.03);
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

void Drawer::drawGrid() 
{
	float i;
	glColor3f(0, 0, 0.4);
	
	glBegin(GL_LINES);
	for (i = -1.4; i < 1.5; i += 0.2) {
		glVertex3f(i, -1.5, 0);
		glVertex3f(i, 1.5, 0);
		glVertex3f(-1.5, i, 0);
		glVertex3f(1.5, i, 0);
	}
	glEnd();

	glColor3f(0, 0, 0);
	glRectf(-1, -1, 1, 1);
}

void Drawer::setView(bool isFirst, Player* player)
{
	Pos playerPos = player->getPos();

	//glViewport(0, 0, 800, 800);
	glLoadIdentity();
	if (isFirst)
		gluLookAt(playerPos.x, playerPos.y, 0.5, 10, 10, -0.5, 1, 0, 0);
	else
		gluLookAt(playerPos.x, playerPos.y, 0.5, 0.3, 0.3, -0.5, 1, 0, 0);
}