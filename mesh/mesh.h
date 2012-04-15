/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** mesh.h ***/

#ifndef _MESH_H
#define _MESH_H

#include <list>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
	static void sortEdges(list<Edge*> *edges);
	void remove(Vertex *v);
    void remove(Edge *e);
    void remove(Triangle *t);

    Vertex* findVertex(uint64_t);

	list<Vertex*>* getVertices(void) const;
	list<Triangle*>* getTriangles(void) const;
	list<Edge*>* getEdges(void) const;

	void setMinMaxValues(void);
	float XMin(void) const;
	float YMin(void) const;
	float ZMin(void) const;
	float XMax(void) const;
	float YMax(void) const;
	float ZMax(void) const;

protected:

	list<Triangle*> *triangles;
	list<Vertex*> *vertices;
	list<Edge*> *edges;
	// verNum records the total vertices number in
	// this mesh
	int verNum;
	float xMin,yMin,zMin,xMax,yMax,zMax;

};

#endif
