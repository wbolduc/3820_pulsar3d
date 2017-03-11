#ifndef _PROJECTILES_H
#define _PROJECTILES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"

#define PI 3.14159
#define MAXPROJECTILES 10
#define BULLETSPEED 1
#define WALLCOLOUR 7
//THOUGHT: I should learn what a slot map is

typedef struct PROJECTILE{
	float x;
	float y;
	float z;

	float ux;
	float uy;
	float uz;
}PROJECTILE;

PROJECTILE pList [MAXPROJECTILES];
static int currentProjectiles = 0;

//0 if cannot add projectile
int addEulerProjectile(float x, float y, float z, float xAxis, float yAxis, float zAxis);
int addUnitProjectile(float x, float y, float z, float ux, float uy, float uz);

void removeProjectile(int projectileToRemove);

void animateProjectiles();

int getCurrentProjectiles();

#endif //_PROJECTILES_H
