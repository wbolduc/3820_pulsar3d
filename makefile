
TARGET = -mmacosx-version-min=10.8
INCLUDES = -w -F/System/Library/Frameworks -framework OpenGL -framework GLUT -lm


all: a1.o graphics.o visible.o projectiles.o map.o
	gcc a1.o graphics.o visible.o projectiles.o map.o -o a1 $(INCLUDES)

a1: a1.c util.h
	gcc -c a1.c util.h $(INCLUDES)

visible: visible.c
	gcc -c visible.c $(INCLUDES)

graphics: graphics.c
	gcc -c graphics.c $(INCLUDES)

projectiles: projectiles.c graphics.h
	gcc -c projectiles.c graphics.h $(INCLUDES)

map: map.c graphics.h projectiles.h
	gcc -c map.c graphics.h projectiles.h $(INCLUDES)

clean:
	rm a1 a1.o graphics.o visible.o projectiles.o