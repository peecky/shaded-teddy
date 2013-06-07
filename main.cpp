// main.cpp

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>

#include "game.h"

#define MAX_LOD 6

//texture는 PPM 데이터를 이용한다.
typedef struct _PPMdata{
	int imagewidth;
	int imageheight;
	GLubyte* data;
} PPMdata;

PPMdata	image[6];
GLuint	texid[6];
PPMdata	tile[5];
GLuint	tile_id[5];


static GLfloat z = 0;
static GLfloat x = 0;
static GLfloat y = 0;

static char readbuf[80];
static char word[80];
static char* ptr = readbuf;
static char* ptr2 = word;
static FILE* texturefile;

//빛의 성질을 결정해 주는 값들
GLfloat light0_position[] = {0.0, 0.0, 10.0, 0.2};
GLfloat light0_ambient[] = {0.7, 0.4, 0.7, 0.2};
GLfloat light0_specular[] = {0.6, 0.7, 0.6, 0.2};
GLfloat light0_diffuse[] = {0.5, 0.6, 0.6, 0.2};

GLfloat light0_ambient2[] = {0.3,0.2, 0.1, 0.2};
GLfloat light0_specular2[] = {0.1, 0.1, 0.1, 0.2};
GLfloat light0_diffuse2[] = {0.1, 0.1, 0.1, 0.2};

GLfloat light1_position[] = {0.0, 0.0, 10.0, 0.2};
GLfloat light1_ambient[] = {1.0, 0.4, 0.7, 0.2};
GLfloat light1_specular[] = {1.0, 0.7, 0.6, 0.2};
GLfloat light1_diffuse[] = {1.0, 0.6, 0.6, 0.2};
GLfloat light1_direction[] = {0.0, 0.0, -1};

//get_word, LoadAsCIIPPMImage에서 PPM파일을 읽어 들이고
//texture로 사용할 수 있는 상태로 만든다.
char* get_word( FILE * texturefile ){
	while(1){
		if(*ptr == '\0'){
			fgets(readbuf, 80, texturefile);
			while(*readbuf == '#'){
				fgets(readbuf, 80, texturefile);
			}
			ptr = readbuf;
			continue;
		}else if(isspace(*ptr)){
			ptr++;
			continue;
		}else{
			break;
		}
	}
	ptr2 = word;
	while(!isspace(*ptr)){
		*ptr2++ = *ptr++;
	}
	*ptr2 = '\0';
	return word;
}

PPMdata* LoadASCIIPPMImage(const char* filename, PPMdata* ppm)
{
	int i, j, isize, jsize;
	GLubyte* data;
	FILE* texturefile;

	if( NULL == (texturefile = fopen(filename, "r"))){
		fprintf(stderr, "Cannot open texure file %s\n", filename);
		return NULL;
	}

	if(strcmp("P3", get_word(texturefile)) != 0 ){
		fprintf(stderr, "Texure file %s is not an ASCII PPM format\n", filename);
		return NULL;
	}

	isize = atoi(get_word(texturefile));

	if ( 2<<(MAX_LOD-1) != isize ){
		fprintf(stderr, "Width of texture filef %s is not %d.\n", filename, 2<<(MAX_LOD-1));
		return NULL;
	}

	jsize = atoi(get_word(texturefile));
	if ( 2<<(MAX_LOD-1) != jsize ){
		fprintf(stderr, "Height of texture filef %s is not %d.\n", filename, 2<<(MAX_LOD-1));
		return NULL;
	}

	if(255 != atoi(get_word(texturefile))){
		fprintf(stderr, "Color component in %s does not fit in a byte.\n",
            filename);
		return NULL;
	}

	if(NULL == (data = (GLubyte*)malloc(3*isize*jsize*sizeof(GLubyte)))){
		fprintf(stderr, "Cannot allocate memory for texture data\n");
		return NULL;
	}

	ppm->imagewidth = jsize;
	ppm->imageheight = isize;
	ppm->data = data;

	for (i = 0; i < isize; i++) {
		for (j = 0; j < jsize; j++) {
			*(data + 3*(jsize*i + j)) = (GLubyte) atoi(get_word(texturefile));
			*(data + 3*(jsize*i + j)+1) = (GLubyte) atoi(get_word(texturefile));
			*(data + 3*(jsize*i + j)+2) = (GLubyte) atoi(get_word(texturefile));
		}
	}

	fclose(texturefile);
	return ppm;
}

//초기 설정
void init_gl_settings(void){
	int i;

	//필요한 그림 파일을 읽어들인다.
	if(NULL == LoadASCIIPPMImage("1.ppm", &image[0])) exit(0);
	if(NULL == LoadASCIIPPMImage("2.ppm", &image[1])) exit(0);
	if(NULL == LoadASCIIPPMImage("3.ppm", &image[2])) exit(0);
	if(NULL == LoadASCIIPPMImage("4.ppm", &image[3])) exit(0);
	if(NULL == LoadASCIIPPMImage("5.ppm", &image[4])) exit(0);
	if(NULL == LoadASCIIPPMImage("6.ppm", &image[5])) exit(0);

	if(NULL == LoadASCIIPPMImage("tile1.ppm", &tile[0])) exit(0);
	if(NULL == LoadASCIIPPMImage("tile2.ppm", &tile[1])) exit(0);
	if(NULL == LoadASCIIPPMImage("tile3.ppm", &tile[2])) exit(0);
	if(NULL == LoadASCIIPPMImage("tile4.ppm", &tile[3])) exit(0);
	if(NULL == LoadASCIIPPMImage("tile5.ppm", &tile[4])) exit(0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures( 6, texid );
	glGenTextures( 6, tile_id );	//텍스쳐 생성

	glShadeModel(GL_FLAT);
	for( i = 0; i < 6 ; i++ ){
		glBindTexture(GL_TEXTURE_2D, texid[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 2<<(MAX_LOD-1),
			2<<(MAX_LOD-1), 0, GL_RGB, GL_UNSIGNED_BYTE, image[i].data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	for( i = 0 ; i < 5 ; i++ ){
		glBindTexture(GL_TEXTURE_2D, tile_id[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 2<<(MAX_LOD-1),
			2<<(MAX_LOD-1), 0, GL_RGB, GL_UNSIGNED_BYTE, tile[i].data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	glEnable(GL_TEXTURE_2D);

	//라이트의 초기 설정을 해준다.
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction );
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 3.5 );
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 128 );
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction );

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
}


int main(int argc, char* argv[])
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Shaded Teddy Block");
	glutReshapeFunc(Game::Reshape);
	glutIdleFunc(Game::Idle);
	glutDisplayFunc(Game::Display);
	glutSpecialFunc(Game::SpecialKey);
	glutKeyboardFunc(Game::KeyInput);
	init_gl_settings();
	glutMainLoop();

	return 0;
}
