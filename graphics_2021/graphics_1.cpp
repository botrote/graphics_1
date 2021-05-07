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
#include "Drawer3D.h"
#include "ShaderDrawer.h"

void drawerUpdate();
void stateUpdate(int value);
void onkeyInput(unsigned char key, int x, int y);
void onSpecialInput(int key, int x, int y);
void init(int argc, char** argv);

void changeSize(int w, int h);

GameManager* gameManager;
ShaderDrawer* shaderDrawer;

int main(int argc, char** argv) 
{
	gameManager = GameManager::getInstance();
	

	init(argc, argv);

	shaderDrawer = new ShaderDrawer();

	//glutIdleFunc(stateUpdate);
	glutTimerFunc(1, stateUpdate, 1);
	glutDisplayFunc(drawerUpdate);
	glutKeyboardFunc(onkeyInput);
	glutSpecialFunc(onSpecialInput);
	//glutReshapeFunc(changeSize);
	glutMainLoop();

	return 0;
}

void init(int argc, char** argv)
{
	glutInit(&argc, argv);			//GLUT라이브러리를 초기화하고 기반 플랫폼의 윈도우 시스템과 연결
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(argv[0]);
	glClearColor(0, 0, 0, 0);		//배경색상 정하기
	glShadeModel(GL_FLAT);



	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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
	//Drawer::drawGame(gameManager);
	//Drawer3D::drawGame(gameManager);
	shaderDrawer->drawGame(gameManager);
}

void onkeyInput(unsigned char key, int x, int y)
{
	gameManager->onKeyInput(key);
}

void onSpecialInput(int key, int x, int y)
{
	gameManager->onSpecialInput(key);
}

void changeSize(int w, int h)
{
	Drawer3D::changeSize(w, h);
}