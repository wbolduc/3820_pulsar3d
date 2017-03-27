
TARGET = -mmacosx-version-min=10.8
INCLUDES = -F/System/Library/Frameworks -framework OpenGL -framework GLUT -lm


all: a1.o graphics.o visible.o projectiles.o map.o mobs.o powerUps.o
	gcc a1.o graphics.o visible.o projectiles.o map.o mobs.o powerUps.o -o a1 $(INCLUDES)

a1: a1.c util.h
	gcc -c a1.c util.h $(INCLUDES)

visible: visible.c
	gcc -c visible.c $(INCLUDES)

graphics: graphics.c
	gcc -c graphics.c $(INCLUDES)

projectiles: projectiles.c graphics.h, mobs.h
	gcc -c projectiles.c graphics.h mobs.h $(INCLUDES)

map: map.c graphics.h projectiles.h powerUps.h
	gcc -c map.c graphics.h projectiles.h mobs.h powerUps.h $(INCLUDES)

mobs: mobs.c
	gcc -c mobs.c $(INCLUDES)

powerUps: powerUps.c
	gcc -c powerUps.c $(INCLUDES)

clean:
	rm a1 a1.o graphics.o visible.o projectiles.o map.o mobs.o
