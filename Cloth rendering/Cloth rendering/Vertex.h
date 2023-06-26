#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include "main.h"
struct Vertex
{
	glm::vec3 force;
	glm::vec3 speed;
	glm::vec3 pos;
	glm::vec3 oldPos;
	std::vector<int> structureNb,shearNb,bendNb;
	bool fixed;
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ)
	{
		force = glm::vec3();
		speed = glm::vec3();
		structureNb = std::vector<int>();
		shearNb = std::vector<int>();
		bendNb = std::vector<int>();
		pos = glm::vec3(inX, inY, inZ);
		oldPos = pos;
		fixed = false;
	}
	void AddForce(float x,float y, float z) {
		force += glm::vec3(x, y, z);
	}
	void AddGravity() {
		force += glm::vec3(0, -9.8f, 0);
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

struct Indice
{
	Indice(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};