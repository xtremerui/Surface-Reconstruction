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

#include <stdlib.h>

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
    int *tri_index;

    /* skip cubes that are completely inside/outside surface */
    if(cube->cube_index == 0 || cube->cube_index == 0xff){
        return;
    }

    /* compute triangle */
    if(cube->level == lvl_max){
        tri_index = tri_table[cube->cube_index];
        /* each triangle */
        for(int i = 0; tri_index[i] != -1; i+=3){
            float tri_pos[3][3];

            for(int j = i; j < i+3; j++){
                int *point = edge_table[tri_index[j]];
                float endpoint[2][3];
                float ep_isolvl[2];

                /* compute coordinate & iso-level of two endpoints */
                for(int k = 0; k < 2; k++){
                    int tmp = point[k] & 0x3;
                    if(tmp < 2){
                        endpoint[k][0] = cube->lower_bound[0];
                    } else {
                        endpoint[k][0] = cube->upper_bound[0];
                    }
                    if(tmp == 0 || tmp == 3){
                        endpoint[k][1] = cube->lower_bound[1];
                    } else {
                        endpoint[k][1] = cube->upper_bound[1];
                    }
                    if(point[k]<4){
                        endpoint[k][2] = cube->lower_bound[2];
                    } else {
                        endpoint[k][2] = cube->upper_bound[2];
                    }

                    ep_isolvl[k] = cube->isolvl[point[k]];
                    ep_isolvl[k] *= ep_isolvl[k]>0.0?1.0:(-1.0);
                }

                /* compute coordinate of triangle */
                for(int t = 0; t < 3; t++){
                    tri_pos[j-i][t] = ep_isolvl[1]*endpoint[0][t] + ep_isolvl[0]*endpoint[1][t];
                    tri_pos[j-i][t] /= ep_isolvl[0] + ep_isolvl[1];
                }
            }

            /* FIXME: add to mesh */
            mesh->addTriangle(new Triangle( 
                    new Vertex(tri_pos[0][0], tri_pos[0][1], tri_pos[0][2]), 
                    new Vertex(tri_pos[1][0], tri_pos[1][1], tri_pos[1][2]),
                    new Vertex(tri_pos[2][0], tri_pos[2][1], tri_pos[2][2])));
        }
    }

    /* next level - subdivide cube into 8 smaller cubes */
    float new_bound[2][3];
    for(int x = 0; x < 2; x++){
        if(x == 0){
            new_bound[0][0] = cube->lower_bound[0];
            new_bound[1][0] = (cube->lower_bound[0] + cube->upper_bound[0]) * 0.5;
        } else {
            new_bound[0][0] = (cube->lower_bound[0] + cube->upper_bound[0]) * 0.5;
            new_bound[1][0] = cube->upper_bound[0];
        }
        for(int y = 0; y < 2; y++){
            if(y == 0){
                new_bound[0][1] = cube->lower_bound[1];
                new_bound[1][1] = (cube->lower_bound[1] + cube->upper_bound[1]) * 0.5;
            } else {
                new_bound[0][1] = (cube->lower_bound[1] + cube->upper_bound[1]) * 0.5;
                new_bound[1][1] = cube->upper_bound[1];
            }

            for(int z = 0; z < 2; z++){
                Cube *sub_cube;

                if(z == 0){
                    new_bound[0][2] = cube->lower_bound[2];
                    new_bound[1][2] = (cube->lower_bound[2] + cube->upper_bound[2]) * 0.5;
                } else {
                    new_bound[0][2] = (cube->lower_bound[2] + cube->upper_bound[2]) * 0.5;
                    new_bound[1][2] = cube->upper_bound[2];
                }


                /* allocate memory*/
                sub_cube = (Cube *)malloc(sizeof(Cube));
                if(!sub_cube){
                    exit(-1);
                }

                /* populate cube */
                memcpy(sub_cube->lower_bound, new_bound[0], sizeof(float)*3);
                memcpy(sub_cube->upper_bound, new_bound[1], sizeof(float)*3);
                sub_cube->level = cube->level + 1;
                compute_cube_isolvl(sub_cube, isolvl_fn);

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



