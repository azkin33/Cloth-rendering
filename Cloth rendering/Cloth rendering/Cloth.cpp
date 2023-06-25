#pragma once
#include "main.h"
class Cloth{
	public:
		int res;
		float startX, startY;
		int step = 0;
	Cloth() {
		res = 0;
		startX = 0;
		startY = 0;
	}
	Cloth(float _startX,float _startY,int _res) {
		startX = _startX;
		startY = _startY;
		res = _res;
		vertexData = new GLfloat[(res + 1) * (res + 1)];
		float stepX = startX * 2 / res;
		float stepY = stepX;
		float springLength = _startX*2/res;
		for (int j = 0; j < res +1; j++)
		{
			for (int i = 0; i < res +1; i++)
			{
				Vertex v = Vertex(-startX + stepX * i, -startY + stepY * j, 0);
				v.restLength = springLength;
				gVertices.push_back(v);
				gNormals.push_back(Normal(0, 1, 0));
			}
		}

		cout << res << "-" << res << endl;
		int vertPerLine = res;
		int index = 0;
		int count = 0;
		int offset = 0;
		for (int i = 0; i < res * res; i++)
		{
			if (i>0 && i % res == 0) index ++;
			int dif = vertPerLine+1;
			gIndices.push_back(Face(index, index + 1, index + dif));
			gIndices.push_back(Face(index+1, index +dif, index +1+ dif));
			index++;
		}
		int numX = res + 1;
		for (int i = 0; i < (numX) * (numX); i++)
		{
			Vertex current = gVertices[i];
			int row = i / numX;
			int column = i % numX;
			if (row > 0) {
				current.structureNb.push_back(i - numX);
			}
			if (row < numX) {
				current.structureNb.push_back(i + numX);
			}
			if (column != 0) {
				current.structureNb.push_back(i - 1);
				if (column != 1) {
					current.bendNb.push_back(i - 2);
				}
				if (row > 0) {
					current.shearNb.push_back(i - numX - 1);
				}
				if (row < numX - 1) 
				{
					current.shearNb.push_back(i + numX - 1);
				}
			}
			if (column != numX-1) {
				current.structureNb.push_back(i + 1);
				if (column != numX-2) {
					current.bendNb.push_back(i + 2);
				}
				if (row > 0) {
					current.shearNb.push_back(i - numX + 1);
				}
				if (row < numX - 1)
				{
					current.shearNb.push_back(i + numX + 1);
				}
			}

			if (row > 1) {
				current.bendNb.push_back(i - numX * 2);
			}
			if (row < numX-1) {
				current.bendNb.push_back(i + numX * 2);
			}
			
			
			PrintNeighbours(i, &current);
		}
		cout << "numX: " << res << endl;
	}

	void AddForceV(Vertex* vertex1, Vertex* vertex2) {
		glm::vec3 distance = glm::vec3(vertex1->pos.x - vertex2->pos.x, vertex1->pos.y - vertex2->pos.y, vertex1->pos.z - vertex2->pos.z);


		float length = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));

		glm::vec3 normalized = distance / length;
		float v1 = glm::dot(vertex1->speed, normalized);
		float v2 = glm::dot(vertex2->speed, normalized);

		float fSD = -0 * (vertex1->restLength - length) - 0 * (v1 - v2);

		glm::vec3 force = distance * fSD;

		vertex1->AddForce(force.x, force.y, force.z);
		//p2->addForce(-fx, -fy, -fz);
	}

	void ApplyForcesToVertex(Vertex *vertex)
	{
		for (int i = 0; i < vertex->structureNb.size(); i++)
		{
			AddForceV(vertex, &gVertices[vertex->structureNb[i]]);
		}
	}
	void ApplyAllForces() {
		for (int i = 0; i < gVertices.size(); i++)
		{
			ApplyForcesToVertex(&gVertices[i]);
		}
		for (int i = 0; i < gVertices.size(); i++)
		{
			Vertex v = gVertices[i];
			v.pos.x += 0.1f;
		}
		cout << "Applied:"<<step++ << endl;
	}
	
	void PrintNeighbours(int i, Vertex* v) {

		cout << i << "=============" << endl;
		for (int i = 0; i < v->structureNb.size(); i++)
		{
			cout << v->structureNb[i] << "\t";
		}
		cout << endl;
		for (int i = 0; i < v->bendNb.size(); i++)
		{
			cout << v->bendNb[i] << "\t";
		}
		cout << endl;
		for (int i = 0; i < v->shearNb.size(); i++)
		{
			cout << v->shearNb[i] << "\t";
		}
		cout << endl;
		cout << "===============" << endl;
	}
};

