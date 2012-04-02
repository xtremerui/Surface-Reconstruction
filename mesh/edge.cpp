/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** edge.cpp ***/

#include "edge.h"

Edge::Edge(Vertex *v1, Vertex *v2)
{
  vertices[0]=v1;
  vertices[1]=v2;
}

Edge::Edge()
{
	vertices[0]=NULL; vertices[1]=NULL;
}

Edge::~Edge()
{
  for (int i=0; i < 2; i++)
    vertices[i]->deleteEdge(this);
}

bool Edge::equal(const Edge *e) const
{
  if (vertices[0]->equal(e->vertices[0]) && vertices[1]->equal(e->vertices[1]))
    return true;
  if (vertices[0]->equal(e->vertices[1]) && vertices[1]->equal(e->vertices[0]))
    return true;

  return false;
}

bool Edge::equal(const Vertex *v1, const Vertex *v2) const
{
  if ((vertices[0] == v1 && vertices[1] == v2) ||
      (vertices[0] == v2 && vertices[1] == v1))
    return true;
  else
    return false;
}

void Edge::changeVertex(const Vertex *oldV, Vertex *newV)
{
	for (int i=0 ; i<2 ; i++){

			if (vertices[i]->equal(oldV))
				vertices[i]=newV;
		}
}
