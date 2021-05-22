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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
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
void drawCube(const vec4 color, bool mode);
void drawSquare(const vec4 color, bool isFill);
void drawLine(const vec4 color);
void drawSphere(float radius, const vec4 color, bool mode);
void drawHiddenSphere(float width,  const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P);
void drawHiddenCube(float width, const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P);

GLint AmbientProduct, DiffuseProduct, SpecularProduct, LightPosition, LightPosition2, Shininess, Attenuation;
vec4 light_position(0.0, 0.0, -1.0, 0.0);
vec4 light_position2(0.0, 0.0, -1.0, 0.0);
vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);

vec4 material_specular(1.0, 1.0, 1.0, 1.0);
float material_shininess = 5.0;

static const GLfloat verteces_fillCube[] = {

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

static const GLfloat verteces_lineCube[] = {

	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,

	-1.0, -1.0, -1.0,
	-1.0, 1.0, -1.0,

	-1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0,

	1.0, -1.0, 1.0,
	-1.0, -1.0, 1.0,

	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,

	1.0, -1.0, 1.0,
	1.0, -1.0, -1.0,

	-1.0, 1.0, 1.0,
	1.0, 1.0, 1.0,

	-1.0, 1.0, 1.0,
	-1.0, -1.0, 1.0,

	-1.0, 1.0, 1.0,
	-1.0, 1.0, -1.0,

	1.0, 1.0, -1.0,
	-1.0, 1.0, -1.0,

	1.0, 1.0, -1.0,
	1.0, -1.0, -1.0,

	1.0, 1.0, -1.0,
	1.0, 1.0, 1.0,

};

static const GLfloat verteces_square[] = {
	-1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, -1.0f
}; 
static const GLfloat verteces_square_normal[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

static const GLfloat verteces_line[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
};

void ShaderDrawer::changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	Projection = perspective(radians(45.0f), 1.0f * w / h, 0.1f, 100.0f);
	updateViewing();
}

ShaderDrawer::ShaderDrawer()
{
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	//g_uniformColor = glGetUniformLocation(programID, "color");
	MatrixMV = glGetUniformLocation(programID, "ModelView");
	MatrixP = glGetUniformLocation(programID, "Projection");

	// Initialize shader lighting parameters
	AmbientProduct = glGetUniformLocation(programID, "AmbientProduct");
	DiffuseProduct = glGetUniformLocation(programID, "DiffuseProduct");
	SpecularProduct = glGetUniformLocation(programID, "SpecularProduct");
	LightPosition = glGetUniformLocation(programID, "LightPosition");
	LightPosition2 = glGetUniformLocation(programID, "LightPosition2");
	Attenuation = glGetUniformLocation(programID, "Attenuation");
	Shininess = glGetUniformLocation(programID, "Shininess");

	Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	LightAngle = 0;
	// Camera matrix
	mat4 View = mat4(1.0f);
	Model = mat4(1.0f);
	MV = View * Model;
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
		View = lookAt(vec3(playerX, 1.25, playerZ - 0.4f), vec3(playerX, 1.25, playerZ - 1.4f), vec3(0.0f, 1.0f, 0.0f));
	else
		View = lookAt(vec3(playerX, 1.75, playerZ + 2), vec3(playerX, 1.75, playerZ + 1), vec3(0.0f, 1.0f, 0.0f));
}

void ShaderDrawer::drawGame(GameManager* gameManager)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	updateViewing();

	updateLight();
	drawGrid();
	drawBullets();
	drawPlanetaries();
	drawPlayer();
	drawEnemy();
	drawUI();
	glutSwapBuffers();
}

void ShaderDrawer::drawEnemy()
{
	GameManager* gameManager = GameManager::getInstance();
	Enemy* enemy = gameManager->getEnemy();

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();
	if (enemy == NULL)
		return;

	float enemyX = enemy->getPos().x;
	float enemyZ = -enemy->getPos().y;
	Color enemyColor = enemy->getColor();
	const vec4 color(enemyColor.r, enemyColor.g, enemyColor.b, 0.0f);

	Model = mat4(1.0f);
	Model = translate(mat4(1.0f), vec3(enemyX, 1.0f, enemyZ));
	Model = rotate(Model, 0.0f, vec3(0.0f, 0.0f, 1.0f));
	Model = scale(Model, vec3(0.35f, 0.35f, 0.35f));
	mat4 bodyModel = Model;
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.35, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false);
	}

	Model = translate(Model, vec3(0.0f, 0.0f, 1.0f));
	Model = scale(Model, vec3(0.3f, 0.3f, 0.5f));
	MV = Projection * View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.3, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false);
	}

	Model = translate(bodyModel, vec3(0.9f, 0.0f, -0.9f));
	Model = rotate(Model, -45.0f, vec3(0.0f, 1.0f, 0.0f));
	Model = scale(Model, vec3(0.4f, 0.4f, 0.8f));
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.4, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube( color, false);
	}

	Model = translate(bodyModel, vec3(-0.9f, 0.0f, -0.9f));
	Model = rotate(Model, 45.0f, vec3(0.0f, 1.0f, 0.0f));
	Model = scale(Model, vec3(0.4f, 0.4f, 0.8f));
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.4, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube( color, false);
	}
}

void ShaderDrawer::drawPlayer()
{
	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	if (player == NULL)
		return;

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;
	Color playerColor = player->getColor();
	const vec4 color(playerColor.r, playerColor.g, playerColor.b, 0.0f);

	Model = mat4(1.0f);
	MV = View * Model;

	Model = translate(mat4(1.0f), vec3(playerX, 1.0f, playerZ));
	Model = scale(Model, vec3(0.35f, 0.35f, 0.35f));
	mat4 bodyModel = Model;
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.35, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube( color, false); //몸통
	}

	Model = translate(Model, vec3(0.0f, 0.0f, -1.0f));
	Model = scale(Model, vec3(0.3f, 0.3f, 0.5f));
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.3, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false); //몸통
	}

	Model = translate(bodyModel, vec3(0.9f, 0.0f, 0.9f));
	Model = rotate(Model, 45.0f, vec3(0.0f, 1.0f, 0.0f));
	Model = scale(Model, vec3(0.4f, 0.4f, 0.8f));
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.4, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false); //몸통
	}

	Model = translate(bodyModel, vec3(-0.9f, 0.0f, 0.9f));
	Model = rotate(Model, -45.0f, vec3(0.0f, 1.0f, 0.0f));
	Model = scale(Model, vec3(0.4f, 0.4f, 0.8f));
	MV = View * Model;
	if (hidden_rendering_mode) {
		drawHiddenCube(0.4, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false); //몸통
	}
}

void ShaderDrawer::drawBullets()
{
	std::list<Bullet*> bullets = GameManager::getInstance()->getBullets();
	for (Bullet* b : bullets)
	{
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();
		const vec4 color(bulletColor.r, bulletColor.g, bulletColor.b, 0.0f);

		float bulletX = bulletPos.x;
		float bulletZ = -bulletPos.y;

		Model = mat4(1.0f);
		Model = translate(Model, vec3(bulletX, 1.0f, bulletZ));
		Model = scale(Model, vec3(0.1f, 0.1f, 0.1f));

		if (b->getType() == Type::ITEM)
		{
			Model = rotate(Model, radians(45.0f), vec3(0.0f, 0.0f, 1.0f));
			Model = rotate(Model, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
			MV = View * Model;
			updateMatrix();
			drawCube(color, true);
			//drawSphere(1, color, true);
		}
		else
		{
			MV = View * Model;
			updateMatrix();
			drawCube(color, true);
			//drawSphere(1, color, true);
		}
	}
}

void ShaderDrawer::drawGrid()
{
	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, -0.5f, 0.0f));
	Model = scale(Model, vec3(6.0f, 1.0f, 11.0f));
	MV = View * Model;
	const vec4 white(1);
	const vec4 blue(0.0f, 0.0f, 0.7f, 0.0f);
	const vec4 red(1.0f, 0.0f, 0.0f, 0.0f);

	updateMatrix();
	drawSquare( white, true);

	for (int i = 0; i < 40; i++)
	{
		Model = mat4(1.0f);
		Model = translate(Model, vec3(0.0f, -0.49f, -20.0f + i));
		Model = scale(Model, vec3(20.0f, 1.0f, 1.0f));
		MV = View * Model;
		updateMatrix();
		drawLine( blue);

		Model = mat4(1.0f);
		Model = translate(Model, vec3(-20.0f + i, -0.49f, 0.0f));
		Model = rotate(Model, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));

		Model = scale(Model, vec3(20.0f, 1.0f, 1.0f));
		MV = View * Model;
		updateMatrix();
		drawLine(blue);
	}

}

void ShaderDrawer::drawPlanetaries()
{
	GameManager* gameManager = GameManager::getInstance();
	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	for (Planetary* planetary : gameManager->getPlanetaries())
	{	
		vec3 pos(0.0, 1.0, 0.0);

		Model = mat4(1.0f);
		Model = translate(Model, vec3(planetary->starPosition.x, 0.5f, -planetary->starPosition.y));
		Model = rotate(Model, radians(planetary->starAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = scale(Model, vec3(planetary->starRadius));
		mat4 MV_star = View * Model;
		vec4 star_pos = MV_star * vec4(pos * planetary->starRadius, 1.0);
		const vec4 color(planetary->starColor.r, planetary->starColor.g, planetary->starColor.b, 0.0f);

		Model = rotate(Model, radians(planetary->planetAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = translate(Model, vec3(planetary->planetRevoRadius, 0.0f, 0.0f));
		Model = scale(Model, vec3(planetary->planetRadius));
		mat4 MV_planet = View * Model;
		vec4 planet_pos = MV_planet * vec4(pos * planetary->planetRadius, 1.0);
		const vec4 colorPlanet(planetary->planetColor.r, planetary->planetColor.g, planetary->planetColor.b, 0.0f);

		Model = rotate(Model, radians(planetary->satelliteAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = translate(Model, vec3(0.0f, 0.0f, planetary->satelliteRevoRadius));
		Model = scale(Model, vec3(planetary->satelliteRadius));
		mat4 MV_satellite = View * Model;
		vec4 satellite_pos = MV_satellite * vec4(pos * planetary->satelliteRadius, 1.0);
		const vec4 colorSatellite(planetary->satelliteColor.r, planetary->satelliteColor.g, planetary->satelliteColor.b, 0.0f);

		light_position = vec4(0.0, 0.5, 0.0, 0.0) + satellite_pos;

		if (hidden_rendering_mode) {
			glUniform1f(Attenuation, get_attenuation(star_pos));
			drawHiddenSphere(planetary->starRadius, color, MatrixMV, MatrixP, MV_star, Projection);

			glUniform1f(Attenuation, get_attenuation(planet_pos));
			drawHiddenSphere(planetary->satelliteRadius, colorPlanet, MatrixMV, MatrixP, MV_satellite, Projection);

			glUniform1f(Attenuation, get_attenuation(satellite_pos));
			drawHiddenSphere(planetary->planetRadius, colorPlanet, MatrixMV, MatrixP, MV_planet, Projection);
		}
		else
		{
			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_star[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(star_pos));
			drawSphere(planetary->starRadius, color, hidden_rendering_mode);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_planet[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(planet_pos));
			drawSphere(planetary->planetRadius, colorPlanet, hidden_rendering_mode);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_satellite[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(satellite_pos));
			drawSphere(planetary->satelliteRadius, colorSatellite, hidden_rendering_mode);
		}

	}
}

void ShaderDrawer::drawUI()
{
	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();

	if (player == NULL)
		return;

	const vec4 red(1.0f, 0.0f, 0.0f, 0.0f);
	const vec4 black(0.0f, 0.0f, 0.0f, 0.0f);
	const vec4 gray(0.5f, 0.5f, 0.5f, 0.0f);

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;

	bool isFirst = GameManager::getInstance()->isFirstViewing();

	Model = mat4(1.0f);
	if (isFirst)
		Model = translate(Model, vec3(playerX, -0.5f, playerZ - 2.4));
	else
		Model = translate(Model, vec3(playerX, 0.0f, playerZ));

	Model = rotate(Model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	Model = translate(Model, vec3(-1.05f, 0.0f, 0.97f));
	Model = scale(Model, vec3(0.03f, 0.03f, 0.03f));
	mat4 tempModel = Model;
	MV = View * Model;

	if (gameManager->isAllFail())
	{
		updateMatrix();
		drawSquare(red, true);
		Model = translate(Model, vec3(-0.065f, 0.1f, 0.0f));
		Model = scale(Model, vec3(1.2f, 1.0f, 1.2f));
		MV = View * Model;
		updateMatrix();
		drawSquare(gray, true);

		Model = translate(Model, vec3(0.0f, -0.15f, 0.1f));
		Model = rotate(Model, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
		Model = scale(Model, vec3(0.2f, 1.0f, 1.225f));
		MV = View * Model;
		updateMatrix();
		drawSquare(gray, true);
	}
	else
	{
		updateMatrix();
		drawSquare(red, true);
		for (int i = 1; i < player->getHealth(); i++)
		{
			Model = translate(Model, vec3(2.75f, 0.0f, 0.0f));
			MV = View * Model;
			updateMatrix();
			drawSquare( red, true);
		}
	}

	Enemy* enemy = gameManager->getEnemy();
	if (enemy == NULL)
		return;

	Model = translate(tempModel, vec3(0.0f, 0.0f, 51.5f));
	MV = View * Model;

	if (gameManager->isAllPass())
	{
		updateMatrix();
		drawSquare(red, true);
		Model = translate(Model, vec3(-0.065f, 0.1f, 0.1f));
		Model = scale(Model, vec3(1.2f, 1.0f, 1.2f));
		MV = View * Model;
		updateMatrix();
		drawSquare( gray, true);

		Model = translate(Model, vec3(0.05f, -0.15f, -0.05f));
		Model = rotate(Model, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
		Model = scale(Model, vec3(0.2f, 1.0f, 1.225f));
		MV = View * Model;
		updateMatrix();
		drawSquare(gray, true);
	}
	else
	{
		updateMatrix();
		drawSquare(red, true);
		for (int i = 1; i < enemy->getHealth(); i++)
		{
			Model = translate(Model, vec3(2.75f, 0.0f, 0.0f));
			MV = View * Model;
			updateMatrix();
			drawSquare(red, true);
		}
	}



}

void drawSphere(float radius, const vec4 color, bool mode)
{
	int slices = 20;
	int stacks = 20;
	int numIndicies = (slices * stacks + slices) * 6;
	using namespace std;

	const float pi = 3.1415926535897932384626433832795f;
	const float _2pi = 2.0f * pi;

	vector<vec3> positions;
	vector<vec3> normals;

	for (int i = 0; i <= stacks; ++i)
	{
		// V texture coordinate.
		float V = i / (float)stacks;
		float phi = V * pi;

		for (int j = 0; j <= slices; ++j)
		{
			// U texture coordinate.
			float U = j / (float)slices;
			float theta = U * _2pi;

			float X = cos(theta) * sin(phi);
			float Y = cos(phi);
			float Z = sin(theta) * sin(phi);

			positions.push_back(vec3(X, Y, Z) * radius);
			normals.push_back(vec3(X, Y, Z));
		}
	}

	// Now generate the index buffer
	vector<GLuint> indicies;

	for (int i = 0; i < slices * stacks + slices; ++i)
	{
		indicies.push_back(i);
		indicies.push_back(i + slices + 1);
		indicies.push_back(i + slices);

		indicies.push_back(i + slices + 1);
		indicies.push_back(i);
		indicies.push_back(i + 1);
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbos[4];
	glGenBuffers(4, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (void *)(0));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_STATIC_DRAW);

	//glBindVertexArray(vao);
	//glUniform4fv(g_uniformColor, 1, value_ptr(color));

	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));

	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);


	if (mode == 1)
	{
		glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, (void*)(0));
	}
	else
	{
		glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, (void*)(0));
	}


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void drawCube(const vec4 color, bool mode)
{

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	if (mode)
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_fillCube), verteces_fillCube, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);
	
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	if (mode)
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_fillCube), verteces_fillCube, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);
	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

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
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glVertexAttribPointer(
		2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	if (mode == true)
	{
		glDrawArrays(GL_QUADS, 0, 4 * 6);
	}
	else
	{
		glDrawArrays(GL_LINES, 0, 2 * 12);
	}

	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &vertexbuffer);
}

void drawSquare(const vec4 color, bool isFill)
{
	/*
	if (isFill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		*/
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), verteces_square, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), verteces_square_normal, GL_STATIC_DRAW);

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));


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
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * color;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

	if (isFill)
		glDrawArrays(GL_QUADS, 0, 15);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);

	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &vertexbuffer);
}

void drawLine(const vec4 color)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_line), verteces_line, GL_STATIC_DRAW);

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * color;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

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

	glDrawArrays(GL_LINES, 0, 2); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);

	glDeleteBuffers(1, &vertexbuffer);
}

void drawHiddenSphere(float width,const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 Projection)
{
	glColorMask(false, false, false, false);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(width * 0.8, 1);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawSphere(width, color, 1);

	glDepthMask(GL_FALSE);
	glColorMask(true, true, true, true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawSphere(width, color, 0);
	glDepthMask(GL_TRUE);
}

void drawHiddenCube(float width, const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 Projection)
{
	glColorMask(false, false, false, false);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(width * 0.8, 1);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawCube(color, 1);

	glDepthMask(GL_FALSE);
	glColorMask(true, true, true, true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawCube(color, 0);
	glDepthMask(GL_TRUE);
}

void ShaderDrawer::updateMatrix() {
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
}

float ShaderDrawer::get_attenuation(vec4 pos) {
	vec4 sub = light_position - pos;
	float d = length(sub);
	float dd = length(sub * sub);

	float result = 5 / (1 + d + dd);
	return result;

}

void ShaderDrawer::updateLight() {
	LightAngle += 0.01;
	
	if (LightAngle > 3.14)
		LightAngle = 0;


	light_position2 = vec4 ( -30*cos(LightAngle)-10, 30*sin(LightAngle), 5.0, 0.0);

}

// glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
// draw square할 때마다 업뎃이 되어야함. 
// hidden drawing에서는 MV, P를 인자로 받아올 것.