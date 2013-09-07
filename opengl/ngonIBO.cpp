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
/* GLM */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define PI 3.1415926535897932384626433832795
GLuint program;
GLint attribute_coord3d,attribute_v_color,uniform_mvp;
int screen_width=640, screen_height=480;
int maxNGons=80.0f;

class NGon{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
	int sides;
	GLfloat depth,r,g,b,alpha;
public:
	NGon *next;
	float rotate;
	float radius;
	NGon(int nsides,GLfloat ndepth,GLfloat nr, GLfloat ng, GLfloat nb,GLfloat nalpha)
	:vbo_vertices(0),ibo_elements(0),vbo_triangle_colors(0),rotate(0),radius(0.8){
		sides=nsides;
		depth=ndepth;
		r=nr;
		g=ng;
		b=nb;
		alpha=nalpha;
		next=NULL;
	};
	~NGon(){
		if (vbo_vertices != 0)
			glDeleteBuffers(1, &vbo_vertices);
		if (ibo_elements != 0)
			glDeleteBuffers(1, &ibo_elements);
	}
	GLfloat* generateNGon(){
		GLfloat* polygonPoints = new GLfloat[6*this->sides];
		int curpos = -1;
		float angle = rotate;
		float steps = 360.0f/this->sides;
		for (int i = 0; i<this->sides;i++){
			polygonPoints[++curpos]=(cos(angle*PI/180)*this->radius);
			polygonPoints[++curpos]=(sin(angle*PI/180)*this->radius);
			polygonPoints[++curpos]=this->depth;
			polygonPoints[++curpos]=this->r;
			polygonPoints[++curpos]=this->g;
			polygonPoints[++curpos]=this->b;
			angle+=steps;
		}
		curpos-=3;
		polygonPoints[++curpos]=1.0f;
		polygonPoints[++curpos]=1.0f;
		polygonPoints[++curpos]=1.0f;
		return polygonPoints;
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
			0  		   // Offset
		);
		glVertexAttribPointer(
			attribute_v_color,      // attribute
			3,                      // number of elements per vertex, here (r,g,b)
			GL_FLOAT,               // the type of each element
			GL_FALSE,               // take our values as-is
			sizeof(GLfloat) * 6,    // next color appears every 6 floats
			(GLvoid*) (3 * sizeof(GLfloat))  // offset of first element
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
		glDrawElements(GL_LINE_LOOP, this->sides,GL_UNSIGNED_SHORT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
NGon *parent;

void displayNGons(NGon *p){
	if(p == NULL){
		return;
	}
	displayNGons(p->next);
	p->onDisplay();
}
void onDisplay(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);
	displayNGons(parent);
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	glutSwapBuffers();
}
void onIdle() {
	float close = ((glutGet(GLUT_ELAPSED_TIME)%((int)maxNGons * 1000)) / 1000.0);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -maxNGons+close));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -maxNGons+close), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 20.0f);
	glm::mat4 mvp = projection * view * model;
	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glutPostRedisplay();
}
void deleteNGons(NGon *p){
	if(p == NULL){
		return;
	}
	deleteNGons(p->next);
	delete(p);
}
void initNGons(NGon *p){
	if(p == NULL){
		return;
	}
	initNGons(p->next);
	p->init_resources();
}
void free_resources(){
	glDeleteProgram(program);
	deleteNGons(parent);
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
	initNGons(parent);
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
	const char* uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}
	return 1;
}
void initNGons(){
	NGon *tmp;
	float r,g,b;
	r = 0.0f;
	g = 1.0f;
	b = 0.0f;
	parent = new NGon(40,0.04f,r,g,b,1.0f);
	int elements=40*6;//x,y,z,r,g,b
	tmp = parent;
	printf("Initializing %i NGons IBOs\n",maxNGons);
	for(float i = 0.0f;i<maxNGons;i+=1.0f){
		r=i/maxNGons;
		g=1.0f - i/maxNGons;
		b=i/maxNGons/2;
		tmp->next = new NGon(4+(int)i,i,r,g,b,1.0f);
		if(tmp->next == NULL){
			break;
		}
		tmp = tmp->next;
		elements+=(4+(int)i)*6;//x,y,z,r,g,b per polygon vertex
	}
	printf("Total IBOs: %i, total size: %lu\n",elements,(sizeof(GLfloat)*elements));
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
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
	initNGons();
	if (init_resources()) {
		glutDisplayFunc(onDisplay);
		glutIdleFunc(onIdle);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutMainLoop();
	}
	free_resources();
	return EXIT_SUCCESS;
}
