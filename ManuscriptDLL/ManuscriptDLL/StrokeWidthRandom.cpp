#include "stdafx.h"
#include <opencv\ml.h>

#include "DImage.h"


#include "StrokeWidthRandom.h"

#define INRANGE(R, C)  ((R >= 0) && ( R < mat.rows) && ( C >= 0) && ( C < mat.cols) )

using namespace cv;
using namespace std;

StrokeWidthRandom::StrokeWidthRandom(){
	_nsamples = 100 ;
	_nruns = 10 ;
	_strokeLabel = 0;
	_mlModel = ml::EM::create();
}


StrokeWidthRandom::~StrokeWidthRandom(){
}

void  StrokeWidthRandom::setImage(DImage *img){
	StrokeWidthEstimator::setImage(img);
	_nsamples = (int) (img->getMat().size().area() / 1000);
	_nruns = (int)(_nsamples / 100);
}

void StrokeWidthRandom::samples(int samples) {
	_nsamples = samples;
	_nruns = (int)(_nsamples / 10);
}

void StrokeWidthRandom::strokeLabel(int label){
	_strokeLabel = label;
}


uchar StrokeWidthRandom::drawPixel(Range row, Range col){
	int r = _random.uniform(row.start, row.end);
	int c = _random.uniform(col.start, col.end);
	return _image->getMat().at<uchar>(r, c);
}

int StrokeWidthRandom::getMaxRun(){
	int max = 0 ;
	for each (RunLength r in _runs){
		if(r.length() > max)
			max = r.length();
	}
	return max;
}

void StrokeWidthRandom::learnGaryLevelDistribution(){
	Mat mat = _image->getMat();
	Range row_range(0, mat.rows);
	Range col_range(0, mat.cols);
	cv::Mat samples(_nsamples, 1, CV_32FC1);
	for (int i = 0; i < _nsamples; i++)
		samples.at<float>(i, 0) = (float)drawPixel(row_range, col_range);
	_mlModel->setClustersNumber(2);
	_mlModel->trainEM(samples);
}

int StrokeWidthRandom::getGrayLevelLabel(float level){
	Mat props;
	Mat test(1, 1, CV_32FC1);
	test.at<float>(0, 0) = level;
	_mlModel->predict2(test, props);
	if (props.at<float>(0, 0) < 0.5)
		return 0;
	else
		return 1;
}

void StrokeWidthRandom::collectSegments(Mat mat, Point pt, int lp, Point dir, int n, vector<RunLength>& segments){
	RunLength run;
	Point offset = Point(0,0);
	while ( INRANGE(pt.y + offset.y, pt.x + offset.x ) &&  
		    getGrayLevelLabel(mat.at<uchar>(pt.y + offset.y, pt.x + offset.x)) == lp)
		offset += dir;

	for (int i = 0; i < n; i++){
		if ( INRANGE(pt.y + offset.y, pt.x + offset.x)) {
			run.start((offset.x == 0) ? offset.y : offset.x);
			int label = getGrayLevelLabel(mat.at<uchar>(pt.y + offset.y, pt.x + offset.x));
			run.label(label);
			while (INRANGE(pt.y + offset.y, pt.x + offset.x) && (pt.x + offset.x < mat.cols) &&
				getGrayLevelLabel(mat.at<uchar>(pt.y + offset.y, pt.x + offset.x)) == label) {
				offset += dir;
			}
			run.end((offset.x == 0) ? offset.y : offset.x);
			segments.push_back(run);
		}
	}
}

void StrokeWidthRandom::randomSample(cv::Point p){
	Mat mat = _image->getMat();
	int label = getGrayLevelLabel(mat.at<uchar>(p.y, p.x));
	vector<RunLength> intervals[8];

	int dir = 0;
	for (int i = -1; i < 2; i++){
		for (int j = -1; j < 2; j++){
			if (!(i == 0 && j == 0))
				collectSegments(mat, p, label, Point(i, j), _nruns, intervals[dir++]);
		}
	}
	for (int i = 0; i < 8; i++){
		_runs.insert(_runs.end(), intervals[i].begin(), intervals[i].end());
	}

	for (int i = 0; i < 4; i++){
		if ((intervals[i].size() > 0) && (intervals[7 - i].size() > 0)){
			RunLength run(intervals[i][0].start(), intervals[7 - i][0].start());
			run.label(1 - intervals[i][0].label());
			_runs.push_back(run);
		}
	}
	
}

float StrokeWidthRandom::estimate() { 
	Mat mat = _image->getMat();

	learnGaryLevelDistribution();
	
	for (int i = 0; i < _nsamples ; i++){
		int r = _random.uniform(0, mat.rows);
		int c = _random.uniform(0, mat.cols);
		randomSample(Point(c, r));
	}
	int max_run = getMaxRun();
	int* histogram = new int[max_run];
	memset(histogram, 0, max_run*sizeof(int));
	for each (RunLength r in _runs){
		if (r.label() == _strokeLabel )
			histogram[r.length()] ++;
	}

	int occurence = 0;
	int width_idx = 0;
	for (int i = 0; i < max_run; i++){
		if (occurence < histogram[i]) {
			width_idx = i;
			occurence = histogram[i];
		}
	}

	return width_idx;
}
