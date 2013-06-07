// map.cpp

#include "map.h"

extern int level;
extern GLuint texid[];

const GLubyte *errString;
GLenum errCode;

int	vertex[][3] ={
	{ 1, 1, 1 }, { 1, -1, 1 }, {-1, -1, 1 }, {-1, 1, 1}, { 1, 1, -1 }, { 1, -1, -1 }, { -1, -1, -1 }, {-1, 1, -1 }
};

int	face[][4] ={
	{ 4, 5, 6, 7 },  //bottom
	{ 1, 2, 6, 5 },  //south
	{ 0, 3, 7, 4 },  //north
	{ 2, 3, 7, 6 },	 //west
	{ 4, 5, 1, 0 },	 //east
	{ 0, 1, 2, 3 }  //top
};

int face_n[][3] = {	// face normal vectors
	{0, 0, -1},	// bottom
	{ 0, -1, 0},	// south
	{0, 1, 0},	// north
	{-1, 0, 0},		// west
	{1, 0, 0},	// east
	{0, 0, 1}	// top
};

void Map::initMap()	// it is used as Constructor
{
	// memory allocation and initialzing
	tile = new Tile*[MAP_HEIGHT];
	for(int i = 0; i < MAP_HEIGHT; i++) {
		tile[i] = new Tile[MAP_WIDTH];
		for(int j = 0; j < MAP_WIDTH; j++) {
			tile[i][j].isflash = false;
			tile[i][j].isred = false;
			tile[i][j].ismoving = false;
			tile[i][j].isempty = true;
			tile[i][j].movestate = MAX_TILE_MOVESTATE;
			tile[i][j].removestate = MAX_REMOVESTATE;
		}
	}
	max_combo = 0;
}

Map::~Map()
{
	if(tile != 0) {
		for(int i = 0; i < MAP_WIDTH; i++)
			if(tile[i] != 0) delete[] tile[i];
		delete[] tile;
	}
}

//주사위의 맨 위값을 리턴한다.
int Map::getDICEtop(int x, int y)
{
	if( tile[y][x].isempty != true )
	return tile[y][x].num[top];
	else return 0;
}

//지정된 타일에 주사위를 생성한다.ㅣ
void Map::setTile(int x, int y )
{
	int temp1, temp2, temp3;
	tile[y][x].isempty = false;
	temp1 = rand()%6 + 1;
	tile[y][x].num[bottom] = temp1;
	tile[y][x].num[top] = 7-temp1;

	temp2 = rand()%6 + 1;
	while( temp1 == temp2 || temp1 == 7 - temp2 )temp2 = rand()%6 + 1;
	tile[y][x].num[north] = temp2;
	tile[y][x].num[south] = 7-temp2;
	
	temp3 = rand()%6 + 1;			//주사위 눈은 랜덤이되, 앞에 이미 생성한것과는 달라야 한다.
	while( temp1 == temp3||temp1 == 7 - temp3 || temp2 == temp3 || temp2 == 7-temp3)
		temp3 = rand()%6 + 1;
	tile[y][x].num[west] = temp3;
	tile[y][x].num[east] = 7-temp3;
}

void Map::moveTile(int src_x, int src_y, int dst_x, int dst_y)
{
	if(src_x < 0 || src_x >= MAP_WIDTH) return;
	if(dst_x < 0 || dst_x >= MAP_WIDTH) return;
	if(src_y < 0 || src_y >= MAP_HEIGHT) return;
	if(dst_y < 0 || dst_y >= MAP_HEIGHT) return;

	tile[dst_y][dst_x] = tile[src_y][src_x];	// 타일을 덮어 씀
	
	// 원본 타일은 초기화, except type
	tile[src_y][src_x].isflash = false;
	tile[src_y][src_x].ismoving = false;
	tile[src_y][src_x].isred = false;
	tile[src_y][src_x].isempty = true;
	tile[src_y][src_x].movestate = MAX_TILE_MOVESTATE;
	tile[src_y][src_x].removestate = MAX_REMOVESTATE;
}

bool Map::isFlash(int x, int y)
{
	return tile[y][x].isflash;
}

bool Map::isRed(int x, int y)
{
	return tile[y][x].isred;
}
bool Map::EMPTY(int x, int y)
{
	return tile[y][x].isempty;
}
bool Map::isMoving(int x, int y)
{
	return tile[y][x].ismoving;
}

Tile::Direction Map::getDirection(int x, int y)
{
	return tile[y][x].direction;
}

int Map::getMoveState(int x, int y)
{
	return tile[y][x].movestate;
}
//주사위가 이동하면 그 이동에 맞추어 주사위 눈의 값을 바꾼다.
//ex 주사위 top값은 위로 이동하면 north의 값이 된다.
void Map::setDirection(int x, int y, Tile::Direction direction)
{
	int bottom_temp = tile[y][x].num[bottom];
	int	top_temp = tile[y][x].num[top];
	int src_x = x, src_y = y;
	switch(direction) {
	case Tile::UP:
		tile[y][x].num[bottom] = tile[y][x].num[north];
		tile[y][x].num[top] = tile[y][x].num[south];
		tile[y][x].num[south] = bottom_temp;
		tile[y][x].num[north] = top_temp;
		y++;
		break;
	case Tile::DOWN:
		tile[y][x].num[bottom] = tile[y][x].num[south];
		tile[y][x].num[top] = tile[y][x].num[north];
		tile[y][x].num[south] = top_temp;
		tile[y][x].num[north] = bottom_temp;
		y--;
		break;
	case Tile::LEFT:
		tile[y][x].num[bottom] = tile[y][x].num[west];
		tile[y][x].num[top] = tile[y][x].num[east];
		tile[y][x].num[west] = top_temp;
		tile[y][x].num[east] = bottom_temp;
		x--;
		break;
	case Tile::RIGHT:
		tile[y][x].num[bottom] = tile[y][x].num[east];
		tile[y][x].num[top] = tile[y][x].num[west];
		tile[y][x].num[east] = top_temp;
		tile[y][x].num[west] = bottom_temp;
		x++;
		break;
	}
	moveTile(src_x, src_y, x, y);
	tile[y][x].direction = direction;
	tile[y][x].ismoving = true;
}

bool Map::Update()
{
	bool flag = false;	// return value
	addscore = 0;	// score to add

	for(int i = 0; i < MAP_HEIGHT; i++)
		for(int j = 0; j < MAP_WIDTH; j++) {
			if(tile[i][j].ismoving == true){
				tile[i][j].movestate--;
				if(tile[i][j].movestate <= 0) {
					tile[i][j].movestate = MAX_TILE_MOVESTATE;
					tile[i][j].ismoving = false;
				}
				else flag = true;
			}
		}

	for(i = 0; i < MAP_HEIGHT; i++)
		for(int j = 0; j < MAP_WIDTH; j++) {
			int tempnumber = 1;
			if(tile[i][j].isempty != true  && AdjacencyTest(j, i, tile[i][j].num[top], Tile::NONE, tempnumber) == true)
				tile[i][j].isflash = true;	// 같은 눈이 4개 모였을 경우

			if(tile[i][j].isflash == true) {
				tile[i][j].isred = !(tile[i][j].isred);
				tile[i][j].removestate--;
			}

			if(tile[i][j].removestate - level/2 <= 0) {	// remove figures
				tile[i][j].isflash = false;
				tile[i][j].isred = false;
				tile[i][j].ismoving = false;
				tile[i][j].movestate = MAX_TILE_MOVESTATE;
				tile[i][j].isempty = true;
				tile[i][j].removestate = MAX_REMOVESTATE;
				addscore++;
				is_combo = COMBO_TERM;
				combo++;
				if( combo > max_combo ) max_combo = combo;
			}
		}

	is_combo--;
	if( is_combo == 0 ) combo = 0;
	return flag;
}

bool Map::AdjacencyTest(int x, int y, int top_value, Tile::Direction from, int& count)
{
	if(tile[y][x].isempty == true )
		return false;
	if(tile[y][x].ismoving == true) return false;
	if(count >= 4) return true;

	if(from != Tile::RIGHT && (x < MAP_WIDTH-1) && tile[y][x+1].isempty == false && tile[y][x+1].num[top] == top_value && tile[y][x+1].ismoving == false) {
		count++;
		AdjacencyTest(x+1, y, top_value, Tile::LEFT, count);
	}
	if(from != Tile::UP && (y < MAP_HEIGHT-1) && tile[y+1][x].isempty == false && tile[y+1][x].num[top] == top_value && tile[y+1][x].ismoving == false) {
		count++;
		AdjacencyTest(x, y+1, top_value, Tile::DOWN, count);
	}
	if(from != Tile::LEFT && (x > 0) && tile[y][x-1].isempty == false && tile[y][x-1].num[top] == top_value && tile[y][x-1].ismoving == false) {
		count++;
		AdjacencyTest(x-1, y, top_value, Tile::RIGHT, count);
	}
	if(from != Tile::DOWN && (y > 0) && tile[y-1][x].isempty == false && tile[y-1][x].num[top] == top_value && tile[y-1][x].ismoving == false) {
		count++;
		AdjacencyTest(x, y-1, top_value, Tile::UP, count);
	}

	if(count >= 4) return true;
	else return false;
}

int Map::ScoreUpdate()
{
	return addscore;
}

int Map::show_combo(){
	return combo;
}
int Map::show_combomx(){
	return max_combo;
}

void Map::draw_dice( int x, int y ){
		
	static GLfloat surface_ambient[] = {0.7, 0.8, 0.9, 1.0};
	static GLfloat surface_ambient0[] = {1.0, 0.0, 0.0, 1.0};
	static GLfloat surface_specular[] = {0.7, 0.5, 0.6, 1.0};
	static GLfloat surface_specular0[] = {0.0, 0.0, 0.0, 1.0};
	static GLfloat surface_diffuse[] = {0.7, 0.7, 1.0, 0.0};
	static GLfloat surface_diffuse0[] = { 1.0, 0, 0, 0 };

	for( int  i = 0; i < 6; i++ ){

		//주사위가 반짝거리는 경우에 주사위의 빛에 대한 성질을 바꾸어서 이를 표현한다.
		if( tile[y][x].isflash == true && tile[y][x].isred == true) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, surface_ambient0);
			glMaterialfv(GL_FRONT, GL_SPECULAR, surface_specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, surface_diffuse0);
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, surface_ambient);
			glMaterialfv(GL_FRONT, GL_SPECULAR, surface_specular0);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, surface_diffuse);
		}
		glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
		
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texid[ tile[y][x].num[i]-1 ] );	//bind되는 texture는 그 면의 값에 의해 결정

		glBegin( GL_POLYGON );
		glNormal3iv(face_n[i]);
        /*glNormal3iv(vertex[face[i][0]]),*/ glTexCoord2f( 0,0 ), glVertex3iv(vertex[face[i][0]]);
		/*glNormal3iv(vertex[face[i][1]]),*/ glTexCoord2f( 1,0 ), glVertex3iv(vertex[face[i][1]]);
		/*glNormal3iv(vertex[face[i][2]]),*/ glTexCoord2f( 1,1 ), glVertex3iv(vertex[face[i][2]]);
		/*glNormal3iv(vertex[face[i][3]]),*/ glTexCoord2f( 0,1 ), glVertex3iv(vertex[face[i][3]]);
		glEnd();

		// initialze again
		glMaterialfv(GL_FRONT, GL_AMBIENT, surface_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, surface_specular0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, surface_diffuse);
	}
}