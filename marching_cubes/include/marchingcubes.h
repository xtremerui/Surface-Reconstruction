/*
 *------------------------------------------------------------------------
 *
 *  marchingcubes.h
 *  
 *  This file contains Marching Cubes Algorithm declarations.
 *
 *  Created by Zhuoli Jiang    Apr 08, 2012
 *
 *  Copyright (c) 2012 Zhuoli Jiang.
 *  All rights reserved.
 *
 *------------------------------------------------------------------------
 */

#ifndef _MARCHINGCUBES_H_
#define _MARCHINGCUBES_H_

#include <stdint.h>
#include "mesh.h"

/*
 *---------------------------------------------------------
 *
 *  Declarations
 *
 *---------------------------------------------------------
 */

typedef float (*isolvl_function) (float x, float y, float z);

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
        Mesh **result);

#endif

