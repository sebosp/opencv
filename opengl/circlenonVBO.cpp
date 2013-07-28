#include <stdio.h>
#include <stdlib.h>
#include <iostream>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/glut.h>
#include "shader_utils.h"
#include <math.h>

#define PI 3.14159265

GLfloat remnants2D[6];
GLuint program;
GLint attribute_coord2d;
void addRemnants2D(float newx, float newy){
	for(int i=0;i<4;i++){//Shift
		remnants2D[i]=remnants2D[i+2];
	}
	remnants2D[4]=newx;
	remnants2D[5]=newy;
  	glVertexAttribPointer(
	    attribute_coord2d, // attribute
	    2,                 // number of elements per vertex, here (x,y)
	    GL_FLOAT,          // the type of each element
	    GL_FALSE,          // take our values as-is
	    0,                 // no extra data between each position
	    remnants2D  // pointer to the C array
	);
  	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int generateNGon(float radius,float strokewidth,int sides){
	float r1 = radius-(strokewidth/2);
	float r2 = radius+(strokewidth/2);
	float x1 = r1;//initial values at y = 0, precalculated just because
	float x2 = r2;//we want to have remnants prepopulated.
	float y1 = 0;
	float y2 = 0;
	float tempArray[6] = {0,0,x1,y1,x2,y2};
	std::copy(tempArray,tempArray+sizeof(tempArray)/sizeof(tempArray[0]),remnants2D);
	//2 dimensions * 2  * 3 vertex per calculation per degree, 360 degrees
	int curpos = 0;
	int steps = 360/sides;
	for (int angle = 0; angle <= 360; angle+=steps){
		//Inner radius, should we cache the sine result for r2?
		y1 = (sin(angle*PI/180)*r1);
		x1 = (cos(angle*PI/180)*r1);
		addRemnants2D(x1,y1);
  		/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
		curpos+=6;//3 2D triangle vertices
		//Outter radius
		y2 = (sin(angle*PI/180)*r2);
		x2 = (cos(angle*PI/180)*r2);
		addRemnants2D(x2,y2);
		curpos+=6;
	}
	return curpos;
}
 
int init_resources(void){
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
/* Clear the background as black */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(program);
  glEnableVertexAttribArray(attribute_coord2d);
  int totalVertices = generateNGon(0.8,0.005,5);
  printf("Triangles assigned, got [%i] vertices\n",totalVertices);
  /* Push each element in buffer_vertices to the vertex shader */
  totalVertices = generateNGon(0.75,0.005,5);
  printf("Triangles assigned, got [%i] vertices\n",totalVertices);
  glDisableVertexAttribArray(attribute_coord2d);
  glutSwapBuffers();
}

void free_resources(){
  glDeleteProgram(program);
}

void Keyboard(unsigned char key, int x, int y){
	switch (key){
		case 27:             // ESCAPE key
		exit (0);
		break;
	}
}

int main(int argc, char* argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutCreateWindow("You know, for kids");

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

