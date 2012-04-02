/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** triangle.h ***/

#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <vector>
#include "vertex.h"
#include "edge.h"
#include "mathwork.h"

using namespace std;

class Triangle
{
public:
	Triangle(Vertex *v1, Vertex *v2, Vertex *v3);
	~Triangle();
	void calcNormal(void);
	void calcKp(void);

	const float* floatNormal(void) const;
	const MathWork* getKp(void) const;
	const MathWork* getNormal(void) const;
	bool equal(const Triangle *tri) const;
	void changeVertex(const Vertex *oldV, Vertex *newV);
	Vertex *vertices[3];

	private:
	MathWork triNormal;
	MathWork Kp;
};

#endif
