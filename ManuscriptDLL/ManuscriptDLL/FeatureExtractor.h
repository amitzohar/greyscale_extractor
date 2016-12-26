#ifndef _FEATURE_EXTRACTOR_H 
#define _FEATURE_EXTRACTOR_H 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include "Feature.h"


class MANUSCRIPT_API FeatureExtractor {
protected:
	Mat _image ;

public:
	FeatureExtractor(void){;}
	FeatureExtractor(cv::Mat& img){
		_image = img ;
	}
	~FeatureExtractor(void){;}

	void setImage(cv::Mat& img) { _image = img ;}
	void extract(cv::Mat& img, vector<Feature*>& list){
		_image = img ;
		extract(list) ; 
	}

	virtual void extract(vector<Feature*>& ) = 0;
	
};

#endif
