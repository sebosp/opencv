
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define _DEBUG
using namespace std;

#define BUFFER_OFFSET(x)  ((const void*) (x))
//#include "LoadShaders.h"
#include <GL/glew.h> 
#include <GL/freeglut.h>
#include "shader_utils.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum VAO_IDs{Triangles, NumVAOs};
enum Buffer_IDs{ArrayBuffer, NumBuffers};
enum Attrib_IDs{vPosition=0};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint program;

const GLuint NumVertices = 6;

void init(void){
	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = create_shader("ch01.v.glsl", GL_VERTEX_SHADER))   == 0) return;
	if ((fs = create_shader("ch01.f.glsl", GL_FRAGMENT_SHADER)) == 0) return;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		print_log(program);
		return;
	}
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	GLfloat vertices[NumVertices][2] = {
		{ -0.90, -0.90 },
		{  0.85, -0.90 },
		{ -0.90,  0.85 },
		{  0.90, -0.85 },
		{  0.90,  0.90 },
		{ -0.85,  0.90 },
	};
	glGenBuffers(NumBuffers,Buffers);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	glUseProgram(program);
	glVertexAttribPointer(vPosition,2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}
void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	//glBindVertexArray(VAOs[Triangles]);
	//glDrawArrays(GL_TRIANGLES,0,NumVertices);
	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ; // save
	glLoadIdentity();// and clear
	glMatrixMode( GL_MODELVIEW ) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	glDisable( GL_DEPTH_TEST ) ; // also disable the depth test so renders on top

	glRasterPos2f(0,0); // center of screen. (-1,0) is center left.
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	char buf[300];
	sprintf( buf, "Oh hello" );
	const char * p = buf;
	do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);while(*(++p));

	glEnable(GL_DEPTH_TEST); // Turn depth testing back on

	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // revert back to the matrix I had before.
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glFlush();
}

int main(int argc, char* argv[]){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512,512);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	printf("Got opengl version: %s\n", glGetString(GL_VERSION));
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}
