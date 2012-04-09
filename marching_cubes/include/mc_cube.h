/*
 *------------------------------------------------------------------------
 *
 *  cube.h
 *  
 *  This file contains Cube declarations.
 *
 *  Created by Zhuoli Jiang    Apr 08, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#ifndef _CUBE_H_
#define _CUBE_H_

#include <stdint>
#include <sys/queue.h>

/*
 *---------------------------------------------------------
 *
 *  Class Definitions
 *
 *---------------------------------------------------------
 */

typedef struct cube_s {
    LIST_ENTRY(struct cube_s) stack_entry;
    float lower_bound[3];
    float upper_bound[3];
    uint32_t level;
    uint8_t cube_index;
} Cube;


/*
 *---------------------------------------------------------
 *
 *  Functions
 *
 *---------------------------------------------------------
 */

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

void cube_destroy(Cube *cube){
    free(cube);
}

#endif

