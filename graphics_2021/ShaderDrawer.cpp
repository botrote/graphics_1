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
	//printf("Compiling shader : %s\n", vertex_file_path);
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
	//printf("Compiling shader : %s\n", fragment_file_path);
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
	//printf("Linking program\n");
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
void drawCube(GLuint texture, const vec4 color);
void drawSquare(const vec4 color, bool isFill);
void drawSquare(GLuint texture, const vec4 color);
void drawLine(const vec4 color);
void drawSphere(float radius, const vec4 color, bool mode, GLint normalTexture);
void drawSphere(float radius, GLuint texture, const vec4 color, GLint normalTexture);
void drawHiddenSphere(float width, const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P, GLuint normalTexture);
void drawHiddenCube(float width, const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 P);
void drawTexturedCube(const vec4 color, GLuint texture);

GLint AmbientProduct, DiffuseProduct, SpecularProduct, LightPosition, LightPosition2, Shininess, Attenuation, Mode;
GLint FragTextureModeID, TextureID;
GLint NormalModeID, NormalID;
GLint defaultNormalTexture;

vec4 light_position(0.0, 0.0, -1.0, 0.0);
vec4 light_position2(0.0, 0.0, -1.0, 0.0);
vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);

vec4 material_specular(1.0, 1.0, 1.0, 1.0);
float material_shininess = 5.0;
bool ShadingMode;

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

	1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 1.0f
}; 

static const GLfloat uvs_square[] = {
	1.0f, 1.0f - 0.0f,
	0.0f, 1.0f - 0.0f,
	0.0f, 1.0f - 1.0f,

	1.0f, 1.0f - 1.0f,
	1.0f, 1.0f - 0.0f,
	0.0f, 1.0f - 1.0f
};

static const GLfloat verteces_square_normal[] = {
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f
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

static const GLfloat verteces_triangleCube_normal[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f, -1.0f,
		1.0f,- 1.0f,- 1.0f,
		-1.0f, -1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f, -1.0f,1.0f,
		-1.0f,1.0f, -1.0f,
		1.0f,1.0f,1.0f,
		 -1.0f,1.0f,1.0f,
		 -1.0f, -1.0f,1.0f,
		 -1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,1.0f,
		-1.0f,1.0f, -1.0f,
		1.0f,1.0f, -1.0f,
		1.0f,1.0f,1.0f,
		1.0f,- 1.0f, -1.0f,
		1.0f,1.0f, -1.0f,
		- 1.0f,1.0f, -1.0f,
		 -1.0f,- 1.0f, -1.0f,
		 -1.0f,1.0f,1.0f,
		 -1.0f, -1.0f,1.0f,
		 -1.0f,1.0f,1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f,1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,1.0f,
		1.0f, -1.0f,1.0f,
		 -1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,1.0f,
		1.0f, -1.0f, -1.0f,
		- 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		- 1.0f,1.0f, -1.0f
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
	updateShader(true);

	//planeTexture = loadBMP_custom("uvtemplate.bmp");
	//planeTexture = loadBMP_custom("3.bmp");
	//wallTexture = loadBMP_custom("galaxy_1.bmp");
	//floorTexture = loadBMP_custom("3.bmp");
	wallTexture = loadBMP_custom("textures/galaxy_2.bmp");
	floorTexture = loadBMP_custom("textures/galaxy_1.bmp");
	playerTexture = loadBMP_custom("textures/machine_1.bmp");
	enemyTexture = loadBMP_custom("textures/machine_2.bmp");
	itemTexture = loadBMP_custom("textures/diamond.bmp");
	bulletTexture = loadBMP_custom("textures/electric.bmp");
	starTexture = loadBMP_custom("textures/sun.bmp");
	planetTexture = loadBMP_custom("textures/planet.bmp");
	satteliteTexture = loadBMP_custom("textures/moon.bmp");

	starNormalTexture = loadBMP_custom("textures/sun_normal.bmp");
	planetNormalTexture = loadBMP_custom("textures/planet_normal.bmp");
	satelliteNormalTexture = loadBMP_custom("textures/moon_normal.bmp");

	defaultNormalTexture = starNormalTexture;
	
	Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	LightAngle = 0;
	ShadingMode = true;
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
	glUniform1i(NormalModeID, 0);
	glUniform1i(FragTextureModeID, 0);

	updateViewing();

	updateLight();
	drawGrid();
	drawBackground();
	drawBullets();
	drawPlanetaries();
	drawPlayer();
	drawEnemy();
	drawUI();

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.5f, 1.0f, 0.0f));
	MV = View * Model;
	updateMatrix();
	//drawCube(vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
	//drawSquare(vec4(1.0f), true);
	//drawCube(floorTexture, vec4(1.0f));
	//drawSquare(floorTexture, vec4(1.0f));
	//drawSphere(1.0f, itemTexture, vec4(1.0f));

	glutSwapBuffers();
}

void ShaderDrawer::drawEnemy()
{
	glUniform1i(NormalModeID, 0);

	GameManager* gameManager = GameManager::getInstance();
	Enemy* enemy = gameManager->getEnemy();
	bool isTextured = gameManager->isTextured();

	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();
	if (enemy == NULL)
		return;

	float enemyX = enemy->getPos().x;
	float enemyZ = -enemy->getPos().y;
	Color enemyColor = enemy->getColor();
	const vec4 color = vec4(enemyColor.r, enemyColor.g, enemyColor.b, 0.0f);

	Model = mat4(1.0f);
	Model = translate(mat4(1.0f), vec3(enemyX, 1.0f, enemyZ));
	Model = rotate(Model, 0.0f, vec3(0.0f, 0.0f, 1.0f));
	Model = scale(Model, vec3(0.35f, 0.35f, 0.35f));
	mat4 bodyModel = Model;
	MV = View * Model;
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(enemyTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(enemyTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(enemyTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(enemyTexture, color);
	}
	if (hidden_rendering_mode) 
	{
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
	glUniform1i(NormalModeID, 0);

	GameManager* gameManager = GameManager::getInstance();
	Player* player = gameManager->getPlayer();

	bool isTextured = gameManager->isTextured();
	bool hidden_rendering_mode = gameManager->isHiddenRenderingMode();

	if (player == NULL)
		return;

	float playerX = player->getPos().x;
	float playerZ = -player->getPos().y;
	Color playerColor = player->getColor();
	const vec4 color = vec4(playerColor.r, playerColor.g, playerColor.b, 0.0f);

	Model = mat4(1.0f);
	MV = View * Model;

	Model = translate(mat4(1.0f), vec3(playerX, 1.0f, playerZ));
	Model = scale(Model, vec3(0.35f, 0.35f, 0.35f));
	mat4 bodyModel = Model;
	MV = View * Model;
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(playerTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
		drawHiddenCube(0.35, color, MatrixMV, MatrixP, MV, Projection);
	}
	else
	{
		updateMatrix();
		drawCube(color, false); //몸통
		//drawCube(planeTexture, color);
	}

	Model = translate(Model, vec3(0.0f, 0.0f, -1.0f));
	Model = scale(Model, vec3(0.3f, 0.3f, 0.5f));
	MV = View * Model;

	if (isTextured)
	{
		updateMatrix();
		drawCube(playerTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(playerTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	
	if (isTextured)
	{
		updateMatrix();
		drawCube(playerTexture, color);
	}
	else if (hidden_rendering_mode) 
	{
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
	glUniform1i(NormalModeID, 0);

	std::list<Bullet*> bullets = GameManager::getInstance()->getBullets();
	for (Bullet* b : bullets)
	{
		Pos bulletPos = b->getPos();
		Color bulletColor = b->getColor();
		const vec4 color = 1.2f * vec4(bulletColor.r, bulletColor.g, bulletColor.b, 0.0f);

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
			
			if(GameManager::getInstance()->isTextured())
				drawCube(itemTexture, vec4(1.0f)); 
			else
				drawCube(color, true);
		}
		else
		{
			MV = View * Model;
			updateMatrix();
			//drawSphere(1, color, true);
			if (GameManager::getInstance()->isTextured())
				drawCube(bulletTexture, color);
			else
				drawCube(color, true);
		}
	}
}

void ShaderDrawer::drawGrid()
{
	glUniform1i(NormalModeID, 0);

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, -0.5f, 0.0f));
	Model = scale(Model, vec3(6.0f, 1.0f, 11.0f));
	MV = View * Model;
	const vec4 white(1);
	const vec4 blue(0.1f, 0.7f, 0.1f, 0.0f);
	const vec4 red(1.0f, 0.0f, 0.0f, 0.0f);

	updateMatrix();
	//drawSquare(white, true);

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
	glUniform1i(NormalModeID, 0);

	const vec4 white(1);

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, -1.0f, 0.0f));
	Model = scale(Model, vec3(20.0f, 1.0f, 20.0f));
	MV = View * Model;
	updateMatrix();

	drawSquare(floorTexture, white);

	Model = mat4(1.0f);
	Model = translate(Model, vec3(0.0f, 0.0f, -20.0f));
	Model = rotate(Model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	Model = scale(Model, vec3(20.0f, 1.0f, 20.0f));
	MV = View * Model;
	updateMatrix();

	drawSquare(wallTexture, white);
}

void ShaderDrawer::drawPlanetaries()
{
	glUniform1i(NormalModeID, 0);

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

		light_position = vec4(0.0, 0.8, 0.0, 0.0) + satellite_pos;

		if (gameManager->isTextured())
		{
			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_star[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(star_pos));
			drawSphere(planetary->starRadius, starTexture, color, starNormalTexture);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_planet[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(planet_pos));
			drawSphere(planetary->planetRadius, planetTexture, colorPlanet, planetNormalTexture);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_satellite[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(satellite_pos));
			drawSphere(planetary->satelliteRadius, satteliteTexture, colorSatellite, satelliteNormalTexture);
		}
		else if (hidden_rendering_mode) 
		{
			glUniform1f(Attenuation, get_attenuation(star_pos));
			drawHiddenSphere(planetary->starRadius, color, MatrixMV, MatrixP, MV_star, Projection, starNormalTexture);

			glUniform1f(Attenuation, get_attenuation(planet_pos));
			drawHiddenSphere(planetary->satelliteRadius, colorPlanet, MatrixMV, MatrixP, MV_satellite, Projection, planetNormalTexture);

			glUniform1f(Attenuation, get_attenuation(satellite_pos));
			drawHiddenSphere(planetary->planetRadius, colorPlanet, MatrixMV, MatrixP, MV_planet, Projection, starNormalTexture);
		}
		else
		{
			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_star[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(star_pos));
			drawSphere(planetary->starRadius, color, hidden_rendering_mode, starNormalTexture);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_planet[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(planet_pos));
			drawSphere(planetary->planetRadius, colorPlanet, hidden_rendering_mode, planetNormalTexture);

			glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV_satellite[0][0]);
			glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(Attenuation, get_attenuation(satellite_pos));
			drawSphere(planetary->satelliteRadius, colorSatellite, hidden_rendering_mode, satelliteNormalTexture);
		}

	}
}

void ShaderDrawer::drawUI()
{
	glUniform1i(NormalModeID, 0);

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

void drawSphere(float radius, const vec4 color, bool mode, GLint normalTexture)
{
	int slices = 20;
	int stacks = 20;
	int numIndicies = (slices * stacks + slices) * 6;
	using namespace std;

	const float pi = 3.1415926535897932384626433832795f;
	const float _2pi = 2.0f * pi;

	vector<vec3> positions;
	vector<vec2> uvs;
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

			uvs.push_back(vec2(U, V));
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
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(1);

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

	glUniform1i(Mode, 1);
	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

	glUniform1i(FragTextureModeID, 0);

	if (GameManager::getInstance()->isNormalMode())
	{
		glUniform1i(NormalModeID, 1);
		glActiveTexture(GL_TEXTURE1);
		if (normalTexture != -1)
			glBindTexture(GL_TEXTURE_2D, normalTexture);
		else
			glBindTexture(GL_TEXTURE_2D, normalTexture);
		glUniform1i(NormalID, 1);
	}

	if (mode == 1)
	{
		glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, (void*)(0));
	}
	else
	{
		glDrawElements(GL_LINES, numIndicies, GL_UNSIGNED_INT, (void*)(0));
	}

	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUniform1i(NormalID, 0);
}

void drawSphere(float radius, GLuint texture, const vec4 color, GLint normalTexture)
{
	int slices = 20;
	int stacks = 20;
	int numIndicies = (slices * stacks + slices) * 6;
	using namespace std;

	const float pi = 3.1415926535897932384626433832795f;
	const float _2pi = 2.0f * pi;

	vector<vec3> positions;
	vector<vec2> uvs;
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

			uvs.push_back(vec2(U, V));
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
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (void*)(0));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_STATIC_DRAW);

	//glBindVertexArray(vao);
	//glUniform4fv(g_uniformColor, 1, value_ptr(color));

	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform1i(Mode, 1);
	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition, 1, value_ptr(light_position));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

	glUniform1i(FragTextureModeID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	if (GameManager::getInstance()->isNormalMode())
	{
		glUniform1i(NormalModeID, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glUniform1i(NormalID, 1);
	}

	glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, (void*)(0));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUniform1i(NormalModeID, 0);

}

void drawCube(const vec4 color, bool mode)
{

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	if (mode)
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_triangleCube), verteces_triangleCube, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_triangleCube), uvs_triangleCube, GL_STATIC_DRAW);
	
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);

	if (mode)
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_triangleCube), verteces_triangleCube, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);
	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	if (mode)
		light_position2 = vec4(-1, -1, 1, 1) * light_position2;
	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);
	glUniform1i(Mode, 0);

	glUniform1i(FragTextureModeID, 0);

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
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	}
	else
	{
		glDrawArrays(GL_LINES, 0, 2 * 12);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &vertexbuffer2);

	if (mode)
		light_position2 = vec4(-1, -1, 1, 1) * light_position2;
}

void drawCube(GLuint texture, const vec4 color)
{

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_triangleCube), verteces_triangleCube, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_triangleCube), uvs_triangleCube, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_triangleCube), verteces_triangleCube_normal, GL_STATIC_DRAW);

	//glUniform4fv(g_uniformColor, 1, value_ptr(color));
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);
	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	light_position2 = vec4(-1, -1, 1, 1) * light_position2;
	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);
	glUniform1i(Mode, 0);

	glUniform1i(FragTextureModeID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

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

	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &vertexbuffer2);
	light_position2 = vec4(-1, -1, 1, 1) * light_position2;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square_normal), verteces_square_normal, GL_STATIC_DRAW);

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
	if (ShadingMode)
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	else
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
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
	glUniform1i(Mode, 0);

	if (isFill)
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);

	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &vertexbuffer);
}

void drawSquare(GLuint texture, const vec4 color)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_square), verteces_square, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_square), uvs_square, GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	glUniform1i(FragTextureModeID, 1);

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

	if (ShadingMode)
	{
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
	}
	vec4 material_diffuse(color.r * 0.8, color.g * 0.8, color.b * 0.8, 1.0);

	vec4 ambient_product = light_ambient * color;
	vec4 diffuse_product = light_diffuse * color;
	vec4 specular_product = light_specular * material_specular;


	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);
	glUniform1i(Mode, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	if (ShadingMode)
		glDisableVertexAttribArray(2);
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
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);
	glUniform1i(Mode, 0);

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

void drawHiddenSphere(float width,const vec4 color, GLuint MatrixMV, GLuint MatrixP, mat4 MV, mat4 Projection, GLuint normalTexture)
{
	glColorMask(false, false, false, false);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(width * 0.8, 1);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawSphere(width, color, 1, normalTexture);

	glDepthMask(GL_FALSE);
	glColorMask(true, true, true, true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
	drawSphere(width, color, 0, normalTexture);
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

void drawTexturedCube(const vec4 color, GLuint texture)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_triangleCube), uvs_triangleCube, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verteces_lineCube), verteces_lineCube, GL_STATIC_DRAW);

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
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	//light_position2 = vec4(1, -1, 1, 1) * light_position2;
	glUniform1i(Mode, 0);
	glUniform4fv(AmbientProduct, 1, value_ptr(ambient_product));
	glUniform4fv(DiffuseProduct, 1, value_ptr(diffuse_product));
	glUniform4fv(SpecularProduct, 1, value_ptr(specular_product));
	glUniform4fv(LightPosition2, 1, value_ptr(light_position2));
	glUniform1f(Shininess, material_shininess);

	glUniform1i(FragTextureModeID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	//glUniform1i(TextureID, 0);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

	//light_position2 = vec4(1, -1, 1, 1) * light_position2;
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &uvbuffer);

}

void ShaderDrawer::updateMatrix() {
	glUniformMatrix4fv(MatrixMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &Projection[0][0]);
}

float ShaderDrawer::get_attenuation(vec4 pos) {
	vec4 sub = light_position - pos;
	float d = length(sub);
	float dd = length(sub * sub);

	float result = 1 / (1 + d + dd);
	return result;

}

void ShaderDrawer::updateLight() {
	static bool isRight = true;

	if (isRight)
		LightAngle += 0.01;
	else
		LightAngle -= 0.01;

	if (LightAngle >= 3.14)
	{
		LightAngle = 3.14;
		isRight = false;
	}
	else if (LightAngle <= 0)
	{
		LightAngle = 0;
		isRight = true;
	}


	light_position2 = vec4 ( -30*cos(LightAngle)-10, 30*sin(LightAngle), -10.0, 1.0);

}

void ShaderDrawer::updateShader(bool shading_mode) {

	ShadingMode = shading_mode;
	if (shading_mode)
		programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	else
		programID = LoadShaders("PhongVertexShader.vertexshader", "PhongFragmentShader.fragmentshader");

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
	Mode = glGetUniformLocation(programID, "Mode");

	FragTextureModeID = glGetUniformLocation(programID, "isTexturedFrag");
	NormalModeID = glGetUniformLocation(programID, "isNormalMapping");
	NormalID = glGetUniformLocation(programID, "myNormalSampler");

	glutSwapBuffers();
}

// glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
// draw square할 때마다 업뎃이 되어야함. 
// hidden drawing에서는 MV, P를 인자로 받아올 것.