#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <vector>
#include "GameManager.h"
#include "Plane.h"
#include "Player.h"
#include "default.h"
#include "Drawer.h"

void drawerUpdate();
void stateUpdate(int value);
void onkeyInput(unsigned char key, int x, int y);
void onSpecialInput(int key, int x, int y);
void init(int argc, char** argv);

GameManager* gameManager;

int main(int argc, char** argv) 
{
	gameManager = GameManager::getInstance();

	init(argc, argv);

	//glutIdleFunc(stateUpdate);
	glutTimerFunc(1, stateUpdate, 1);
	glutDisplayFunc(drawerUpdate);
	glutKeyboardFunc(onkeyInput);
	glutSpecialFunc(onSpecialInput);
	glutMainLoop();

	return 0;
}

void init(int argc, char** argv)
{
	glutInit(&argc, argv);			//GLUT라이브러리를 초기화하고 기반 플랫폼의 윈도우 시스템과 연결
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(750, 800);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(argv[0]);
	glClearColor(0, 0, 0, 0);		//배경색상 정하기
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.5, 1.5, -1.5, 1.5);
}

void stateUpdate(int value)
{
	gameManager->updateState();
	glutTimerFunc(1, stateUpdate, 100);
}

void drawerUpdate()
{
	Drawer::drawGame(gameManager);
}

void onkeyInput(unsigned char key, int x, int y)
{
	gameManager->onKeyInput(key);
}

void onSpecialInput(int key, int x, int y)
{
	gameManager->onSpecialInput(key);
}