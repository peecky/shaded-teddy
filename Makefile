UNAME := $(shell uname -s)
CC = g++
OBJ = game.o kuma.o main.o map.o
LDFLAGS = -lGL -lGLU -lglut

# Mac OS
ifeq ($(UNAME), Darwin)
	LDFLAGS = -framework OpenGL -framework glut
endif

shaded_teddy : $(OBJ)
	$(CC) -o shaded_teddy $(OBJ) $(LDFLAGS)

game.o : game.cpp game.h map.h kuma.h
	$(CC) -c -g game.cpp

kuma.o : kuma.cpp kuma.h game.h map.h
	$(CC) -c -g kuma.cpp

main.o : main.cpp game.h map.h kuma.h
	$(CC) -c -g main.cpp

map.o : map.cpp map.h
	$(CC) -c -g map.cpp

clean :
	rm $(OBJ)
