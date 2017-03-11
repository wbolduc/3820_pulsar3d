#include "mobs.h"
#include "graphics.h"
#include <stdlib.h>

#define LOS_GRANULARITY 1

extern void getViewPosition(float *x, float *y, float *z);
extern void showMob(int number);
extern void setMobPosition(int number, float x, float y, float z, float mobroty);

void updatevMob(vMOB * mob)
{
	static int frame = 0;
	static int bulletReload = 0;
	float px, py, pz, ux, uy, uz, mx, my, mz;
	float dist;
	float mobGirth = 3;

	//shoot the player
	if (bulletReload == mob->reload)
	{
		bulletReload = 0;
	}
	else if (bulletReload != 0)
	{
		bulletReload++;
	}
	else if (canSeePlayer(mob))
	{
		printf("PEW %d\n", frame);
		bulletReload++;

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

	if (frame++ == mob->frameTime)
	{
		drawNextvMobFrame(mob);
		frame = 0;
	}
}

vMOB* createvMob(int x, int y, int z, int xSize, int ySize, int zSize, char **** mobAnimation, int frameCount, int frameTime, int reload)
{
	vMOB* newMob = malloc(sizeof(vMOB));

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

	return newMob;
}

void drawNextvMobFrame(vMOB* mob)
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

	if(++mob->currentFrame == mob->frameCount)
		mob->currentFrame = 0;
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

	float mobBody = 3; //temp, onyl really works for 3x3 mobs with center eyes //:NOTE THIS NEEDS TO BE CALCULATED, BIGGER MODELS MIGHT BE BLIND

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

	for (dist; dist > mobBody; dist -= LOS_GRANULARITY)
	{
		//printf("%f, %f, %f\n", -ux*dist, -uy*dist, -uz*dist);
		if (world[(int)(ux * dist + mx)][(int)(uy * dist + my)][(int)(uz * dist + mz)] != 0)
		{
			return 0;
		}
	}
	return 1;
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
