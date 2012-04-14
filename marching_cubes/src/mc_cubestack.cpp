/*
 *------------------------------------------------------------------------
 *
 *  mc_cubestack.cpp
 *  
 *  This file contains MC Cube Stack implementations.
 *
 *  Created by Zhuoli Jiang   Apr 08, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#include "mc_cubestack.h"

#include "stdio.h"

#define NULL 0

/*
 *---------------------------------------------------------
 *
 *  Functions
 *
 *---------------------------------------------------------
 */

void mc_cbstack_init (){
    SLIST_INIT(&cube_stack);
}

void mc_cbstack_push (Cube* cube){
    if(cube){
        SLIST_INSERT_HEAD(&cube_stack, cube, stack_entry);
    }
}

Cube* mc_cbstack_pop (){
    Cube *result;
    result = cube_stack.slh_first;
    if(result){
        SLIST_REMOVE_HEAD(&cube_stack, stack_entry);
    }

    return result;
}

int mc_cbstack_isempty (){
    return (cube_stack.slh_first == 0);
}


