#ifndef _PATTERN_SPOTTER__H_ 
#define _PATTERN_SPOTTER__H_ 

#include <vector>
#include <opencv/cv.h>
#include "Feature.h"
#include "Metric.h" 
#include "FeatureExtractor.h"

class PatternSpotter{
	float _match_threshold ;
	FeatureExtractor* _feature_extractor ;
	vector<Feature*>  _feature_list ;
	Metric*           _metric ;

public:
	PatternSpotter(void){
		_match_threshold = 0 ;
		_feature_extractor = NULL ;
		_metric = NULL ;
		_feature_list.clear() ;
	}

	PatternSpotter(FeatureExtractor* feature_extractor, Metric* metric, float threshold){
		_match_threshold   = threshold ;
		_feature_extractor = feature_extractor ;
		_metric            = metric ;
		_feature_list.clear() ;
	}

	void setFeatureExtractor(FeatureExtractor* feature_extractor){
		_feature_extractor = feature_extractor ;
	}

	void setMetric(Metric* metric){
		_metric = metric ;
	}

	void setMatchthreshold(float threshold){
		_match_threshold = threshold ;
	}

	double spot(Mat img, vector<Feature*> features){
		vector<Feature*> features_img ;
		_feature_extractor->setImage(img);
		_feature_extractor->extract(features_img);
		return _metric->distance(features_img, features);
	}

	double spot(Mat img, Mat qry){	
		vector<Feature*> features_qry ;
		_feature_extractor->setImage(qry);
		_feature_extractor->extract(features_qry);
		return spot(img, features_qry);
	}

	~PatternSpotter(void){
	}
};

#endif