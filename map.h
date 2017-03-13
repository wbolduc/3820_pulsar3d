#ifndef _MAP_H
#define _MAP_H

#include "graphics.h"
#include "util.h"
#include "projectiles.h"
#include <stdio.h>

#define topx 1023
#define topy 767

static float playerColor[]	= {1.0,  1.0, 1.0, 1.0};
static float bulletColor[] = {0.5,	0.5, 0.5, 1.0};
static float outWall[]		= {1.0,  0.0, 0.0, 1.0};
static float worldFloor[] 	= {0.0,  1.0, 0.0, .5};
static float inWall[]		= {1.0, 0.64, 0.0, 1.0};
static float pillar[]		= {0.0,  0.0, 0.0, 1.0};

static float red[]   	= {1.0, 0.0, 0.0, 1.0};
static float yellow[]   = {1.0, 1.0, 0.0, 1.0};



void drawMap(int x, int y, int size);



#endif //_MAP_H
