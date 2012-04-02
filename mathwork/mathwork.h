/*** Rui Yang 764 Assignment 2 SMF Viewer with decimation***/
/*** mathwork.h ***/

#ifndef _MATHWORK_H
#define _MATHWORK_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

class MathWork
{
public:
	MathWork();

	void scale(double scale);
	void normalise(void);
    static void add(const MathWork* m1, MathWork* m2);
    static int matrixInverse(float *a,int n);
    static void matrixMul(float* m1, int m, int n, int k, float* m2, float* result);
	float value[3];
	float v[4][4];
};

#endif
