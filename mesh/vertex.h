/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** vertex.h ***/

#ifndef _VERTEX_H
#define _VERTEX_H

#include <list>
#include "triangle.h"
#include "edge.h"
//#include <utility>
#include <stdint.h>

#include "mathwork.h"

using namespace std;

class Triangle;
class Edge;

class Vertex
{
public:
	Vertex(float x, float y, float z);
	Vertex();
	~Vertex();

	void set(float x, float y, float z);
	void addEdge(Edge* edge);
	bool checkAndAddEdge(Edge* edge);
	void addTriangle(Triangle* triangle);
	void deleteTriangle(Triangle* triangle);
	list<Triangle*>* getTriangles(void);
	list<Edge*>* getEdges(void);

	void deleteEdge(Edge* edge);

	void calQ(void);
	void resetQ(void);
	void move(float x, float y, float z);
	const MathWork* getQ(void);

	bool equal(const Vertex* v) const;

	const float* floatData(void) const;
	const MathWork* mathData(void) const;

	const float* floatNormal(void) const;
	MathWork* getNormal(void);
	void calNormal(void);
	void addNormal(const MathWork *norm);
	void setNormal(const MathWork *norm);

	float x(void) const;
	float y(void) const;
	float z(void) const;

	uint64_t index;
	float disc;
	list<Vertex*> kNNbr;

private:
	MathWork *vNormal, data;
	MathWork Q;
	list<Edge*> edges;
	list<Triangle*> triangles;
};

#endif
