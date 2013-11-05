#include <time.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <iostream>
#include <climits>
#include <boost/regex.hpp>
#define _DEBUG
using namespace std;
using namespace boost;

#define BUFFER_OFFSET(x)  ((const void*) (x))
//#include "LoadShaders.h"
#include <GL/glew.h> 
#include <GL/freeglut.h>
#include "shader_utils.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "wostat.h"

enum VAO_IDs{Triangles, NumVAOs};
enum Buffer_IDs{ArrayBuffer, NumBuffers};
enum Attrib_IDs{vPosition=0};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint program;
wostat *root;

const GLuint NumVertices = 6;

void Keyboard(unsigned char key, int x, int y){
	switch (key){
		case 27:
			root->deleteAll();
			exit (0);
		break;
	}
}
GLfloat* generatePath(int totalunits){
	GLfloat* Points = new GLfloat[totalunits*2*2];//Two points (start/end) and two dimensions (for now)
	wostat *tmp = root->next;
	int curpos = -1;
	long maxproc = root->processend;
	while(tmp){
		Points[++curpos]=1.0f-(2*tmp->processstart/maxproc)
		Points[++curpos]=0.0f;
		tmp=tmp->next;
	}
	return Points;
}
long date_to_epoch(string datechars){
	struct tm tm;
	time_t t;
	if(strptime(datechars.c_str(),"%Y-%m-%d %H:%M:%S",&tm) == NULL){
		return -1;
	}
	t = mktime(&tm);
	if (t == -1){
		return -1;
	}
	return (long) t;

}
int gatherMTAData(void){
	string line;
	//unsigned int i=0;
	ifstream infile("/home/sebas/mtadata.log");
	boost::regex_constants::syntax_option_type flags = boost::regex::extended;
	string pid,wostart,woseq,aid;
	int size,soft,hard,sent;
	pid=wostart=woseq=aid="0";
	long processstart,processend;
	processstart=processend=0;
	root=new wostat(wostart,pid,woseq,processstart);
	wostat *tmp;
	regex wsre("^([-0-9: ]{19})\\.\\d* \\d* pbs: \\(Q*(\\d*)_*(\\d*)_(\\d*)_([-\\d]*)\\) Starting on.*$",flags);
	regex were("^([-0-9: ]{19})\\.\\d* \\d* pbs: \\(Q*(\\d*)_*(\\d*)_(\\d*)_([-\\d]*)\\) done with.*; \\((\\d+)\\) (\\d+) total, (\\d+) S, (\\d+) T \\(R.(\\d*),.*$",flags);
	if (infile.is_open()){
		while (getline(infile, line)){
			smatch sm;
			if(regex_match(line,sm,wsre)){
				/*#ifdef _DEBUG
				cout << "[II]: start = ";
				for(i=0;i<sm.size();i++){
					cout << "[II] match " << i << " : " << sm[i] << endl;
				}
				#endif*/
				processstart=date_to_epoch(sm[1]);
				if(processstart  < 0){
					cout << "[EE] Could not translate " << sm[1] << " to epoch... bailing..." << endl;
					break;
				}
				if(sm[3]==""){//This is a initial WO, not a retry WO
					wostart=sm[2];
				}else{
					//sm[2] seems to be the retry time at which it should start...
					wostart=sm[3];
				}
				pid=sm[4];
				woseq=sm[5];
				tmp=new wostat(wostart,pid,woseq,processstart);
				root->add(tmp);
			}else if(regex_match(line,sm,were)){
				processend=date_to_epoch(sm[1]);
				if(processend  < 0){
					cout << "[EE] Could not translate " << sm[1] << " to epoch... bailing..." << endl;
					break;
				}
				if(sm[3]==""){//This is an initial WO, not a retry WO
					wostart=sm[2];
				}else{
					wostart=sm[3];
				}
				pid=sm[4];
				woseq=sm[5];
				aid=sm[6];
				size=stoi(sm[7]);
				hard=stoi(sm[8]);
				soft=stoi(sm[9]);
				sent=stoi(sm[10]);
				if(!root->exists(wostart,pid,woseq)){
					//We might have partial information on the WO process...
					//Let's set the processstart to -1, 
					//special value we will adjust later to the minimum valid epoch found.
					tmp=new wostat(wostart,pid,woseq,-1);
					root->add(tmp);
				}
				if(!root->update(wostart,pid,woseq,aid,size,soft,hard,processend,sent)){
					cout << "[EE] Could not update, bad data in wo stats?"<< endl;
				}
			}else{
				cout << "[EE]: Pattern not found for line -> " << line << endl;
			}
		}
		infile.close();
	}else{
		fprintf(stderr, "EE: Unable to open file\n");
		return 0;
	}
	if(!root->next){
		fprintf(stderr, "EE: No entries processable found...\n");
		return 0;
	}
	tmp=root->next;
	int count = 0;
	long minproc=LONG_MAX;
	long maxproc=-1;
	while(tmp != NULL){
		if(minproc > tmp->processstart && tmp->processstart > 0){
			minproc = tmp->processstart;
		}
		if(maxproc < tmp->processend && tmp->processend > 0){
			maxproc = tmp->processend;
		}
		count++;
		tmp=tmp->next;
	}
	cout << "Minimizing epochs: (" << minproc << "," << maxproc << ") to: (0," << (maxproc-minproc) << ")" << endl;
	root->next->normalize(minproc,(maxproc-minproc));
	root->processend=(maxproc-minproc);
	tmp = root->next;
	#ifdef _DEBUG
	count=0;
	while(tmp != NULL){
		cout << "[II]: Final list status i :";
		cout << count << " = wostart " << tmp->wostart;
		cout << " pid " << tmp->pid;
		cout << " woseq " << tmp->woseq;
		cout << " processstart " << tmp->processstart;
		cout << " processend " << tmp->processend;
		cout << " aid " << tmp->aid;
		cout << " size " << tmp->size;
		cout << " soft " << tmp->soft;
		cout << " hard " << tmp->hard;
		cout << endl;
		count++;
		tmp=tmp->next;
	}
	#endif
	return count;
}
void init(int numUnits){
	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = create_shader("mtastat.v.glsl", GL_VERTEX_SHADER))   == 0) return;
	if ((fs = create_shader("mtastat.f.glsl", GL_FRAGMENT_SHADER)) == 0) return;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		print_log(program);
		return;
	}
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	GLfloat *vertices = generatePath(numUnits);
	glGenBuffers(NumBuffers,Buffers);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	glUseProgram(program);
	glVertexAttribPointer(vPosition,2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}
void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_LINE_LOOP,0,NumVertices);
	glFlush();
}

int main(int argc, char* argv[]){
	//move to after init
	int numUnits=gatherMTAData();
	if (numUnits){
		cout << "MTA Data not found:" << endl;
		return EXIT_SUCCESS;
	}
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512,512);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	//printf("Got opengl version: %s\n", glGetString(GL_VERSION));
	init(numUnits);
	glutKeyboardFunc (Keyboard);
	glutDisplayFunc(display);
	glutMainLoop();
}
