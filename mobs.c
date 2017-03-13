#
#include "mobs.h"
#include "graphics.h"
#include <stdlib.h>

#define LOS_GRANULARITY 1

extern int dWall;

extern void getViewPosition(float *x, float *y, float *z);
extern void showMob(int number);
extern void setMobPosition(int number, float x, float y, float z, float mobroty);
extern void getViewOrientation(float *xaxis, float *yaxis, float *zaxis);

int dodgeDirection(vMOB * mob)
{
	int i;
	float dx = 0, dy = 0, dz = 0;

	for (i = 0; i < currentProjectiles; i++)
	{
		return 0;
	}
	return 1;
}

moveDirection_t pickDirection (int usedDirs[4], int remaining)
{
	int i;
	int pick = 0;

	if (remaining == 0)
	{
		return STALL;
	}

	i = rand() % remaining;
	while (1)
	{
		while (usedDirs[pick] == 1)
		{
			pick++;
		}
		if(i == 0)
		{
			return pick;
		}
		i--;
		pick++;
	}
	return pick;
}

moveDirection_t randomMove(vMOB * mob)
{
	int newX, newZ;
	int stop;
	int hitSide;
	int i, j;

	int line;
	int ori;

	int usedDirs[4] = {0};
	int remaining = 4;
	int pick;

	if(mob->moveDir == STALL)
	{
		mob->moveDir = rand() % 4;
	}

	//find which wall is currently closing
	ori = dWall % 11;
	if (ori < 5)
		line = (ori + 1) * COMBINEDWIDTH;
	else
		line = (dWall / 11 + 1) * COMBINEDWIDTH;
	ori = (ori < 5);
	//printf("Wall %d, ori %d, line %d ", dWall, ori, line);

	//try directions
TRYDIRECTION:
	switch(mob->moveDir)
	{
		case UP:
		{
			//reduce posible motion set
			usedDirs[UP] = 1;
			remaining--;

			stop = mob->x + mob->xSize;
			hitSide = mob->z + mob->zSize + 1;
			//printf("dir = %d, hitSide = %d\n", mob->moveDir, hitSide);
			if (!ori && hitSide == line)
			{
				//printf("WALL-------------\n");
				mob->moveDir = pickDirection(usedDirs, remaining);
				goto TRYDIRECTION;
			}

			for (i = mob->x; i < stop; i++)
				if (world[i][mob->y][hitSide] != 0)
				{
					//collision found, pick new direction
					mob->moveDir = pickDirection(usedDirs, remaining);
					goto TRYDIRECTION;
				}

			//no collision, move
			newZ = mob->z + 1;
			newX = mob->x;
			break;
		}
		case DOWN:
		{
			usedDirs[DOWN] = 1;
			remaining--;

			stop = mob->x + mob->xSize;
			hitSide = mob->z - 1;

			//printf("dir = %d, hitSide = %d\n", mob->moveDir, hitSide);
			if (!ori && hitSide == line)
			{
				//printf("WALL-------------\n");
				mob->moveDir = pickDirection(usedDirs, remaining);
				goto TRYDIRECTION;
			}

			for (i = mob->x; i < stop; i++)
				if (world[i][mob->y][hitSide] != 0)
				{
					//collision found, pick new direction
					mob->moveDir = pickDirection(usedDirs, remaining);
					goto TRYDIRECTION;
				}

			//no collision, move
			newZ = mob->z - 1;
			newX = mob->x;
			break;
		}
		case LEFT:
		{
			usedDirs[LEFT] = 1;
			remaining--;

			stop = mob->z + mob->zSize;
			hitSide = mob->x - 1;

			//printf("dir = %d, hitSide = %d\n", mob->moveDir, hitSide);
			if (ori && hitSide == line)
			{
				//printf("WALL-------------\n");
				mob->moveDir = pickDirection(usedDirs, remaining);
				goto TRYDIRECTION;
			}

			for (i = mob->z; i < stop; i++)
				if (world[hitSide][mob->y][i] != 0)
				{
					//collision found, pick new direction
					mob->moveDir = pickDirection(usedDirs, remaining);
					goto TRYDIRECTION;
				}

			//no collision, move
			newZ = mob->z;
			newX = mob->x - 1;
			break;
		}
		case RIGHT:
		{
			usedDirs[RIGHT] = 1;
			remaining--;

			stop = mob->z + mob->zSize;
			hitSide = mob->x + mob->xSize + 1;

			//printf("dir = %d, hitSide = %d\n", mob->moveDir, hitSide);
			if (ori && hitSide == line)
			{
				//printf("WALL-------------\n");
				mob->moveDir = pickDirection(usedDirs, remaining);
				goto TRYDIRECTION;
			}

			for (i = mob->z; i < stop; i++)
				if (world[hitSide][mob->y][i] != 0)
				{
					//collision found, pick new direction
					mob->moveDir = pickDirection(usedDirs, remaining);
					goto TRYDIRECTION;
				}

			//no collision, move
			newZ = mob->z;
			newX = mob->x + 1;
			break;
		}
		case STALL:
		{
			//printf("Mob Stuck\n");
			newX = mob->x;
			newZ = mob->z;
		}
	}

	erasevMob(mob);
	mob->x = newX;
	mob->z = newZ;
	drawvMob(mob);
	return mob->moveDir;
}

void updatevMob(vMOB * mob)
{
	float px, py, pz, ux, uy, uz, mx, my, mz;
	float dist;
	float mobGirth = mob->mobGirth;

	//check if in bullet path
	if (mob->moveTimer++ == mob->moveSpeed)
	{
		inPlayerFOV(mob);
		switch (mob->moveState)
		{
			case RANDOM:
			{
				randomMove(mob);
				break;
			}
			case DODGING:
			{
				printf("fuck");
				break;
			}
		}
		mob->moveTimer = 0;
	}

	//shoot the player
	if (mob->reloadTimer == mob->reload)
	{
		mob->reloadTimer = 0;
	}
	else if (mob->reloadTimer != 0)
	{
		mob->reloadTimer++;
	}
	else if (canSeePlayer(mob))
	{
		mob->reloadTimer++;

		getViewPosition(&px, &py, &pz);
		px = -px;
		py = -py;
		pz = -pz;
		mx = (float)(mob->xEye + mob->x) + .5;
		my = (float)(mob->yEye + mob->y) + .5;
		mz = (float)(mob->zEye + mob->z) + .5;

		dist = distance(px,py,pz, mx,my,mz);

		//mob to player
		ux = (px - mx) / dist;
		uy = (py - my) / dist;
		uz = (pz - mz) / dist;
		addUnitProjectile(ux * mobGirth + mx, uy * mobGirth + my, uz * mobGirth + mz, ux, uy, uz);
	}

	if (mob->frameTimer++ == mob->frameTime)
	{
		drawvMob(mob);
		if(++mob->currentFrame == mob->frameCount)
			mob->currentFrame = 0;
		mob->frameTimer = 0;
	}
}

vMOB* createvMob(int mobType, int x, int y, int z, int xSize, int ySize, int zSize, char **** mobAnimation, int frameCount, int frameTime, int reload, int moveSpeed)
{
	float a,b,c;
	vMOB* newMob = malloc(sizeof(vMOB));

	newMob->mobType = mobType;

	newMob->x = x;
	newMob->y = y;
	newMob->z = z;

	newMob->xSize = xSize;
	newMob->ySize = ySize;
	newMob->zSize = zSize;

	newMob->xEye = xSize / 2;
	newMob->yEye = ySize / 2;
	newMob->zEye = zSize / 2;

	newMob->animation = mobAnimation;
	newMob->frameCount = frameCount;
	newMob->currentFrame = 0;
	newMob->frameTime = frameTime;

	newMob->reload = reload;

	a = xSize / 2;
	b = ySize / 2;
	c = zSize / 2;
	newMob->mobGirth = sqrt(a*a + b*b + c*c);

	newMob->moveSpeed = moveSpeed;

	newMob->moveTimer = 0;
	newMob->reloadTimer = 0;
	newMob->frameTimer = 0;

	newMob->moveState = RANDOM;
	newMob->moveDir = STALL;
	return newMob;
}

void erasevMob(vMOB* mob)
{
	int x = mob->x;
	int y = mob->y;
	int z = mob->z;
	int xStop = x + mob->xSize;
	int yStop = y + mob->ySize;
	int zStop = z + mob->zSize;

	int i,j,k;

	for (j = y; j < yStop; j++)
		for (i = x; i < xStop; i++)
			for (k = z; k < zStop; k++)
				world[i][j][k] = 0;
}

void drawvMob(vMOB* mob)
{
	int x = mob->x;
	int y = mob->y;
	int z = mob->z;
	int xSize = mob->xSize;
	int ySize = mob->ySize;
	int zSize = mob->zSize;
	int xStop = x + xSize;
	int yStop = y + ySize;
	int zStop = z + zSize;

	int i,j,k, a,b,c;
	int curFrame = mob->currentFrame;
	char *ani = (char *)mob->animation;

	for (a = 0, j = y; j < yStop; j++, a++)
		for (b = 0, i = x; i < xStop; i++, b++)
			for (c = 0, k = z; k < zStop; k++, c++)
				world[i][j][k] = ani[zSize * ( ySize * ( (curFrame * xSize) + a) + b) + c];
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float a,b,c;
	a = x2 - x1;
	b = y2 - y1;
	c = z2 - z1;
	a *= a;
	b *= b;
	c *= c;
	return sqrt(a+b+c);
}

int canSeePlayer(vMOB* mob)
{
	float px, py, pz;	//player coordinates
	float mx, my, mz; //mob coordinates
	float ux, uy, uz;	//unit vector
	float dist;	//distance between the two

	float mobGirth = mob->mobGirth; //temp, onyl really works for 3x3 mobs with center eyes //:NOTE THIS NEEDS TO BE CALCULATED, BIGGER MODELS MIGHT BE BLIND

	//initialize the coords
	getViewPosition(&px, &py, &pz);
	px = -px;
	py = -py;
	pz = -pz;
	mx = (float)(mob->xEye + mob->x) + .5;
	my = (float)(mob->yEye + mob->y) + .5;
	mz = (float)(mob->zEye + mob->z) + .5;

	dist = distance(px,py,pz, mx,my,mz);

	//mob to player
	ux = (px - mx) / dist;
	uy = (py - my) / dist;
	uz = (pz - mz) / dist;
	//printf("player = %f, %f, %f\n", px,py,pz);
	//printf("mob    = %f, %f, %f\n", mx,my,mz);

	for (dist; dist > mobGirth; dist -= LOS_GRANULARITY)
	{
		//printf("%f, %f, %f\n", -ux*dist, -uy*dist, -uz*dist);
		if (world[(int)(ux * dist + mx)][(int)(uy * dist + my)][(int)(uz * dist + mz)] != 0)
		{
			return 0;
		}
	}
	return 1;
}

int inPlayerFOV(vMOB* mob)
{	//not taking into account vertical
	float angle;
	float px, pz;
	float pux, puz;
	float mux, muz;
	float dist;

	if (!canSeePlayer(mob))	//cant see at all, don't bother with the cone
	{
		return 0;
	}

	getViewPosition(&px, &dist, &pz);
	getViewOrientation(&pux, &angle, &puz);

	//player look unit vector
	pux = cos((angle - 90) * PI / 180);
	puz = sin((angle - 90) * PI / 180);

	//player to mob unit vector
	mux = ((float)(mob->x + mob->xEye) + .5 + px);
	muz = ((float)(mob->z + mob->zEye) + .5 + pz);
	dist = sqrt(mux * mux + muz * muz);
	mux /= dist;
	muz /= dist;

	if (acos(pux * mux + puz * muz) <= VIEW_CONE)
	{
		return 1;
	}
	return 0;
}

int checkIfHit(vMOB* mob)
{
	int x = mob->x;
	int y = mob->y;
	int z = mob->z;
	int xSize = mob->xSize;
	int ySize = mob->ySize;
	int zSize = mob->zSize;
	int xStop = x + xSize;
	int yStop = y + ySize;
	int zStop = z + zSize;

	int i,j,k, a,b,c;
	int curFrame = mob->currentFrame;
	char *ani = (char *)mob->animation;

	for (a = 0, j = y; j < yStop; j++, a++)
		for (b = 0, i = x; i < xStop; i++, b++)
			for (c = 0, k = z; k < zStop; k++, c++)
				if(world[i][j][k] != ani[zSize * ( ySize * ( (curFrame * xSize) + a) + b) + c])
					return 1;
	return 0;
}
