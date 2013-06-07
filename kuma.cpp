// kuma.cpp

#include "kuma.h"
#include "game.h"

extern int Light_Mode;
extern GLfloat light1_position[];

Teddy::Teddy( int X, int Y, dest D )
{
	x = X;
	y = Y;
	where = D;
	count();
	status = 0;

	// make callist
	int i;
	glNewList( Ted, GL_COMPILE);
	for (i = 0; i < face_num; i++) {
		glBegin( GL_POLYGON );
		glVertex3dv(vertex[face[i][0]]);
		glVertex3dv(vertex[face[i][1]]);
		glVertex3dv(vertex[face[i][2]]);
		glEnd();
	}
	glEndList();

	glNewList(VNormalTed, GL_COMPILE);
	for(i = 0; i < face_num; i++) {
		glBegin(GL_POLYGON);
		glNormal3dv(vertex_n[face[i][0]]);
		glVertex3dv(vertex[face[i][0]]);
		glNormal3dv(vertex_n[face[i][1]]);
		glVertex3dv(vertex[face[i][1]]);
		glNormal3dv(vertex_n[face[i][2]]);
		glVertex3dv(vertex[face[i][2]]);
		glEnd();
	}
	glEndList();

	glNewList(FNormalTed, GL_COMPILE);
	for(i = 0; i < face_num; i++) {
		glBegin(GL_POLYGON);
		glNormal3dv(normal_v[i]);
		glVertex3dv(vertex[face[i][0]]);
		glVertex3dv(vertex[face[i][1]]);
		glVertex3dv(vertex[face[i][2]]);
		glEnd();
	}
	glEndList();
}

bool Teddy::Move(){
	if( where == up )
		y += 1.0/MAX_TILE_MOVESTATE;
	else if( where == down )
		y -= 1.0/MAX_TILE_MOVESTATE;
	else if( where == left )
		x -= 1.0/MAX_TILE_MOVESTATE;
	else if( where == right )
		x += 1.0/MAX_TILE_MOVESTATE;

	status++;
	if( status == MAX_TILE_MOVESTATE ){
		status = 0;
		return false;
	}
	return true;
}

void Teddy::set_dest( dest New ){
	where = New;
}

void Teddy::Draw(bool isgameover, Shade shadetype)
{
	glPushMatrix();

	glTranslatef(-(MAP_WIDTH * FIGURE_SIZE) / 2.0, -(MAP_HEIGHT * FIGURE_SIZE) / 2.0, 0);
	
	glRotatef( 90, 1, 0 ,0 );
	if( where == down ){
		glTranslatef(FIGURE_SIZE * (x + 0.5), FIGURE_SIZE, -FIGURE_SIZE * (y + 0.5) );
	}
	else if( where == up ){
		glRotatef( 180, 0, 1, 0 );
		glTranslatef(-FIGURE_SIZE * (x + .5), FIGURE_SIZE, FIGURE_SIZE * (y + 0.5) );
	}
	else if( where == left ){
		glRotatef( -90, 0, 1, 0 );
		glTranslatef(-FIGURE_SIZE * (y + .5), FIGURE_SIZE, -FIGURE_SIZE * (x + 0.5) );
	}
	else{
		glRotatef( 90, 0, 1, 0 );
		glTranslatef(FIGURE_SIZE * (y + 0.5), FIGURE_SIZE, FIGURE_SIZE * (x + 0.5) );
	}

	if(isgameover == true) {
		static int windmill = 0;
		windmill += 5;
		if(windmill >= 360) windmill = 0;
		glRotatef(windmill, 0, 1, 0);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	switch (shadetype) {
	default:
	case WIRE_ONLY:
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glColor3f(BACK_R, BACK_G, BACK_B);
		glCallList( Ted );
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, .0, .0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glCullFace(GL_BACK);
		glCallList(Ted);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case WIRE:
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		if(Light_Mode)glEnable(GL_LIGHT1);
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glCallList( FNormalTed );
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		if(Light_Mode)glDisable(GL_LIGHT1);

		glColor3f(1.0, .0, .0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glCullFace(GL_BACK);
		glCallList(Ted);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case CONSTANT:
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		if(Light_Mode)glEnable(GL_LIGHT1);

		glShadeModel(GL_FLAT);
		glCullFace(GL_BACK);
		glCallList(FNormalTed);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		if(Light_Mode)glDisable(GL_LIGHT1);

		break;
	case GOURAUD:
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		if(Light_Mode)glEnable(GL_LIGHT1);

		glShadeModel(GL_SMOOTH);
		glCullFace(GL_BACK);
		glCallList(VNormalTed);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		if(Light_Mode)glDisable(GL_LIGHT1);

		glShadeModel(GL_FLAT);
		break;
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPopMatrix();
}

void Teddy::count()
{
	std::ifstream obj;
	int number=0;
	char check;
	double temp;
	int temp2;
	
	obj.open("teddy.obj", ios::in);
	if(!obj) {
		//MessageBox(NULL, "Cannot find teddy.obj", "Error", MB_ICONERROR | MB_OK);
		std::cerr << "Cannot find teddy.obj" << std::endl;
		exit(0);
	}
	
	//count the number of vertex
	while(1){	
		obj>>check;
		if( check == 'v' ){
			number++;
			obj>>temp;
			obj>>temp;
			obj>>temp;
		}
		else break;	
	}
	obj.close();
	
	obj.open( "teddy.obj", ios::in );
	vertex = new GLdouble * [number];
	vertex_n = new GLdouble*[number];
	
	//store the position of vertex
	int i, j;
	for( i = 0; i < number ; i++ ) {
		vertex[i] = new GLdouble [6];
		obj>>check;
		obj>>vertex[i][0];
		obj>>vertex[i][1];
		obj>>vertex[i][2];

		vertex[i][3] = vertex[i][0] + 1.0;
		vertex[i][4] = vertex[i][1] + 1.0;
		vertex[i][5] = vertex[i][2];
		GLdouble abs_vec = vertex[i][3] * vertex[i][3]
			+ vertex[i][4] * vertex[i][4]
			+ vertex[i][5] * vertex[i][5];
		abs_vec = sqrt(abs_vec);
		vertex[i][3] /= abs_vec;
		vertex[i][4] /= abs_vec;
		vertex[i][5] /= abs_vec;

		vertex_n[i] = new GLdouble[3];
		vertex_n[i][0] = 0;
		vertex_n[i][1] = 0;
		vertex_n[i][2] = 0;
	}
	vertex_num = number;
	
	number = 0;
	//do same thing on the face also
	while(1){	
		if( obj>>check ){
			number++;
			obj>>temp2;
			obj>>temp2;
			obj>>temp2;
		}
		else break;	
	}
	obj.close();
	
	obj.open( "teddy.obj", ios::in );
	face = new int * [number];
	normal_v = new GLdouble*[number];
	
	while(1){
		obj>>check;
		if( check == 'f' )
			break;
		else{
			obj>>temp;
			obj>>temp;
			obj>>temp;
		}
	}
	
	for( i = 0; i < number ; i++ ){
		face[i] = new int [3];
		
		obj>>face[i][0];
		obj>>face[i][1];
		obj>>face[i][2];
		obj>>check;
		face[i][0]--;
		face[i][1]--;
		face[i][2]--;

		normal_v[i] = new GLdouble[3];
		GLdouble a[3], b[3];
		
		// cross product
		for(j = 0; j < 3; j++) {
			a[j] = vertex[face[i][0]][j] - vertex[face[i][1]][j];
			b[j] = vertex[face[i][0]][j] - vertex[face[i][2]][j];
		}
		normal_v[i][0] = a[1] * b[2] - b[1] * a[2];
		normal_v[i][1] = a[2] * b[0] - b[2] * a[0];
		normal_v[i][2] = a[0] * b[1] - b[0] * a[1];

		// unitize
		GLdouble abs_vector = normal_v[i][0] * normal_v[i][0]
			+ normal_v[i][1] * normal_v[i][1]
			+ normal_v[i][2] * normal_v[i][2];
		abs_vector = sqrt(abs_vector);
		normal_v[i][0] /= abs_vector;
		normal_v[i][1] /= abs_vector;
		normal_v[i][2] /= abs_vector;

		for(j = 0; j < 3; j++) {
			vertex_n[face[i][0]][j] += normal_v[i][j];
			vertex_n[face[i][1]][j] += normal_v[i][j];
			vertex_n[face[i][2]][j] += normal_v[i][j];
		}
	}
	face_num = number;

	obj.close();

	// unitizing vertex normal vectors
	for( i = 0; i < vertex_num; i++) {
		GLdouble abs_vec_temp = vertex_n[i][0] * vertex_n[i][0]
			+ vertex_n[i][1] * vertex_n[i][1]
			+ vertex_n[i][2] * vertex_n[i][2];
		abs_vec_temp = sqrt(abs_vec_temp);
		vertex_n[i][0] /= abs_vec_temp;
		vertex_n[i][1] /= abs_vec_temp;
		vertex_n[i][2] /= abs_vec_temp;
	}
}

Teddy::~Teddy()
{
	int i;
	for(i = 0; i < face_num; i++) {
		delete face[i];
		delete normal_v[i];
	}
	delete face;
	delete normal_v;

	for(i = 0; i < vertex_num; i++) {
		delete vertex[i];
		delete vertex_n[i];
	}
	delete vertex;
	delete vertex_n;
}

Teddy::dest Teddy::getDest()
{
	return where;
}

int Teddy::getMovingStatus()
{
	return status;
}
