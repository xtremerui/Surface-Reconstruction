/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** vertex.cpp ***/

#include "vertex.h"

Vertex::Vertex(float x, float y, float z)
{ 
	data.value[0]=x;
	data.value[1]=y;
	data.value[2]=z;
	vNormal=NULL;

}

Vertex::Vertex()
{
	vNormal = NULL;
}

Vertex::~Vertex()
{
	//delete vNormal;
}

bool Vertex::checkAndAddEdge(Edge* edge)
{
	list<Edge*>::iterator ie;

    // if the edge is exist, dont' add it
	for (ie=edges.begin();ie!=edges.end();ie++){

		if((*ie)->equal(edge))
			return false;
	}
	edges.push_back(edge);

	return true;
}

void Vertex::set(float x, float y, float z)
{
	data.value[0]=x;
	data.value[1]=y;
	data.value[2]=z;
    vNormal = NULL;
}

void Vertex::addEdge(Edge* edge)
{
	edges.push_back(edge);
}

void Vertex::addTriangle(Triangle* triangle)
{
	triangles.push_back(triangle);
}

void Vertex::deleteTriangle(Triangle* triangle)
{
  triangles.remove(triangle);
}

list<Triangle*>* Vertex::getTriangles(void)
{
	return &triangles;
}

list<Edge*>* Vertex::getEdges(void)
{
	return &edges;
}

void Vertex::resetQ(void)
{
    for (int i=0;i<4;i++)
    	for(int j=0;j<4;j++)
    		Q.v[i][j] = 0.0;
}

void Vertex::move(float x, float y, float z)
{
  data.value[0] = x;
  data.value[1] = y;
  data.value[2] = z;
}

void Vertex::calQ(void)
{
	list<Triangle*>::iterator it;


    for (it=(&triangles)->begin();it!=(&triangles)->end();it++){

    	MathWork::add((*it)->getKp(), &Q);
    }
}

const MathWork* Vertex::getQ(void)
{
	return &Q;
}

void Vertex::deleteEdge(Edge* edge)
{
  edges.remove(edge);

}

bool Vertex::equal(const Vertex *v) const
{
	return data.value[0] == v->data.value[0] &&
			data.value[1] == v->data.value[1] &&
			data.value[2] == v->data.value[2];
}

const float* Vertex::floatData(void) const
{
	return data.value;
}

const MathWork* Vertex::mathData(void) const
{
	return &data;
}

const float* Vertex::floatNormal(void) const
{
	return vNormal != NULL ? vNormal->value : NULL;
}

MathWork* Vertex::getNormal(void)
{
	return vNormal;
}

void Vertex::addNormal(const MathWork *norm)
{
	if (vNormal == NULL)
		vNormal = new MathWork();
	// Add the passed in surface normal to this vertex
	// and normalise the vertex
	vNormal->value[0] += norm->value[0];
	vNormal->value[1] += norm->value[1];
	vNormal->value[2] += norm->value[2];

	vNormal->normalise();
}

void Vertex::setNormal(const MathWork *norm)
{
	if (vNormal == NULL)
		vNormal = new MathWork();
	// Add the passed in surface normal to this vertex
	// and normalise the vertex
	vNormal->value[0] = norm->value[0];
	vNormal->value[1] = norm->value[1];
	vNormal->value[2] = norm->value[2];

	vNormal->normalise();
}

void Vertex::calNormal(void)
{
	delete vNormal;
	vNormal = NULL;
	list<Triangle*>::iterator it;
	for (it = triangles.begin();it!=triangles.end();it++)
		addNormal((*it)->getNormal());
}

float Vertex::x(void) const
{
  return data.value[0];
}

float Vertex::y(void) const
{
  return data.value[1];
}

float Vertex::z(void) const
{
  return data.value[2];
}
