#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "wostat.h"
#define PI 3.1415926535897932384626433832795
//GLint attribute_coord3d,attribute_v_color,uniform_mvp;
wostat::wostat(std::string nwostart,std::string npid,std::string nwoseq,long nprocessstart)
:x(0.0f),y(0.0f),z(0),sent(0),soft(0),hard(0){
	wostart=nwostart;
	pid=npid;
	woseq=nwoseq;
	next=NULL;
	prev=NULL;
	processstart=nprocessstart;
	processend=0;
}
wostat::~wostat(){
	if(this->prev != NULL){
		this->prev->next = this->next;
	}
}
void wostat::splitIfContained(std::string nwostart){
	//if(this->

}
void wostat::setPos(GLfloat nx,GLfloat ny,GLfloat nz){
	x=nx;y=ny;z=nz;
}
void wostat::setColor(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha){
	r=nr;g=ng;b=nb;alpha=nalpha;
}
bool wostat::exists(std::string nwostart,std::string npid,std::string nwoseq){
	if(this->wostart == nwostart && this->pid == npid && this->woseq == nwoseq){
		return true;
	}else{
		if(this->next != NULL){
			return(this->next->exists( nwostart,npid,nwoseq));
		}
		return false;
	}
}
bool wostat::add(wostat *tmp){
	if(this->processstart < tmp->processstart){
		//This should never happen, we shouldn't be sending before epoch 0 amirite?
		return false;
	}
	if(this->next){
		if(this->next->processstart > tmp->processstart){
			//insert here
		}else{
			return this->next->add(tmp);
		}
	}
}
/*GLfloat* wostat::generatewostat(){
	GLfloat* polygonPoints = new GLfloat[7*this->sides];
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
		polygonPoints[++curpos]=this->alpha;
		angle+=steps;
	}
	curpos-=3;
	polygonPoints[++curpos]=1.0f;
	polygonPoints[++curpos]=1.0f;
	polygonPoints[++curpos]=1.0f;
	return polygonPoints;
}
 

void wostat::init_resources(){
	GLfloat *vertices = generatewostat();
	glGenBuffers(1, &this->vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->sides*7, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLushort elements[this->sides];
	for(int i=0;i<this->sides;i++)elements[i]=i;
	glGenBuffers(1, &this->ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void wostat::onDisplay(GLint attribute_coord3d,GLint attribute_v_color){
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		sizeof(GLfloat)*7, // 7 items, x,y,z,r,g,b,a
		0  		   // Offset
	);
	glVertexAttribPointer(
		attribute_v_color,      // attribute
		4,                      // number of elements per vertex, here (r,g,b,a)
		GL_FLOAT,               // the type of each element
		GL_FALSE,               // take our values as-is
		sizeof(GLfloat) * 7,    // next color appears every 7 floats
		(GLvoid*) (3 * sizeof(GLfloat))  // offset of first element
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
	//TODO support fill wostats
	glDrawElements(GL_LINE_LOOP, this->sides,GL_UNSIGNED_SHORT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
//call me onDisplay on main
void wostat::displayAll(GLint attribute_coord3d,GLint attribute_v_color){
	if(this->next != NULL){
		this->next->displayAll(attribute_coord3d,attribute_v_color);
	}
	this->onDisplay(attribute_coord3d,attribute_v_color);
}
//Call me on init_resources on main
void wostat::initAll(){
	if(this->next != NULL){
		this->next->initAll();
	}
	this->init_resources();
}*/
//Call me on free_resources on main
void wostat::deleteAll(){
	if(this->next != NULL){
		this->next->deleteAll();
		delete(this->next);
	}
}
