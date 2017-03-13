#include "map.h"
#include "mobs.h"
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);

extern void getViewPosition(float *x, float *y, float *z);

extern int getCurrentProjectiles();

extern vMOB* mobs[MOBCOUNT];

void fixedDraw2Dbox(int x, int y, int xSize, int ySize)		//just had to transpose, could fix later if I cared
{
	//draw2Dbox(x,y,xSize,ySize);
	draw2Dbox(y,x,ySize,xSize);
}

void drawvMobOnMap(int m, int mapx, int mapy, int size)
{
	vMOB *mob = mobs[m];
	int pixelSize = size / TOTALGRIDSIZE;

	if (mob->mobType == 0)
		set2Dcolour(yellow);
	else
		set2Dcolour(red);

	fixedDraw2Dbox(mapx + (mob->x * pixelSize), mapy + (mob->z * pixelSize), mapx + ((mob->x + mob->xSize ) * pixelSize) + pixelSize, mapy + ((mob->z + mob->zSize) * pixelSize) + pixelSize);

	/*for (b = 0, i = x; i < xStop; i++, b++)
		for (c = 0, k = z; k < zStop; k++, c++)
			if((pixel = ani[zSize * ( ySize * ( (curFrame * xSize) + a) + b) + yMid]))
			{
				if(pixel == 3)
					set2Dcolour(red);
				else if (pixel == 8)
					set2Dcolour(yellow);
				fixedDraw2Dbox(mapx + (i * pixelSize), mapy + (k * pixelSize), mapx + (i * pixelSize) + pixelSize, mapy + (k * pixelSize) + pixelSize);
			}*/
}


void drawMap(int x, int y, int size)
{
	int i, j;
	int bullets;
	int heightToCheck;
	float px,py, pz;
	//float angle;
	int pixelSize = size / TOTALGRIDSIZE;

	//drawMobs
	for (i = 0; i < MOBCOUNT; i++)
		drawvMobOnMap(i, x, y, size);

	//drawPlayer  //TODO: FIX POSITIONING, NEED TO TRANSPOSE POSITION
	set2Dcolour(playerColor);
	//getViewOrientation(&px,&angle,&pz); //might want to show which direction the player is facing on the map
	getViewPosition(&px, &py, &pz);
	px *= -1;
	py *= -1;
	pz *= -1;
	fixedDraw2Dbox(x + ((int)px * pixelSize), y + ((int)pz * pixelSize), x + pixelSize + ((int)px * pixelSize), y + pixelSize + ((int)pz * pixelSize));

	//draw bullets
	set2Dcolour(bulletColor);
	bullets = getCurrentProjectiles();
	for(i = 0; i < bullets; i++)
	{
		fixedDraw2Dbox(x + ((int)pList[i].x * pixelSize), y + ((int)pList[i].z * pixelSize), x + pixelSize + ((int)pList[i].x * pixelSize), y + pixelSize + ((int)pList[i].z * pixelSize));
	}


	//draw pillars
	set2Dcolour(pillar);
	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 7; j++)
		{
			fixedDraw2Dbox(x + (i * COMBINEDWIDTH * pixelSize), y + (j * COMBINEDWIDTH * pixelSize), x + pixelSize + (i * COMBINEDWIDTH * pixelSize), y + pixelSize + (j * COMBINEDWIDTH * pixelSize));
		}
	}

	//draw inside walls
	set2Dcolour(inWall);

	heightToCheck = (int)py;
	if (heightToCheck > WALLHEIGHT)
	{
		heightToCheck = 1;
	}

	for(i = COMBINEDWIDTH; i < TOTALGRIDSIZE - COMBINEDWIDTH; i += COMBINEDWIDTH)
		for (j = 1; j < TOTALGRIDSIZE-1; j++)
		{
			if (!j%COMBINEDWIDTH)	//skip checking pillars
			{
				continue;
			}

			if (world[i][heightToCheck][j])	//check verticals (before transpose fix)
			{
				fixedDraw2Dbox(x + (i * pixelSize), y + (j * pixelSize), x + pixelSize + (i * pixelSize), y + pixelSize + (j * pixelSize));
			}

			//check horizontals (before transpose fix)
			if (world[j][heightToCheck][i])
			{
				fixedDraw2Dbox(x + (j * pixelSize), y + (i * pixelSize), x + pixelSize + (j * pixelSize), y + pixelSize + (i * pixelSize));
			}
		}

	//draw Outside walls
	set2Dcolour(outWall);
	fixedDraw2Dbox(x, y, x + (pixelSize * TOTALGRIDSIZE), y + pixelSize);
	fixedDraw2Dbox(x, y, x + pixelSize, y + (pixelSize * TOTALGRIDSIZE));
	fixedDraw2Dbox(x + (pixelSize * TOTALGRIDSIZE) - pixelSize, y, x + (pixelSize * TOTALGRIDSIZE), y + (pixelSize * TOTALGRIDSIZE));
	fixedDraw2Dbox(x, y + (pixelSize * TOTALGRIDSIZE) - pixelSize, x + (pixelSize * TOTALGRIDSIZE), y + (pixelSize * TOTALGRIDSIZE));

	//draw floor
	set2Dcolour(worldFloor);
	fixedDraw2Dbox(x, y, x + (pixelSize * TOTALGRIDSIZE), y + (pixelSize * TOTALGRIDSIZE));

}
