#ifndef _EVOLUTION_MAPS_H_ 
#define _EVOLUTION_MAPS_H_ 

#include <vector>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <DImage.h>
#include <Feature.h>
#include <ComponentExtractorBinary.h>
#include <ComponentAttributeGetter.h>
#include "cv_macros.h"

using namespace std ;

class  EvolutionMaps{
	cv::Mat _img ;
	cv::Mat _map ;
	int     _mode;
	int     _bins;
	vector<int> _thresholds ;

public:
	EvolutionMaps() {
		_bins = 256;
		_mode = UNIFORM ;
		for (int i = 1; i < 255; i++)
			_thresholds.push_back(i);
	}

	EvolutionMaps(cv::Mat img, int bin_count, int mode);

	~EvolutionMaps(void);
	
	void setMode(int mode) {
		_mode = mode;
	}

	void setImage(cv::Mat img) {
		_img = img;
	}

	void setThreshold();
	cv::Mat getMap(ComponentAttributeGetter* attributer);
	
	static const int UNIFORM  = 1 ;
	static const int ADAPTIVE = 2 ;
};

#endif 