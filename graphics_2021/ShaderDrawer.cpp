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


void drawTexturedCube(const vec4 color, GLuint texture);


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
GLuint loadBMP_custom(const char* imagepath) {

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file;
	fopen_s(&file, imagepath, "rb");

	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    fclose(file); return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    fclose(file); return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	printf("load success: ID is %d\n", textureID);
	return textureID;
}

void drawCube(const vec4 color, bool mode);
void drawSquare(const vec4 color, bool isFill);
void drawTexturedSquare(const vec4 color, GLuint texture);
void drawLine(const vec4 color);
void drawSphere(float radius, const vec4 color, bool mode);
void drawHiddenSphere(float width,  const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P);
void drawHiddenCube(float width, const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P);

GLint g_uniformColor = -1;
GLint AmbientProduct, DiffuseProduct, SpecularProduct, LightPosition, Shininess;
vec4 light_position(0.0, 0.0, -1.0, 0.0);
vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);

vec4 material_specular(1.0, 1.0, 1.0, 1.0);
float material_shininess = 5.0;

GLuint TextureID = -1;

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

static const GLfloat uvs_square[] = {
	1.0f, 1.0f - 0.0f,
	0.0f, 1.0f - 0.0f,
	0.0f, 1.0f - 1.0f,
	1.0f, 1.0f - 1.0f
};

static const GLfloat verteces_line[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
};

static const GLfloat verteces_triangleCube[] = {
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
};

static const GLfloat uvs_triangleCube[] = {
	0.000059f, 1.0f - 0.000004f,
	0.000103f, 1.0f - 0.336048f,
	0.335973f, 1.0f - 0.335903f,
	1.000023f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.336024f, 1.0f - 0.671877f,
	0.667969f, 1.0f - 0.671889f,
	1.000023f, 1.0f - 0.000013f,
	0.668104f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.000059f, 1.0f - 0.000004f,
	0.335973f, 1.0f - 0.335903f,
	0.336098f, 1.0f - 0.000071f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.336024f, 1.0f - 0.671877f,
	1.000004f, 1.0f - 0.671847f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.668104f, 1.0f - 0.000013f,
	0.335973f, 1.0f - 0.335903f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.668104f, 1.0f - 0.000013f,
	0.336098f, 1.0f - 0.000071f,
	0.000103f, 1.0f - 0.336048f,
	0.000004f, 1.0f - 0.671870f,
	0.336024f, 1.0f - 0.671877f,
	0.000103f, 1.0f - 0.336048f,
	0.336024f, 1.0f - 0.671877f,
	0.335973f, 1.0f - 0.335903f,
	0.667969f, 1.0f - 0.671889f,
	1.000004f, 1.0f - 0.671847f,
	0.667979f, 1.0f - 0.335851f
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
	Shininess = glGetUniformLocation(programID, "Shininess");

	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	printf("TextureID: %d\n", TextureID);

	planeTexture = loadBMP_custom("uvtemplate.bmp");
	//planeTexture = loadBMP_custom("3.bmp");
	wallTexture = loadBMP_custom("galaxy_1.bmp");
	floorTexture = loadBMP_custom("galaxy_2.bmp");
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, planeTexture);

	Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
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


	drawGrid();
	drawBackground();
	drawBullets();
	drawPlanetaries();
	drawPlayer();
	drawEnemy();
	drawUI();
	//glutSwapBuffers();

	Model = mat4(1.0f);
	Model = translate(mat4(1.0f), vec3(0, 1.0f, 0));
	MV = View * Model;
	updateMatrix();


	if (gameManager->isTextured())
	{
		drawTexturedCube(glm::vec4(1, 1, 1, 1), planeTexture);
	}
	else
	{
		drawCube(glm::vec4(1, 0, 0, 1), true);
	}
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
		drawCube(color, false); //몸통
		//drawTexturedCube(color, planeTexture); //몸통
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
		drawCube(color, false);
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
			drawSphere(1, color, true);
		}
		else
		{
			MV = View * Model;
			updateMatrix();
			drawSphere(1, color, true);
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
	//drawSquare( white, true);

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

void ShaderDrawer::drawBackground()
{
	const vec4 white(1);

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, -1.0f, 0.0f));
	Model = scale(Model, vec3(20.0f, 1.0f, 20.0f));
	MV = View * Model;
	updateMatrix();

	drawTexturedSquare(white, floorTexture);

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, 0.0f, -20.0f));
	Model = rotate(Model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	Model = scale(Model, vec3(20.0f, 1.0f, 20.0f));
	MV = View * Model;
	updateMatrix();

	drawTexturedSquare(white, wallTexture);
}

void ShaderDrawer::drawPlanetaries()
{
	GameManager* gameManager = GameManager::getInstance();
	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	for (Planetary* planetary : gameManager->getPlanetaries())
	{
		Model = mat4(1.0f);
		Model = translate(Model, vec3(planetary->starPosition.x, 0.5f, -planetary->starPosition.y));
		Model = rotate(Model, radians(planetary->starAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = scale(Model, vec3(planetary->starRadius));
		MV = View * Model;

		const vec4 color(planetary->starColor.r, planetary->starColor.g, planetary->starColor.b, 0.0f);

		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->starRadius, color, MatrixMV, MatrixP, MV, Projection);
		}
		else
		{
			updateMatrix();
			drawSphere(planetary->starRadius, color, hidden_rendering_mode);
		}

		Model = rotate(Model, radians(planetary->planetAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = translate(Model, vec3(planetary->planetRevoRadius, 0.0f, 0.0f));
		Model = scale(Model, vec3(planetary->planetRadius));
		MV = View * Model;
		const vec4 colorPlanet(planetary->planetColor.r, planetary->planetColor.g, planetary->planetColor.b, 0.0f);

		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->planetRadius, colorPlanet, MatrixMV, MatrixP, MV, Projection);
		}
		else
		{
			updateMatrix();
			drawSphere(planetary->planetRadius, colorPlanet, hidden_rendering_mode);
		}

		Model = rotate(Model, radians(planetary->satelliteAngle), vec3(0.0f, 1.0f, 0.0f));
		Model = translate(Model, vec3(0.0f, 0.0f, planetary->satelliteRevoRadius));
		Model = scale(Model, vec3(planetary->satelliteRadius));
		MV = View * Model;

		const vec4 colorSatellite(planetary->satelliteColor.r, planetary->satelliteColor.g, planetary->satelliteColor.b, 0.0f);
		if (hidden_rendering_mode) {
			drawHiddenSphere(planetary->satelliteRadius, colorPlanet, MatrixMV, MatrixP, MV, Projection);
		}
		else
		{
			updateMatrix();
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
	glUniform1f(Shininess, material_shininess);


	if (mode == 1)
	{
		glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, (void*)(0));
	}
	else
	{
		glDrawElements(GL_LINE_LOOP, numIndicies, GL_UNSIGNED_INT, (void*)(0));
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
	
	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
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

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * color;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
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

	if (isFill)
		glDrawArrays(GL_QUADS, 0, 15);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);

	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &vertexbuffer);
}

void drawTexturedSquare(const vec4 color, GLuint texture)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), verteces_square, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_square), uvs_square, GL_STATIC_DRAW);

	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * color;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
	glUniform1f(Shininess, material_shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

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

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	glDrawArrays(GL_QUADS, 0, 15);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
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
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
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

// glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
// draw square할 때마다 업뎃이 되어야함. 
// hidden drawing에서는 MV, P를 인자로 받아올 것.



// Two UV coordinatesfor each vertex. They were created with Blender.


void drawTexturedCube(const vec4 color, GLuint texture)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_triangleCube), verteces_triangleCube, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_triangleCube), uvs_triangleCube, GL_STATIC_DRAW);


	
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
	glUniform1f(Shininess, material_shininess);
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

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

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);

}