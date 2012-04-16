/****************************************************************************

  Rui Yang
  301130789
  ruiy@sfu.ca
  764 Assignment 2
  SMF Viewer v2
  Added the mesh decimation option. Add a class named edge.
  mcaq.cpp
	   
****************************************************************************/

#include <string.h>
#include <GL/glui.h>
#include <algorithm>
#include "mesh.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Eigen/Core"
#include "Eigen/Eigen"
#include "marchingcubes.h"

using namespace std;
using namespace Eigen;

float xy_aspect;
int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;

/** Live variables updated by GLUI ***/

int   displayMode = 1;
int   obj_type = 1;
int   light0_enabled = 1;
int   light1_enabled = 1;
float light0_intensity = 1.0;
float light1_intensity = .4;
int   main_window;
float scale = 1.0;
int show_text = 1;

float obj_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };
const char *string_list[] = {"flat shaded", "wireframe", "shaded with edges"};
int option = 0;
int curr_string = 0;


/**********Data Storage**************/

Mesh *mesh;
list<Triangle*> *listTriangles;
list<Vertex*>   *listVertices;
float thre;
vector<Vertex*> vertexIndex;
list<Edge*> result;
list<Edge*> result1;
list<Edge*> testNormal;
list<Vertex*> tangentPlnCen;

Triangle *tri;
Vertex *v;
Edge *e;
char nextChar;
int p1, p2, p3;
float x, y, z;
bool isFileRead = false;
const char *filename;
int dec_num;
bool showEdges = false;

#define K_NEAREST_NBR 15
/** Pointers to the GLUI windows and controls **/

GLUI            *glui;
GLUI_Spinner    *light0_spinner;
GLUI_Panel      *obj_panel;
GLUI_Listbox    *listbox;
GLUI_EditText   *edittext;
GLUI_EditText   *collapse_edge;


/********** User IDs for callbacks ********/
#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  260

#define OPEN_ID              400
#define SAVE_ID              401

#define MODE_ID              402
#define TEXT_ID              403

#define OI_ID                601

/********** Miscellaneous global variables **********/

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

Vertex tmp_v;

bool sortFunction(const Edge* e1, const Edge* e2)
{
	return e1->weight < e2->weight;
}

bool sortVertices(const Vertex* v1, const Vertex* v2)
{
	return v1->disc < v2->disc;
}

void DFS (Vertex* v , bool visited[])
{
	//Mark the current node as node visited
	visited[v->index-1] = true;

    //printf("vindex: %d ", v->index);
	//Recur for all the vertices adjacent to this vertex
	list<Edge*>::iterator ie;
	for( ie = v->getEdges()->begin(); ie != v->getEdges()->end(); ie++)
	{
		Vertex* w = v->equal((*ie)->vertices[0]) ? (*ie)->vertices[1] : (*ie)->vertices[0];
		//printf("windex: %d ", w->index);
		if( !visited[w->index-1])
		{
			//propagate normal through this edge
			float result = 0.0;
			MathWork::dotProduct(v->floatNormal(),3,w->floatNormal(), &result);
			//printf("x:%f y:%f z:%f \n", w->floatNormal()[0],w->floatNormal()[1],w->floatNormal()[2]);
			if (result < 0){
				w->getNormal()->opposit();
				//printf("ax:%f ay:%f az:%f \n", w->floatNormal()[0],w->floatNormal()[1],w->floatNormal()[2]);
			}


			// call DFS for the child vertex w
			DFS(w, visited);
		}
	}
}

float distance (Vertex* p1, Vertex* p2){

	return (p1->x()-p2->x())*(p1->x()-p2->x()) +
			(p1->y()-p2->y())*(p1->y()-p2->y())+
			(p1->z()-p2->z())*(p1->z()-p2->z());

}

float SDF (Vertex* p){
	// find the closest tangent plane center oi
	list<Vertex*>::iterator iv;
	Vertex* closestCen = *(tangentPlnCen.begin());
	float minDisc = distance (p, closestCen);
	//printf("p: %f %f %f ", p->x(),p->y(),p->z());
	for(iv = tangentPlnCen.begin(); iv!= tangentPlnCen.end();iv++){
		float temp = distance (p, *iv);
        //printf("d:%f ", temp);fflush(stdout);
		if (minDisc > temp){
			closestCen = *iv;
			minDisc = temp;
		}
	}
	if (thre< minDisc && 0)
		return 1;
	//printf("minDisc:%f ", minDisc);fflush(stdout);
	// f(p) = (p-oi).ni
	float shortDisc = 0.0;
    const float *normal = closestCen->floatNormal();
    //for(int i = 0; i < 3; i++)
    	//printf("normal: %f ", normal[i]);
	float diff[]={p->x()-closestCen->x(), p->y()-closestCen->y(),p->z()-closestCen->z()};
    for(int i = 0; i < 3; i++){
        shortDisc += diff[i]*normal[i];
    }


    //printf("%f", shortDisc);
	return shortDisc;

}

static float isolvl_fn (float x, float y, float z){
	//printf("tmp: %f %f %f ", tmp_v.x(),tmp_v.y(),tmp_v.z());
    tmp_v.set(x, y, z);
    //printf("tmp1: %f %f %f ", tmp_v.x(),tmp_v.y(),tmp_v.z());
    return SDF(&tmp_v);
    //return sqrt(x*x+y*y+z*z)-sqrt(3.0)*0.5;
}

/**************IO function********************/

void read(FILE *f)
{
    char nextChar;

    mesh = new Mesh();
    mesh->clear();

    listTriangles = new list<Triangle*>;
   // listVertices = new list<Vertex*>;
    vertexIndex.clear();

    while ((nextChar=fgetc(f)) != EOF)
    {
      switch (nextChar)
      {
      	  case '#': // Comment
      		  do    // Reads till end of the line
      		  {
      			  nextChar=fgetc(f);
      		  }
      		  while (nextChar != EOF && nextChar != '\n');
      		  break;

          case 'v': case 'V': // Vertex format: v x y z
        	  nextChar=fgetc(f);
        	  if (nextChar == ' '){
        		  if (fscanf(f," %f %f %f\n",&x,&y,&z) != 3)
        		  	  	  return;

                  // x y z are the 3D coordinates of vertex v
        		  v = new Vertex(x,y,z);
        		  mesh->addVertex(v);
	  
        		  // save current vertex into a vector for
        		  // further use of triangles
        		  vertexIndex.push_back(v);
        	  } else {
        		  do    // Reads till end of the line
        		  {
        			  nextChar=fgetc(f);
        		  } while (nextChar != EOF && nextChar != '\n');
        	  }
        	  break;

          /*case 'f': case 'F': // Triangle format: f p1 p2 p3
        	  nextChar=fgetc(f);
        	  if (nextChar == ' '){
        		  if (fscanf(f," %d %d %d\n",&p1,&p2,&p3) != 3)
        			  return;

        		  //add all 3 edges into the mesh
        		  e = new Edge(vertexIndex[p1-1], vertexIndex[p2-1]);
        		  mesh->addEdge(e);


        		  e = new Edge(vertexIndex[p1-1], vertexIndex[p3-1]);
        		  mesh->addEdge(e);


        		  e = new Edge(vertexIndex[p2-1], vertexIndex[p3-1]);
        		  mesh->addEdge(e);


                  // p1 p2 p3 are the vertex index of triangle tri
        		  tri = new Triangle(vertexIndex[p1-1], vertexIndex[p2-1], vertexIndex[p3-1]);

        		  //add this triangle into mesh's triangle list
        		  mesh->addTriangle(tri);

        		  // save in a list for the triangle
        		  listTriangles->push_back(tri);
        	  	  }else{
        	  		  do    // Reads till end of the line
        	  		  {
        	  			  nextChar=fgetc(f);
        	  		  } while (nextChar != EOF && nextChar != '\n');

        	  	  }

        	  break;*/
      }
    }

    //mesh->sortEdges();
}

void write(FILE *f){

	  list<Triangle*>::iterator it;
	  list<Vertex*>::iterator iv;
	  list<Vertex*> *vertices = mesh->getVertices();
	  list<Triangle*> *triangles = mesh->getTriangles();

	  // Write the comment for vertices number and triangles number
	  fprintf(f, "# %d %d\n", vertices->size(), mesh->getTriangles()->size());

      // Write coordinates of each vertex
	  for (iv = vertices->begin(); iv != vertices->end(); iv++)

	      fprintf(f,"v %f %f %f\n",(double)((*iv)->floatData()[0]),
	    		  (double)((*iv)->floatData()[1]),(double)((*iv)->floatData()[2]));

	  // Write the vertices index of each triangle
	  for (it = triangles->begin(); it != triangles->end(); it++)

		  fprintf(f,"f %d %d %d\n", (*it)->vertices[0]->index,
				  (*it)->vertices[1]->index, (*it)->vertices[2]->index);

}

/********* GLUI callback control function ************/

void control_cb( int control )
{
	// When the OPEN button is pressed
	if (control == OPEN_ID)
	{   showEdges = false;
		filename = edittext->get_text();
		FILE *f;
		if ((f = fopen(filename,"r")) == NULL)
		{   // If the file is invalid, ignore it
			isFileRead=false;
		}
		else{ //Otherwise read it and flag
			read(f);
			fclose(f);
			isFileRead=true;
		}
	}
	// When the SAVE button is pressed
	else if (control == SAVE_ID)
	{
        if (mesh != NULL){
        	FILE *f;
        	f = fopen("saved.smf","wt");
        	write(f);
        	fclose(f);

        }
	}
    // When the mode list is changed
	else if (control == MODE_ID){
		option = listbox->get_int_val();
		switch (option)
		{
		case 0:
			displayMode = 1; // flat shaded
			break;
		case 1:
			displayMode = 2; // wireframe
			break;
		case 2:
			displayMode = 3; // shaded with wireframe
			break;
		};
	}

	// Compute centroid Oi for Nbr(k, xi)
	else if ( control == OI_ID) {

		list<Vertex*>* vertices;
		vertices = mesh->getVertices();

		list<Vertex*>::iterator iv1;

        time_t mytime = time(0);
        printf("\nStart time: %s\n", asctime(localtime(&mytime)));

        thre = -1;
		for (iv1=vertices->begin(); iv1 != vertices->end(); iv1++)
		{
			list<Vertex*>::iterator iv2;
			list<Vertex*> kNNbr;
			kNNbr.clear();
			//printf("before:%d\n", kNNbr.size());fflush(stdout);
			/*for (iv2=vertices->begin(); iv2 != vertices->end(); iv2++)
			{
				(*iv2)->disc = distance(*iv1,*iv2);
				if (kNNbr.size() < K_NEAREST_NBR){
					kNNbr.push_back((*iv2));

				}
				else{
					list<Vertex*>::iterator iv3;
					for (iv3 = kNNbr.begin() ; iv3 != kNNbr.end(); iv3++){
                        // replace the point in the set with a new point if that new point has less
						// distance to Xi
						if ((*iv2)->disc < (*iv3)->disc){
                            // might be wrong here! warning !!!!!!!!!
							*iv3 = *iv2;
							break;
						}
					}
				}
			}*/
			for (iv2=vertices->begin(); iv2 != vertices->end(); iv2++)
				(*iv2)->disc = distance((*iv1),(*iv2));

			vector<Vertex*> temp(vertices->begin(), vertices->end());
			sort(temp.begin(),temp.end(),sortVertices);
			if (thre < temp[1]->disc)
				thre = temp[1]->disc;

			//printf("first: %f ", temp[0]->disc);
			for (int i=1;i<K_NEAREST_NBR+1;i++){
				//printf("next 15: %f ", temp[i]->disc);
					kNNbr.push_back(temp[i]);


			}
			//printf("knn size: %d ",kNNbr.size());
            // save the final set of k nearest neighbor to Xi
            (*iv1)->kNNbr = kNNbr;

            // compute the centroid oi of k nearest neighbor
            list<Vertex*>::iterator iv3;
            float oix =0,oiy=0,oiz=0;

			for (iv3 = kNNbr.begin() ; iv3 != kNNbr.end(); iv3++){

				oix += (*iv3)->x();
				oiy += (*iv3)->y();
				oiz += (*iv3)->z();
			}
			Vertex* oi = new Vertex(oix/K_NEAREST_NBR,oiy/K_NEAREST_NBR,oiz/K_NEAREST_NBR);
			oi->index = (*iv1)->index;


            // PCA compute the normal of oi
            int m = K_NEAREST_NBR;
            int n = 3;
            MatrixXd DataPoints(m,n);
            double mean;
            VectorXd meanVector;
            int row = 0;
            typedef pair<double, int> myPair;
            typedef vector<myPair> PermutationIndices;

            for (iv3 = kNNbr.begin() ; iv3 != kNNbr.end(); iv3++){
            	DataPoints(row,0) = (*iv3)->x();
				DataPoints(row,1) = (*iv3)->y();
				DataPoints(row,2) = (*iv3)->z();
				row++;
            }
            //printf("row %d", row);

            for (int i = 0; i < DataPoints.cols(); i++){
            	   mean = (DataPoints.col(i).sum())/m;		 //compute mean
            	   meanVector  = VectorXd::Constant(m,mean); // create a vector with constant value = mean
            	   DataPoints.col(i) -= meanVector;
            	  // std::cout << meanVector.transpose() << "\n" << DataPoints.col(i).transpose() << "\n\n";
              }

              // get the covariance matrix
              MatrixXd Covariance = MatrixXd::Zero(n, n);
             // MatrixXd Covariance1 = MatrixXd::Zero(n, n);
              Covariance =  (1/(double)n)* DataPoints.transpose() * DataPoints ;
            //  std::cout << Covariance ;
             /* for (iv3 = kNNbr.begin() ; iv3 != kNNbr.end(); iv3++){
                   float x = (*iv3)->x()-oi->x();
                   float y = (*iv3)->y()-oi->y();
                   float z = (*iv3)->z()-oi->z();
                   Covariance1(0,0) += x*x;
                   Covariance1(0,1) += x*y;
                   Covariance1(0,2) += x*z;
                   Covariance1(1,0) += y*x;
                   Covariance1(1,1) += y*y;
                   Covariance1(1,2) += y*z;
                   Covariance1(2,0) += z*x;
                   Covariance1(2,1) += z*y;
                   Covariance1(2,2) += z*z;
                          }*/
             // Covariance1 = (1/(double)n)*Covariance1;

              /*for(int i=0;i<n;i++)
            	  for(int j=0;j<n;j++){
            		  printf("c[%d][%d]:%f ", i,j,Covariance(i,j));
            		 // printf("c1: %f", Covariance1(i,j));
            	  }*/

              // compute the eigenvalue on the Cov Matrix
              EigenSolver<MatrixXd> m_solve(Covariance);

              VectorXd eigenvalues = VectorXd::Zero(n);
              eigenvalues = m_solve.eigenvalues().real();

              MatrixXd eigenVectors = MatrixXd::Zero(n, n);  // matrix (n x m) (points, dims)
              eigenVectors = m_solve.eigenvectors().real();

              // sort and get the permutation indices
              PermutationIndices pi;
              for (int i = 0 ; i < n; i++)
            	  pi.push_back(std::make_pair(eigenvalues(i), i));

              sort(pi.begin(), pi.end());
             // printf("start");
/*
              for (unsigned int i = 0; i < n ; i++){

            	  printf("eigen=%f" , pi[i].first );
					printf(" pi= %d" ,pi[i].second );
				    printf(" v[%d] =[%f %f %f]\n",i, eigenVectors.col(pi[i].second)(0),
							 eigenVectors.col(pi[i].second)(1),
							 eigenVectors.col(pi[i].second)(2));
              }
*/
              MathWork* ni = new MathWork();
              ni->value[0] = eigenVectors.col(pi[0].second)(0);
              ni->value[1] = eigenVectors.col(pi[0].second)(1);
              ni->value[2] = eigenVectors.col(pi[0].second)(2);

             /* ni->value[0]=oi->x();
              ni->value[1]=oi->y();
              ni->value[2]=oi->z();*/
            	  //printf("nx=%f ny=%f nz=%f \n" , ni->value[0],ni->value[1],ni->value[2] );

            //	  printf("end");

              oi->setNormal(ni);
              tangentPlnCen.push_back(oi);
		}
		printf("tangentPlnCen size:%d ", tangentPlnCen.size());fflush(stdout);

		// compute the k nearest neighbor of oi

		for (iv1=tangentPlnCen.begin(); iv1 != tangentPlnCen.end(); iv1++)
		{
			list<Vertex*>::iterator iv2;
			list<Vertex*> kNNbr;

			for (iv2=tangentPlnCen.begin(); iv2 != tangentPlnCen.end(); iv2++){
				(*iv2)->disc = distance((*iv1),(*iv2));
				/*if ((*iv2)->index < (*iv1)->index){
					float disc = distance((*iv1),(*iv2));
					e = new Edge(*iv1,*iv2);
					e->weight = disc;
					mesh->addEdge(e);
				}*/

			}
			vector<Vertex*> temp(tangentPlnCen.begin(), tangentPlnCen.end());
			sort(temp.begin(),temp.end(),sortVertices);

			//printf("first: %f ", temp[0]->disc);
			for (int i=1;i<K_NEAREST_NBR+1;i++){
				//printf("next 15: %f ", temp[i]->disc);
					kNNbr.push_back(temp[i]);


			}

			for (iv2=kNNbr.begin(); iv2 != kNNbr.end(); iv2++){
				float disc = distance((*iv1),(*iv2));
								e = new Edge(*iv1,*iv2);
								e->weight = disc;
								mesh->addEdge(e);


						}

			// save the final set of k nearest neighbor to Oi
			(*iv1)->kNNbr = kNNbr;

		}




		// MST based on the weight as Euclidean distance (EMST)

		vector<Vertex*> m_vertexs(tangentPlnCen.begin(),tangentPlnCen.end());
		vector<Edge*> m_edges(mesh->getEdges()->begin(), mesh->getEdges()->end());


		sort (m_edges.begin(),m_edges.end(), sortFunction);
		printf("begin size:%d ", m_edges.size());fflush(stdout);

		DisjointSet<Vertex> dv;
		dv.makeSet(m_vertexs);
		vector<Edge*>::iterator it = m_edges.begin();

		for (;it!= m_edges.end();++it)
		{
			Vertex p1;
			Vertex p2;
			bool b1 = dv.findSet((*it)->vertices[0], &p1 );
			bool b2 = dv.findSet((*it)->vertices[1], &p2 );
			//printf("p1: %d\n", p1.index);fflush(stdout);
			//printf("p2: %d\n", p2.index);fflush(stdout);
			if ( b1&& b2 && p1.index != p2.index)
			{
				dv.Union(&p1, &p2);
				result.push_back(*it);
				//printf("ow: %f ",(*it)->weight );fflush(stdout);
			}

		}
		printf("result size:%d ", result.size());fflush(stdout);

		// Riemannian graph
		for (iv1=tangentPlnCen.begin(); iv1 != tangentPlnCen.end(); iv1++)
		{
			list<Vertex*>::iterator iv2;
			for (iv2=(*iv1)->kNNbr.begin(); iv2 != (*iv1)->kNNbr.end(); iv2++)
			{
				e = new Edge((*iv1),(*iv2));
				result.push_back(e);

			}
		}
		printf("result1 size:%d ", result.size());fflush(stdout);
        Mesh::sortEdges(&result);
		printf("result12 size:%d ", result.size());fflush(stdout);

		// recompute the weight of each edge (i,j) in Riemannian Graph
		// now the weight is 1-|ni dotproduct nj|
        list<Edge*>::iterator ie;
        for (ie = result.begin() ; ie != result.end() ; ie++){
        	float result = 0.0;
        	MathWork::dotProduct((*ie)->vertices[0]->floatNormal(),3,
        			             (*ie)->vertices[1]->floatNormal(),
        			             &result);
        //	printf("ow: %f ", (*ie)->weight);
        	(*ie)->weight = 1 - fabs(result);
        //	printf("nw: %f ", (*ie)->weight);
        }

        // find the MST of the Riemannian Graph based on the new weight

        vector<Edge*> Rieman(result.begin(), result.end());


		sort (Rieman.begin(),Rieman.end(), sortFunction);
		printf("begin size:%d ", Rieman.size());fflush(stdout);

		DisjointSet<Vertex> dv1;
		dv1.makeSet(m_vertexs);
		vector<Edge*>::iterator ie1 = Rieman.begin();

		for (;ie1!= Rieman.end();++ie1)
		{
			Vertex p1;
			Vertex p2;
			bool b1 = dv1.findSet((*ie1)->vertices[0], &p1 );
			bool b2 = dv1.findSet((*ie1)->vertices[1], &p2 );
			//printf("p1: %d\n", p1.index);fflush(stdout);
			//printf("p2: %d\n", p2.index);fflush(stdout);
			if ( b1&& b2 && p1.index != p2.index)
			{
				dv1.Union(&p1, &p2);
				result1.push_back(*ie1);
				//update the adjacent relation to both end of this edge
				(*ie1)->vertices[0]->addEdge((*ie1));
				(*ie1)->vertices[1]->addEdge((*ie1));

				//printf("ow: %f ",(*it)->weight );fflush(stdout);
			}

		}
		printf("result1 size:%d ", result1.size());fflush(stdout);

		// find the oi with max z value
		iv1=tangentPlnCen.begin();
		Vertex* oiWithMaxZ = (*iv1);

		for (iv1=tangentPlnCen.begin(); iv1 != tangentPlnCen.end(); iv1++)
		{
			if ((*iv1)->z() > oiWithMaxZ->z())
				oiWithMaxZ = *iv1;
		}

		// force the normal to point to z+
		float result = 0.0;
		MathWork::dotProduct(oiWithMaxZ->floatNormal(),3,
								oiWithMaxZ->floatData(), &result);
		if (result < 0)
			oiWithMaxZ->getNormal()->opposit();

		//printf("maxZ: %f ", oiWithMaxZ->z());

		// traverse the MST tree with root oi(with max z) to propagate normal

		int oiSize = tangentPlnCen.size();

		bool *visited = new bool[oiSize];
		for( int i = 0; i < oiSize; i++)  // mark all oi as unvisited
			visited[i] = false;

		DFS(oiWithMaxZ, visited);

		for(iv1 = tangentPlnCen.begin(); iv1!= tangentPlnCen.end(); iv1++) {
			Vertex* normalV = new Vertex((*iv1)->x()+0.05*(*iv1)->floatNormal()[0],
					(*iv1)->y()+0.05*(*iv1)->floatNormal()[1],
					(*iv1)->z()+0.05*(*iv1)->floatNormal()[2]);
			/*for(int i=0;i<3;i++)
				printf("n[%d]:%f ",i,(*iv1)->floatNormal()[i]);*/
			Edge* normal = new Edge(*iv1, normalV);
			testNormal.push_back(normal);

		}

		// signed distance

        mesh->setMinMaxValues();
        float bound[2][3];
        bound[0][0] = mesh->XMin()*1.2;
        bound[0][1] = mesh->YMin()*1.2;
        bound[0][2] = mesh->ZMin()*1.2;
        bound[1][0] = mesh->XMax()*1.2;
        bound[1][1] = mesh->YMax()*1.2;
        bound[1][2] = mesh->ZMax()*1.2;
        Mesh *new_mesh;


        int rc = marchingcube(bound[0], bound[1], 6, isolvl_fn, 0.0, &new_mesh);

        if(rc){
            free(new_mesh);
        } else {
            free(mesh);
            mesh = new_mesh;
        }

        showEdges = true;

        mytime = time(0);
        printf("\nEnd time: %s\n", asctime(localtime(&mytime)));

	}

    // Control the movable light
	else if ( control == LIGHT0_ENABLED_ID ) {
		if ( light0_enabled ) {
			glEnable( GL_LIGHT0 );
			light0_spinner->enable();
		}
		else {
			glDisable( GL_LIGHT0 );
			light0_spinner->disable();
		}
	}
	// Control the fixed light
	else if ( control == LIGHT1_ENABLED_ID ) {
		if ( light1_enabled ) {
			glEnable( GL_LIGHT1 );
		}
		else {
			glDisable( GL_LIGHT1 );
		}
	}
	// Control the movable light setting
	else if ( control == LIGHT0_INTENSITY_ID ) {
		float v[] = {
				light0_diffuse[0],  light0_diffuse[1],
				light0_diffuse[2],  light0_diffuse[3] };
    
		v[0] *= light0_intensity;
		v[1] *= light0_intensity;
		v[2] *= light0_intensity;

		glLightfv(GL_LIGHT0, GL_DIFFUSE, v );
	}
	// Control the fixed light setting
	else if ( control == LIGHT1_INTENSITY_ID ) {
		float v[] = {
				light1_diffuse[0],  light1_diffuse[1],
				light1_diffuse[2],  light1_diffuse[3] };
    
		v[0] *= light1_intensity;
		v[1] *= light1_intensity;
		v[2] *= light1_intensity;

		glLightfv(GL_LIGHT1, GL_DIFFUSE, v );
	}

}

/********* myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;
  };
  
  glutPostRedisplay();
}

/*****display points  *****/
/*** Only for testing and for point base presenting***/

void displayPoints()
{
	list<Vertex*>::iterator iv;

	list<Vertex*>* vertices = mesh->getVertices();
	for (iv=tangentPlnCen.begin(); iv != tangentPlnCen.end(); iv++)
    	{

		glBegin(GL_POINTS);

        if ( (*iv)->floatNormal() != NULL )
        	glNormal3fv( (GLfloat*) (*iv)->floatNormal() );

        glVertex3fv( (GLfloat*) (*iv)->floatData() );
        glEnd();
     }
}

void displayEdges()
{
	list<Edge*>::iterator ie;


		for (ie=result1.begin(); ie != result1.end(); ie++)
	    	{

			glBegin(GL_LINES);

	        /*if ( (*ie)->vertices[0]->floatNormal() != NULL )
	        	glNormal3fv( (GLfloat*) (*ie)->vertices[0]->floatNormal() );*/
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv( (GLfloat*) (*ie)->vertices[0]->floatData() );
			/*if ( (*ie)->vertices[1]->floatNormal() != NULL )
				glNormal3fv( (GLfloat*) (*ie)->vertices[1]->floatNormal() );*/
			glVertex3fv( (GLfloat*) (*ie)->vertices[1]->floatData() );

	        glEnd();
	     }

}

/*** Mesh rendering  ***/

void displayMesh(){

	list<Triangle*>::iterator it;
	list<Triangle*> *triangles;

	// Fetch tiangle list from storage mesh object
	triangles = mesh->getTriangles();

	if (triangles != NULL)
    {
        switch(displayMode){

		    case 1://flat shaded
			      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			      for (it=triangles->begin(); it != triangles->end(); it++)
			      {
			    	  glBegin(GL_TRIANGLES);
                      // Fetch the triangle surface normal
			    	  glNormal3fv( (GLfloat*) (*it)->floatNormal() );

			    	  glVertex3fv( (GLfloat*) (*it)->vertices[0]->floatData() );

			    	  glVertex3fv( (GLfloat*) (*it)->vertices[1]->floatData() );

			    	  glVertex3fv( (GLfloat*) (*it)->vertices[2]->floatData() );

			    	  glEnd();
			      }
			      break;

		    case 2://wireframe
		    	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			    for (it=triangles->begin(); it != triangles->end(); it++)
			    {
			    	glBegin(GL_TRIANGLES);
                    // Neither surface nor vertex normal needed
			    	glVertex3fv( (GLfloat*) (*it)->vertices[0]->floatData() );

			    	glVertex3fv( (GLfloat*) (*it)->vertices[1]->floatData() );

			    	glVertex3fv( (GLfloat*) (*it)->vertices[2]->floatData() );

			    	glEnd();
			  }
			  break;

		    case 3://shaded with edges
		    	for (it=triangles->begin(); it != triangles->end(); it++)
		    	{
		    		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	                // Render the solid triangles first
					glBegin(GL_TRIANGLES);

					glNormal3fv( (GLfloat*) (*it)->floatNormal() );

					glVertex3fv( (GLfloat*) (*it)->vertices[0]->floatData() );

					glVertex3fv( (GLfloat*) (*it)->vertices[1]->floatData() );

					glVertex3fv( (GLfloat*) (*it)->vertices[2]->floatData() );

					glEnd();

					// Render the wireframe second
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
					// Disable lighting
					glDisable( GL_LIGHTING );
					// Enable line offset
					glEnable(GL_POLYGON_OFFSET_LINE);
					// Set the line width 1 and color black
					glLineWidth(1);
					glColor4f(0.0,0.0,0.0, 0.0);
					// Draw the wireframe towards the screen a little bit
					glPolygonOffset(-1, -1);

					glBegin(GL_TRIANGLES);

					glVertex3fv( (GLfloat*) (*it)->vertices[0]->floatData() );

					glVertex3fv( (GLfloat*) (*it)->vertices[1]->floatData() );

					glVertex3fv( (GLfloat*) (*it)->vertices[2]->floatData() );

					glEnd();
					// Restore the line offset and light setting
					glDisable(GL_POLYGON_OFFSET_LINE);
					glEnable( GL_LIGHTING );

		    	}
		    	break;
		    default:
			    break;
        };
    }
}

/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
  myGlutKeyboard( value, 0, 0 );
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
  /*** Force GLUT to bind with main window ***/
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  

  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{ // Full control by GLUI. Mouse disabled.
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
  glutPostRedisplay(); 
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  glViewport( tx, ty, tw, th );

  xy_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}



/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
	glClearColor( .9f, .9f, .9f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xy_aspect*.04, xy_aspect*.04, -.04, .04, .1, 15.0 );

	glMatrixMode( GL_MODELVIEW );

	glLoadIdentity();
	glMultMatrixf( lights_rotation );
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -2.6f );
	glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] );
	glMultMatrixf( view_rotate );

	glScalef( scale, scale, scale );

	/*** render the mesh object ***/

	glPushMatrix();

	glTranslatef( 0.0, 0.0, 0.0 );
	glMultMatrixf( obj_rotate );

	// Display mesh only a valid file is loaded
    if(isFileRead){
    	if (!showEdges)
    		displayPoints();
    	else
    		displayMesh();
    }
    glPopMatrix();
    // Force re-render the object in case a new file is loaded
    //glutPostRedisplay(); //warning!!!!!!!!!!!!

    if ( show_text )
    { // Disable lighting while we render text
		glDisable( GL_LIGHTING );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glColor3ub( 0, 0, 0 );
		glRasterPos2i( 10, 10 );

    /*** Render the live character array 'text' ***/
    //int i;
	//for( i=0; i<(int)strlen( string_list[curr_string] ); i++ )
	//	glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string_list[curr_string][i] );
	//
    char buf[30];
    if (mesh != NULL){
       sprintf(buf,"edge: %d",mesh->getEdges()->size());
       //sprintf(buf,"testQ: %f", vertexIndex[0]->getQ()->v[0][1]);
       for(int i=0; i<(int)strlen( buf); i++ )
    		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, buf[i] );

    }
    }
    // Enable the light after text
    glEnable( GL_LIGHTING );

    glutSwapBuffers();
}


/**************************************** main() ********************/

int main(int argc, char* argv[])
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );


  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 800, 650 );
 
  main_window = glutCreateWindow( "SMF Viewer -- Final Project" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );

  /****************************************/
  /*       Set up OpenGL lights           */
  /****************************************/

  glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*Enable normalization and shadow smooth*/
  /****************************************/

  glEnable( GL_NORMALIZE );
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

  /****************************************/
  /*          GLUI configure code         */
  /****************************************/

  printf( "GLUI version: %3.2f\nRui Yang 764 Assignment 2", GLUI_Master.get_version() );

  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_RIGHT );


  /**** Add file read/write panel ****/
  //GLUI_Rollout *roll_file = new GLUI_Rollout(glui, "File Operation", false );
  GLUI_Panel *readWrite_panel = new GLUI_Panel( glui, ".SMF File" );
  edittext = new GLUI_EditText( readWrite_panel, "Filename:");
  
  new GLUI_Button( readWrite_panel, "Open", OPEN_ID, control_cb );
  new GLUI_Button( readWrite_panel, "Save", SAVE_ID, control_cb );
  new GLUI_Button( readWrite_panel, "Quit", 0,(GLUI_Update_CB)exit );

  /**** Add mesh displaying mode ****/
    GLUI_Panel *view_panel = new GLUI_Panel( glui, "View" );
    new GLUI_StaticText( glui, "" );

    listbox = new GLUI_Listbox( view_panel, "Mode:", &curr_string, MODE_ID, control_cb );
    int i;
    for( i=0; i<3; i++ )
      listbox->add_item( i, string_list[i] );

    new GLUI_StaticText( glui, "" );

    /*surface construction step*/
    GLUI_Panel *pbr_panel = new GLUI_Panel( glui, "PBR" );
    new GLUI_Button( pbr_panel, "Oi", OI_ID, control_cb );

  /****** Add camera control panel *****/

  GLUI_Panel *camera_panel = new GLUI_Panel( glui, "Camera" );

  GLUI_Translation *trans_z =
      new GLUI_Translation( camera_panel, "Zooming", GLUI_TRANSLATION_Z, &obj_pos[2] );
    trans_z->set_speed( .015 );

  GLUI_Translation *trans_xy =
      new GLUI_Translation(camera_panel, "XY Translation", GLUI_TRANSLATION_XY, obj_pos );
    trans_xy->set_speed( .005 );

  GLUI_Rotation *obj_rot = new GLUI_Rotation(camera_panel, "Rotation", obj_rotate );
    obj_rot->set_spin( .98 );


  /******** Add some controls for lights ********/

  GLUI_Panel *light_panel = new GLUI_Panel(glui, "Light" );

  new GLUI_Checkbox( light_panel, "Enabled movable light", &light0_enabled,
					 LIGHT0_ENABLED_ID, control_cb );
  new GLUI_Checkbox( light_panel, "Enabled fixed light", &light1_enabled,
					   LIGHT1_ENABLED_ID, control_cb );
  light0_spinner =
	new GLUI_Spinner( light_panel, "Intensity:",
					  &light0_intensity, LIGHT0_INTENSITY_ID,
					  control_cb );
  light0_spinner->set_float_limits( 0.0, 1.0 );
  GLUI_Scrollbar *light_control;
  light_control = new GLUI_Scrollbar( light_panel, "Red",GLUI_SCROLL_HORIZONTAL,
						   &light0_diffuse[0],LIGHT0_INTENSITY_ID,control_cb);
  light_control->set_float_limits(0,1);
  light_control = new GLUI_Scrollbar( light_panel, "Green",GLUI_SCROLL_HORIZONTAL,
						   &light0_diffuse[1],LIGHT0_INTENSITY_ID,control_cb);
  light_control->set_float_limits(0,1);
  light_control = new GLUI_Scrollbar( light_panel, "Blue",GLUI_SCROLL_HORIZONTAL,
						   &light0_diffuse[2],LIGHT0_INTENSITY_ID,control_cb);
  light_control->set_float_limits(0,1);

  GLUI_Rotation *lights_rot = new GLUI_Rotation(light_panel, "Light", lights_rotation );


  /**** Link GLUI to the GLUT main window ******/
  
  glui->set_main_gfx_window( main_window );


#if 0
  /****  idle callback with GLUI ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
#endif

  /****  GLUT main loop ****/
  
  glutMainLoop();

  return EXIT_SUCCESS;
}

