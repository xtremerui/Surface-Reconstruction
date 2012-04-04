/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation ***/
/*** mathwork.cpp ***/

#include "mathwork.h"


MathWork::MathWork(){
	value[0] = value[1] = value[2] = 0.0;
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			v[i][j] = 0.0;
}

void MathWork::normalise()
{
	scale(1.0/(sqrt((double) value[0]*value[0] +
			        (double) value[1]*value[1] +
			        (double) value[2]*value[2])));
}

void MathWork::scale(double scale)
{
	value[0]*=scale;
	value[1]*=scale;
	value[2]*=scale;
}

void MathWork::add(const MathWork* m1, MathWork* m2)
{
	for(int i = 0;i<4;i++){
			for(int j = 0;j<4;j++)
				m2->v[i][j] += m1->v[i][j];
		}
}

int MathWork::matrixInverse(float *a,int n)
{
	 int *is,*js,i,j,k,l,u,v;
	    float d,p;
	    is=(int*)malloc(n*sizeof(int));
	    js=(int*)malloc(n*sizeof(int));
	    for (k=0; k<=n-1; k++)
	      { d=0.0;
	        for (i=k; i<=n-1; i++)
	        for (j=k; j<=n-1; j++)
	          { l=i*n+j; p=fabs(a[l]);
	            if (p>d) { d=p; is[k]=i; js[k]=j;}
	          }
	        if (d+1.0==1.0)
	          { free(is); free(js);
	            // a is not invertible
	            return(0);
	          }
	        if (is[k]!=k)
	          for (j=0; j<=n-1; j++)
	            { u=k*n+j; v=is[k]*n+j;
	              p=a[u]; a[u]=a[v]; a[v]=p;
	            }
	        if (js[k]!=k)
	          for (i=0; i<=n-1; i++)
	            { u=i*n+k; v=i*n+js[k];
	              p=a[u]; a[u]=a[v]; a[v]=p;
	            }
	        l=k*n+k;
	        a[l]=1.0/a[l];
	        for (j=0; j<=n-1; j++)
	          if (j!=k)
	            { u=k*n+j; a[u]=a[u]*a[l];}
	        for (i=0; i<=n-1; i++)
	          if (i!=k)
	            for (j=0; j<=n-1; j++)
	              if (j!=k)
	                { u=i*n+j;
	                  a[u]=a[u]-a[i*n+k]*a[k*n+j];
	                }
	        for (i=0; i<=n-1; i++)
	          if (i!=k)
	            { u=i*n+k; a[u]=-a[u]*a[l];}
	      }
	    for (k=n-1; k>=0; k--)
	      { if (js[k]!=k)
	          for (j=0; j<=n-1; j++)
	            { u=k*n+j; v=js[k]*n+j;
	              p=a[u]; a[u]=a[v]; a[v]=p;
	            }
	        if (is[k]!=k)
	          for (i=0; i<=n-1; i++)
	            { u=i*n+k; v=i*n+is[k];
	              p=a[u]; a[u]=a[v]; a[v]=p;
	            }
	      }
	    free(is); free(js);
	    return(1);


}

// m1[m][n], m2[n][k], result[m][k]
void MathWork::matrixMul(float* m1, int m, int n, int k, float* m2, float* result)
{
	int i,j,l,u;
		for (i=0; i<=m-1; i++){

		    for (j=0; j<=k-1; j++)
		      {
		    	u=i*k+j;
		    	result[u]=0.0;

		        for (l=0; l<=n-1; l++){
		        	result[u]=result[u]+m1[i*n+l]*m2[l*k+j];


		        }
		      }
		}

}

void MathWork::dotProduct(const float* m1, int n, const float* m2, float* result)
{
	for (int i = 0;i<n;i++)
		*result = *result + m1[i]*m2[i];

}

void MathWork::opposit()
{
	value[0] = -value[0];
	value[1] = -value[1];
	value[2] = -value[2];
}
