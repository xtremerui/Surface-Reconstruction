/*
 *------------------------------------------------------------------------
 *
 *  mc_cubestack.h
 *  
 *  This file contains MC Cube Stack declarations.
 *
 *  Created by Zhuoli Jiang    Apr 08, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#ifndef _MC_CUBESTACK_H_
#define _MC_CUBESTACK_H_

#include <sys/queue.h>
#include "mc_cube.h"

/*
 *---------------------------------------------------------
 *
 *  Type Definitions
 *
 *---------------------------------------------------------
 */

static SLIST_HEAD(CubeStack, cube_s) cube_stack;

/*
 *---------------------------------------------------------
 *
 *  Functions
 *
 *---------------------------------------------------------
 */

void mc_cbstack_init();

void mc_cbstack_push(Cube* cube);

Cube* mc_cbstack_pop();

int mc_cbstack_isempty();

#endif

