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
:size(0),soft(0),hard(0),sent(0),NumVertices(4),NumVertexAttribs(7){
	wostart=nwostart;
	pid=npid;
	woseq=nwoseq;
	next=NULL;
	prev=NULL;
	processstart=nprocessstart;
	processend=-1;
}
wostat::~wostat(){
	if(this->prev != NULL){
		this->prev->next = this->next;
	}
}
void wostat::splitIfContained(std::string nwostart){
	//if(this->

}
void wostat::setPos(int vertex, GLfloat nx,GLfloat ny,GLfloat nz){
	//fugly but clear code, next use an array
	switch(vertex){
		case 1: x1=nx;y1=ny;z1=nz;
			break;
		case 2: x2=nx;y2=ny;z2=nz;
			break;
		case 3: x3=nx;y3=ny;z3=nz;
			break;
		case 4: x4=nx;y4=ny;z4=nz;
			break;
	}
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
void wostat::add(wostat *tmp){
	if(this->processstart > tmp->processstart){
		//This should never happen, we shouldn't be sending before epoch 0 amirite?
		#ifdef _DEBUG
		printf("Discarding strange entry with processstart = %l\n",tmp->processstart);
		#endif
		return;
	}
	if(this->next){
		if(this->next->processstart > tmp->processstart){
			tmp->next=this->next;
			tmp->prev=this;
			this->next->prev=tmp;
			this->next=tmp;
		}else if(this->next->processstart == tmp->processstart){
			std::string wofullnxt=this->next->wostart + this->next->pid + this->next->woseq;
			std::string wofullcur=this->wostart + this->pid + this->woseq;
			//Hum not equal than because we shouldn't reprocess the same woretry...
			if(wofullnxt > wofullcur){
				tmp->next=this->next;
				tmp->prev=this;
				this->next->prev=tmp;
				this->next=tmp;
			}else{
				this->next->add(tmp);
			}
		}else{
			this->next->add(tmp);
		}
	}else{
		tmp->prev=this;
		this->next=tmp;
	}
}
bool wostat::update(std::string nwostart,std::string npid,std::string nwoseq, std::string naid, int nsize, int nsoft, int nhard, long nprocessend, int nsent){
	if(this->wostart == nwostart && this->pid == npid && this->woseq == nwoseq){
		if(this->processstart > nprocessend){//Lil' sanity check
			return false;
		}
		aid=naid;
		size=nsize;
		soft=nsoft;
		hard=nhard;
		processend=nprocessend;
		sent=nsent;
		return true;
	}else{
		if(this->next != NULL){
			return(this->next->update(nwostart,npid,nwoseq,naid,nsent,nsoft,nhard,nprocessend,nsize));
		}
		return false;
	}
}
void wostat::normalize(long min,long max){
	//Some values might be unknown or not accounted for, they are set to -1
	if(this->processstart == -1){
		this->processstart = 1;
	}else{
		this->processstart-=(min-1);
	}
	if(this->processend == -1){
		this->processend = max;
	}else{
		this->processend-=(min-1);
	}
	if(this->next != NULL){
		this->next->normalize(min,max);
	}
}
void wostat::init_resources(){
	glGenVertexArrays(NumVAOs,VAOs);
	glBindVertexArray(VAOs[Triangles]);
	//to be changet do use this->x,this->y, should also be [NumVertices][NumVertexAttribs]
	GLfloat vertices[4][2] = {
		{ this->x1,this->y1/*,this->z1,this->r,this->g,this->b,this->alpha*/},
		{ this->x2,this->y2/*,this->z2,this->r,this->g,this->b,this->alpha*/},
		{ this->x3,this->y3/*,this->z3,this->r,this->g,this->b,this->alpha*/},
		{ this->x4,this->y4/*,this->z4,this->r,this->g,this->b,this->alpha*/},
	};
        glGenBuffers(NumBuffers,Buffers);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(0);
}

void wostat::onDisplay(){
        glBindVertexArray(VAOs[Triangles]);
        glDrawArrays(GL_LINE_LOOP,0,NumVertices);
}

void wostat::displayAll(){
	if(this->next != NULL){
		this->next->displayAll();
	}
	this->onDisplay();
}
//Call me on init_resources on main
void wostat::initAll(){
	if(this->next != NULL){
		this->next->initAll();
	}
	this->init_resources();
}
//Call me on free_resources on main
void wostat::deleteAll(){
	if(this->next != NULL){
		this->next->deleteAll();
		delete(this->next);
	}
}
