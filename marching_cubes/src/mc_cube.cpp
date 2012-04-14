/*
 *------------------------------------------------------------------------
 *
 *  mc_cube.cpp
 *  
 *  This file contains Marching Cubes Cube implementations.
 *
 *  Created by Zhuoli Jiang   Apr 13, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#include "mc_cube.h"

#include <string.h>
#include <stdlib.h>


/*
 *---------------------------------------------------------
 *
 *  Functions
 *
 *---------------------------------------------------------
 */

void cube_init(Cube* cube,
        float lower_bound[3],
        float upper_bound[3],
        uint32_t level)
{
    memcpy(cube->lower_bound, lower_bound, sizeof(float)*3);
    memcpy(cube->upper_bound, upper_bound, sizeof(float)*3);
    cube->level = level;
    cube->cube_index = 0;
}

Cube* cube_create(float lower_bound[3],
        float upper_bound[3],
        uint32_t level)
{
    Cube* cube;

    cube = (Cube*)malloc(sizeof(Cube));
    if(cube){
        cube_init(cube, lower_bound, upper_bound, level);
    }

    return cube;
}

void cube_destroy(Cube *cube){
    free(cube);
}


