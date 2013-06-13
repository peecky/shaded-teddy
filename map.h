// map.h
#include <stdlib.h>
#include <time.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <iostream>

#ifndef __pi_piano_MAP_H__
#define __pi_piano_MAP_H__

#define MAP_WIDTH 6
#define MAP_HEIGHT 6
#define MAX_TILE_MOVESTATE 10
#define MAX_REMOVESTATE 120

#define COMBO_TERM 200

#define bottom	0
#define south	1
#define	north	2
#define west	3
#define	east	4
#define	top		5

struct Tile {
	bool isempty;
	int num[6];
	bool isflash;	// 반짝이는가?
	bool isred;	// red/black Toggle
	bool ismoving;
	enum Direction {UP, LEFT, DOWN, RIGHT, NONE};
	Direction direction;
	int movestate;
	int removestate;
};

class Map {
public:
	void initMap();	// be used for constructor
	~Map();
	int getDICEtop(int x, int y);
	bool isFlash(int x, int y);
	bool isRed(int x, int y);
	bool isMoving(int x, int y);
	Tile::Direction getDirection(int x, int y);
	int getMoveState(int x, int y);
	void setTile(int x, int y );
	void setDirection(int x, int y, Tile::Direction);
	void moveTile(int src_x, int src_y, int dst_x, int dxt_y);
	bool EMPTY(int x, int y);
	void draw_dice( int x, int y );

	bool Update();	/* update move state, remove state, score, combo, is_combo
	if moving is end then return false */
	int ScoreUpdate();
	int show_combo();
	int show_combomx();
private:
	Tile **tile;
	int addscore;
	int combo;
	int is_combo;
	int max_combo;
	bool AdjacencyTest(int x, int y, int top_value, Tile::Direction from, int& count);	// 같은 도형 4개 인접성 체크
};

#endif
