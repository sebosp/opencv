//Compilation: g++ -Wall circle.cpp -lglut -lGLEW -lGL -o circle
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/glut.h>
/* Global variables */
GLuint program;
GLint attribute_coord2d;
GLfloat remnants2D[6];

#define PI 3.14159265

void addRemnants2D(float newx, float newy){
	for(int i=0;i<4;i++){//Shift
		remnants2D[i]=remnants2D[i+2];
	}
	remnants2D[4]=newx;
	remnants2D[5]=newy;
}

GLfloat* generateCircle(float radius,float strokewidth){
	float r1 = radius-(strokewidth/2);
	float r2 = radius+(strokewidth/2);
	float x1 = r1;//initial values at y = 0, precalculated just because
	float x2 = r2;//we want to have remnants prepopulated.
	float y1 = 0;
	float y2 = 0;
	float tempArray[6] = {0,0,x1,y1,x2,y2};
	std::copy(tempArray,tempArray+sizeof(tempArray)/sizeof(tempArray[0]),remnants2D);
	//2 dimensions * 2  * 3 vertex per calculation per degree, 360 degrees
	GLfloat* circlePoints = new GLfloat[2*3*2*360];
	int curpos = 0;
	int i = 0;
	for (int angle = 1; angle <= 360; angle++){
		//Inner radius, should we cache the sine result for r2?
		y1 = (sin(angle*PI/180)*r1);
		x1 = (cos(angle*PI/180)*r1);
		addRemnants2D(x1,y1);
		for(i=0;i<6;i++){
			circlePoints[curpos++]=remnants2D[i];
		}
		//Outter radius
		y2 = (sin(angle*PI/180)*r2);
		x2 = (cos(angle*PI/180)*r2);
		addRemnants2D(x2,y2);
		for(int i=0;i<6;i++){
			circlePoints[curpos++]=remnants2D[i];
		}
	}
	return circlePoints;
}
 
int init_resources(void){
   GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  const char *vs_source = 
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"  // OpenGL ES 2.0
#else
    "#version 120\n"  // OpenGL 2.1
#endif
    "attribute vec2 coord2d;                  "
    "void main(void){                         "
    "  gl_Position = vec4(coord2d, 0.0, 1.0); "
    "}";
  glShaderSource(vs, 1, &vs_source, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
  if (0 == compile_ok){
    fprintf(stderr, "Error in vertex shader\n");
    return 0;
  }
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fs_source =
    "#version 120           \n"
    "void main(void) {        "
    "  gl_FragColor[0] = 0.0; "
    "  gl_FragColor[1] = 1.0; "
    "  gl_FragColor[2] = 0.0; "
    "}";
  glShaderSource(fs, 1, &fs_source, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
  if (!compile_ok) {
    fprintf(stderr, "Error in fragment shader\n");
    return 0;
  }
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
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
  GLfloat* triangle_vertices = generateCircle(0.8,0.005);
  /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
  glVertexAttribPointer(
    attribute_coord2d, // attribute
    2,                 // number of elements per vertex, here (x,y)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // no extra data between each position
    triangle_vertices  // pointer to the C array
  );
  printf("Triangles assigned\n");
  /* Push each element in buffer_vertices to the vertex shader */
  glDrawArrays(GL_TRIANGLES, 0, 2*3*2*360);
  glDisableVertexAttribArray(attribute_coord2d);
  /* Display the result */
  glutSwapBuffers();
}
 
void free_resources(){
	glDeleteProgram(program);
}
 
int main(int argc, char* argv[]){
  /* Glut-related initialising functions */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutCreateWindow("You know, for kids");
 
  /* Extension wrangler initialising */
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK){
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }
 
  /* When all init functions runs without errors,
  the program can initialise the resources */
  if (1 == init_resources()){
    /* We can display it if everything goes OK */
    glutDisplayFunc(onDisplay);
    glutMainLoop();
  }
 
  /* If the program exits in the usual way,
  free resources and exit with a success */
  free_resources();
  return EXIT_SUCCESS;
}
