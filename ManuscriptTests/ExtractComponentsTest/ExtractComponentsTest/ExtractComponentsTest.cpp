// ExtractComponentsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.hpp>
#include <DImage.h>
#include <BinarizerOtsuGray.h>
#include <ComponentExtractorBinary.h>

using namespace cv;

int main(int argc, char* argv[]){
	Mat mat;
	vector<ConnectedComponent*> components;

	mat = imread("D:/Datasets/Images/img1.jpg");
	DImage di(mat);
	DImage* gray = di.convert(CV_8U);
	BinarizerOtsuGray binarizer;
	DImage* binary = gray->binarize(binarizer);

	ComponentExtractorBinary extractor;
	binary->extractComponents(extractor, components);

	di.display("DImage Test ");
	gray->display("Gray");
	binary->display("Binary");

	cout << "Components: " << components.size() << endl ;
	waitKey(0);

	return 0;
}

