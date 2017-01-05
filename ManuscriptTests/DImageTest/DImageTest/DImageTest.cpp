// DImageTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv\cv.hpp>
#include <DImage.h>

using namespace cv;

int main(int argc, char* argv[]){
	Mat mat;

	mat = imread("D:/Datasets/Images/img1.jpg");
	DImage di(mat);

	di.display("DImage Test ");
	waitKey(0);
	return 0;
}

