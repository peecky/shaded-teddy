// game.cpp

#include <cmath>

#include "game.h"

#include <sys/time.h>

// initialize static members
Game::State Game::state = INIT;
bool Game::work = true;
bool Game::figure_moving = false;
bool Game::teddy_moving = false;
int Game::cursor_x = 0;
int Game::cursor_y = 0;
unsigned Game::score = 0;
double Game::playtime = 0;
unsigned Game::oldtime = 0;
Map Game::map;
int Game::window_w;
int Game::window_h;
double Game::eye_x;
double Game::eye_y;
double Game::eye_z;
double Game::view_pix = INIT_VIEW_PIX;
double Game::theta = 3.9375;
double Game::phi = 0.7875;
bool Game::static_view = true;
Teddy *Game::kuma;
Teddy::Shade Game::shade_type = Teddy::WIRE_ONLY;

int level = 0;
int TILE = 0;
int Light_Mode = 0;

extern GLfloat light0_position[];
extern GLfloat light0_ambient[];
extern GLfloat light0_specular[];
extern GLfloat light0_diffuse[];


extern GLfloat light0_ambient2[];
extern GLfloat light0_specular2[];
extern GLfloat light0_diffuse2[];

extern GLfloat light1_position[];
extern GLfloat light1_ambient[];
extern GLfloat light1_specular[];
extern GLfloat light1_diffuse[];
extern GLfloat light1_direction[];

extern GLuint	tile_id[];

static int timeGetTime() {
	static time_t init_sec = 0;
	timeval tv;
	gettimeofday(&tv, NULL);
	if (init_sec == 0) {
		init_sec = tv.tv_sec;
	}
	return (tv.tv_sec - init_sec) * 1000 + tv.tv_usec / 1000;
}

void Game::tile_set(){
	static int Normal[] = { 0, 0, 1 };

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, tile_id[TILE] );

	//glTranslatef( -(MAP_WIDTH * FIGURE_SIZE) / 2.0, -(MAP_HEIGHT * FIGURE_SIZE) / 2.0, -1.2 );

	glEnable(GL_NORMALIZE);
	glShadeModel( GL_FLAT );

	glBegin( GL_POLYGON );
	glNormal3iv(Normal);
	glTexCoord2f( 0,1 ), glVertex3f( 0, 0, 0 );
	glTexCoord2f( 1,1 ), glVertex3f( MAP_WIDTH * FIGURE_SIZE, 0, 0 );
	glTexCoord2f( 1,0 ), glVertex3f( MAP_WIDTH * FIGURE_SIZE, MAP_HEIGHT * FIGURE_SIZE, 0 );
	glTexCoord2f( 0,0 ), glVertex3f( 0, MAP_HEIGHT * FIGURE_SIZE, 0 );
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_NORMALIZE);


}

void draw_floor(){
	glColor3f( 1.0, 0, 0 );
	glBegin( GL_POLYGON );
		glVertex3f( -3, 0, 0 );
		glVertex3f( -3, -3, 0 );
		glVertex3f( 0, -3, 0 );
		glVertex3f( 0, 0, 0 );
	glEnd();
}

void Game::Display()
{
	if (state == INIT) return;

	glClearColor(BACK_R, BACK_G, BACK_B, 0.0);	// set Background color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	double teddy_x = 0, teddy_y = 0;	// character centered view 일 때 시점 계산하는 변수
	double teddy_status = kuma->getMovingStatus();
	if(teddy_moving == true) {
		switch(kuma->getDest()) {
		case Teddy::up:
			teddy_y = teddy_status / MAX_TILE_MOVESTATE - 1.0;
			break;
		case Teddy::down:
			teddy_y = -teddy_status / MAX_TILE_MOVESTATE + 1.0;
			break;
		case Teddy::left:
			teddy_x = -teddy_status / MAX_TILE_MOVESTATE + 1.0;
			break;
		case Teddy::right:
			teddy_x = teddy_status / MAX_TILE_MOVESTATE - 1.0;
			break;
		}
	}
	gluLookAt(eye_x + (static_view == false)*(FIGURE_SIZE * (cursor_x - MAP_WIDTH/2.0 + teddy_x)),
		eye_y + (static_view == false)*(FIGURE_SIZE * (cursor_y - MAP_WIDTH/2.0 + teddy_y)),
		eye_z,
		0.0 + (static_view == false)*(FIGURE_SIZE * (cursor_x - MAP_WIDTH/2.0 + teddy_x)),
		0.0 + (static_view == false)*(FIGURE_SIZE * (cursor_y - MAP_WIDTH/2.0 + teddy_y)),
		0.0,
		0.0, 0.0, 1.0 );

	glPushMatrix();
	glTranslatef( -(MAP_WIDTH * FIGURE_SIZE) / 2.0, -(MAP_HEIGHT * FIGURE_SIZE) / 2.0, -1.2 );
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	light1_position[0] = (cursor_x + teddy_x + 0.5)/2.5;
	light1_position[1] = (cursor_y + teddy_y + 0.5)/2.5;
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	tile_set();
	glPopMatrix();

	if(state == START) {
		kuma->Draw(false, shade_type);
		DrawMap();
	}
	else if(state == END) {
		kuma->Draw(true, shade_type);
		DrawMap();
	}


	glDisable(GL_DEPTH_TEST);
	Write_score();
	Write_combo();

	glutSwapBuffers();
}

void write_number( int score ){
	int temp = score;
	int temp2;
	int i = 0;
	int j;

	while( temp/10 != 0 ) temp /= 10, i++;
	temp = score;

	for( ; i >= 0 ; i-- ){
		j = i;
		temp2 = temp;
		while( j != 0 ) temp/= 10, --j;
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , char( '0' + temp ) );
		temp = temp2 - temp*pow(10, i);
	}
}

void write_words( const char *string ){
	while( *string != '\0' ){
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , *string );
		string++;
	}
}

void Game::Write_combo(){

	glLoadIdentity();
	if( state == Game::END ) glColor3f( ( rand()%256 / 255.0 ),( rand()%256 / 255.0 ),( rand()%256 / 255.0 ) );
	else glColor3f( 0.0, 1.0, 0.0 );

	glRasterPos2f( -2, 8 );
	write_words( "Max combo : " );
	write_number( map.show_combomx() );
	if( map.show_combo() < 2 ) return;

	if( map.show_combo() >= 10 && map.show_combo() < 20 ) glColor3f( 251/255.0, 216/255.0, 51/255.0 );
	else if( map.show_combo() >= 20 && map.show_combo() < 30) glColor3f( 255/255.0, 128/255.0, 0/255.0 );
	else if( map.show_combo() >= 30 ) glColor3f( 1.0, 0.0, 0.0 );
	else glColor3f( 0.5, 0.5, 0.5 );

	glRasterPos2f( -2, 9 );
	write_number( map.show_combo() );
	write_words( "Combo!!" );
}

void Game::Write_score(){
	static bool start = false;
	glLoadIdentity();
	if( work == false && state != Game::END){
		glColor3f( ( rand()%256 / 255.0 ),( rand()%256 / 255.0 ),( rand()%256 / 255.0 ) );
		glRasterPos2f( -4.0, 0.0 );

		if( start == false )write_words( "Press Space Bar to Start!! " );
		else write_words( "PAUSE - Press SpaceBar to Continue" );
	}
	if( start == false && work == true ) start = true;
	if( state == Game::END ){
		glColor3f( ( rand()%256 / 255.0 ),( rand()%256 / 255.0 ),( rand()%256 / 255.0 ) );
		glRasterPos2f( -2.0, 0.0 );

		write_words( "GAME OVER !! " );
	}
	else glColor3f( 0.5, 0.2, 0.0 );

	glRasterPos2f( -2, -8 );
	write_words( "Score : " );
	write_number( score );

	glRasterPos2f( -3, -9 );
	write_words(" Play Time : ");
	write_number((int)playtime);

	glColor3f( 0.5, 0.9, 0.3 );
	glRasterPos2f( -8, -8 );
	write_words( "LEVEL : " );

	if( level<20 ) write_number( level );
	else write_words( "HELL MODE" );
}



void Game::DrawTeddy(){
	kuma = new Teddy( cursor_x, cursor_y, Teddy::down );
}

void Game::Idle()
{
	if(work == false && state != END ) {
		Display();
		return;
	}

	switch(state) {
	case INIT:
		Initialize();
		Display();
		break;
	case START:
		RunGame();
		break;
	case END:
		Display();
		break;
	}
}

void Game::Reshape(int w, int h)
{
	window_w = w;
	window_h = h;

	glViewport(0, 0, w, h);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(-view_pix, view_pix, -view_pix, view_pix, -view_pix, view_pix);

	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity();
}

void Game::SpecialKey(int key, int x, int y)
{
	if(work == false) return;
	if( teddy_moving == true) return;	// while moving, ignore input

	switch(key) {
	case GLUT_KEY_UP:
		if(cursor_y >= MAP_HEIGHT-1) break;	// bound check
		if(map.EMPTY(cursor_x, cursor_y + 1) == true){	// figure collision check
			map.setDirection(cursor_x, cursor_y, Tile::UP);
			figure_moving = true;
		}
		kuma->set_dest( Teddy::up );
		teddy_moving = true;
		cursor_y++;
		break;
	case GLUT_KEY_DOWN:
		if(cursor_y <= 0) break;
		if(map.EMPTY(cursor_x, cursor_y - 1) == true){
			map.setDirection(cursor_x, cursor_y, Tile::DOWN);
			figure_moving = true;
		}
		kuma->set_dest( Teddy::down );
		teddy_moving = true;
		cursor_y--;
		break;
	case GLUT_KEY_LEFT:
		if(cursor_x <= 0) break;
		if(map.EMPTY(cursor_x - 1, cursor_y) == true){
			map.setDirection(cursor_x, cursor_y, Tile::LEFT);
			figure_moving = true;
		}
		kuma->set_dest( Teddy::left );
		teddy_moving = true;
		cursor_x--;
		break;
	case GLUT_KEY_RIGHT:
		if(cursor_x >= MAP_WIDTH-1) break;
		if(map.EMPTY(cursor_x + 1, cursor_y) == true){
			map.setDirection(cursor_x, cursor_y, Tile::RIGHT);
			figure_moving = true;
		}
		kuma->set_dest( Teddy::right );
		cursor_x++;
		teddy_moving = true;
		break;
	}

	Idle();
}

void Game::KeyInput(unsigned char key, int x, int y)
{
	if(key == ' ') work = !work;

	else if(key == 'v' || key == 'V') static_view = !static_view;
	else if(key == '+') view_pix -= 0.25;
	else if(key == '-') view_pix += 0.25;
	else if(key == 'm'){
		Light_Mode = !Light_Mode;
		if( Light_Mode ){
			glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient2);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse2);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular2);
		}
		else{
			glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
		}
	}
	else if(key == 't' || key == 'T') TILE++;

	else if(key == '1') shade_type = Teddy::WIRE_ONLY;
	else if(key == '2') shade_type = Teddy::WIRE;
	else if(key == '3') shade_type = Teddy::CONSTANT;
	else if(key == '4') shade_type = Teddy::GOURAUD;

	else if(key == 'd' || key == 'D') theta += VIEW_THETA;
	else if(key == 'a' || key == 'A') theta -= VIEW_THETA;
	else if(key == 'W' || key == 'w') phi += VIEW_PHI;
	else if(key == 's' || key == 'S') phi -= VIEW_PHI;

	if(phi >= LIMIT_PHI) phi = LIMIT_PHI;
	else if(phi <= -LIMIT_PHI) phi = -LIMIT_PHI;
	if(view_pix <= 2.5) view_pix = 2.5;
	if(TILE > 4) TILE = 0;

	eye_x = cos(theta) * cos(phi);
	eye_y = sin(theta) * cos(phi);
	eye_z = sin(phi);

	Reshape(window_w, window_h);

	Idle();
}

void Game::Initialize()
{
	int x, y;
	srand((unsigned)timeGetTime());

	map.initMap();

	for(int i = 0; i < INIT_FIGURES; i++) {
		do {
			x = rand() % MAP_WIDTH;
			y = rand() % MAP_HEIGHT;
		} while(map.EMPTY(x, y) != true);
		map.setTile(x, y );
	}

	cursor_x = x;	// set teddy's initial position
	cursor_y = y;

	eye_x = cos(theta) * cos(phi);
	eye_y = sin(theta) * cos(phi);
	eye_z = sin(phi);

	oldtime = timeGetTime();
	state = START;
	work = false;	// to start with space bar
	DrawTeddy();	// make teddy
}

void Game::RunGame()
{
	unsigned time = timeGetTime();
	if((time - oldtime) < TIMEFACTOR) return;
	oldtime = time;

	figure_moving = map.Update();
	if( teddy_moving == true ) teddy_moving = kuma->Move();

	int temp_score = map.ScoreUpdate();
	score += map.show_combo() * map.show_combo() * temp_score * temp_score;
	playtime += TIMEFACTOR / 1000.0;

	if(checkGameOver() == true) {
		state = END;
		return;
	}

	// generate firuges
	int x, y, loop_limit = (MAP_WIDTH * MAP_HEIGHT * 2);
	level = score / 5000;

	if(level > 20) level = 20;
	if(!(rand() % ( 2*(INIT_DIFFICULTY - level*400 )/100))) {
		do {
			x = rand() % MAP_WIDTH;
			y = rand() % MAP_HEIGHT;
			loop_limit--;	// to avoid too many loops
		} while(map.EMPTY(x, y) != true && loop_limit > 0);
		if(loop_limit > 0) map.setTile(x, y);
	}

	Display();
}

void Game::DrawMap()
{
	glColor3f( 0.0, 0.0, 0.0 );

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);

	if(Light_Mode){
		glEnable(GL_LIGHT1);
		glShadeModel( GL_SMOOTH );
	}
	else 	glShadeModel( GL_FLAT );


	glPushMatrix();
	glTranslatef(-(MAP_WIDTH * FIGURE_SIZE) / 2.0, -(MAP_HEIGHT * FIGURE_SIZE) / 2.0, 0);
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			if( map.EMPTY( j, i ) == true ) continue;
			glPushMatrix();
			setMatrixProperties(j, i);	// set the position and color
			map.draw_dice( j, i );
			glPopMatrix();
		}
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(Light_Mode) glDisable(GL_LIGHT1);

}

void Game::setMatrixProperties(int x, int y)
{
	glTranslatef(FIGURE_SIZE * (x + 0.5), FIGURE_SIZE * (y + 0.5), 0);
	/*if(map.isFlash(x, y) == true && map.isRed(x, y) == true)
		glColor3f(1, 0, 0);		// red
	else glColor3f(0, 0, 0);	// black*/
	if(map.isMoving(x, y) == true) {
		switch(map.getDirection(x, y)) {
		case Tile::UP:
			glTranslatef( 0, -1, -1 );
			glRotatef( 45*map.getMoveState(x,y) * FIGURE_SIZE / MAX_TILE_MOVESTATE, 1,0,0 );
			glTranslatef( 0, 1, 1 );
			break;
		case Tile::LEFT:
			glTranslatef( 1, 0, -1 );
			glRotatef( 45*map.getMoveState(x,y) * FIGURE_SIZE / MAX_TILE_MOVESTATE, 0,1,0 );
			glTranslatef( -1, 0, 1 );
			break;
		case Tile::DOWN:
			glTranslatef( 0, 1, -1 );
			glRotatef( -45*map.getMoveState(x,y) * FIGURE_SIZE / MAX_TILE_MOVESTATE, 1,0,0 );
			glTranslatef( 0, -1, 1 );
			break;
		case Tile::RIGHT:
			glTranslatef( -1, 0, -1 );
			glRotatef( -45*map.getMoveState(x,y) * FIGURE_SIZE / MAX_TILE_MOVESTATE, 0,1,0 );
			glTranslatef( 1, 0, 1 );
			break;
		}
	}
}

bool Game::checkGameOver()
{
	if( map.EMPTY( cursor_x, cursor_y ) == true )	return true;	// character밑에 firgure가 없을 때

	for(int i = 0; i < MAP_HEIGHT; i++)
		for(int j = 0; j < MAP_WIDTH; j++)
			if(map.EMPTY(j, i) == true || map.isFlash(j, i) == true)
			return false;	// 가득찼을 때
	return true;
}
