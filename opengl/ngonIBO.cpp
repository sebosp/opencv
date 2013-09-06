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
GLuint program;
GLint attribute_coord3d,attribute_v_color;

class NGon{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
	int sides;
	GLfloat depth,r,g,b,alpha;
public:
	int rotate;
	float radius;
	NGon(int nsides,GLfloat ndepth,GLfloat nr, GLfloat ng, GLfloat nb,GLfloat nalpha)
	:vbo_vertices(0),ibo_elements(0),vbo_triangle_colors(0),rotate(0),radius(0.8){
		sides=nsides;
		depth=ndepth;
		r=nr;
		g=ng;
		b=nb;
		alpha=nalpha;
	};
	~NGon(){
		if (vbo_vertices != 0)
			glDeleteBuffers(1, &vbo_vertices);
		if (ibo_elements != 0)
			glDeleteBuffers(1, &ibo_elements);
	}
	GLfloat* generateNGon(){
		GLfloat* circlePoints = new GLfloat[6*this->sides];
		int curpos = -1;
		int angle = rotate;
		int steps = 360/this->sides;
		for (int i = 0; i<this->sides;i++){
			circlePoints[++curpos]=(cos(angle*PI/180)*this->radius);
			circlePoints[++curpos]=(sin(angle*PI/180)*this->radius);
			circlePoints[++curpos]=this->depth;
			circlePoints[++curpos]=this->r;
			circlePoints[++curpos]=this->g;
			circlePoints[++curpos]=this->b;
			angle+=steps;
		}
		return circlePoints;
	}
 

	void init_resources(){
		GLfloat *vertices = generateNGon();
		glGenBuffers(1, &this->vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->sides*6, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		GLushort elements[this->sides];
		for(int i=0;i<this->sides;i++)elements[i]=i;
		glGenBuffers(1, &this->ibo_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}

	void onDisplay(){
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
		glVertexAttribPointer(
			attribute_coord3d, // attribute
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			sizeof(GLfloat)*6, // 6 items, x,y,z,r,g,b
//XXX			sizeof(GLfloat)*3, // 6 items, x,y,z
			0  		   // Offset
		);
		glVertexAttribPointer(
			attribute_v_color,      // attribute
			3,                      // number of elements per vertex, here (r,g,b)
			GL_FLOAT,               // the type of each element
			GL_FALSE,               // take our values as-is
			6 * sizeof(GLfloat),    // next color appears every 6 floats
			(GLvoid*) (3 * sizeof(GLfloat))  // offset of first element
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
		glDrawElements(GL_LINE_LOOP, this->sides,GL_UNSIGNED_SHORT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
NGon rectangle(4,0.1,1.0f,0.0f,0.0f,1.0f);
NGon  pentagon(5,0.9,0.0f,1.0f,0.0f,1.0f);
NGon  heptagon(6,0.5,0.0f,0.0f,1.0f,1.0f);

void onDisplay(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);
	rectangle.onDisplay();
	pentagon.onDisplay();
	heptagon.onDisplay();
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	glutSwapBuffers();
}
void free_resources(){
	glDeleteProgram(program);
}
void Keyboard(unsigned char key, int x, int y){
	switch (key){
		case 27:
			free_resources();
			exit (0);
		break;
	}
}
int init_resources(){
	rectangle.init_resources();
	pentagon.init_resources();
	heptagon.init_resources();
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
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1){
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	return 1;
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
