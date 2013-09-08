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
#include "NGon.h"
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
NGon *parent;

void onDisplay(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);
	if (parent)
		parent->displayAll(attribute_coord3d,attribute_v_color);
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	glutSwapBuffers();
}
void onIdle() {
	float close = ((glutGet(GLUT_ELAPSED_TIME)%((int)maxNGons * 1000)) / 1000.0);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -maxNGons+close));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -maxNGons+close), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, (float)(maxNGons)/2);
	glm::mat4 mvp = projection * view * model;
	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glutPostRedisplay();
}
void free_resources(){
	glDeleteProgram(program);
	if (parent)
		parent->deleteAll();
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
	if(parent)
	parent->initAll();
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
	printf("Initializing %i NGon IBOs\n",maxNGons);
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
