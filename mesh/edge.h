/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** mesh.h ***/

#ifndef _EDGE_H
#define _EDGE_H

#include <list>

#include "triangle.h"
#include "vertex.h"
#include "mathwork.h"

using namespace std;

class Vertex;
class Triangle;

class Edge{
public:
	Edge(Vertex *v1, Vertex *v2);
	Edge();
	~Edge();
	bool equal(const Edge *e) const;
	bool equal(const Vertex *v1, const Vertex *v2) const;
	void changeVertex(const Vertex *oldV, Vertex *newV);
	Vertex *vertices[2];
	list<Triangle*> triangles;
    float error;
    Vertex *newpoint;
    MathWork Qw;
    float weight;

};

#endif
