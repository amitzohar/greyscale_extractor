#include <iostream>
#include "DImage.h"
#include "TextLineHeightEstimatorDerived.h"
#include "StrokeWidthEstimatorDerived.h"


using namespace std;
using namespace cv;


int main(int argc, char ** argv){

	if( argc != 3)
	{
		cout <<" Usage: DocImgTools H/W (stands for height or width) the_image_path" << endl;
		return -1;
	}
	Mat img;
	string s= argv[1];
	const char* filename =argv[2];
	if(s=="H"){

		img = imread(filename, CV_LOAD_IMAGE_COLOR);

		if(!img.data)                              // Check for invalid input
		{
			cout <<  "Could not open or find the image" << endl ;
			return -1;
		}

	DImage* dim= new DImage(img);
	TextLineHeightEstimator* lh=new TextLineHeightEstimatorDerived();
	float avgHeight= dim->textLineHeight(*lh);
	cout<< " The average line height is: " << avgHeight << endl;
	}
	else
		if(s=="W"){
			img = imread(filename,  CV_LOAD_IMAGE_GRAYSCALE);

			DImage* dim= new DImage(img);
			StrokeWidthEstimator* sw = new StrokeWidthEstimatorDerived();
			float avgWidth = dim->strokeWidth(*sw);
			cout<< "The average stroke width is: " << avgWidth << endl;
			imshow("Src image",img);
			waitKey(0);
		}

	return 0;
}




