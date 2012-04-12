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

#include <stdlib>

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
compute_cube_isolvl(Cube *cube, isolvl_function isolvl_fn)
{
    float isolvl[8];
    float v_pos[2][3];
    uint8_t cube_index = 0;

    /* compute grid coordinates */
    memcpy(v_pos[0], cube->lower_bound, sizeof(float)*3);
    memcpy(v_pos[1], cube->upper_bound, sizeof(float)*3);

    /* compute isolvl(signed distance) */
    isolvl[0] = isolvl_fn(v_pos[0][0], v_pos[0][1], v_pos[0][2]);
    isolvl[1] = isolvl_fn(v_pos[1][0], v_pos[0][1], v_pos[0][2]);
    isolvl[2] = isolvl_fn(v_pos[1][0], v_pos[1][1], v_pos[0][2]);
    isolvl[3] = isolvl_fn(v_pos[0][0], v_pos[1][1], v_pos[0][2]);
    isolvl[4] = isolvl_fn(v_pos[0][0], v_pos[0][1], v_pos[1][2]);
    isolvl[6] = isolvl_fn(v_pos[1][0], v_pos[0][1], v_pos[1][2]);
    isolvl[7] = isolvl_fn(v_pos[1][0], v_pos[1][1], v_pos[1][2]);
    isolvl[8] = isolvl_fn(v_pos[0][0], v_pos[1][1], v_pos[1][2]);

    /* compute cube index */
    cube_index = 0;
    for(int i = 0; i < 8; i++){
        if(cube->isolvl[i] < threshold){
            cube_index |= 1<<i;
        }
    }

    /* record result */
    memcpy(cube->isolvl, isolvl, sizeof(float)*8);
    cube->cube_index = cube_index;
    return;
}

static void
process_cube(
        Cube *cube, 
        uint32_t lvl_max, 
        isolvl_function isolvl_fn,
        float threshold, 
        Mesh* mesh)
{
    Cube *cube[2][2][2];
    int *tri_index;

    /* skip cubes that are completely inside/outside surface */
    if(cube->cube_index == 0 || cube->cube_index == 0xff){
        return;
    }

    /* compute triangle */
    if(cube->level == lvl_max){
        tri_index = tri_table[cube->cube_index];
        for(int i = 0; tri_index[i] != -1; i+=3){
            
        }
    }

    /* next level - subdivide cube into 8 smaller cubes */
    for(int x = 0; x < 2; x++){
        for(int y = 0; y < 2; y++){
            for(int z = 0; z < 2; z++){
                Cube *sub_cube;

                /* allocate memory*/
                sub_cube = (Cube *)malloc(sizeof(Cube));
                if(!sub_cube){
                    exit(-1);
                }

                /* populate cube */

                /* push into stack */
                SLIST_INSERT_HEAD(&cube_stack, cube, stack_entry);
            }
        }
    }
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
        isolvl_function isolvl_fn,
        float isolvl_threshold,
        Mesh **result)
{
    Cube *root_cube;

    if( !isolvl_fn || !result || (level == 0)){
        return -1;
    }

    /* initialize */
    mc_cbstack_init();

    /* create the first cube */
    root_cube = cube_create(lower_bound, upper_bound, 0);
    if(!root_cube){
        return -2;
    }
    memset(root_cube->cube_pos, 0, sizeof(uint64_t)*3);

    /* initialize mesh */
    *result = new Mesh();
    if(!(*result)){
        cube_destroy(root_cube);
        return -1;
    }
    
    /* push into cube */
    compute_cube_isolvl(root_cube, isolvl_fn);
    mc_cbstack_push(root_cube);

    /* main loop */
    while(!mc_cbstack_isempty()){
        process_cube(mc_cbstack_pop(), level, isolvl_fn, isolvl_threshold, result);
    }

    return 0;
}



