// KabirBinarization.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.hpp>
#include <DImage.h>
#include <BinarizerNiblack.h>

#include <StrokeWidthRandom.h>
#include "BinarizerKabir.h"

using namespace cv;

int main(int argc, char* argv[]){
	Mat mat;

	mat = imread("D:/Datasets/Images/img0.jpg");
	DImage* di = new DImage(mat);
	di->display("DImage Test ");

	DImage* gry = di->convert(CV_8U);
	gry->display("Gray Image ...");

	StrokeWidthRandom stroke_width;
	int sw = gry->strokeWidth(stroke_width);

	cout << "Stroke Width: " << sw << endl;
	BinarizerKabir binarizerKabir;
	binarizerKabir.setStrokeWidth(5);
	binarizerKabir.setK(10);
	DImage* kabir = gry->binarize(binarizerKabir);
	kabir->display("Kabir Binary Image ...");

	waitKey(0);

	return 0;
}