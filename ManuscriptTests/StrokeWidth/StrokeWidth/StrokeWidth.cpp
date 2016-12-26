// StrokeWidth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.hpp>
#include <DImage.h>
#include <BinarizerNiblack.h>
#include <StrokeWidthEstimator.h>
#include "StrokeWidthRandom.h"

using namespace cv;

int main(int argc, char* argv[]){
	Mat mat;

	mat = imread("D:/Datasets/Images/img1.jpg");
	DImage* di = new DImage(mat);
	di->display("DImage Test ");

	DImage* gray = di->convert(CV_8U);
	gray->display("Gray Image  ");

	BinarizerNiblack binarizer;
	DImage* binary = gray->binarize(binarizer);
	binary->display("Binary Image ");

	StrokeWidthRandom sw;

	sw.setImage(gray);
	sw.strokeLabel(0);
	cout << "Stroke Width: " << sw.estimate() << endl;

	waitKey(0);
	return 0;
}
