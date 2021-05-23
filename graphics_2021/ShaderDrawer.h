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
	void changeSize(int w, int h);
	void updateShader(bool shading_mode);
private:
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 MV;
	GLuint programID;
	GLuint MatrixMV;
	GLuint MatrixP;
	float LightAngle;

	GLuint planeTexture;
	GLuint bulletTexture;
	GLuint itemTexture;
	GLuint starTexture;
	GLuint planetTexture;
	GLuint satteliteTexture;
	GLuint floorTexture;
	GLuint wallTexture;

	void updateViewing();
	void drawEnemy();
	void drawPlayer();
	void drawBullets();
	void drawGrid();
	void drawPlanetaries();
	void drawUI();
	void updateMatrix();
	float get_attenuation(glm::vec4 pos);
	void updateLight();
};

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);


