#pragma once

#include <opencv/ml.h>
#include "StrokeWidthEstimator.h"
#include "RunLength.h"

class StrokeWidthRandom : public StrokeWidthEstimator {
protected:
	int _nsamples;
	int _strokeLabel;
	int _nruns;
	RNG _random;
	Ptr<ml::EM>       _mlModel;
	vector<RunLength> _runs;

	
	void  randomSample(cv::Point p);
	uchar drawPixel(Range row, Range col);
	int   getGrayLevelLabel(float level);
	void  collectSegments(Mat mat, Point pt, int label, Point dir, int n, vector<RunLength>& segments);
	int   getMaxRun();
	void learnGaryLevelDistribution();

public:
	StrokeWidthRandom();
	~StrokeWidthRandom();

	void setImage(DImage *img);
	void samples(int samples); 
	void strokeLabel(int label);
	float estimate();
};


