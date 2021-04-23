#include "Drawer3D.h"
#include <gl\glut.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdlib.h>
#include <iostream>

void drawGrid();
void drawPlayer();
void drawEnemy();
void drawBullets();
void drawUI();
void drawPlanetary();
void drawHiddenCube(float width);
void drawHiddenSphere(float width);

void Drawer3D::changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Drawer3D::updateViewing();
}

void Drawer3D::drawGame(GameManager* gameManager) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground
	glColor3f(0.9f, 0.9f, 0.9f);
	
	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();
	if (hidden_rendering_mode) {
		glEnable(GL_DEPTH_TEST);
	}

	drawGrid();
	drawBullets();
	drawPlayer();
	drawEnemy();
	//drawUI();
	drawPlanetary();
	glutSwapBuffers();

	if (hidden_rendering_mode) {
		glDisable(GL_DEPTH_TEST);
	}
}

void drawPlayer()
{
	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();

	if (player == NULL)
		return;

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;
	Color playerColor = player->getColor();

	glColor3f(playerColor.r, playerColor.g, playerColor.b);
	glPushMatrix();
	glTranslatef(playerX, 1, playerZ);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.35);
	}
	else
	{
		glutWireCube(0.7);
	}

	glPushMatrix();
	glTranslatef(0, 0, -1);
	glScalef(1, 1, 2);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, 0, 0.5);
	glScalef(1, 1, 2.5);
	glRotatef(-30, 0, 1, 0);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.5, 0, 0.5);
	glScalef(1, 1, 2.5);
	glRotatef(30, 0, 1, 0);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();


	glPopMatrix();

}

void drawEnemy()
{
	GameManager* gameManager = GameManager::getInstance();
	Enemy* enemy = gameManager->getEnemy();

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();
	if (enemy == NULL)
		return;

	float enemyX = enemy->getPos().x;
	float enemyZ = -enemy->getPos().y;
	Color enemyColor = enemy->getColor();

	glColor3f(enemyColor.r, enemyColor.g, enemyColor.b);
	glPushMatrix();
	glTranslatef(enemyX, 1, enemyZ);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.35);
	}
	else
	{
		glutWireCube(0.7);
	}

	glPushMatrix();
	glTranslatef(0, 0, 1);
	glScalef(1, 1, 2);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, 0, -0.5);
	glScalef(1, 1, 2.5);
	glRotatef(30, 0, 1, 0);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.5, 0, -0.5);
	glScalef(1, 1, 2.5);
	glRotatef(-30, 0, 1, 0);
	if (hidden_rendering_mode) {
		drawHiddenCube(0.2);
	}
	else
	{
		glutWireCube(0.4);
	}
	glPopMatrix();

	glPopMatrix();
}

void drawBullets()
{
	std::list<Bullet*> bullets = GameManager::getInstance()->getBullets();
	for (Bullet* b : bullets)
	{
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();
		glColor3f(bulletColor.r, bulletColor.g, bulletColor.b);

		float bulletX = bulletPos.x;
		float bulletZ = -bulletPos.y;

		glPushMatrix();
		glTranslatef(bulletX, 1, bulletZ);
		if (b->getType() == Type::ITEM)
		{
			//glRotatef(45, 0, 0, 1);
			//glRectf(-0.03, -0.03, 0.03, 0.03);
			glutSolidCube(0.25);
		}
		else
		{
			glutSolidSphere(0.25, 20, 20);
		}
		glPopMatrix();
	}
}

void drawUI()
{
	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();
	Enemy* enemy = gameManager->getEnemy();
	bool allPass = gameManager->isAllPass();
	bool allFail = gameManager->isAllFail();

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

void drawGrid()
{

	glColor3f(0.9, 0.9, 0.9);

	glBegin(GL_QUADS);
	glVertex3f(-5.5f, -0.5f, -7.3f);      //plane 크기에 따라 면적 조정할 것.
	glVertex3f(-5.5f, -0.5f, 7.3f);
	glVertex3f(5.5f, -0.5f, 7.3f);
	glVertex3f(5.5f, -0.5f, -7.3f);
	glEnd();

	float i;
	glColor3f(0, 0, 0.4);

	glBegin(GL_LINES);
	for (i = -15; i < 15; i += 0.5) {
		glVertex3f(i, -0.49f, -15.0f);
		glVertex3f(i, -0.49f, 15.0f);
		glVertex3f(-15.0f, -0.49f, i - 5.0f);
		glVertex3f(15.0f, -0.49f, i - 5.0f);
	}
	glEnd();

}

void drawPlanetary()
{
	GameManager* gameManager = GameManager::getInstance();

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	for (Planetary* planetary : gameManager->getPlanetaries())
	{
		glColor3f(planetary->starColor.r, planetary->starColor.g, planetary->starColor.b);
		glPushMatrix();
		glTranslatef(planetary->starPosition.x, 0, -planetary->starPosition.y);
		glRotatef(planetary->starAngle, 0, 1, 0);
		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->starRadius);
		}
		else
		{
			glutWireSphere(planetary->starRadius, 20, 20);
		}

		glColor3f(planetary->planetColor.r, planetary->planetColor.g, planetary->planetColor.b);
		glPushMatrix();
		glRotatef(planetary->planetAngle, 0, 1, 0);
		glTranslatef(planetary->planetRevoRadius, 0, 0);
		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->planetRadius);
		}
		else
		{
			glutWireSphere(planetary->planetRadius, 20, 20);
		}

		glColor3f(planetary->satelliteColor.r, planetary->satelliteColor.g, planetary->satelliteColor.b);
		glPushMatrix();
		glRotatef(planetary->satelliteAngle, 0, 1, 0);
		glTranslatef(0, 0, planetary->satelliteRevoRadius);
		glutWireSphere(planetary->satelliteRadius, 20, 20);
		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->satelliteRadius);
		}
		else
		{
			glutWireSphere(planetary->satelliteRadius, 20, 20);
		}

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();


	}
}

void Drawer3D::updateViewing()
{
	GameManager* gameManager = GameManager::getInstance();

	if (gameManager->getPlayer() == NULL)
		return;

	float playerX = gameManager->getPlayer()->getPos().x;
	float playerZ = -gameManager->getPlayer()->getPos().y;
	glLoadIdentity();

	bool isFirst = gameManager->isFirstViewing();
	if(isFirst)
		gluLookAt(playerX, 1.25, playerZ - 0.4, playerX, 1.25, playerZ - 1.4, 0.0f, 1.0f, 0.0f);
	else
		gluLookAt(playerX, 1.75, playerZ + 2, playerX, 1.75, playerZ + 1, 0.0f, 1.0f, 0.0f);
}

void drawHiddenCube(float width)
{
	glColorMask(false, false, false, false);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(width, 1);
	glutSolidCube(2*width);

	glDepthMask(GL_FALSE);
	glColorMask(true, true, true, true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glutWireCube(2*width);
	glDepthMask(GL_TRUE);
}

void drawHiddenSphere(float width)
{
	glColorMask(false, false, false, false);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(width/2, 1);
	glutSolidSphere(width, 20, 20);

	glDepthMask(GL_FALSE);
	glColorMask(true, true, true, true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glutWireSphere(width, 20, 20);
	glDepthMask(GL_TRUE);
}