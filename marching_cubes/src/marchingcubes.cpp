/*
 *------------------------------------------------------------------------
 *
 *  marchingcubes.cpp
 *  
 *  This file contains Marching Cubes implementations.
 *
 *  Created by Zhuoli Jiang   Apr 08, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#include "marchingcubes.h"
#include "mc_tables.h"
#include "mc_cubestack.h"
#include "mc_cube.h"


/*
 *---------------------------------------------------------
 *
 *  Static Functions
 *
 *---------------------------------------------------------
 */

static void
process_cube(
        Cube *cube, 
        uint32_t lvl_max, 
        isolevel_function isolvl_fn,
        float threshold, 
        Mesh* mesh)
{
    
}

/*
 *---------------------------------------------------------
 *
 *  Functions
 *
 *---------------------------------------------------------
 */

int marchingcube(
        float lower_bound[3],
        float upper_bound[3],
        uint32_t level,
        isolevel_function isolevel_fn,
        float isolevel_threshold,
        Mesh **result)
{
    Cube *root_cube;

    if( !isolevel_fn || !result || (level == 0)){
        return -1;
    }

    /* initialize */
    mc_cbstack_init();

    /* create the first cube */
    root_cube = cube_create(lower_bound, upper_bound, 0);
    if(!root_cube){
        return -2;
    }

    /* initialize mesh */
    *result = new Mesh();
    if(!(*result)){
        cube_destroy(root_cube);
        return -1;
    }
    
    /* push into cube */
    mc_cbstack_push(root_cube);

    /* main loop */
    while(!mc_cbstack_isempty()){
        process_cube(cube_stack.lh_first, level, isolevel_fn, 
                isolevel_threshold, result);
    }

    return 0;
}



