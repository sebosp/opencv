#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std; 

//  X  X  X  
//  TL TC TR Y
//  CL CC CR Y
//  BL BC BR Y
//std::vector< std::vector<int> > x( std::vector<int>(256),256 );

Mat& scanImageAndReduce(Mat& I, const uchar* const table){
    // accept only char type matrices
    CV_Assert(I.depth() != sizeof(uchar));
    int channels = I.channels();
    int nRows = I.rows * channels;
    int nCols = I.cols;
    cout <<  "nCols: " << nCols << ". nRows/3: " << (nRows/3) << endl;
    if (I.isContinuous()){
        nCols *= nRows;
        nRows = 1;
    }
    cout <<  "nCols: " << nCols << ". nRows: " << nRows << endl;
    int i,j;
    uchar* p;
    cout << "Initializing maxColors" << endl;
    vector<int> vI1Matrix(256,0);
    vector< vector<int> > vI2Matrix(256, vI1Matrix);
    vector< vector< vector<int> > > maxColors(256, vI2Matrix);
    //uchar maxColors[256][256][];
    int r,g,b;
    for( i = 0; i < nRows; ++i){
        p = I.ptr<uchar>(i);
        for ( j = 0; j < nCols; ++j){
//            p[j] = table[p[j]];//Color reduction
            if (!(j % 3) && j > 0){
//                cout << "Adding R: " << p[j-3] << ". G: " << p[j-2] << ". B: " << p[j-1] << ". maxColors: " << maxColors[p[j-3]][p[j-2]][p[j-1]] << endl;
//            	maxColors[p[j-3]][p[j-2]][p[j-1]]++;
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
    //cout << "Max Color set is: R: " << rgb[0] << ". G: " << rgb[1] << ". B: " << rgb[2] << ". maxCount is " << maxCount << endl;
    //Perform color reduction.
    for(i = 0; i < nRows; ++i){
        p = I.ptr<uchar>(i);
        for ( j = 0; j < nCols; ++j){
            //cout << table[p[j]] << ":";
            if (!(j % 3) && j > 0){
	        //Set max used color to white, altho it should be set to alpha next time
//                if (p[j-3] == rgb[0] && p[j-2] == rgb[1] && p[j-1] == rgb[2]){ 
                if (table[p[j-3]] == rgb[0] && table[p[j-2]] == rgb[1] && table[p[j-1]] == rgb[2]){ 
	                p[j-3] = 255;
        	        p[j-2] = 255;
                	p[j-1] = 255;
                }
		//cout << endl;
            }
        }
    }
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
    imshow( "Display window", inputImg );                   // Show our image inside it.
    waitKey(0);
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
//    Mat outputImg = scanImageAndReduce(inputImg,table);
    Mat outputImg = scanImageAndReduce(inputImg,table);
    namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", outputImg );                   // Show our image inside it.
    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
