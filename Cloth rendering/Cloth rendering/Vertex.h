#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include "main.h"
struct Vertex
{
	glm::vec3 force;
	glm::vec3 speed;
	glm::vec3 accel;
	glm::vec3 pos;
	std::vector<int> structureNb,shearNb,bendNb;
	float mass = 1;
	float restLength;
	bool nailed;
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ)
	{
		force = glm::vec3();
		speed = glm::vec3();
		accel = glm::vec3();
		structureNb = std::vector<int>();
		shearNb = std::vector<int>();
		bendNb = std::vector<int>();
		pos = glm::vec3(inX, inY, inZ);
		nailed = false;
	}
	void AddForce(float x,float y, float z) {
		force += glm::vec3(x, y, z);
	}
	
	
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};