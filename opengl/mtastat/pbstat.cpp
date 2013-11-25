#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "pbstat.h"
#define PI 3.1415926535897932384626433832795
//GLint attribute_coord3d,attribute_v_color,uniform_mvp;
//TODO: Cammel case.
pbstat::pbstat(long nprocessstart,GLfloat ny)
:NumVertices(2),NumVertexAttribs(7){
	next=NULL;
	processstart=nprocessstart;
	ystep=0.0201f;
	y=ny;
	r=0.0f;g=1.0f;b=0.0f;
	z=0.0f;
}
pbstat::~pbstat(){
}
void pbstat::raiseAll(){
	this->y+=this->ystep;
	if(this->next){
		this->next->raiseAll();
	}
}
pbstat* pbstat::getItemBySizeIndex(int sizeIdx){
	if(this->sizeIndex == sizeIdx){
		return(this);
	}
	if(this->next){
		return(this->next->getItemBySizeIndex(sizeIdx));
	}
	return(NULL);
}
void pbstat::add(pbstat *tmp){
	pbstat *tail=this;
	while(tail->next){
		tail=tail->next;
	}
	tail->next=tmp;
}
void pbstat::normalize(long min,long max){
	//Some values might be unknown or not accounted for, they are set to -1
	if(this->processstart == -1){
		this->processstart = 1;
	}else{
		this->processstart-=min;
	}
	if(this->next != NULL){
		this->next->normalize(min,max);
	}
}
void pbstat::init_resources(){
	glGenVertexArrays(NumVAOs,VAOs);
	glBindVertexArray(VAOs[WOTimes]);
	//Should also be [NumVertices][NumVertexAttribs]//This is CCW
	GLfloat vertices[2][7] = {
		{this->x,-1.0f,this->z,this->r,this->g,this->b,this->alpha},
		{this->x,this->y,this->z,this->r,this->g,this->b,this->alpha},
	};
        glGenBuffers(NumBuffers,Buffers);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
        //glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
        glVertexAttribPointer(
		vPosition,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat)*7,
		0
	);
        glVertexAttribPointer(
		vColor,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat)*7,
		(GLvoid*) (3 * sizeof(GLfloat))
	);
        glEnableVertexAttribArray(vPosition);
        glEnableVertexAttribArray(vColor);
}

void pbstat::onDisplay(){
        glBindVertexArray(VAOs[WOTimes]);
        glDrawArrays(GL_LINES,0,NumVertices);
}

void pbstat::printAll(){
	std::cout << "\t\t{" << this->x << "f," << this->y << "f,1.0f,1.0f,1.0f,1.0f,1.0f}," << std::endl;
	if(this->next){
		this->next->printAll();
	}
}
void pbstat::displayAll(){
	if(this->next != NULL){
		this->next->displayAll();
	}
	this->onDisplay();
}
//Call me on init_resources on main
void pbstat::initAll(){
	if(this->next != NULL){
		this->next->initAll();
	}
	this->init_resources();
}
//Call me on free_resources on main
void pbstat::deleteAll(){
	if(this->next != NULL){
		this->next->deleteAll();
		delete(this->next);
	}
}
void pbstat::setColor(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha){
	r=nr;g=ng;b=nb;alpha=nalpha;
}
