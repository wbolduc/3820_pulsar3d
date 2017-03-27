#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "powerUps.h"
#include "graphics.h"
#include "util.h"

extern float distance(float x1, float y1, float z1, float x2, float y2, float z2);
extern void getViewPosition(float *x, float *y, float *z);

extern void setViewPosition(float x, float y, float z);

extern int playerHasKey;
extern powerUp_t underEffect;

void teleEffect()
{
	printf("teleport~~~~\n");
	int x, z;
	do
	{
		x = rand() % TOTALGRIDSIZE - 1;
		z = rand() % TOTALGRIDSIZE - 1;

	}while(world[x][3][z] != 0);

	printf("%d,%d,%d~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", x, 3, z);
	setViewPosition(-(float)x, -3.0, -(float)z);
}

void delay(int time)
{
	int endTime;
	endTime = glutGet(GLUT_ELAPSED_TIME) + time;

	while (endTime > glutGet(GLUT_ELAPSED_TIME)){}
}

void bounceEffect()
{
	static int midBounce = 0;
	static int timer;
	static float px, py, pz;
	static float sx, sy, sz;
	static float ux, uz; //translation unit vectors
	static float stepDist;
	static float totalDist;
	static float distanceSoFar = 0;
	int nx, nz; //new location
	printf("bounced~~~~\n");

	//lift player
	if (!midBounce)
	{
		getViewPosition(&px,&py,&pz);
		px *= -1;
		py *= -1;
		pz *= -1;

		sx = px;
		sz = pz;
		//pick new location
		do
		{
			nx = rand() % (TOTALGRIDSIZE - 2) + 1;
			nz = rand() % (TOTALGRIDSIZE - 2) + 1;

		}while(world[nx][3][nz] != 0);

		printf("%d, %d\n", nx, nz);

		//slide player
		ux = (float)nx - px;
		uz = (float)nz - pz;
		totalDist = sqrt(ux*ux + uz*uz);
		stepDist = totalDist / bounce_time;
		ux = ux / totalDist * stepDist;
		uz = uz / totalDist * stepDist;
		distanceSoFar = 0;

		printf("%d %d %f %f\n", nx, nz, ux, uz);

		midBounce = 1;
		timer = bounce_time;
	}

	if (timer-- > 0)
	{
		px += ux;
		pz += uz;
		distanceSoFar += stepDist;
		setViewPosition(-px, 4 * bounce_height * distanceSoFar * (distanceSoFar - totalDist) / (totalDist * totalDist) - 2, -pz);
	}
	else
	{
		midBounce = 0;
		underEffect = none;
	}
}

void rainEffect()
{
	printf("rain~~~~\n");
}


POWERUP newPowerUp()
{
	POWERUP pu;

	pu.powerType = rand() % 3;

	do
	{
		pu.x = 1 + (rand() % TOTALGRIDSIZE);
		pu.y = 1;
		pu.z = 1 + (rand() % TOTALGRIDSIZE);
	}
	while (pu.x % COMBINEDWIDTH == 0 || pu.z % COMBINEDWIDTH == 0);
	return pu;
}

void placePowerUp(POWERUP pu)
{
	int colour;
	//map colour
	switch (pu.powerType)
	{
		case teleport:
			colour = 3;
			break;
		case bounce:
			colour = 1;
			break;
		case rain:
			colour = 2;
			break;
		case key:
			colour = 5;
			break;
	}

	//place cube in world
	world[pu.x][pu.y][pu.z] = colour;
}

void initPowerUps()
{
	int i;
	POWERUP pu;

	for (i = 0; i < power_up_count - 1; i++)
	{
		powerList[i] = newPowerUp();
		placePowerUp(powerList[i]);
	}

	//make a key
	pu.powerType = key;
	do
	{
		pu.x = 1 + (rand() % TOTALGRIDSIZE);
		pu.y = 1;
		pu.z = 1 + (rand() % TOTALGRIDSIZE);
	}
	while (pu.x % COMBINEDWIDTH == 0 || pu.z % COMBINEDWIDTH == 0);
	powerList[power_up_count - 1] = pu;
	placePowerUp(pu);
}

void updatePowerUps()
{
	int i;
	float px, py, pz;

	getViewPosition(&px, &py, &pz);

	for (i = 0; i < power_up_count; i++)
	{
		if (distance(-px, -py, -pz, (float)powerList[i].x + .5, (float)powerList[i].y + .5, (float)powerList[i].z + .5) < pickup_dist)
		{
			//remove picked up block
			world[powerList[i].x][powerList[i].y][powerList[i].z] = 0;

			//do effect
			switch (powerList[i].powerType)
			{
				case teleport:
					teleEffect();
					break;
				case bounce:
					underEffect = bounce;
					break;
				case rain:
					rainEffect();
					break;
				case key:
					playerHasKey = 1;
					printf("got key\n");
					break;
			}

			powerList[i] = newPowerUp();
			placePowerUp(powerList[i]);
		}
	}
}
