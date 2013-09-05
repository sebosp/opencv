/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/glut.h>
#include "shader_utils.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795

GLuint vbo_vertices;
GLuint ibo_elements;
GLuint program;
GLint attribute_coord2d;
int sides = 7;

GLfloat* generateNGon(float radius){
	int steps = 360/sides;
	GLfloat* circlePoints = new GLfloat[2*sides];
	int curpos = -1;
	for (int angle = 0; angle < 360; angle+=steps){
		circlePoints[++curpos]=(cos(angle*PI/180)*radius);
		circlePoints[++curpos]=(sin(angle*PI/180)*radius);
	}
	return circlePoints;
}
 

int init_resources(){
  GLfloat *vertices = generateNGon(0.8);
  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sides*2, vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
 
  GLushort elements[sides];
  for(int i=0;i<sides;i++)elements[i]=i;
  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLint link_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader("triangle.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = create_shader("triangle.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(program);
    return 0;
  }
  const char* attribute_name = "coord2d";
  attribute_coord2d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord2d == -1){
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }
  return 1;
}

void onDisplay(){
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(program);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glEnableVertexAttribArray(attribute_coord2d);
  glVertexAttribPointer(
    attribute_coord2d, // attribute
    2,                 // number of elements per vertex, here (x,y)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // no extra data between each position
    0  		       // Offset
  );
 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glDrawElements(GL_LINE_LOOP, sides,GL_UNSIGNED_SHORT,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
  glDisableVertexAttribArray(attribute_coord2d);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glutSwapBuffers();
}

void free_resources(){
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_vertices);
  glDeleteBuffers(1, &ibo_elements);
}

void Keyboard(unsigned char key, int x, int y){
	switch (key){
		case 27:
			free_resources();
			exit (0);
		break;
	}
}

int main(int argc, char* argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("NGon");

  GLenum glew_status = glewInit();
  glutKeyboardFunc (Keyboard);
  if (glew_status != GLEW_OK){
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }

  if (!GLEW_VERSION_2_0) {
    fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
    return 1;
  }

  if (init_resources()) {
    glutDisplayFunc(onDisplay);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();
  }

  free_resources();
  return EXIT_SUCCESS;
}

