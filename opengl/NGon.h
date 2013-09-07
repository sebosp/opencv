#ifndef _NGON_H
#define _NGON_H
#include <GL/glew.h>
class NGon{
private:
	GLuint vbo_vertices,ibo_elements,vbo_triangle_colors;
	int sides;
	GLfloat depth,r,g,b,alpha;
public:
	NGon *next;
	float rotate;
	float radius;
	NGon(int nsides,GLfloat ndepth,GLfloat nr, GLfloat ng, GLfloat nb,GLfloat nalpha);
	~NGon();
	GLfloat* generateNGon();
	void init_resources();
	void onDisplay(GLint attribute_coord3d,GLint attribute_v_color);
	void displayAll(GLint attribute_coord3d,GLint attribute_v_color);
	void deleteAll();
	void initAll();
};
#endif
