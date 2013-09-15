#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "NGon.h"
#define PI 3.1415926535897932384626433832795
//GLint attribute_coord3d,attribute_v_color,uniform_mvp;
//TODO: support alpha
NGon::NGon(int nsides,GLfloat nz,GLfloat nr, GLfloat ng, GLfloat nb,GLfloat nalpha)
:vbo_vertices(0),ibo_elements(0),vbo_triangle_colors(0),rotate(0),radius(0.8),x(0.0f),y(0.0f){
	sides=nsides;
	z=nz;
	r=nr;g=ng;b=nb;
	alpha=nalpha;
	next=NULL;
}
NGon::NGon(int nsides,GLfloat nr, GLfloat ng, GLfloat nb,GLfloat nalpha,GLfloat nradius,GLfloat nx, GLfloat ny,GLfloat nz)
:vbo_vertices(0),ibo_elements(0),vbo_triangle_colors(0),rotate(0){
	sides=nsides;
	r=nr;g=ng;b=nb;
	alpha=nalpha;
	x=nx;y=ny;z=nz;
	radius=nradius;
	next=NULL;
}
NGon::~NGon(){
	if (vbo_vertices != 0)
		glDeleteBuffers(1, &vbo_vertices);
	if (ibo_elements != 0)
		glDeleteBuffers(1, &ibo_elements);
}
GLfloat* NGon::generateNGon(){
	GLfloat* polygonPoints = new GLfloat[6*this->sides];
	int curpos = -1;
	float angle = rotate;
	float steps = 360.0f/this->sides;
	for (int i = 0; i<this->sides;i++){
		polygonPoints[++curpos]=x+(cos(angle*PI/180)*this->radius);
		polygonPoints[++curpos]=y+(sin(angle*PI/180)*this->radius);
		polygonPoints[++curpos]=this->z;
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
 

void NGon::init_resources(){
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

void NGon::onDisplay(GLint attribute_coord3d,GLint attribute_v_color){
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
//call me onDispla on main
void NGon::displayAll(GLint attribute_coord3d,GLint attribute_v_color){
	if(this->next != NULL){
		this->next->displayAll(attribute_coord3d,attribute_v_color);
	}
	this->onDisplay(attribute_coord3d,attribute_v_color);
}
//Call me on free_resources on main
void NGon::deleteAll(){
	if(this->next != NULL){
		this->next->deleteAll();
		delete(this->next);
	}
}
//Call me on init_resources on main
void NGon::initAll(){
	if(this->next != NULL){
		this->next->initAll();
	}
	this->init_resources();
}
