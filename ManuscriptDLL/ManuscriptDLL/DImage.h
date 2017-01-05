#ifndef _DImage_H_ 
#define _DImage_H_

#include "dll_export_import.h"

#include <iostream>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

#include "ImageFilter.h"
#include "ImageBinarizer.h"
#include "ImageEnhancer.h"
#include "ImageConverter.h"
#include "ImageProjector.h"
#include "Contour.h"
#include "Feature.h"
#include "ComponentExtractor.h"
#include "FeatureExtractor.h"
#include "Metric.h"
#include "StrokeWidthEstimator.h" 
#include "TextLineHeightEstimator.h"
#include "TextLineExtractor.h"
#include "PatternSpotter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	An image class that include the back functionality and property of an image </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API DImage {
	cv::Mat      _mat;
	cv::Mat      _mask;
	Contour      _boundary;
	Point2f      _com;   // Center Of Mass ;
	int          _layout;
	vector<Feature*> _featureVector;
	vector<float>    _bof;

public:
	const static int LAYOUT_UNSET = 0;
	const static int LAYOUT_TEXT = 1;
	const static int LAYOUT_DRAW = 2;
	const static int LAYOUT_IMAGE = 3;
	const static int LAYOUT_PARAGRAPH = 4;
	const static int LAYOUT_PAGE_MAIN = 5;
	const static int LAYOUT_PAGE_MARGIN = 6;

public:
	DImage();
	DImage(cv::Mat mat);
	~DImage(void);

	void setLayoutLabel(int label);
	int  getLayoutLabel();

	void setMat(cv::Mat mat);
	cv::Mat& getMat();

	cv::Point2f setCenterOfMass();
	cv::Point2f getCenterOfMass();

	vector<Feature*>& featureVector();
	vector<float>& bof();

	
	double distance(FeatureExtractor* fx, Metric* metric, cv::Mat& other_img);
	int spotSegmentableComponents(FeatureExtractor* fx, Metric* metric, cv::Mat& query_img);
	int spotPattern(PatternSpotter* ps, Metric* metric, cv::Mat& query_img);

	DImage* negate();
	DImage* convert(int to_type);
	DImage* convert(ImageConverter& ic);
	DImage* enhance(ImageEnhancer& image_enhancer);
	DImage* project(ImageProjector& ip);
	DImage* binarize(ImageBinarizer& binarizer);
	DImage* filter(ImageFilter& image_filter);

	void extractFeatures(FeatureExtractor& feature_extractor, vector<Feature*>& feature_list);
	void extractComponents(ComponentExtractor& ex, vector<ConnectedComponent*>& components);
	float strokeWidth(StrokeWidthEstimator& sw);
	float textLineHeight(TextLineHeightEstimator& est);

	void extractTextLines(TextLineExtractor&, vector<TextLine*>& lines);
	// Debug 
	void display(String win);
};


#endif 