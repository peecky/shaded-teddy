CC = g++
OBJ = game.o kuma.o main.o map.o

shaded_teddy : $(OBJ)
	$(CC) -o shaded_teddy $(OBJ) -L/usr/X11R6/lib -lglut -lgl -lglu

game.o : game.cpp game.h
	$(CC) -c -g game.cpp

kuma.o : kuma.cpp kuma.h game.h
	$(CC) -c -g kuma.cpp

main.o : main.cpp game.h
	$(CC) -c -g main.cpp

map.o : map.cpp map.h
	$(CC) -c -g map.cpp

clean :
	rm $(OBJ)
