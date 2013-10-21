
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
	}else{
		printf("Linking done.\n");
	}
	printf("Attempting glGenVertexArrays\n");
	glGenVertexArrays(NumVAOs, VAOs);
	printf("Done glGenVertexArrays\n");
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
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES,0,NumVertices);
	glFlush();
}

int main(int argc, char* argv[]){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512,512);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}
