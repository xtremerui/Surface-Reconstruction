/*
 *------------------------------------------------------------------------
 *
 *  mc_test.cpp
 *  
 *  This file contains Marching Cubes Unit Test implementations.
 *
 *  Created by Zhuoli Jiang   Apr 12, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#include <cmath>

#include "marchingcubes.h"

/* a ball of radius=2 */
float isolvl_fn (float x, float y, float z){
    float t = sqrt(x * x + y * y + z * z) - 2.0*sqrt(3.0);

    return t;
}

int main(int argc, char *argv[]){
    float bound[2][3];
    Mesh *result;
    int rc;

    bound[0][0] = -3.0;
    bound[0][1] = -3.0;
    bound[0][2] = -3.0;
    bound[1][0] = 3.0;
    bound[1][1] = 3.0;
    bound[1][2] = 3.0;

    rc = marchingcube(bound[0], bound[1], 5, isolvl_fn, 0.0, &result);

    //FIXME: output mesh
    return 0;
}

