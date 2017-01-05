#include "stdafx.h"
#include <iostream>
#include "ImageTools.h" 
#include "EvolutionMaps.h"

using namespace cv;

EvolutionMaps::EvolutionMaps(Mat img, int bin_count, int mode ){
	int histogram[256] ;
	_bins = bin_count;
	_img = img;
	_mode = mode;
	ImageTools::histogramGrayImage(img, histogram, bin_count);
	setThreshold();
}

EvolutionMaps::~EvolutionMaps(void){
}

void EvolutionMaps::setThreshold() {
	if (_mode == UNIFORM) {
		int delta = (int)(256 / _bins );
		for (int i = delta; i < 256; i++) {
			_thresholds.push_back(i);
			i += delta;
		}
	}
	else {
		// Deal with adaptive 
	}
}

Mat EvolutionMaps::getMap(ComponentAttributeGetter* attributer){
	vector<vector<int>> vect_map ;
	
	for (int athreshold : _thresholds){
		Mat bin_img;
		vector<ConnectedComponent*> components;
		vector<int> map_row;
		threshold(_img, bin_img, athreshold, 255, cv::THRESH_BINARY);
		DImage* dimg = new DImage(bin_img);
		ComponentExtractorBinary extractor;
		dimg->extractComponents(extractor, components);
		for (ConnectedComponent* component : components){
			int attribute = attributer->getAttributeValue(component);
			if (attribute >= map_row.size())
				map_row.insert(map_row.end(), attribute - map_row.size() + 1, 0);
			map_row[attribute]++;
		}
		vect_map.push_back(map_row);
	}
	// Compute the size of the map, which is detemined by 
	// the largest vector
	int max_size = 0 ;
	for (vector<int>& list : vect_map){
		if (list.size() > max_size)
			max_size = list.size();
	}

	int row = 0 ;
	_map = Mat::zeros(_thresholds.size(), max_size, CV_32S);
	for (vector<int>& list : vect_map){
		double min, max; 
		for (int col = 0; col < list.size(); col++){
			_map.at<int>(row, col) = list[col];
		}
		row++;
	}

	return _map ;
}