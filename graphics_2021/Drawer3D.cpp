#include "Drawer3D.h"
#include <gl\glut.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdlib.h>
#include <iostream>

static float angle = 0.0, ratio;
static float x = 0.0f, y = 1.75f, z = 5.0f;
static float lx = 0.0f, ly = 0.0f, lz = -1.0f;
static GLint snowman_display_list;
void drawPlayer();

void Drawer3D::changeSize(int w, int h)
{
	GameManager* gameManager = GameManager::getInstance();

	if (gameManager->getPlayer() == NULL)
		return;

	float playerX = gameManager->getPlayer()->getPos().x;
	float playerZ = -gameManager->getPlayer()->getPos().y;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(playerX, y, playerZ, playerX + lx, y + ly, playerZ - 1, 0.0f, 1.0f, 0.0f);


}


void drawSnowMan() {


	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Body	
	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);


	// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f, 20, 20);

	// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	// Draw Nose
	glColor3f(1.0f, 0.5f, 0.5f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
}



GLuint createDL() {
	GLuint snowManDL;

	// Create the id for the list
	snowManDL = glGenLists(1);

	// start list
	glNewList(snowManDL, GL_COMPILE);

	// call the function that contains the rendering commands
	drawSnowMan();

	// endList
	glEndList();

	return(snowManDL);
}

void Drawer3D::initScene() {

	glEnable(GL_DEPTH_TEST);
	snowman_display_list = createDL();

}





void Drawer3D::drawGame(GameManager* gameManager) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	// Draw 36 SnowMen

	for (int i = -3; i < 3; i++)
		for (int j = -3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i * 10.0, 0, j * 10.0);
			glCallList(snowman_display_list);;
			glPopMatrix();
		}

	drawPlayer();
	glutSwapBuffers();
}

void drawPlayer()
{
	GameManager* gameManager = GameManager::getInstance();

	Player* player = gameManager->getPlayer();

	if (player == NULL)
		return;

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;
	Color playerColor = player->getColor();
	glColor3f(1, 0, 0);
	glPushMatrix();
	glTranslatef(playerX, 1, playerZ -1);
	glutWireCube(1);
	glPopMatrix();

}

void orientMe(float ang) {


	lx = sin(ang);
	lz = -cos(ang);
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);
}

void moveMeFlat(int i) {
	x = x + i * (lx) * 0.1;
	z = z + i * (lz) * 0.1;
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);
}

void inputKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT: angle -= 0.01f; orientMe(angle); break;
	case GLUT_KEY_RIGHT: angle += 0.01f; orientMe(angle); break;
	case GLUT_KEY_UP: moveMeFlat(1); break;
	case GLUT_KEY_DOWN: moveMeFlat(-1); break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}