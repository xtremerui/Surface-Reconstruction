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
compute_cube_isolvl(Cube *cube, isolvl_function isolvl_fn, float threshold)
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
    isolvl[5] = isolvl_fn(v_pos[1][0], v_pos[0][1], v_pos[1][2]);
    isolvl[6] = isolvl_fn(v_pos[1][0], v_pos[1][1], v_pos[1][2]);
    isolvl[7] = isolvl_fn(v_pos[0][0], v_pos[1][1], v_pos[1][2]);

    /* compute cube index */
    cube_index = 0;
    for(int i = 0; i < 8; i++){
        if(isolvl[i] > threshold){
            cube_index |= 1<<i;
        }
    }

    //printf("cube_index = %x\n", threshold, cube_index);

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
    if(cube->level > (lvl_max-1) && (cube->cube_index == 0 || cube->cube_index == 0xff)){
        //printf("not touching, skipping\n");
        return;
    }

    /* compute triangle */
    if(cube->level == lvl_max){
        tri_index = tri_table[cube->cube_index];
        /* each triangle */
        for(int i = 0; tri_index[i] != -1; i+=3){
            float tri_pos[3][3];
            Vertex *tri_v[3];
            uint64_t mc_index[3];
	    uint8_t mc_type;

            memcpy(mc_index, cube->mc_index, sizeof(uint64_t)*3);

            for(int j = i; j < i+3; j++){
                int *point = vertex_table[tri_index[j]];
                float endpoint[2][3];
                float ep_isolvl[2];

                /* FIXME:compute coordinate & iso-level of two endpoints */
                for(int k = 0; k < 2; k++){
                    int tmp = point[k] % 4;
                    if(tmp < 2){
                        endpoint[k][1] = cube->lower_bound[1];
                    } else {
                        endpoint[k][1] = cube->upper_bound[1];
                    }
                    if(tmp == 0 || tmp == 3){
                        endpoint[k][0] = cube->lower_bound[0];
                    } else {
                        endpoint[k][0] = cube->upper_bound[0];
                    }
                    if(point[k]<4){
                        endpoint[k][2] = cube->lower_bound[2];
                    } else {
                        endpoint[k][2] = cube->upper_bound[2];
                    }

                    ep_isolvl[k] = cube->isolvl[point[k]];
                    ep_isolvl[k] *= ep_isolvl[k]>0.0?1.0:(-1.0);
                }

		/* compute mc index */
		if(endpoint[0][0] != endpoint[1][0]){
			mc_type = 0;
		} 
		else if (endpoint[0][1] != endpoint[1][1]){
                        mc_type = 1;
                } 
                else if (endpoint[0][2] != endpoint[1][2]){
                        mc_type = 2;
                }
		else {
			mc_type = 8;
		}
		for(int t = 0; t < 3; t++){
			float min_ep;
			if(endpoint[0][t] < endpoint[1][t]){
				min_ep = endpoint[0][t];
			} else {
				min_ep = endpoint[1][t];
			}
			if(min_ep > cube->lower_bound[t]){
				mc_index[t] |= ( 1 << (63-cube->level));
			}
		}
		

                /* compute coordinate of triangle */
                for(int t = 0; t < 3; t++){
                    tri_pos[j-i][t] = ep_isolvl[1]*endpoint[0][t] + ep_isolvl[0]*endpoint[1][t];
                    tri_pos[j-i][t] /= ep_isolvl[0] + ep_isolvl[1];
                }
		
		/* add triangle */
		//printf("finding vertex: (%x, %x, %x), %u  ...", 
		//		(unsigned)mc_index[0], (unsigned)mc_index[1], (unsigned)mc_index[2], mc_type);
		tri_v[j-i] = NULL;//mesh->findMCVertex(mc_index, mc_type);
		if(!tri_v[j-i]){
			tri_v[j-i] = new Vertex(tri_pos[j-i][0], tri_pos[j-i][1], tri_pos[j-i][2], 
					mc_index, mc_type);
			//printf("not found\n");
		}else {
			//printf("found\n");
		}
		mesh->addVertex(tri_v[j-i]);
            }

            /* add to mesh */
           // printf("MC: Triangle pushed\n");
            mesh->addTriangle(new Triangle(tri_v[0], tri_v[1], tri_v[2]));

            /*
            for(int m = 0; m < 3; m++){
                printf("v %f %f %f\n", tri_pos[m][0], tri_pos[m][1], tri_pos[m][2]);
            }
            */
        }
        return;
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
                uint64_t mc_index = 0;

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
                    printf("out of memory\n");
                    exit(-1);
                }

                /* populate cube */
                memcpy(sub_cube->lower_bound, new_bound[0], sizeof(float)*3);
                memcpy(sub_cube->upper_bound, new_bound[1], sizeof(float)*3);
                sub_cube->level = cube->level + 1;
                compute_cube_isolvl(sub_cube, isolvl_fn, threshold);

		if(x) {
			sub_cube->mc_index[0] = cube->mc_index[0] | (1 << (62-cube->level));
		} else {
			sub_cube->mc_index[0] = cube->mc_index[0];
		}

		if(y) {
                        sub_cube->mc_index[1] = cube->mc_index[1] | (1 << (62-cube->level));
                } else {
                        sub_cube->mc_index[1] = cube->mc_index[1];
                }

		if(z) {
                        sub_cube->mc_index[2] = cube->mc_index[2] | (1 << (62-cube->level));
                } else {
                        sub_cube->mc_index[2] = cube->mc_index[2];
                }
		

                /* push into stack */
                mc_cbstack_push(sub_cube);
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
    memset(root_cube->mc_index, 0, sizeof(uint64_t)*3);

    /* initialize mesh */
    *result = new Mesh();
    if(!(*result)){
        cube_destroy(root_cube);
        return -1;
    }
    
    /* push into cube */
    compute_cube_isolvl(root_cube, isolvl_fn, isolvl_threshold);
    mc_cbstack_push(root_cube);

    /* main loop */
    printf("starting MC\n");
    while(!mc_cbstack_isempty()){
        //printf("processing cube\n");
        process_cube(mc_cbstack_pop(), level, isolvl_fn, isolvl_threshold, *result);
    }

    printf("MC completed\n");
    return 0;
}



