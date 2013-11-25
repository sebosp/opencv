#ifndef _PBSTAT_H
#define _PBSTAT_H
#include <GL/glew.h>
class pbstat{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
public:
	GLfloat r,g,b,alpha;
	GLfloat x,y,z;
	GLfloat ystep;
	long processstart;
	enum VAO_IDs{WOTimes, NumVAOs};
	enum Buffer_IDs{ArrayBuffer, NumBuffers};
	enum Attrib_IDs{vPosition=0,vColor=1};
	GLuint VAOs[NumVAOs];
	GLuint Buffers[NumBuffers];
	const GLuint NumVertices;
	const GLuint NumVertexAttribs;
	int sizeIndex;

	pbstat *next;
	pbstat(long nprocessstart,GLfloat ny);
	~pbstat();
	void raiseAll();
	void setColor(GLfloat nr,GLfloat ng,GLfloat nb,GLfloat nalpha);
	void add(pbstat *tmp);
	bool isSizeFinished();
	pbstat* getItemBySizeIndex(int sizeIdx);
	void normalize(long min,long max);
	void init_resources();
	void printAll();
	void onDisplay();
	void displayAll();
	void deleteAll();
	void initAll();
};
#endif
