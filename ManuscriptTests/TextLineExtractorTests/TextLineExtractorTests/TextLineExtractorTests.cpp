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

	mat = imread("C:\\Images\\G67CU.png");
	//mat = imread("C:\\Users\\Study\\Courses\\images\\printed_04.jpg");
	DImage* di = new DImage(mat);
	
	DImage* gry = di->convert(CV_8U);
	gry->display("Gray Image ...");
	
	vector<TextLine*> text_lines;

	TextLineExtractorGraySeam textLineExtractor(9);
	gry->extractTextLines(textLineExtractor, text_lines);

	cout << "Text Lines " << text_lines.size() << endl;
	waitKey(0);

	return 0;
}

