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
	int queueSize=(int)((1.0f+this->y)/this->ystep);
	if(queueSize >= 20){
		if(queueSize <= 25){
			this->r=1.0f;this->g=1.0f;this->b=0.0f;
		}else{
			this->r=1.0f;this->g=0.0f;this->b=0.0f;
		}
	}
}
void pbstat::init_resources(){
	if(!this->next){
		return;
	}
	glGenVertexArrays(NumVAOs,VAOs);
	glBindVertexArray(VAOs[WOTimes]);
	//Should also be [NumVertices][NumVertexAttribs]//This is CCW
	GLfloat vertices[2][7] = {
		{this->x,this->y,this->z,this->r,this->g,this->b,this->alpha},
		{this->next->x,this->next->y,this->next->z,this->r,this->g,this->b,this->alpha},
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
	if(!this->next){
		return;
	}
        glBindVertexArray(VAOs[WOTimes]);
        glDrawArrays(GL_LINES,0,NumVertices);
}

void pbstat::printAll(int severity,std::string curVar){
	int queueSize=(1.0f + this->y)/this->ystep;
	int curSeverity = (queueSize<20?0:(queueSize < 25?1:2));
	//std::cout << "//queueSize = " << queueSize << " - curSeverity = " << curSeverity << " y:" << this->y << " ;" << std::endl;
	if(this->next){
		if(curSeverity != severity){
			if(curVar != ""){
				std::cout << "\ttime" << curVar << ".computeLineDistances();";
				std::cout << "\tvar object" << curVar << " = new THREE.Line("<< "time" << curVar <<","<< (severity == 0?"low":(severity == 1?"medium":"high")) <<");";
				std::cout << "\tobjects.push(object" << curVar << ");scene.add(object" << curVar << ");" <<std::endl;
			}
			char meh[50] ="\0";
			sprintf(meh,"%i",this->processstart);curVar=meh;
			curVar += "_";
			sprintf(meh,"%i",queueSize);curVar+=meh;
			
			std::cout << "\t\t\t\tvar time" << curVar << "= new THREE.Geometry();";
			std::cout << "\ttime" << curVar << ".vertices.push( new THREE.Vector3(" << this->x << "," << this->y << ",0.0));";
			std::cout << "\ttime" << curVar << ".vertices.push( new THREE.Vector3(" << this->next->x << "," << this->next->y << ",0.0));";
		}else{
			std::cout << "\ttime" << curVar << ".vertices.push( new THREE.Vector3(" << this->x << "," << this->y << ",0.0));";
			std::cout << "\ttime" << curVar << ".vertices.push( new THREE.Vector3(" << this->next->x << "," << this->next->y << ",0.0));";
		}
		
		this->next->printAll(curSeverity,curVar);
	}else{
		if(curVar != ""){
			std::cout << "\ttime" << curVar << ".computeLineDistances();";
			std::cout << "\tvar object" << curVar << " = new THREE.Line("<< "time" << curVar <<","<< (queueSize < 20?"low":(queueSize < 25?"medium":"high")) <<");";
			std::cout << "\tobjects.push(object" << curVar << ");scene.add(object" << curVar << ");" <<std::endl;
		}
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
