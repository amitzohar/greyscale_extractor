
#include "stdafx.h"
#include "DImage.h"
#include "ComponentExtractorBinary.h"


using namespace cv;

DImage::DImage(){
	_layout = LAYOUT_UNSET;
	_com = Point(-1, -1);
}

DImage::DImage(cv::Mat mat){
	_mat = mat;
	_layout = LAYOUT_UNSET;
	_com = Point(-1, -1);
}

DImage::~DImage(void){
	_mat.deallocate();
}

void DImage::setLayoutLabel(int label) {
	_layout = label;
}

int DImage::getLayoutLabel() {
	return _layout;
}

void DImage::setMat(cv::Mat mat){
	_mat = mat;
}

cv::Mat& DImage::getMat(){
	return _mat;
}

Point2f DImage::getCenterOfMass(){
	return _com;
}

vector<Feature*>& DImage::featureVector(){
	return _featureVector;
}

vector<float>& DImage::bof(){
	return _bof;
}

DImage* DImage::negate(){
	DImage *img = new DImage(cv::Scalar::all(255) - _mat);
	return img;
}

double DImage::distance(FeatureExtractor* fx, Metric* metric, cv::Mat& other_img){
	vector<Feature*> this_features;
	vector<Feature*> other_features;
	fx->extract(_mat, this_features);
	fx->extract(other_img, other_features);
	double d = metric->distance(this_features, other_features);
	return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Spot segmentable components. If first segment the image in components and compare the 
/// 			query image with each component </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="fx">			[in,out] If non-null, the effects. </param>
/// <param name="metric">   	[in,out] If non-null, the metric. </param>
/// <param name="query_img">	[in,out] The query image. </param>
///
/// <returns>	An int. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int DImage::spotSegmentableComponents(FeatureExtractor* fx, Metric* metric, cv::Mat& query_img){
	ComponentExtractorBinary     component_extractor(this);
	vector<ConnectedComponent*>  components;
	extractComponents(component_extractor, components);

	vector<Feature*> query_features;
	fx->extract(query_img, query_features);

	vector<ConnectedComponent*>::iterator iter;
	for (iter = components.begin(); iter != components.end(); iter++){
		vector<Feature*> this_features;
		fx->extract(_mat, this_features);

		double d = metric->distance(this_features, query_features);
	}
	return (int)query_features.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Spot the appearance of the pattern in the query image with the image  </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="ps">			[in,out] If non-null, the effects. </param>
/// <param name="metric">   	[in,out] If non-null, the metric. </param>
/// <param name="query_img">	[in,out] The query image. </param>
///
/// <returns>	An int. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int DImage::spotPattern(PatternSpotter* ps, Metric* metric, cv::Mat& query_img){

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Converts an image to a different format </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="to_type">	Type of to. </param>
///
/// <returns>	null if it fails, else a DImage*. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

DImage* DImage::convert(ImageConverter& ic){
	ic.setImage(this);
	return ic.convert();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Enhances the given image using image_enhancer. </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="ih">	[in,out] The ih. </param>
///
/// <returns>	null if it fails, else a DImage*. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

DImage* DImage::enhance(ImageEnhancer& image_enhancer){
	image_enhancer.setImage(this);
	return image_enhancer.enhance();
}

DImage* DImage::project(ImageProjector& ip){
	ip.setImage(this);
	return ip.project();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Binarizes the image using the given binarizer class . </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="binarizer">	[in,out] The binarizer. </param>
///
/// <returns>	null if it fails, else a DImage*. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

DImage* DImage::binarize(ImageBinarizer& binarizer) {
	binarizer.setImage(this);
	DImage* img = binarizer.binarize();
	return img;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Filters the given image using image_filter. </summary>
///
/// <remarks>	Jihad, 07/01/2015. </remarks>
///
/// <param name="image_filter">	[in,out] A filter specifying the image. </param>
///
/// <returns>	null if it fails, else a DImage*. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

DImage* DImage::filter(ImageFilter& image_filter) {
	image_filter.setImage(this);
	DImage* res = image_filter.filter();
	return res;
}

void DImage::extractFeatures(FeatureExtractor& feature_extractor, vector<Feature*>& feature_list){
	feature_extractor.setImage(this->getMat());
	feature_extractor.extract(feature_list);
}

void DImage::extractComponents(ComponentExtractor& ex, vector<ConnectedComponent*>& components){
	ex.setImage(this);
	ex.extract(components);
}

void DImage::extractTextLines(TextLineExtractor& ex, vector<TextLine*>& lines){
	ex.setImage(this);
	ex.extract(lines);
}

float DImage::strokeWidth(StrokeWidthEstimator& sw){
	sw.setImage(this);
	return sw.estimate();
}

float DImage::textLineHeight(TextLineHeightEstimator& est){
	est.setImage(this);
	return est.estimate();
}


DImage*  DImage::convert(int to_type){
	cv::Mat gray;
	if (_mat.type() == to_type)
		return this;

	if (_mat.channels() == 1){
		_mat.convertTo(gray, CV_8U);
		return new DImage(gray);
	}

	if (_mat.channels() == 3 && to_type == CV_8U) {
		cv::Mat gray;
		cvtColor(_mat, gray, CV_BGR2GRAY);
		return new DImage(gray);
	}
	return this;

}

Point2f DImage::setCenterOfMass(){
	_com = Point(0, 0);
	float npixels = (float)(_mat.cols * _mat.rows);
	for (int col = 0; col < _mat.cols; col++)
		for (int row = 1; row < _mat.rows; row++){
		float g = (float)(_mat.at<char>(row, col));
		_com.x += col * g;
		_com.y += row * g;
		}
	_com.x /= npixels * 255.0f;
	_com.y /= npixels * 255.0f;
	return _com;
}

// Debug 
void DImage::display(String win){
	namedWindow(win, cv::WINDOW_NORMAL);
	imshow(win, this->getMat());
}