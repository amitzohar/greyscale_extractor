#include "stdafx.h"
#include <opencv/cv.hpp>
#include <DImage.h>
#include <ImageTools.h>
#include <BinarizerNiblack.h>
#include <BinarizerOtsu.h>
#include <BinarizerGlobal.h>
#include <ComponentExtractorBinary.h>
#include <DistanceTransformSigned.h>
#include "TextLineExtractorGraySeam.h"

using namespace cv;

int main(int argc, char* argv[]){
	Mat mat;

	mat = imread("C://Images//docset-811_doc-812_img.jpg");
	//mat = imread("D:/Datasets/Images/printed1_eng.jpg");
	//mat = imread("D:/Datasets/Images/ellipse.jpg");
	DImage* di = new DImage(mat);
	
	DImage* gry = di->convert(CV_8U);
	gry->display("Gray Image ...");

	//BinarizerNiblack binarizer;	
	/**
	DImage* negative = binary->negate();
	
	vector<ConnectedComponent*> components;
	ComponentExtractorBinary extractor;
	negative->extractComponents(extractor, components);

	ImageTools::draw(binary->getMat(), components, Scalar(0, 255, 0));
	//cout << "Components: " << components.size() << endl;
	DistanceTransformSigned transform(negative);
	transform.setComponents(&components);
	DImage* dist_map = transform.transform();

	Mat vis = Mat(dist_map->getMat().size(), CV_32FC1);
	normalize(dist_map->getMat(), vis, 0, 1, NORM_MINMAX, CV_32F);
	//ImageTools::display(" Distance ", vis);
	*/
	vector<TextLine*> text_lines;

	TextLineExtractorGraySeam textLineExtractor(10);
	gry->extractTextLines(textLineExtractor, text_lines);
	
	cout << "Text Lines " << text_lines.size() << endl;
	waitKey(0);

	return 0;
}

