#ifndef _WOSEQ_H
#define _WOSEQ_H
#include <GL/glew.h>
class wostat{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
	GLfloat r,g,b,alpha,x,y,z;
	std::string pid,wostart,woseq,aid;
	long processstart,processend;
	int sent,soft,hard;

public:
	wostat *next;
	wostat *prev;
	wostat(std::string nwostart,std::string npid,std::string nwoseq,long nprocessstart);
	~wostat();
	void splitIfContained(std::string nprocessdate);
	bool exists(std::string nwostart,std::string npid,std::string nwoseq);
	void setPos(GLfloat nx,GLfloat ny,GLfloat nz);
	void setColor(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha);
	//void init_resources();
	//void onDisplay(GLint attribute_coord3d,GLint attribute_v_color);
	//void displayAll(GLint attribute_coord3d,GLint attribute_v_color);
	void deleteAll();
	//void initAll();
};
#endif
