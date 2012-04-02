/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** triangle.cpp ***/

#include "triangle.h"

Triangle::Triangle(Vertex *v1, Vertex *v2, Vertex *v3)
{
	vertices[0]=v1;
	vertices[1]=v2;
	vertices[2]=v3;

	calcNormal();

	// Add the surface normal to all 3 vertices that define
	// the surface
	for (int i=0; i < 3; i++){
		vertices[i]->addTriangle(this);
		vertices[i]->addNormal(&triNormal);
	}
}

Triangle::~Triangle()
{

  for (int i=0; i < 3; i++)
      vertices[i]->deleteTriangle(this);
}

/*** Calculate the surface normal  ***/

void Triangle::calcNormal(void)
{
	// calculate the vectors between the vertices
	MathWork edge[2];

	// find out the edge vector between vertex 0 and vertex 1
	// vertex 0 - vertex 1
	edge[0].value[0] = vertices[0]->mathData()->value[0] -
						vertices[1]->mathData()->value[0];
	edge[0].value[1] = vertices[0]->mathData()->value[1] -
						vertices[1]->mathData()->value[1];
	edge[0].value[2] = vertices[0]->mathData()->value[2] -
						vertices[1]->mathData()->value[2];

	// find out the edge vector between vertex 0 and vertex 2
	// vertex 0 - vertex 2
	edge[1].value[0] = vertices[0]->mathData()->value[0] -
						vertices[2]->mathData()->value[0];
	edge[1].value[1] = vertices[0]->mathData()->value[1] -
						vertices[2]->mathData()->value[1];
	edge[1].value[2] = vertices[0]->mathData()->value[2] -
						vertices[2]->mathData()->value[2];

	// calculate surface normal since two edges can span a surface
	// cross product between two edges
	triNormal.value[0] = edge[0].value[1] * edge[1].value[2] -
							edge[0].value[2] * edge[1].value[1];
	triNormal.value[1] = edge[0].value[2] * edge[1].value[0] -
			  	  	  		edge[0].value[0] * edge[1].value[2];
	triNormal.value[2] = edge[0].value[0] * edge[1].value[1] -
							edge[0].value[1] * edge[1].value[0];

 	triNormal.normalise();




}

void Triangle::calcKp(void)
{
	// calculate the plain ax+by+cz+d = 0
	// first calculate d by using normal and the first vertex of this triangle
	float d = -(triNormal.value[0] * vertices[0]->x() +
				triNormal.value[1] * vertices[0]->y() +
				triNormal.value[2] * vertices[0]->z());
	// put d into the plane definition vector p = [a b c d]
	float a = triNormal.value[0]; //a
	float b = triNormal.value[1]; //b
	float c = triNormal.value[2]; //c

	// define the fundamental error quadric
	/*****  Kp
	 * [a^2  ab  ac  ad ]
	 * [ab   b^2 bc  bd ]
	 * [ac   bc  c^2 cd ]
	 * [ad   bd  cd  d^2]
	 */
	Kp.v[0][0] = a*a;        Kp.v[0][1] = a*b;        Kp.v[0][2] = a*c;        Kp.v[0][3] = a*d;
	Kp.v[1][0] = Kp.v[0][1]; Kp.v[1][1] = b*b;        Kp.v[1][2] = b*c; 	   Kp.v[1][3] = b*d;
	Kp.v[2][0] = Kp.v[0][2]; Kp.v[2][1] = Kp.v[1][2]; Kp.v[2][2] = c*c; 	   Kp.v[2][3] = c*d;
	Kp.v[3][0] = Kp.v[0][3]; Kp.v[3][1] = Kp.v[1][3]; Kp.v[3][2] = Kp.v[2][3]; Kp.v[3][3] = d*d;
}

const float* Triangle::floatNormal(void) const
{
	return triNormal.value;
}

const MathWork* Triangle::getNormal(void) const
{
    return &triNormal;
}

const MathWork* Triangle::getKp(void) const
{
	return &Kp;
}

bool Triangle::equal(const Triangle *tri) const
{
  return vertices[0] == tri->vertices[0] &&
    vertices[1] == tri->vertices[1] && vertices[2] == tri->vertices[2];
}

void Triangle::changeVertex(const Vertex *oldV, Vertex *newV)
{
	for (int i=0 ; i<3 ; i++){
		if (vertices[i]->equal(oldV))
			vertices[i]=newV;
	}
	calcNormal();
}
