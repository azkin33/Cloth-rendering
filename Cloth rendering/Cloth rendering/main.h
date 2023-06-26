#pragma once
#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <vector>
#include "Vertex.h"
#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char*)NULL + (i))
using namespace std;


extern GLint modelingMatrixLoc[2];
extern GLint viewingMatrixLoc[2];
extern GLint projectionMatrixLoc[2];
extern GLint eyePosLoc[2];

extern GLfloat* vertexData;
extern GLfloat* normalData;
extern GLuint* indexData;
extern float cubeSize;
extern glm::vec3 cubeMin, cubeMax;
extern float clothHeight;
extern unsigned int clothTexture;
extern GLuint gVertexAttribBuffer, gIndexBuffer;
extern int gVertexDataSizeInBytes, gNormalDataSizeInBytes,indexDataSizeInBytes;
extern float ks,kd,kShear;

extern vector<Vertex> gVertices;
extern vector<Indice> gIndices;
extern vector<Texture> gTextures;
extern vector<Normal> gNormals;

