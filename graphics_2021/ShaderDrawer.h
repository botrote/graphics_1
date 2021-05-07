#pragma once
#include "Bullet.h"
#include "GameManager.h"
#include "Plane.h"
#include "Planetary.h"
#include "default.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderDrawer
{
public:
	ShaderDrawer();
	void drawGame(GameManager* gameManager);
	//static void changeSize(int w, int h);
	//static void updateViewing();
private:
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 MVP;
	GLuint programID;
	GLuint MatrixID;
};


