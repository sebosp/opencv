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
	minx = miny = 0; 
	if (cx > maxx || cy > maxy || cx < minx || cy < miny){
		return I;
	}
	for (float angle = 0; angle < 90; angle+=0.25){
		// X = (cos(angle*PI/180)*radius)
		// Y = (sin(angle*PI/180)*radius)
		x = (int)(cos(angle*PI/180)*radius + 0.5);
		y = (int)(sin(angle*PI/180)*radius + 0.5);
		if (cx + x <= maxx && cy + y <= maxy){ I.at<cv::Vec3b>(cx + x,cy + y) = {0,255,0};}
		if (cx + x <= maxx && cy - y >= miny){ I.at<cv::Vec3b>(cx + x,cy - y) = {0,255,0};}
		if (cx - x >= minx && cy + y <= maxy){ I.at<cv::Vec3b>(cx - x,cy + y) = {0,255,0};}
		if (cx - x >= minx && cy - y >= miny){ I.at<cv::Vec3b>(cx - x,cy - y) = {0,255,0};}
	}
	return I;
}
int matchingDegs(Mat &I,cont uchar* const table,int x, int y){
	int r,g,b,r2,g2,b2,incidence,x2,y2
	r = I.at<cv::Vec3b>(x,y)[0];
	g = I.at<cv::Vec3b>(x,y)[1];
	b = I.at<cv::Vec3b>(x,y)[2];
	if (r == 255 && g == 255 && b == 255){// TODO replace white for most common color from maxColors matrix
		return 0;
	}
	for (int i = x + 5; i < I.cols; i++){//Min radius is gonna be 5
		incidence = 0;
		for (float angle = 0; angle < 90; angle+=0.25){
			if (incidence == -30){//Let's not bother if we already got 30 unmatching degrees.
				return 0;
			}
			x2 = (int)(cos(angle*PI/180)*i + 0.5);
			y2 = (int)(sin(angle*PI/180)*i + 0.5);
//			if ((x + (x2 - x)/2) - x >= minx && cy + y <= maxy){ I.at<cv::Vec3b>(cx - x,cy + y) = {0,255,0};}//TODO Just copy pasted
//			if ((x + (x2 - x)/2) - x >= minx && cy - y >= miny){ I.at<cv::Vec3b>(cx - x,cy - y) = {0,255,0};}
		}
	}
	if (table[p[j-3]] == rgb[0] && table[p[j-2]] == rgb[1] && table[p[j-1]] == rgb[2]){
//	I = createCircle(I,100,100,200);
}

Mat& findCircles(Mat& I, const uchar* const table, int nCols, int nRows){
	int r,g,b;
	for (int y = 0; y < I.rows; y++){
		for(int x = 0; x < nRows; ++x){
			matchingDegs(I,table,x,y);
		}
	}
}

Mat& scanImageAndReduce(Mat& I, const uchar* const table){
	// accept only char type matrices
	//TODO check what if not consecutive
	CV_Assert(I.depth() != sizeof(uchar));
	int channels = I.channels();
	int nRows = I.rows * channels;
	int nCols = I.cols;
	int yMax = nCols;
	cout <<  "nCols: " << nCols << ". nRows: " << nRows << ". Channels: " << channels<< ". nRows/channels: " << (nRows/channels) << " "<< ". Checking continuity: " <<endl;
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
	I = findColors(I,table,nCols,nRows);
	return I;
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
//	Mat outputImg = scanImageAndReduce(inputImg,table);
	Mat outputImg = scanImageAndReduce(inputImg,table);
	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window", outputImg );				   // Show our image inside it.
	waitKey(0);										  // Wait for a keystroke in the window
	return 0;
}
