// kuma.h

#ifndef KUMA_H
#define KUMA_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <cmath>
#include <fstream>
#include <iostream>

#define Ted 1	// simple polygon
#define VNormalTed 2	// polygon with vertex normal vector
#define FNormalTed 3	// polygon with face normal vector

class Teddy{

public:	 
	enum dest{up, down, right, left};
	enum Shade {WIRE_ONLY, WIRE, CONSTANT, GOURAUD};
	Teddy( int, int, dest );	// make call list of character
	~Teddy();
	void Draw(bool isgameover = false, Shade shadetype = WIRE_ONLY);
	void set_dest( dest );
	bool Move();	// update x, y position of chararcter and status
	dest getDest();
	int getMovingStatus();
private:
	void count();	// read the file "teddy.obj"
	float x, y;	// position of the character
	int status;	// move status
	dest where;
	GLdouble **vertex;
	GLdouble **vertex_n;	// unit normal vector of each vertex
	int **face;
	int face_num;
	int vertex_num;
	GLdouble **normal_v;	// unit normal vector of each faces
};

#endif
