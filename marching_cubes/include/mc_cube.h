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

#include <stdint.h>
#include <sys/queue.h>

/*
 *---------------------------------------------------------
 *
 *  Class Definitions
 *
 *---------------------------------------------------------
 */
typedef struct cube_s {
    SLIST_ENTRY(cube_s) stack_entry;
    float lower_bound[3];
    float upper_bound[3];
    float isolvl[8];
    uint8_t level;
    uint8_t cube_index;
    uint64_t mc_index[3];
} Cube;


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
        uint32_t level);

Cube* cube_create(float lower_bound[3],
        float upper_bound[3],
        uint32_t level);


void cube_destroy(Cube *cube);

#endif

