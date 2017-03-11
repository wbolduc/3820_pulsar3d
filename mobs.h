#ifndef _MOBS_H
#define _MOBS_H

#include "graphics.h"
#include "util.h"
#include "projectiles.h"
#include <stdio.h>

typedef struct vMOB{
	int x;
	int y;
	int z;

	int xSize;
	int ySize;
	int zSize;

	int xEye;
	int yEye;
	int zEye;

	char **** animation;
	int frameCount;
	int frameTime;
	int currentFrame;

	int reload;
}vMOB;

//mobs get drawn bottom up
static char yellowMob [4][3][3][3] = {{{{0,0,7},
					      						 {0,7,0},
					      						 {7,0,0}},
					      				   	{{7,7,7},
					      						 {0,3,0},
					      						 {7,7,7}},
					      		   	  	   {{0,0,7},
					      						 {0,7,0},
					      						 {7,0,0}}},
					      				 	  {{{0,7,0},
					      						 {0,7,0},
					      						 {0,7,0}},
					      				  	   {{7,7,0},
					      						 {7,3,7},
					      						 {0,7,7}},
					      				  	   {{0,7,0},
					      				  		 {0,7,0},
					      						 {0,7,0}}},
					      				 	  {{{7,0,0},
					      						 {0,7,0},
					      						 {0,0,7}},
					      				  	   {{7,0,7},
					      						 {7,3,7},
					      						 {7,0,7}},
					      				  	   {{7,0,0},
					      				   	 {0,7,0},
					      						 {0,0,7}}},
					      				 	  {{{0,0,0},
					      				 		 {7,7,7},
					      						 {0,0,0}},
					      				  	   {{0,7,7},
					      				  		 {7,3,7},
					      						 {7,7,0}},
					      				  	   {{0,0,0},
					      				   	 {7,7,7},
					      						 {0,0,0}}}};

static char redMob [4][3][3][3] = {{{{0,0,0},
 					      					 {0,0,0},
					      					 {0,0,0}},
					      			   	{{0,0,0},
					      				 	 {0,3,0},
 					      					 {0,0,0}},
 					      		     	   {{0,0,0},
 					      					 {0,0,0},
 					      					 {0,0,0}}},
 					      			 	  {{{0,0,0},
 					      					 {0,3,0},
 					      					 {0,0,0}},
 					      			  	   {{0,3,0},
 					      					 {3,3,3},
 					      					 {0,3,0}},
 					      			  	   {{0,0,0},
 					      			  		 {0,3,0},
 					      					 {0,0,0}}},
 					      			 	  {{{0,3,0},
 					      					 {3,3,3},
 					      					 {0,3,0}},
 					      			  	   {{3,3,3},
 					      					 {3,3,3},
 					      					 {3,3,3}},
 					      			  	   {{0,3,0},
 					      			   	 {3,3,3},
 					      					 {0,3,0}}},
 					      			 	  {{{3,0,3},
 					      			 		 {0,3,0},
 					      					 {3,0,3}},
 					      			  	   {{0,0,0},
 					      			  		 {0,3,0},
					      					 {0,0,0}},
 					      			  	   {{3,0,3},
 					      			   	 {0,0,0},
					      					 {3,0,3}}}};

void updatevMob(vMOB * mob);

float distance(float x1, float y1, float z1, float x2, float y2, float z2);

vMOB* createvMob(int x, int y, int z, int xSize, int ySize, int zSize, char **** mobAnimation, int frameCount, int frameTime, int reload);

void drawNextvMobFrame(vMOB* mob);

int canSeePlayer(vMOB* mob);

int checkIfHit(vMOB* mob);

#endif //_MOBS_H
