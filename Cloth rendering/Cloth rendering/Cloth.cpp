#pragma once
#include "main.h"
#define SQRT2  1.41421356237
enum SimType {
	FourCorners,
	TwoCorners,
	Drop
};
class Cloth{
	public:
		int res;
		float startX, startY;
		int step = 0;
		float springLength;
		SimType type;
		GLfloat* textureData;
		int numX;
		float nodeSize = 0.3f;
	Cloth() {
		res = 0;
		startX = 0;
		startY = 0;
	}
	Cloth(float _startX,float _startY,int _res) {
		startX = _startX;
		startY = _startY;
		res = _res;
		numX = res + 1;
		vertexData = new GLfloat[(res + 1) * (res + 1)*3];
		normalData = new GLfloat[(res + 1) * (res + 1) * 3];
		indexData = new GLuint[res * res * 2*3];
		type = FourCorners;
		float stepX = startX * 2 / res;
		float stepY = stepX;
		springLength = _startX*2/res;
		int vIndex = 0;
		textureData = new GLfloat[numX * numX*2];
		for (int j = 0; j < res +1; j++)
		{
			for (int i = 0; i < res +1; i++)
			{
				Vertex v = Vertex(-startX + stepX * i,clothHeight, -startY + stepY * j);
				gVertices.push_back(v);
				vertexData[3*vIndex] = v.pos.x;
				vertexData[3*vIndex+1] = v.pos.y;
				vertexData[3*vIndex+2] = v.pos.z;

				textureData[vIndex * 2] = i / (float)numX;
				textureData[vIndex * 2 + 1] = j / (float)numX;
				vIndex++;
				gNormals.push_back(Normal(0, 1, 0));
				gTextures.push_back(Texture(i / numX, j/ numX));
			}
		}
		
		
		int vertPerLine = res;
		int index = 0;
		int count = 0;
		int offset = 0;
		vIndex = 0;
		for (int i = 0; i < res * res; i++)
		{
			if (i>0 && i % res == 0) index ++;
			int dif = vertPerLine+1;
			gIndices.push_back(Indice(index, index + 1, index + dif));
			gIndices.push_back(Indice(index+1, index +dif, index +1+ dif));
			indexData[3*vIndex] = index;
			indexData[3*vIndex+1] = index+1;
			indexData[3*vIndex+2] = index+dif;
			vIndex++;

			indexData[3*vIndex] = index+1;
			indexData[3*vIndex+1] = index+dif;
			indexData[3*vIndex+2] = index+dif+1;
			vIndex++;
			index++;
		}
		for (int i = 0; i < (numX) * (numX); i++)
		{
			Vertex *current = &gVertices[i];
			int row = i / numX;
			int column = i % numX;
			if (row > 0) {
				current->structureNb.push_back(i - numX);
			}
			if (row < numX-1) {
				current->structureNb.push_back(i + numX);
			}
			if (column != 0) {
				current->structureNb.push_back(i - 1);
				if (column != 1) {
					current->bendNb.push_back(i - 2);
				}
				if (row > 0) {
					current->shearNb.push_back(i - numX - 1);
				}
				if (row < numX - 1) 
				{
					current->shearNb.push_back(i + numX - 1);
				}
			}
			if (column != numX-1) {
				current->structureNb.push_back(i + 1);
				if (column != numX-2) {
					current->bendNb.push_back(i + 2);
				}
				if (row > 0) {
					current->shearNb.push_back(i - numX + 1);
				}
				if (row < numX - 1)
				{
					current->shearNb.push_back(i + numX + 1);
				}
			}

			if (row > 1) {
				current->bendNb.push_back(i - numX * 2);
			}
			if (row < numX-2) {
				current->bendNb.push_back(i + numX * 2);
			}
			
			
			//PrintNeighbours(i, current);
		}
		
		
	}

	void SwitchType(SimType type) {
		this->type = type;
		for (int i = 0; i < gVertices.size(); i++)
		{
			gVertices[i].fixed = false;
		}
		SetFixedVertices();
	}
	void SetFixedVertices() {
		switch (type)
		{
		case FourCorners:
			gVertices[res * (res + 1)].fixed = true;
			gVertices[(res + 1) * (res + 1) - 1].fixed = true;
			gVertices[ res].fixed = true;
			gVertices[0].fixed = true;
			break;
		case TwoCorners:
			gVertices[res * (res + 1)].fixed = true;
			gVertices[(res + 1) * (res + 1) - 1].fixed = true;
			break;
		case Drop:
			break;
		default:
			break;
		}
	}
	
	
	
	void draw() {
		glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, clothTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(gTextures.size() * 3 * sizeof(GLfloat)));
		glDrawElements(GL_TRIANGLES, gIndices.size() * 3, GL_UNSIGNED_INT, 0);
	}
	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
	}
	void bufferData()
	{
		glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

		glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gTextures.size() * 2 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
		glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gTextures.size() * 2 * sizeof(GLfloat), textureData);
		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, gTextures.size()*2*sizeof(GLfloat), textureData, GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,0, (const GLvoid*)(gTextures.size() * 3 * sizeof(GLfloat)));
;
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	}
	void initVBO()
	{


		GLuint vao;
		glGenVertexArrays(1, &vao);
		assert(vao > 0);
		glBindVertexArray(vao);
		cout << "vao = " << vao << endl;

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		assert(glGetError() == GL_NONE);

		glGenBuffers(1, &gVertexAttribBuffer);
		glGenBuffers(1, &gIndexBuffer);

		assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

		gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
		gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
		indexDataSizeInBytes = gIndices.size() * 3 * sizeof(GLuint);
		vertexData = new GLfloat[gVertices.size() * 3];
		normalData = new GLfloat[gNormals.size() * 3];
		indexData = new GLuint[gIndices.size() * 3];
		float minX = 1e6, maxX = -1e6;
		float minY = 1e6, maxY = -1e6;
		float minZ = 1e6, maxZ = -1e6;

		for (int i = 0; i < gVertices.size(); ++i)
		{
			vertexData[3 * i] = gVertices[i].pos.x;
			vertexData[3 * i + 1] = gVertices[i].pos.y;
			vertexData[3 * i + 2] = gVertices[i].pos.z;

			minX = std::min(minX, gVertices[i].pos.x);
			maxX = std::max(maxX, gVertices[i].pos.x);
			minY = std::min(minY, gVertices[i].pos.y);
			maxY = std::max(maxY, gVertices[i].pos.y);
			minZ = std::min(minZ, gVertices[i].pos.z);
			maxZ = std::max(maxZ, gVertices[i].pos.z);
		}

		for (int i = 0; i < gNormals.size(); ++i)
		{
			normalData[3 * i] = gNormals[i].x;
			normalData[3 * i + 1] = gNormals[i].y;
			normalData[3 * i + 2] = gNormals[i].z;
		}

		for (int i = 0; i < gIndices.size(); ++i)
		{
			indexData[3 * i] = gIndices[i].x;
			indexData[3 * i + 1] = gIndices[i].y;
			indexData[3 * i + 2] = gIndices[i].z;
		}
	}

	void AddForceV(Vertex* p1, Vertex* p2, float ks, float springLength) {
		float ex = p2->pos.x - p1->pos.x;
		float ey = p2->pos.y - p1->pos.y;
		float ez = p2->pos.z - p1->pos.z;

		float length = sqrt(pow(ex, 2) + pow(ey, 2) + pow(ez, 2));
		if (length > springLength) {
			glm::vec3 dir = glm::normalize(p2->pos - p1->pos);
			if (p1->fixed) {
				p2->pos += (length - springLength) * -dir;
			}
			else if (p2->fixed) {
				p1->pos += (length - springLength) * dir;
			}
			else {
				p1->pos += (length - springLength)/2 * dir;
				p2->pos += (length - springLength)/2 * -dir;
			}
			return;
		}
		ex = ex / length;
		ey = ey / length;
		ez = ez / length;

		float v1 = ex * p1->speed.x + ey * p1->speed.y + ez * p1->speed.z;
		float v2 = ex * p2->speed.x + ey * p2->speed.y + ez * p2->speed.z;

		float fSD = -ks * (springLength - length) - kd * (v1 - v2);


		float fx = fSD * ex;
		float fy = fSD * ey;
		float fz = fSD * ez;

		p1->AddForce(fx, fy, fz);
		//p2->AddForce(-fx, -fy, -fz);
	}
	
	glm::vec3* CalcVerlet(Vertex *v) 
	{
		glm::vec3 temp;
		temp =  2.0f * v->pos - v->oldPos + v->force * 0.0001f;
		return &temp;
	}
	void ApplyForcesToVertex(Vertex* vertex)
	{
		vertex->force = glm::vec3();
		for (int i = 0; i < vertex->structureNb.size(); i++)
		{
			AddForceV(vertex, &gVertices[vertex->structureNb[i]], ks, springLength);
		}
		for (int i = 0; i < vertex->shearNb.size(); i++)
		{
			AddForceV(vertex, &gVertices[vertex->shearNb[i]], ks, springLength * SQRT2);
		}
		for (int i = 0; i < vertex->bendNb.size(); i++)
		{
			AddForceV(vertex, &gVertices[vertex->bendNb[i]], ks, springLength*2);
		}

		vertex->AddGravity();
		//cout << gVertices[0].force.x << "\t" << gVertices[0].force.y << "\t" << gVertices[0].force.z << endl;
	}
	bool CheckColisionCube(glm::vec3 nextPos)
	{
		float x = nextPos.x;
		float y = nextPos.y;
		float z = nextPos.z;
		if (x<=cubeMax.x&& x>=cubeMin.x && z<=cubeMax.z&& z>=cubeMin.z) {
			if (y > cubeMin.y && y < cubeMax.y) {
				return false;
			}
		}
		return true;
	}

	void ApplyAllForces() {
		for (int i = 0; i < gVertices.size(); i++)
		{
			if (gVertices[i].fixed) continue;
			ApplyForcesToVertex(&gVertices[i]);
		}
		for (int i = 0; i < gVertices.size(); i++)
		{
			Vertex *v = &gVertices[i];
			if (!v->fixed)
			{
				glm::vec3* verlet = CalcVerlet(v);
				v->oldPos = v->pos;
				if (CheckColisionCube(*verlet)) {
					v->pos = *verlet;
					v->speed = v->force *= 0.01f;
					vertexData[i * 3] = v->pos.x;
					vertexData[i * 3 + 1] = v->pos.y;
					vertexData[i * 3 + 2] = v->pos.z;
				}
				else {
					v->pos += glm::normalize(v->oldPos -*verlet)*nodeSize;
					v->speed = glm::vec3();

				}
				/*float t;
				if (aabbCollisionCheck(&v->oldPos, verlet, &t)) {
					v->oldPos = v->pos;
					v->speed = glm::vec3();
					v->accel = glm::vec3();

					cout << "colldied" << endl;
				}
				else {

					cout << "nnncolldied" << endl;
					v->oldPos = v->pos;
					v->pos = *verlet;
					v->speed = v->force *= 0.001f;
					vertexData[i * 3] = v->pos.x;
					vertexData[i * 3 + 1] = v->pos.y;
					vertexData[i * 3 + 2] = v->pos.z;
				}*/
			}
		}
		
	}
	
	void PrintNeighbours(int i, Vertex* v) {

		cout << i << "=============" << endl;
		/*for (int i = 0; i < v->structureNb.size(); i++)
		{
			cout << v->structureNb[i] << "\t";
		}
		cout << endl;*/
		for (int i = 0; i < v->bendNb.size(); i++)
		{
			cout << v->bendNb[i] << "\t";
		}
		cout << endl;
		/*
		for (int i = 0; i < v->shearNb.size(); i++)
		{
			cout << v->shearNb[i] << "\t";
		}
		cout << endl;
		cout << "===============" << endl;*/
	}
};

