
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mobs.h"
#include "graphics.h"
#include "projectiles.h"
#include "map.h"
#include "util.h"

vMOB* mobs[MOBCOUNT];
char mobStart[6][6] = {0};
extern void updatevMob(vMOB * mob);
extern vMOB * createvMob(int x, int y, int z, int xSize, int ySize, int zSize, char **** mobAnimation, int frameCount, int frameTime, int reload, int moveSpeed);
extern void drawNextvMobFrame(vMOB* mob);


	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern int addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);


	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer;
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

/********* end of extern variable declarations **************/


	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
	static float x,y,z;
	static float oldX,oldY,oldZ;
	static float goodX, goodY, goodZ;
	int wallBump = 0;

	getOldViewPosition(&oldX,&oldY,&oldZ);
	getViewPosition(&x,&y,&z);

	goodX = x;
	goodY = y;
	goodZ = z;

	//printf("%f,%f,%f -> %f,%f,%f\n", oldX, oldY, oldZ, x,y,z);
	//printf("%d,%d,%d -> %d,%d,%d\n\n", -(int)(oldX),-(int)(oldY),-(int)(oldZ),-(int)(x),-(int)(y),-(int)(z));

	//check world bounds
	if (-(int)y > WORLDY || -(int)y < 0 || -(int)x > WORLDX || -(int)x < 0 || -(int)z > WORLDZ || -(int)z < 0)
	{
		//printf("would go out of world\n");
		setViewPosition(oldX, oldY, oldZ);
		return;
	}

	//check vertical
	if(world[-(int)oldX][-(int)y][-(int)oldZ] != 0)
	{
		goodY = oldY;
		//printf("bad new Y\n");
	}

	// horizontal
	if(world[-(int)x][-(int)oldY][-(int)oldZ] != 0)
	{
		goodX = oldX;
		wallBump = 1;
		//printf("bad new X\n");
	}

	// depth
	if(world[-(int)oldX][-(int)oldY][-(int)z] != 0)
	{
		goodZ = oldZ;
		wallBump = 1;
		//printf("bad new Z\n");
	}

	//TODO: walking into outward facing corners actually lifts you up the wall
	if (wallBump && world[-(int)x][-(int)(oldY - 1)][-(int)z] == 0)
	{
		goodY--;
		goodX = x;
		goodZ = z;
	}

	setViewPosition(goodX, goodY, goodZ);
}

void gravity()
{
	float x,y,z;
	float speed = .05;

	getViewPosition(&x,&y,&z);

	//check if you can fall further
	if(world[-(int)x][-(int)(y + PLAYERHEIGHT + speed)][-(int)z] == 0)
	{
		y += speed;
	}
	else
	{
		y = -(PLAYERHEIGHT + (float)((int)-y)); //stick the player to the floor
	}

	setViewPosition(x,y,z);
}

void printWalls()
{
	int i,j;
	for (i = 0; i < 55; i += 11)
	{
		for (j = 0; j < 5; j++)
		{
			printf(" %d", WALLS[i+j]);
		}
		printf("\n");
		for (j = 5; j < 11; j++)
		{
			printf("%d ", WALLS[i+j]);
		}
		printf("\n");
	}
	for (j = 0; j < 5; j++)
	{
		printf(" %d", WALLS[55+j]);
	}
	printf("\n---------------------\n");
}

int getOri(int wall)
{
	return (wall % 11 < 5);
}

int getDir(int sWall, int sOri, int dWall)
{
	if (sOri == 1)
	{
		return (dWall > sWall);
	}
	else
	{
		return (dWall % 11 == sWall % 11 - 5);
	}
}

int getPivotX(int wall)
{
	int pivotX;
	int ori;

	ori = getOri(wall);

	if (ori) //vertical
	{
		pivotX = (wall % 11) * COMBINEDWIDTH + COMBINEDWIDTH;
	}
	else
	{
		pivotX = (wall % 11 - 5) * COMBINEDWIDTH;
	}

	//printf("%d,%d -> x%d\n", wall, ori, pivotX);

	return pivotX;
}

int getPivotY(int wall)
{
	int pivotY;
	int ori;

	ori = getOri(wall);

	if (ori)
	{
		pivotY = (wall / 11) * COMBINEDWIDTH;
	}
	else
	{
		pivotY = (wall / 11) * COMBINEDWIDTH + COMBINEDWIDTH;
	}

	//printf("%d,%d -> y%d\n", wall, ori, pivotY);

	return pivotY;
}

int checkWallVacancy(int wall)	//1 if vacant
{
	int crush = wall % 11;
	int line, span, stop;
	int i;

	if (crush < 5)
	{	//vertical Wall
		line = (crush + 1) * COMBINEDWIDTH;
		span = (wall / 11) * COMBINEDWIDTH + 1;
		stop = span + HALLWIDTH;
		for (span; span < stop; span++)
			for (i = 1; i < WALLHEIGHT + 1; i++)
				if (world[line][i][span] != 0)
					return 0;
	}
	else
	{	//horizontal Wall
		line = (wall / 11 + 1) * COMBINEDWIDTH;
		span = (crush - 5) * COMBINEDWIDTH + 1;
		stop = span + HALLWIDTH;
		for (span; span < stop; span++)
			for (i = 1; i < WALLHEIGHT + 1; i++)
				if (world[span][i][line] != 0)
					return 0;
	}
	return 1;
}

void placeBlock(int pivotX, int pivotY, int ori, int dir, int step, int block)
{
	if (ori)
	{
		if (dir)
		{	//vertical going down
			world[pivotX][step % WALLHEIGHT + 1][pivotY + (step / WALLHEIGHT) + 1] = block;
		}
		else
		{	//vertical going up
			world[pivotX][step % WALLHEIGHT + 1][pivotY + HALLWIDTH - (step / WALLHEIGHT)] = block;
		}
	}
	else
	{
		if (dir)
		{	//horizontal destruction going right
			world[pivotX + (step / WALLHEIGHT) + 1][step % WALLHEIGHT + 1][pivotY] = block;
		}
		else
		{	//horizontal destruction going left
			world[pivotX + HALLWIDTH - (step / WALLHEIGHT)][step % WALLHEIGHT + 1][pivotY] = block;
		}
	}
}

int stepsSinceLastUpdate()
{
	static int lastTime;
	int curTime;
	int steps;

	curTime = glutGet(GLUT_ELAPSED_TIME);

	steps = (curTime - lastTime) / MILLISECONDS_BETWEEN_STEPS;

	//printf("time %d -> %d diff %d steps %d\n\n", lastTime, curTime, curTime - lastTime, steps);

	if (steps)
	{
		lastTime = curTime;
	}

	return steps;
}


void moveWall()
{
	static int step = 60;

	int i;

	int availibleWalls = 0;
	int spots[6];

	static int sWall;
	static int sOri;
	static int sDestroyDir;
	static int sPivotX;
	static int sPivotY;

	static int dWall;
	static int dOri;
	static int dBuildDir;
	static int dPivotX;
	static int dPivotY;

	int t;

	if (step == 60)
	{
		do
		{
			do //pick a start wall
			{
				sWall = rand() % 60;
			}while(!WALLS[sWall]);

			if (sWall % 11 < 5) //vertical Wall
			{
				sOri = 1;
				if (sWall > 10)
				{
					if (!WALLS[sWall - 11] && checkWallVacancy(sWall - 11))//MOVE UP
					{
						spots[availibleWalls++] = sWall - 11;
					}
					if (!WALLS[sWall - 6] && checkWallVacancy(sWall - 6)) //UP LEFT
					{
						spots[availibleWalls++] = sWall - 6;
					}
					if (!WALLS[sWall - 5]  && checkWallVacancy(sWall - 5)) //UP RIGHT
					{
						spots[availibleWalls++] = sWall - 5;
					}
				}

				if (sWall < 49)
				{
					if (!WALLS[sWall + 11] && checkWallVacancy(sWall + 11))//MOVE DOWN
					{
						spots[availibleWalls++] = sWall + 11;
					}
					if (!WALLS[sWall + 5] && checkWallVacancy(sWall + 5)) //DOWN LEFT
					{
						spots[availibleWalls++] = sWall + 5;
					}
					if (!WALLS[sWall + 6] && checkWallVacancy(sWall + 6)) //DOWN RIGHT
					{
						spots[availibleWalls++] = sWall + 6;
					}
				}
			}
			else //horizontal wall
			{
				sOri = 0;
				if (sWall % 11 != 5)
				{
					if (!WALLS[sWall - 1] && checkWallVacancy(sWall - 1))//MOVE LEFT
					{
						spots[availibleWalls++] = sWall - 1;
					}
					if (!WALLS[sWall - 6] && checkWallVacancy(sWall - 6)) //LEFT UP
					{
						spots[availibleWalls++] = sWall - 6;
					}
					if (!WALLS[sWall + 5] && checkWallVacancy(sWall + 5)) //LEFT DOWN
					{
						spots[availibleWalls++] = sWall + 5;
					}
				}

				if (sWall % 11 != 10)
				{
					if (!WALLS[sWall + 1] && checkWallVacancy(sWall + 1))//MOVE RIGHT
					{
						spots[availibleWalls++] = sWall + 1;
					}
					if (!WALLS[sWall - 5] && checkWallVacancy(sWall - 5)) //RIGHT UP
					{
						spots[availibleWalls++] = sWall - 5;
					}
					if (!WALLS[sWall + 6] && checkWallVacancy(sWall + 6)) //RIGHT DOWN
					{
						spots[availibleWalls++] = sWall + 6;
					}
				}
			}
		}while(!availibleWalls);

		//select Wall
		dWall = spots[rand() % availibleWalls];

		//move walls in date struct
		//printWalls();
		WALLS[sWall] = 0;
		WALLS[dWall] = 1;


		dOri = getOri(dWall);

		//set build and destroy directions
		sDestroyDir = getDir(sWall, sOri, dWall);
		dBuildDir = 1 - getDir(dWall, dOri, sWall);

		//compute pivot
		sPivotX = getPivotX(sWall);
		sPivotY = getPivotY(sWall);

		dPivotX = getPivotX(dWall);
		dPivotY = getPivotY(dWall);
		checkWallVacancy(dWall);
		/*
		for (i = 0; i < availibleWalls; i++)
		{
			printf("%d ", spots[i]);
		}

		printf("\n%d\t->\t%d\n", sWall, dWall);
		printf("%d\t%d\n", sOri, dOri);
		printf("%d\t%d\n", sDestroyDir, dBuildDir);

		printWalls();
		*/
		step = 0;
	}

	//animate walls

	//destroy Wall
	placeBlock(sPivotX, sPivotY, sOri, sDestroyDir, step, 0);

	//build the wall
	placeBlock(dPivotX, dPivotY, dOri, dBuildDir, step, 7);

	step++;
}

void wallPushingPlayer()	//NOTE: pretty lazy, should REALLY check which way the wall is coming, but for now, it's coo
{
	float fx,fy,fz;
	int x,y,z;
	getViewPosition(&fx,&fy,&fz);
	//getting pushed out of the way of a wall

	x = -(int)fx;
	y = -(int)fy;
	z = -(int)fz;

	if(world[x][y][z] != 0)
	{
		printf("INSIDE A BLOCK");

		if (world[x + 1][y][z] == 0)
		{
			//wall is pushing on z axis (probably) shift on x
			setViewPosition(fx + (rand()%2 * 2 - 1), fy, fz);
		}
		else
		{
			//wall is pushing on x axis (probably) shift on z
			setViewPosition(fx, fy, fz + (rand()%2 * 2 - 1));
		}
	}
}

/** building the world **/
void buildSkeletonWorld()
{
	int chanceOfWall = 50;
	int i, j, k, p, y;

	int wall, ori, dir, px, py;

	//clear world
	for(i=0; i<WORLDX; i++)
		for(j=0; j<WORLDY; j++)
			for(k=0; k<WORLDZ; k++)
				world[i][j][k] = 0;

	//make floor

	for(i=0; i< TOTALGRIDSIZE; i++)
		for(k=0; k< TOTALGRIDSIZE; k++)
			world[i][0][k] = 1;


	//make Outside walls
	for (i = 0; i < TOTALGRIDSIZE; i++)
	{
		for (j = 0; j < WALLHEIGHT + 1; j++)
		{
			world[i][j][0] = 3;
			world[i][j][TOTALGRIDSIZE-1] = 3;
			world[0][j][i] = 3;
			world[TOTALGRIDSIZE-1][j][i] = 3;
		}
	}

	//init the wall array
	for (i = 0; i < 60; i++)
	{
		if (chanceOfWall < rand() % 100)
		{
			WALLS[i] = 1;
			ori = getOri(i);
			px = getPivotX(i);
			py = getPivotY(i);

			for (j = 0; j < 60; j++)
			{
				placeBlock(px, py, ori, 1, j, 7);
			}

		}
		else
		{
			WALLS[i] = 0;
		}
	}

	//make pillars
	for (i = 0; i < TOTALGRIDSIZE; i += COMBINEDWIDTH)
	{
		for (k = 0; k < TOTALGRIDSIZE; k += COMBINEDWIDTH)
		{
			for (j = 0; j < 7; j++)
			{
				world[i][j][k] = 4;
			}
		}
	}

	//make mobs
	for (i = 0; i < 10; i++)
	{
		createMob(i,0,0,0,0);
		hideMob(i);
	}

}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
      set2Dcolour(green);
      draw2Dline(0, 0, 500, 500, 15);
      draw2Dtriangle(0, 0, 200, 200, 0, 200);

      GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
      set2Dcolour(black);
      draw2Dbox(500, 380, 524, 388);
   } else {
		GLfloat white[] = {1.0, 1.0, 1.0, 1};
      set2Dcolour(white);
		draw2Dbox(screenWidth/2 - 1, screenHeight / 2 - 1, screenWidth/2 + 1, screenHeight / 2 + 1);
		switch(displayMap)
		{
			case 0:	//no map
				break;
			case 1: //top right
				drawMap(screenHeight - 10 - SMALLMAPSIZE, screenWidth - 10 - SMALLMAPSIZE, SMALLMAPSIZE);
				break;
			case 2: //middle
				drawMap(screenHeight / 4, (screenWidth / 2) - (screenHeight / 4) ,screenHeight / 2);

				break;
		}
   }
}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k, steps;
float *la;

	/* sample animation for the test world, don't remove this code */
	/* -demo of animating mobs */
   if (testWorld) {

	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;
    /* end testworld animation */

   } else {

		steps = stepsSinceLastUpdate();

		for(steps; steps > 0; steps--)
		{
			if(!flycontrol)
			{
				gravity();
			}

			moveWall();
			wallPushingPlayer();
			animateProjectiles();

			for (i = 0; i < MOBCOUNT; i++)
			{
				updatevMob(mobs[i]);
			}
		}
   }
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */
void mouse(int button, int state, int x, int y) {
	float px,py,pz,pax,pay,paz;

   if (button == GLUT_LEFT_BUTTON)
	printf("left button - ");
   else if (button == GLUT_MIDDLE_BUTTON)
      printf("middle button - ");
   else
      printf("right button - ");

   if (state == GLUT_UP)
      printf("up - ");
   else
	{
	   printf("down - ");
		getViewPosition(&px,&py,&pz);
		getViewOrientation(&pax,&pay,&paz);
		addEulerProjectile(px,py,pz,pax,pay,paz);
	}

   printf("%d %d\n", x, y);
}



int main(int argc, char** argv)
{
int i, j, k;
char **** mobAni;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);

	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);

   } else {

		buildSkeletonWorld();
		setViewPosition(-1.0,-20.0,-1.0);

		for (j = 0; j < MOBCOUNT; j++)
		{
			do
			{
				i = rand() % 6;
				k = rand() % 6;
			}
			while (mobStart[i][k] == 1);
			mobStart[i][k] = 1;

			if(rand()%2)
			{
				mobAni = redMob;
			}
			else
			{
				mobAni = yellowMob;
			}

			mobs[j] = createvMob(i * COMBINEDWIDTH + COMBINEDWIDTH/2, 2, k * COMBINEDWIDTH + COMBINEDWIDTH/2, 3,3,3, mobAni, 4, 20, 10000, 5);
		}

		//init the time in stepsSinceLastUpdate
		stepsSinceLastUpdate();
   }

	/*
	getPivotX(0);
	getPivotY(0);
	getPivotX(5);
	getPivotY(5);
	getPivotX(49);
	getPivotY(49);
	getPivotX(55);
	getPivotY(55);
	getPivotX(59);
	getPivotY(59);
	getPivotX(54);
	getPivotY(54);
	getPivotX(10);
	getPivotY(10);
	getPivotX(4);
	getPivotY(4);
	*/
	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();

	for (i = 0; i < MOBCOUNT; i++)
	{
		free(mobs[i]);
	}
   return 0;
}
