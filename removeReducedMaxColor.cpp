#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std; 
#define PI 3.14159265

//  X  X  X  
//  TL TC TR Y
//  CL CC CR Y
//  BL BC BR Y
Mat& createCircle(Mat& I,int cx, int cy, int radius){
	int x,y;
	int maxx,maxy,minx,miny;
	maxx = I.cols;
	maxy = I.rows;
	minx = 0; miny = 0; 
	if (cx > maxx || cy > maxy || cx < minx || cy < miny){
		return I;
	}
	for (float angle = 0; angle < 90; angle+=0.25){
		// X = (cos(angle*PI/180)*radius)
		// Y = (sin(angle*PI/180)*radius)
		y = (int)abs(cos(angle*PI/180)*radius + 0.5);
		x = (int)abs(sin(angle*PI/180)*radius + 0.5);
		if (cx + x <= maxx && cy - y <= maxy){ I.at<cv::Vec3b>(cx + x,cy - y) = {0,0,255};}
		if (cx + x <= maxx && cy + y >= miny){ I.at<cv::Vec3b>(cx + x,cy + y) = {0,0,255};}
		if (cx - x >= minx && cy - y <= maxy){ I.at<cv::Vec3b>(cx - x,cy - y) = {0,0,255};}
		if (cx - x >= minx && cy + y >= miny){ I.at<cv::Vec3b>(cx - x,cy + y) = {0,0,255};}
	}
	return I;
}
Mat& findCircles(Mat& I, const uchar* const table){
	int r,g,b,incidence,x,y,x2,y2,cx,cy,maxx,maxy,minx,miny,ex,ey;
	maxx = I.cols;
	maxy = I.rows;
	minx = miny = 0;//Just for clarity again. Theoretically checks for minx are never needed in this scenario, but well, remove them later 
	uchar *p = I.ptr<uchar>(0);
	uchar *root = p;
	int curp = 0;
	uchar *xy;
	for (y = 0; y < maxy; y++){
		cout << "Pointing to " << y<<endl;
		for(x = 0; x < (maxx*3); x+=3){
			//cout << "Starting circles x: " << x << ". y: " << y << endl;
			curp+=3;
			r = p[curp];
			g = p[curp+1];
			b = p[curp+2];
			cy = y; //Just for clarity
//			cout << "Checking x: " << x << ". y: " << y << ". x2: " << x2 << ". y2: " << y2 << " " << endl;
			if (r == 255 && g == 255 && b == 255){// TODO replace white for most common color from maxColors matrix
//				cout << "White" << endl;
				continue;
			}
			for (int i = x + 5; i < maxx; i+=2){//Min radius is gonna be 5
				incidence = 0;
				cx = x + (i/2);
				for (int angle = 1; angle < 89; angle+=2){
					if (incidence == -80){//Let's not boher if we already got 30 unmatching degrees.
						continue;
					}
					x2 = (int)(cos(angle*PI/180)*(i/2) + 0.5);//i is diameter.
					y2 = (int)(sin(angle*PI/180)*(i/2) + 0.5);//x2 and y2 are relative points to the center of the hypotetical circle
//					cout << "Checking x: " << x << ". y: " << y << ". Checking x2: " << x2 << ". y2: " << y2 << ". Angle: " << angle << endl;
					if (cx + x2 < maxx && cy + y2 < maxy){
//						cout << "1 cycle" << endl;
						ex = cx + x2;
						ey = cy + y2;
						if (table[root[(ey*maxx)+ex]] == table[r] && table[root[(ey*maxx)+ex+1]] == table[g] && table[root[(ey*maxx)+ex+2]] == table[b]){
							incidence++;
						}else{
							incidence--;
						}
					}
					if (cx + x2 < maxx && cy - y2 >= miny){
//						cout << "2 cycle" << endl;
						ex = cx + x2;
						ey = cy - y2;
						if (table[root[(ey*maxx)+ex]] == table[r] && table[root[(ey*maxx)+ex+1]] == table[g] && table[root[(ey*maxx)+ex+2]] == table[b]){
							incidence++;
						}else{
							incidence--;
						}
					}
					if (cx - x2 >= minx && cy + y2 < maxy){
//						cout << "3 cycle" << endl;
						ex = cx - x2;
						ey = cy + y2;
						if (table[root[(ey*maxx)+ex]] == table[r] && table[root[(ey*maxx)+ex+1]] == table[g] && table[root[(ey*maxx)+ex+2]] == table[b]){
							incidence++;
						}else{
							incidence--;
						}
					}
					if (cx - x2 >= minx && cy - y2 >= miny){
//						cout << "4 cycle" << endl;
						ex = cx - x2;
						ey = cy - y2;
						if (table[root[(ey*maxx)+ex]] == table[r] && table[root[(ey*maxx)+ex+1]] == table[g] && table[root[(ey*maxx)+ex+2]] == table[b]){
							incidence++;
						}else{
							incidence--;
						}
					}
				}
				if (incidence > 20){
					I = createCircle(I,y,cx,i);
					cout << "Creating circle cx: " << cx << ". y: " << y << ". i:" << i <<endl;
				}
			}
//			cout << "Incidence: " << incidence << endl;
		}
	}
	return I;
}

Mat& scanImageAndReduce(Mat& I, const uchar* const table){
	// accept only char type matrices
	//TODO check what if not consecutive
	CV_Assert(I.depth() != sizeof(uchar));
	int channels = I.channels();
	int nRows = I.rows * channels;
	int nCols = I.cols;
	cout <<  "nCols: " << nCols << ". nRows: " << nRows << ". Channels: " << channels<< ". nRows/channels: " << (nRows/channels) << " "<< ". After continuity: " <<endl;
	if (I.isContinuous()){
		nCols *= nRows;
		nRows = 1;
	}
	cout <<  "nCols: " << nCols << ". nRows: " << nRows << endl;
	int i,j;
	uchar* p;
	vector<int> vI1Matrix(256,0);
	vector< vector<int> > vI2Matrix(256, vI1Matrix);
	vector< vector< vector<int> > > maxColors(256, vI2Matrix);
	//Finding max reduced color for deletion
	int r,g,b;
	for( i = 0; i < nRows; ++i){
		p = I.ptr<uchar>(i);
		for ( j = 0; j < nCols; ++j){
			if (!(j % 3) && j > 0){
				maxColors[table[p[j-3]]][table[p[j-2]]][table[p[j-1]]]++;
			}
		}
	}
	//Process Max values:
	uchar rgb[3]={0,0,0};
	int maxCount = 0;
	for (r = 0; r < 256; r++){
		for (g = 0; g< 256; g++){
			for (b = 0; b< 256; b++){
				if (maxColors[r][g][b] > maxCount){
					rgb[0]=r;
					rgb[1]=g;
					rgb[2]=b;
					maxCount = maxColors[r][g][b];
				}
			}
		}
	}
	//Perform color reduction.
	for(i = 0; i < nRows; ++i){
		p = I.ptr<uchar>(i);
		for ( j = 0; j < nCols; ++j){
			if (!(j % 3) && j > 0){
			//Set max used color to white, altho it should be set to alpha next time
				if (table[p[j-3]] == rgb[0] && table[p[j-2]] == rgb[1] && table[p[j-1]] == rgb[2]){
					p[j-3] = 255;
					p[j-2] = 255;
					p[j-1] = 255;
				}
			}
		}
	}
	//return I;
	return findCircles(I,table);
}

int main( int argc, char** argv){ 
	if( argc < 3){
	 cout <<" Usage: Params, duh!" << endl;
	 return -1;
	}
	Mat inputImg;
	if( argc == 4 && !strcmp(argv[3],"G") )
		inputImg = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	else
		inputImg = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	if(! inputImg.data ){
		cout <<  "Could not open or find the image" << endl;
		return -1;
	}
//	imshow( "Display window", inputImg );				   // Show our image inside it.
//	waitKey(0);
	int divideWith;
	stringstream s;
	s << argv[2];
	s >> divideWith;
	if (!s){
		cout << "Invalid number entered for dividing. " << endl;
		return -1;
	}
	uchar table[256];
	for (int i = 0; i < 256; ++i)
	   table[i] = (i/divideWith) * divideWith;
	Mat outputImg = scanImageAndReduce(inputImg,table);
	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window", outputImg );				   // Show our image inside it.
	waitKey(0);										  // Wait for a keystroke in the window
	return 0;
}
