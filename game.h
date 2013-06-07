// game.h

#ifndef __anopi_GAME_H__
#define __anopi_GAME_H__

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cstdlib>

#include "map.h"
#include "kuma.h"

#define WINDOW_WIDTH 800	// initial window size of width
#define WINDOW_HEIGHT 600	// initial window size of height

// gack ground color
#define BACK_R	0.0/255.0
#define BACK_G	0.0/255.0
#define BACK_B	0.0/255.0

#define INIT_VIEW_PIX 10.0	// size of view volume
#define FIGURE_SIZE 2.0	// size of figures
#define TIMEFACTOR 20	// game speed

#define INIT_FIGURES 4	// how many figures when start
#define INIT_DIFFICULTY 10000

#define VIEW_THETA 0.0875	// 5 degree
#define VIEW_PHI 0.0875	// 5 degree
#define LIMIT_PHI 1.4	// 80 degree

class Game {
public:
	static void Reshape(int w, int h);
	static void Display();
	static void Idle();
	static void SpecialKey(int key, int x, int y);
	static void KeyInput(unsigned char key, int x, int y);
private:
	// program running
	enum State {INIT = 0, START, END, QUIT};
	static State state;
	static bool work;
	static bool figure_moving;
	static bool teddy_moving;
	static unsigned oldtime;

	// objects
	static Map map;
	static Teddy *kuma;

	// game variables
	static int cursor_x;	// 주인공 x좌표
	static int cursor_y;	// 주인공 y좌표
	static unsigned score;
	static double playtime;

	// view factors
	static int window_w;	// window size of width
	static int window_h;	// window size of height, Reshape()함수를 명시적으로 부를 때 사용
	static double eye_x;
	static double eye_y;
	static double eye_z;
	static double theta;
	static double view_pix;
	static double phi;
	static bool static_view;	/* static view / character centered view.
	if it is true, map is not scrolled when character is moving */

	static enum Teddy::Shade shade_type;

	static void Initialize();
	static void RunGame();
	static void DrawMap();
	static void setMatrixProperties(int x, int y);
	static void DrawTeddy();
	static bool checkGameOver();
	static void Write_score();
	static void Write_combo();
	static void tile_set();
};

#endif