#include <windows.h>
#include <gl/glut.h>
#include <gl/gl.h>

#include <fstream.h>
#include <iostream.h>

#define Ted 0

enum dest{
	no_move, up, down, right, left
};

class Teddy{

	int x, y;	
	dest where;	
	GLdouble **vertex;
	int **face;
	int face_num;

public:
	Teddy( int, int );	 
	void count();	
	void Draw();
	void move( dest );	
};

Teddy::Teddy( int X, int Y ){
	x = X;
	y = Y;
	count();
		
/*	glNewList( Ted, GL_COMPILE);
		int i;
		glBegin( GL_LINES );
				glColor3f(1.0, 0.0, 0.0);
		for (i = 0; i < face_num; i++) {
			glVertex3dv(vertex[face[i][0]]);
			glVertex3dv(vertex[face[i][1]]);
			glVertex3dv(vertex[face[i][1]]);
			glVertex3dv(vertex[face[i][2]]);
			glVertex3dv(vertex[face[i][2]]);
			glVertex3dv(vertex[face[i][0]]);
		cout<<"I'm called"<<endl;
		}
		glEnd();
	glEndList();
*/
}

void Teddy::Draw(){
		glBegin( GL_LINES );
				glColor3f(1.0, 0.0, 0.0);
		for (int i = 0; i < face_num; i++) {
			glVertex3dv(vertex[face[i][0]]);
			glVertex3dv(vertex[face[i][1]]);
			glVertex3dv(vertex[face[i][1]]);
			glVertex3dv(vertex[face[i][2]]);
			glVertex3dv(vertex[face[i][2]]);
			glVertex3dv(vertex[face[i][0]]);
		cout<<"I'm called"<<endl;
		}
		glEnd();
}

void Teddy::count(){

	ifstream *obj;
	int number=0;
	char check;
	double temp;
	int temp2;
	
	obj = new ifstream;
	obj->open("teddy.obj");
	
	//count the number of vertex
	while(1){	
		
		cout<<number<<endl;
		*obj>>check;
		if( check == 'v' ){
			number++;
			*obj>>temp;
			//cout<<temp<<endl;
			*obj>>temp;
			//cout<<temp<<endl;
			*obj>>temp;
			//cout<<temp<<endl;
		}
		
		else break;	
	}
	
	delete obj;
	
	obj = new ifstream;
	obj->open( "teddy.obj" );
	
	vertex = new GLdouble * [number];
	
	//store the position of vertex
	int i;
	for( i = 0; i < number ; i++ ){
		
		vertex[i] = new GLdouble [3];
		*obj>>check;
		*obj>>vertex[i][0];
		*obj>>vertex[i][1];
		*obj>>vertex[i][2];
	}
	
	cout<<"vertex   "<<number<<endl;
	number = 0;
	
	//do same thing on the face also
	while(1){	
		if( *obj>>check ){
			number++;
			*obj>>temp2;
			//cout<<temp<<endl;
			*obj>>temp2;
			//cout<<temp<<endl;
			*obj>>temp2;
			//cout<<temp<<endl;
		}
		else break;	
	}
	cout<<"face "<<number<<endl;
	
	delete obj;
	
	obj = new ifstream;
	obj->open( "teddy.obj" );
	
	face = new int * [number];
	
	while(1){
		*obj>>check;
		if( check == 'f' )
			break;
		else{
			*obj>>temp;
			*obj>>temp;
			*obj>>temp;
		}
	}
	
	for( i = 0; i < number ; i++ ){
		
		face[i] = new int [3];
		
		*obj>>face[i][0];
		*obj>>face[i][1];
		*obj>>face[i][2];
		*obj>>check;
		face[i][0]--;
		face[i][1]--;
		face[i][2]--;
		//cout<<i<<endl;
	}
	face_num = number;
}
