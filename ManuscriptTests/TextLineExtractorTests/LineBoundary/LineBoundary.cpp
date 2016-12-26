// LineBoundary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.hpp>
#include <DImage.h>
#include <BinarizerKabir.h>
#include <StrokeWidthRandom.h>
#include <ComponentExtractorBinary.h>
#include <TextLine.h>

using namespace cv;

void sort(vector<ConnectedComponent*>& list){

}

int main(int argc, char* argv[]){
	Mat mat;
	struct {
		bool operator()(ConnectedComponent* a, ConnectedComponent* b){
			return a->getContour().getCenter().x < b->getContour().getCenter().x;
		}
	} comp;

	mat = imread("D:/Datasets/Images/printed_eng.jpg");
	DImage* di = new DImage(mat);
	di->display("DImage Test ");

	DImage* gry = di->convert(CV_8U);
	gry->display("Gray Image ...");

	StrokeWidthRandom stroke_width;
	int sw = gry->strokeWidth(stroke_width);

	cout << "Stroke Width: " << sw << endl;
	BinarizerKabir binarizerKabir;
	binarizerKabir.setStrokeWidth(sw);
	binarizerKabir.setK(10);
	DImage* kabir = gry->binarize(binarizerKabir);
	kabir->setMat(cv::Scalar::all(255) - kabir->getMat());
	kabir->display("Kabir Binary Image ...");
	ComponentExtractorBinary extractor;
	vector<ConnectedComponent*> components;
	kabir->extractComponents(extractor, components);
	//std::sort(components.begin(), components.end(), comp);
	TextLine text_line(kabir->getMat());
	text_line.setLineBoundary(components);

	waitKey(0);

	return 0;
}

