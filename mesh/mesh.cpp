/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** mesh.cpp ***/

#include "mesh.h"

Mesh::Mesh()
{
	triangles = new list<Triangle*>;
	vertices = new list<Vertex*>;
	edges = new list<Edge*>;
	verNum = 0;
}

Mesh::~Mesh()
{
	clear();
	delete triangles;
	delete vertices;
	delete edges;
}

void Mesh::clear(void)
{

	list<Triangle*>::iterator it;
	list<Vertex*>::iterator iv;
	list<Edge*>::iterator ie;

	for (it=triangles->begin(); it != triangles->end(); it++)
		delete *it;
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		delete *iv;
    for (ie=edges->begin(); ie != edges->end(); ie++)
		delete *ie;

	triangles->clear();
	vertices->clear();
    edges->clear();

	verNum = 0;
}

void Mesh::addTriangle(Triangle *t)
{
	triangles->push_back(t);
}

void Mesh::addVertex(Vertex *v)
{
	vertices->push_back(v);
	// Records the vertex index for writing SMF file
	v->index = ++verNum;
}

bool Mesh::addEdge(Edge *e)
{

	/*list<Edge*>::iterator ie;

	for (ie=edges->begin(); ie != edges->end(); ie++){
		if((*ie)->equal(e))
			return false;

	}*/
	edges->push_back(e);

	return true;
}

void Mesh::sortEdges(void)
{
	vector<Edge*> edgeIndex (edges->begin(),edges->end());


	int i,j;
	for(i=0;i<edgeIndex.size();i++)
	{
		for(j=i+1;j<edgeIndex.size();j++)
		{
			if(edgeIndex[i]->equal(edgeIndex[j]))
			{
				//edgeIndex[j]=edgeIndex[edgeIndex.size()-1];
				edgeIndex.at(j)=edgeIndex.back();
				edgeIndex.pop_back();
				//size--;
				break;
			}
		}
		//sortedEdges->push_back(edgeIndex[i]);
	}

    edges->assign(edgeIndex.begin(),edgeIndex.end());

    list<Edge*>::iterator ie;

    for (ie = edges->begin();ie!=edges->end();ie++){
    	(*ie)->vertices[0]->addEdge((*ie));
    	(*ie)->vertices[1]->addEdge((*ie));
    }

}

void Mesh::sortEdges(list<Edge*>* edges)
{
	vector<Edge*> edgeIndex (edges->begin(),edges->end());


	int i,j;
	for(i=0;i<edgeIndex.size();i++)
	{
		for(j=i+1;j<edgeIndex.size();j++)
		{
			if(edgeIndex[i]->equal(edgeIndex[j]))
			{
				//edgeIndex[j]=edgeIndex[edgeIndex.size()-1];
				edgeIndex.at(j)=edgeIndex.back();
				edgeIndex.pop_back();
				//size--;
				break;
			}
		}
		//sortedEdges->push_back(edgeIndex[i]);
	}

    edges->assign(edgeIndex.begin(),edgeIndex.end());

}

void Mesh::remove(Vertex *vertex)
{
	int oldIndex = vertex->index;


	// move the vertices behind this one 1 index forward
	list<Vertex*>::iterator iv;
	int i=0;
	for (iv = vertices->begin(); iv!= vertices->end(); iv++){

		if ((*iv)->index > oldIndex)
			((*iv)->index)--;
	}
	vertices->remove(vertex);
		verNum--;
	delete vertex;
	vertex=NULL;

}

void Mesh::remove(Edge *edge)
{
	edges->remove(edge);

	delete edge; ///warning!!
	edge = NULL;
	//printf("edgeremove ");
	//	fflush(stdout);

}

void Mesh::remove(Triangle *triangle)
{
	triangles->remove(triangle);
	delete triangle;
	triangle = NULL;
}

list<Triangle*>* Mesh::getTriangles(void) const
{
	return triangles;
}

list<Vertex*>* Mesh::getVertices(void) const
{
	return vertices;
}

list<Edge*>* Mesh::getEdges(void) const
{
	return edges;
}

void Mesh::setMinMaxValues(void)
{
  list<Vertex*>::iterator iv;

  xMin = xMax = yMin = yMax = zMin = zMax = 0;

  // find smalles & largest vertex in all directions (x, y, z)
  for (iv=vertices->begin(); iv != vertices->end(); iv++)
    {
      if (xMin > (*iv)->x())
	xMin = (*iv)->x();
      if (xMax < (*iv)->x())
	xMax = (*iv)->x();
      if (yMin > (*iv)->y())
	yMin = (*iv)->y();
      if (yMax < (*iv)->y())
	yMax = (*iv)->y();
      if (zMin > (*iv)->z())
	zMin = (*iv)->z();
      if (zMax < (*iv)->z())
	zMax = (*iv)->z();
    }
}

float Mesh::XMin(void) const
{
  return xMin;
}

float Mesh::XMax(void) const
{
  return xMax;
}

float Mesh::YMin(void) const
{
  return yMin;
}

float Mesh::YMax(void) const
{
  return yMax;
}

float Mesh::ZMin(void) const
{
  return zMin;
}

float Mesh::ZMax(void) const
{
  return zMax;
}

