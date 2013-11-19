#ifndef _WOSEQ_H
#define _WOSEQ_H
#include <GL/glew.h>
class wostat{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
public:
	GLfloat r,g,b,alpha;
	GLfloat x1,y1,z1;
	GLfloat x2,y2,z2;
	GLfloat x3,y3,z3;
	GLfloat x4,y4,z4;
	GLfloat ystep;
	std::string pid,wostart,woseq,aid,fullid,depid;
	long processstart,processend;
	int size,soft,hard,sent;
	bool complete;
	enum VAO_IDs{WOTimes, NumVAOs};
	enum Buffer_IDs{ArrayBuffer, NumBuffers};
	enum Attrib_IDs{vPosition=0,vColor=1};
	GLuint VAOs[NumVAOs];
	GLuint Buffers[NumBuffers];
	const GLuint NumVertices;
	const GLuint NumVertexAttribs;
	int sizeIndex;

	wostat *next;
	wostat *prev;
	wostat(std::string nwostart,std::string npid,std::string nwoseq,long nprocessstart);
	~wostat();
	void raise(float newy);
	bool detectOverlaps(long min, long max,GLfloat height,std::string refid);
	wostat* findUnassignedMax(wostat * candidate);
	bool exists(std::string nwostart,std::string npid,std::string nwoseq);
	void setPos(int vertex, GLfloat nx,GLfloat ny,GLfloat nz);
	void setColor(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha);
	void infect(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha, std::string refid);
	void add(wostat *tmp);
	bool isSizeFinished();
	wostat* getItemBySizeIndex(int sizeIdx);
	bool update(std::string nwostart,std::string npid,std::string nwoseq, std::string naid, int nsize, int nsoft, int nhard, long nprocessend,int sent);
	void normalize(long min,long max);
	void init_resources();
	void printAll();
	void printID();
	void onDisplay();
	void displayAll();
	void deleteAll();
	void initAll();
};
#endif
