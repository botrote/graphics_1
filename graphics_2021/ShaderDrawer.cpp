#include "ShaderDrawer.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <GL/glew.h>
#include <gl\glut.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) 
{
	glewInit();
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
void drawCube(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill);
void drawSquare(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill);
void drawLine(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP);
void drawSphere(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill);

static const GLfloat verteces_cube[] = {
	/*
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
	 */
	-1.0, -1.0, -1.0, 
	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0,
	-1.0, -1.0, 1.0,

	-1.0, 1.0, -1.0,
	1.0, 1.0, -1.0,
	1.0, 1.0, 1.0,
	-1.0, 1.0, 1.0,

	-1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0,
	-1.0, 1.0, 1.0,
	-1.0, 1.0, -1.0,

	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,
	1.0, 1.0, -1.0,

	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, 1.0, -1.0,
	-1.0, 1.0, -1.0,

	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,
	-1.0, 1.0, 1.0
};

static const GLfloat verteces_square[] = {
	-1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, -1.0f
};

static const GLfloat verteces_line[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
};

static const GLfloat color_red[] = {
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f
};

static const GLfloat color_green[] = {
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f
};

static const GLfloat color_blue[] = {
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f
};

static const GLfloat color_white[] = {
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f
};

void ShaderDrawer::changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	Projection = glm::perspective(glm::radians(45.0f), 1.0f * w / h, 0.1f, 100.0f);
	updateViewing();
}

ShaderDrawer::ShaderDrawer()
{
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);
	MVP = Projection * View * Model;
}

void ShaderDrawer::updateViewing()
{
	Player* player = GameManager::getInstance()->getPlayer();
	if (player == NULL)
		return;

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;

	bool isFirst = GameManager::getInstance()->isFirstViewing();
	if (isFirst)
		View = glm::lookAt(glm::vec3(playerX, 1.25, playerZ - 0.4f), glm::vec3(playerX, 1.25, playerZ - 1.4f), glm::vec3(0.0f, 1.0f, 0.0f));
	else
		View = glm::lookAt(glm::vec3(playerX, 1.75, playerZ + 2), glm::vec3(playerX, 1.75, playerZ + 1), glm::vec3(0.0f, 1.0f, 0.0f));
}

void ShaderDrawer::drawGame(GameManager* gameManager)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	updateViewing();

	drawEnemy();
	drawPlayer();
	drawGrid();
	drawBullets();
	drawPlanetaries();
	glutSwapBuffers();
}

void ShaderDrawer::drawEnemy()
{
	GameManager* gameManager = GameManager::getInstance();
	Enemy* enemy = gameManager->getEnemy();

	if (enemy == NULL)
		return;

	float enemyX = enemy->getPos().x;
	float enemyZ = -enemy->getPos().y;
	Color enemyColor = enemy->getColor();

	Model = glm::mat4(1.0f);
	MVP = Projection * View * Model;

	Model = glm::translate(glm::mat4(1.0f), glm::vec3(enemyX, 1.0f, enemyZ));
	Model = glm::rotate(Model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	Model = glm::scale(Model, glm::vec3(0.35f, 0.35f, 0.35f));
	glm::mat4 bodyModel = Model;
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_red, MVP, false); //몸통

	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 1.0f));
	Model = glm::scale(Model, glm::vec3(0.3f, 0.3f, 0.5f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_red, MVP, false); //머리

	Model = glm::translate(bodyModel, glm::vec3(0.9f, 0.0f, -0.9f));
	Model = glm::rotate(Model, -45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::scale(Model, glm::vec3(0.4f, 0.4f, 0.8f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_red, MVP, false); //오른날개

	Model = glm::translate(bodyModel, glm::vec3(-0.9f, 0.0f, -0.9f));
	Model = glm::rotate(Model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::scale(Model, glm::vec3(0.4f, 0.4f, 0.8f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_red, MVP, false); //왼날개
}

void ShaderDrawer::drawPlayer()
{
	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();

	if (player == NULL)
		return;

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;
	Color playerColor = player->getColor();

	Model = glm::mat4(1.0f);
	MVP = Projection * View * Model;

	Model = glm::translate(glm::mat4(1.0f), glm::vec3(playerX, 1.0f, playerZ));
	Model = glm::scale(Model, glm::vec3(0.35f, 0.35f, 0.35f));
	glm::mat4 bodyModel = Model;
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_green, MVP, false); //몸통

	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, -1.0f));
	Model = glm::scale(Model, glm::vec3(0.3f, 0.3f, 0.5f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_green, MVP, false); //머리

	Model = glm::translate(bodyModel, glm::vec3(0.9f, 0.0f, 0.9f));
	Model = glm::rotate(Model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::scale(Model, glm::vec3(0.4f, 0.4f, 0.8f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_green, MVP, false); //오른날개

	Model = glm::translate(bodyModel, glm::vec3(-0.9f, 0.0f, 0.9f));
	Model = glm::rotate(Model, -45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::scale(Model, glm::vec3(0.4f, 0.4f, 0.8f));
	MVP = Projection * View * Model;
	drawCube(MatrixID, color_green, MVP, false); //왼날개
}

void ShaderDrawer::drawBullets()
{
	std::list<Bullet*> bullets = GameManager::getInstance()->getBullets();
	for (Bullet* b : bullets)
	{
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();

		float bulletX = bulletPos.x;
		float bulletZ = -bulletPos.y;

		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(bulletX, 1.0f, bulletZ));
		Model = glm::scale(Model, glm::vec3(0.1f, 0.1f, 0.1f));

		if (b->getType() == Type::ITEM)
		{
			Model = glm::rotate(Model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			Model = glm::rotate(Model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			MVP = Projection * View * Model;
			drawSphere(MatrixID, color_green, MVP, true);
		}
		else
		{
			MVP = Projection * View * Model;
			drawSphere(MatrixID, color_red, MVP, true);
		}
	}
}

void ShaderDrawer::drawGrid()
{
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, glm::vec3(0.0f, -0.5f, 0.0f));
	Model = glm::scale(Model, glm::vec3(6.0f, 1.0f, 11.0f));
	MVP = Projection * View * Model;
	drawSquare(MatrixID, color_white, MVP, true);

	Model = glm::mat4(1.0f);
	
	Model = glm::translate(Model, glm::vec3(0.0f, -0.49f, 0.0f));
	MVP = Projection * View * Model;

	for (int i = 0; i < 40; i++)
	{
		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(0.0f, -0.49f, -20.0f + i));
		Model = glm::scale(Model, glm::vec3(20.0f, 1.0f, 1.0f));
		MVP = Projection * View * Model;
		drawLine(MatrixID, color_blue, MVP);

		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(-20.0f + i, -0.49f, 0.0f));
		Model = glm::rotate(Model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
		Model = glm::scale(Model, glm::vec3(20.0f, 1.0f, 1.0f));
		MVP = Projection * View * Model;
		drawLine(MatrixID, color_blue, MVP);
	}

}

void ShaderDrawer::drawPlanetaries()
{
	for (Planetary* planetary : GameManager::getInstance()->getPlanetaries())
	{
		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(planetary->starPosition.x, 0.5f, -planetary->starPosition.y));
		Model = glm::rotate(Model, glm::radians(planetary->starAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::scale(Model, glm::vec3(planetary->starRadius));
		MVP = Projection * View * Model;
		drawSphere(MatrixID, color_blue, MVP, false);

		Model = glm::rotate(Model, glm::radians(planetary->planetAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::translate(Model, glm::vec3(planetary->planetRevoRadius, 0.0f, 0.0f));
		Model = glm::scale(Model, glm::vec3(planetary->planetRadius));
		MVP = Projection * View * Model;
		drawSphere(MatrixID, color_green, MVP, false);

		Model = glm::rotate(Model, glm::radians(planetary->satelliteAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, planetary->satelliteRevoRadius));
		Model = glm::scale(Model, glm::vec3(planetary->satelliteRadius));
		MVP = Projection * View * Model;
		drawSphere(MatrixID, color_red, MVP, true);
		/*
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

		*/
	}
}

void drawSphere(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill)
{
	drawCube(MatrixID, colorbufferdata, MVP, isFill);
}

void drawCube(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill)
{
	if (isFill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_cube), verteces_cube, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_cube), colorbufferdata, GL_STATIC_DRAW);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_QUADS, 0, 4 * 6); // 12*3 indices starting at 0 -> 12 triangles
	//glDrawArrays(GL_LINES, 0, 12 * 2);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
}

void drawSquare(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP, bool isFill)
{
	if (isFill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), verteces_square, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), colorbufferdata, GL_STATIC_DRAW);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_QUADS, 0, 4); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
}

void drawLine(GLuint MatrixID, const GLfloat colorbufferdata[], glm::mat4 MVP)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_line), verteces_line, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), colorbufferdata, GL_STATIC_DRAW);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, 2); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
}
