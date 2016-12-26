// EvoultionMapTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.h>
#include <DImage.h>
#include "EvolutionMaps.h"
#include "ComponentAttributeStrokeWidth.h"

int main(int argc, char* argv[]){

	Mat mat;

	mat = imread("D:/Datasets/Images/img1.jpg");
	DImage* di = new DImage(mat);
	di->display("DImage Test ");

	DImage* gry = di->convert(CV_8U);
	gry->display("Gray Image ...");

	double min, max;
	EvolutionMaps em(gry->getMat(), 10, EvolutionMaps::UNIFORM);
	ComponentAttributeStrokeWidth getter;
	
	//em.setThreshold();

	Mat map = em.getMap(&getter);
	minMaxLoc(map, &min, &max);
	cout << "Map MaxL " << min << "  Max: " << max << endl;
	Mat normalized_map;
	cv::normalize(map, normalized_map, 1.0, 0.0, NORM_MINMAX, CV_32F);
	//map.convertTo(normalized_map, CV_32F);

	DImage* img = new DImage(normalized_map);
	img->display("Testing Map ");
	waitKey(0);
	return 0;
}

