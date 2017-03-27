#ifndef _POWERUPS_H
#define _POWERUPS_H

#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"

#define power_up_count 20
#define pickup_dist 1.5

#define bounce_height 40
#define bounce_time 240

typedef enum {bounce, teleport, rain, none} powerUp_t;

typedef struct POWERUP
{
	int x;
	int y;
	int z;

	powerUp_t powerType;
}POWERUP;

POWERUP powerList [power_up_count];

#endif /*_POWERUPS_H*/
