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
//#include "pbstat.h"

enum Attrib_IDs{vPosition=0};

GLuint program;
wostat *woroot;
//pbstat *pbroot;

const GLuint NumVertices = 6;

void Keyboard(unsigned char key, int x, int y){
	switch (key){
		case 27:
			woroot->deleteAll();
			exit (0);
		break;
	}
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
	woroot=new wostat(wostart,pid,woseq,processstart);
	woroot->x1=1.1f;
	woroot->x2=woroot->x1;
	woroot->x3=-1.1f;
	woroot->x4=-1.1f;
	woroot->y1=0-woroot->ystep;
	woroot->y2=woroot->ystep*30;//hardcoded max slaves? fugly. Sync somehow
	woroot->y3=woroot->y2;
	woroot->y4=woroot->y1;
	GLfloat r,g,b,alpha;
	alpha=1.0f;r=1.0f;g=1.0f;b=1.0f;
	woroot->r=r;woroot->g=g;woroot->b=b;
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
				woroot->add(tmp);
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
				if(!woroot->exists(wostart,pid,woseq)){
					//We might have partial information on the WO process...
					//Let's set the processstart to -1, 
					//special value we will adjust later to the minimum valid epoch found.
					tmp=new wostat(wostart,pid,woseq,-1);
					woroot->add(tmp);
				}
				if(!woroot->update(wostart,pid,woseq,aid,size,soft,hard,processend,sent)){
					cout << "[EE] Could not update, bad data in wo stats? line " << line << endl;
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
	if(!woroot->next){
		fprintf(stderr, "EE: No entries processable found...\n");
		return 0;
	}
	long minproc=LONG_MAX;
	long maxproc=-1;
	tmp=woroot->next;
	while(tmp != NULL){
		if(minproc > tmp->processstart && tmp->processstart > 0){
			minproc = tmp->processstart;
		}
		if(minproc > tmp->processend && tmp->processend > 0){//We may have only the end of a wo process
			minproc = tmp->processend;
		}
		if(maxproc < tmp->processend && tmp->processend > 0){
			maxproc = tmp->processend;
		}
		if(maxproc < tmp->processstart && tmp->processstart > 0){//We may only have the start of a wo process
			maxproc = tmp->processstart;
		}
		tmp=tmp->next;
	}
	tmp=woroot->next;
	cout << "Minimizing epochs: (" << minproc << "," << maxproc << ") to: (0," << (maxproc-minproc) << ")" << endl;
	maxproc=maxproc-minproc;
	woroot->next->normalize(minproc,maxproc);
	minproc=0;
	tmp = woroot->next;
	//Let's translate x (and y initially...)
	while(tmp){
		//preset to CCW...
		//cout << "Transforming ( " << tmp->processstart << "," << tmp->processend << ") to: ";
		tmp->x1=(1.9*tmp->processend/maxproc)-0.95f;
		tmp->x2=tmp->x1;
		tmp->x3=(1.9*tmp->processstart/maxproc)-0.95f;
		tmp->x4=tmp->x3;
		//cout << "(" << tmp->x1 << "," << tmp->x3 << ")" << endl;
		tmp=tmp->next;
	}
	int count = 1;
	//Order them by size
	tmp=woroot->findUnassignedMax(woroot);
	while(!woroot->isSizeFinished()){
		count++;
		tmp->sizeIndex=count;
		//cout << "id: " << tmp->fullid << " . size " << tmp->processend - tmp->processstart << ". count... " << count << endl;
		tmp=woroot->findUnassignedMax(woroot);
	}
	woroot->processend=maxproc;//We need to have this preserved to -1 until after the sizeOrder...
	tmp = woroot->next;
	srand(time(0));
	while(tmp != NULL){
		if(tmp->r == 0.0f && tmp->g == 0.0f && tmp->b == 0.0f){
			r=(float)(rand()%100)/100;
			g=(float)(rand()%100)/100;
			b=(float)(rand()%100)/100;
			tmp->infect(r,g,b,alpha,tmp->depid);
		}
		tmp=tmp->next;
	}
	return count;
}
void resolveOverlaps(int count){
	wostat *tmpA, *tmpB;
	cout << "We got " << count << " items " << endl;
	for(int iterA = 1;iterA <= count;iterA++){
		tmpA=woroot->getItemBySizeIndex(iterA);
		if(!tmpA){
			continue;
		}
		//cout << "id: " << tmpA->fullid << " . size " << tmp->processend - tmp->processstart << ". count... " << count << "iterA =  " << iterA << endl;
		tmpA->y1=0.0f;
		bool isFree=false;
		while(!isFree){
			isFree=true;
			for(int iterB = 1;iterB < iterA;iterB++){
				tmpB=woroot->getItemBySizeIndex(iterB);
				if(!tmpB){
					continue;
				}
				if(tmpA->y1 == tmpB->y1 && tmpA->processstart <= tmpB->processend && tmpB->processstart <= tmpA->processend){
					tmpA->y1+=woroot->ystep;
					iterB=1;
				}
			}
			if(!isFree){
				tmpA->y1+=woroot->ystep;
			}else{
				tmpA->raise(tmpA->y1);
			}
		}
			//cout << "id: " << tmp->fullid << " . size " << tmp->processend - tmp->processstart << ". count... " << count << endl;
	}
	return;
}
void printWebGL(){
	cout << "vertices = [" << endl;
	wostat *tmp = woroot->next;
	while(tmp != NULL){
		//MEH, IBOs next time
		cout << tmp->x1 << "," << tmp->y1 << ",0.0," << endl;
		cout << tmp->x2 << "," << tmp->y2 << ",0.0," << endl;
		cout << tmp->x3 << "," << tmp->y3 << ",0.0," << endl;
		cout << tmp->x2 << "," << tmp->y2 << ",0.0," << endl;
		cout << tmp->x3 << "," << tmp->y3 << ",0.0," << endl;
		cout << tmp->x4 << "," << tmp->y4 << ",0.0," << endl;
		cout << tmp->x3 << "," << tmp->y3 << ",0.0," << endl;
		cout << tmp->x4 << "," << tmp->y4 << ",0.0," << endl;
		cout << tmp->x1 << "," << tmp->y1 << ",0.0," << endl;
		tmp=tmp->next;
	}
	cout << "];" << endl;
	cout << "colors = [" << endl;
	tmp = woroot->next;
	//Colorize different IDs
	while(tmp != NULL){
		//MEH, IBOs next time
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		cout << tmp->r << "," << tmp->g << "," << tmp->b << "," << tmp->alpha << "," << endl;
		tmp=tmp->next;
	}
	cout << "];" << endl;
	return;
}
void init(){
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
	woroot->initAll();
	glUseProgram(program);
	/*glVertexAttribPointer(vPosition,2,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(vPosition);*/
}
void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	woroot->displayAll();
	//woroot->next->onDisplay();
	glFlush();
}

int main(int argc, char* argv[]){
	//1k = 1.953s,1.5k = 4.975s, 2.0ks = 13.685s,2.5ks = 24.861s, ... 3.5ks = 65.780s
	//progression, every 500 lines it takes 2.5 times more than before to order.
	int numUnits=gatherMTAData();
	resolveOverlaps(numUnits);
	if (numUnits < 1){
		cout << "MTA Data not found:" << endl;
		return EXIT_FAILURE;
	}
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1920,1080);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		fprintf(stderr, "EE: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//printf("Got opengl version: %s\n", glGetString(GL_VERSION));
	init();
	glutKeyboardFunc (Keyboard);
	glutDisplayFunc(display);
	glutMainLoop();
}
