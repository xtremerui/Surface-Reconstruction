/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** mesh.h ***/

#ifndef _MESH_H
#define _MESH_H

#include <list>
#include <vector>

#include <stdio.h>
#include <string.h>

#include "triangle.h"
#include "vertex.h"
#include "edge.h"
#include "Disjoint_Set_Forest.h"

using namespace std;

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();
  
	void clear(void);

	void addTriangle(Triangle *t);
	void addVertex(Vertex *v);
	bool addEdge(Edge *e);
	void sortEdges(void);
	void remove(Vertex *v);
    void remove(Edge *e);
    void remove(Triangle *t);

	list<Vertex*>* getVertices(void) const;
	list<Triangle*>* getTriangles(void) const;
	list<Edge*>* getEdges(void) const;

protected:

	list<Triangle*> *triangles;
	list<Vertex*> *vertices;
	list<Edge*> *edges;
	// verNum records the total vertices number in
	// this mesh
	int verNum;
};

#endif
