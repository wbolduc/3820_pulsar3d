#include "projectiles.h"
#include "graphics.h"
#include <stdio.h>


extern void setMobPosition(int number, float x, float y, float z, float mobroty);
extern void hideMob(int number);
extern void showMob(int number);
//0 if cannot add projectile
int addEulerProjectile(float x, float y, float z, float xAxis, float yAxis, float zAxis)
{
	if (currentProjectiles < MAXPROJECTILES)
	{
		pList[currentProjectiles].x = -x;
		pList[currentProjectiles].y = -y;
		pList[currentProjectiles].z = -z;
		pList[currentProjectiles].ux = cos((yAxis - 90) * PI / 180);
		pList[currentProjectiles].uy = -tan(xAxis * PI / 180);
		pList[currentProjectiles].uz = sin((yAxis - 90) * PI / 180);

		currentProjectiles++;
		printf("projectiles = %d\n", currentProjectiles);
		return 1;
	}
	return 0;
}

int addUnitProjectile(float x, float y, float z, float ux, float uy, float uz)
{
	if (currentProjectiles < MAXPROJECTILES)
	{
		pList[currentProjectiles].x = x;
		pList[currentProjectiles].y = y;
		pList[currentProjectiles].z = z;
		pList[currentProjectiles].ux = ux;
		pList[currentProjectiles].uy = uy;
		pList[currentProjectiles].uz = uz;

		currentProjectiles++;
		printf("projectiles = %d\n", currentProjectiles);
		return 1;
	}
	return 0;
}

void removeProjectile(int projectileToRemove)
{	//NOTE: THIS HAS BASICALLY NO ERROR CHECKING
	//just decrement if it's the last projectile in the "stack"
	if (projectileToRemove == currentProjectiles - 1)
	{
		currentProjectiles--;
		hideMob(currentProjectiles);
		printf("dead projectile = %d\n", currentProjectiles);
		return;
	}

	//replace the projectile to remove with the last element in the stack
	if (currentProjectiles > 1)
	{
		pList[projectileToRemove] = pList[currentProjectiles - 1];
		currentProjectiles--;
		hideMob(currentProjectiles);
		printf("last projectile = %d\n", currentProjectiles);
	}
	printf("wut = %d\n", currentProjectiles);
}

void animateProjectiles()
{
	int i, collision;

	//shift all projectiles
	for(i = 0; i < currentProjectiles; i++)
	{
		pList[i].x += BULLETSPEED * pList[i].ux;
		pList[i].y += BULLETSPEED * pList[i].uy;
		pList[i].z += BULLETSPEED * pList[i].uz;

		setMobPosition(i, pList[i].x - .5, pList[i].y, pList[i].z - .5, 1);
	}

	//projectiles flicker for the following code
	for (i = currentProjectiles - 1; i >= 0; i--)
	{
		showMob(i);
		collision = world[(int)(pList[i].x)][(int)(pList[i].y + .5)][(int)(pList[i].z)];
		if (collision != 0)
		{
			printf("boop\n");
			if (collision == WALLCOLOUR)
			{
				world[(int)(pList[i].x)][(int)(pList[i].y + .5)][(int)(pList[i].z)] = 0;
				printf("%d, collide with orange at %d,%d,%d\n", i, (int)(pList[i].x + .5), (int)(pList[i].y + .5), (int)(pList[i].z + .5));
			}
			removeProjectile(i);
			hideMob(i);
		}
		else if ((int)(pList[i].y + .5 > WORLDY) || (int)(pList[i].y + .5 < 0) || (int)(pList[i].x + .5 > WORLDX) || (int)(pList[i].x + .5 < 0) || (int)(pList[i].z + .5 > WORLDZ) || (int)(pList[i].z + .5 < 0))
		{
			printf("beep\n");
			removeProjectile(i);
			hideMob(i);
		}
	}

	/*i = 0;
	while (i < currentProjectiles)
	{
		showMob(i);
		collision = world[(int)(pList[i].x + .5)][(int)(pList[i].y + .5)][(int)(pList[i].z + .5)];
		if (collision != 0)
		{
			if (collision == WALLCOLOUR)
			{
				world[(int)(pList[i].x + .5)][(int)(pList[i].y + .5)][(int)(pList[i].z + .5)] = 0;
				printf("%d, collide with orange at %d,%d,%d\n", i, (int)(pList[i].x + .5), (int)(pList[i].y + .5), (int)(pList[i].z + .5));
			}
			removeProjectile(i);
			hideMob(i);
		}
		else if ((int)(pList[i].y + .5 > WORLDY) || (int)(pList[i].y + .5 < 0) || (int)(pList[i].x + .5 > WORLDX) || (int)(pList[i].x + .5 < 0) || (int)(pList[i].z + .5 > WORLDZ) || (int)(pList[i].z + .5 < 0))
		{
			removeProjectile(i);
			hideMob(i);
		}
		else
		{
			i++;
		}
	}*/
}

int getCurrentProjectiles()
{
	return currentProjectiles;
}
