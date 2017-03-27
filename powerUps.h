#ifndef _POWERUPS_H
#define _POWERUPS_H

#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"

#define power_up_count 30
#define pickup_dist 1.5

#define bounce_height 48
#define bounce_time 240

#define rain_blocks 4

typedef enum {bounce, teleport, rain, key, none} powerUp_t;

typedef struct POWERUP
{
	int x;
	int y;
	int z;

	powerUp_t powerType;
}POWERUP;

POWERUP powerList [power_up_count];

POWERUP RAINLIST [rain_blocks];

#endif /*_POWERUPS_H*/
