CC = g++
OBJ = game.o kuma.o main.o map.p teddy.o

shaded_teddy : $(OBJ)
	$(CC) -o shaded_teddy $(OBJ)

game.o : game.cpp game.h
	$(CC) -c -g game.cpp

kuma.o : kuma.cpp kuma.h game.h
	$(CC) -c -g kuma.cpp

main.o : main.cpp game.h
	$(CC) -c -g main.cpp

map.o : map.cpp map.h
	$(CC) -c -g map.cpp

teddy.o : teddy.cpp
	$(CC) -c -g tedy.cpp

clean :
	rm $(OBJ)
