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
void stateUpdate();
void onkeyInput(unsigned char key, int x, int y);
void onSpecialInput(int key, int x, int y);
void init(int argc, char** argv);

GameManager* gameManager;

void main(int argc, char** argv) 
{
	gameManager = GameManager::getInstance();

	init(argc, argv);

	glutIdleFunc(stateUpdate);
	glutDisplayFunc(drawerUpdate);
	glutKeyboardFunc(onkeyInput);
	glutSpecialFunc(onSpecialInput);
	glutMainLoop();
}

void init(int argc, char** argv)
{
	glutInit(&argc, argv);			//GLUT라이브러리를 초기화하고 기반 플랫폼의 윈도우 시스템과 연결
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glClearColor(1, 1, 1, 0);		//배경색상 정하기
	glShadeModel(GL_FLAT);
}

void stateUpdate()
{
	gameManager->updateState();
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