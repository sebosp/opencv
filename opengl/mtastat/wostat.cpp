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
:x(0.0f),y(0.0f),z(0),size(0),soft(0),hard(0),sent(0){
	wostart=nwostart;
	pid=npid;
	woseq=nwoseq;
	next=NULL;
	prev=NULL;
	processstart=nprocessstart;
	processend=-1;
	bool complete=false;
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
		this->processstart = 0;
	}else{
		this->processstart-=min;
	}
	if(this->processend == -1){
		this->processend = max;
	}else{
		this->processend-=min;
	}
	if(this->next != NULL){
		this->next->normalize(min,max);
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
