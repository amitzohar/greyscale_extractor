
#include "stdafx.h"
#include "DImage.h"
#include "ImageTools.h"
#include "stdext.h"
#include "TextLineExtractorGraySeam.h"
#include "DistanceTransformSigned.h"
#include "ComponentExtractorBinary.h"
#include "BinarizerGlobal.h"
#include "TextLine.h"

using namespace cv;
using namespace std;

struct {
	bool operator()(ConnectedComponent* a, ConnectedComponent* b){
		return a->getContour().getCenter().x < b->getContour().getCenter().x;
	}
} comp;

struct {
	bool operator()(Point a, Point b)
	{
		return a.x < b.x;
	}
} customLess;

const unsigned int TextLineExtractorGraySeam::NUM_OF_AVG_PTS = 200;


/**
* TextLineExtractorGraySeam Constructor
* Input:
*	rows- amount of rows to extract.
*/
TextLineExtractorGraySeam::TextLineExtractorGraySeam(int rows) : m_rows(rows), m_rowHeight(0) {
}

/**
* TextLineExtractorGraySeam Destructor
*/
TextLineExtractorGraySeam::~TextLineExtractorGraySeam(){
}

/**
* Extract - extracts TextLine's from image.
* Input:
*	toExtract- Vector to store extracted text lines to.
* Notes:
*	Must first call setImage with binary image.
*/
void TextLineExtractorGraySeam::extract(vector<TextLine*>& text_lines){
	vector<ConnectedComponent*> component_list;

	DImage* dist_map = calculateDistanceMap(component_list);

	Mat vis = Mat(dist_map->getMat().size(), CV_32FC1);
	m_rowHeight = vis.rows / m_rows;
	normalize(dist_map->getMat(), vis, 0, 1, NORM_MINMAX, CV_32F);
	ImageTools::display(" Distance ", vis);
	
	Mat energy_map = getEnergyMapFromDistance(dist_map->getMat());
	normalize(energy_map, vis, 0, 1, NORM_MINMAX, CV_32F);
	ImageTools::display(" Energy Map ", vis);
	for (int i = 0; i < m_rows; i++) {
		Mat neg_energy_map = negateEnergyMap(energy_map);
		vector<cv::Point> seam = getNextSeam(energy_map);
		vector<cv::Point> upperSeam = getUpperSeam(energy_map, neg_energy_map, seam);
		vector<cv::Point> lowerSeam = getLowerSeam(energy_map, neg_energy_map, seam);

		drawDisplay(seam);
		drawDisplay(upperSeam);
		drawDisplay(lowerSeam);
		normalize(energy_map, vis, 0, 1, NORM_MINMAX, CV_32F);
		TextLine line = TextLine();
		line.setLowerBound(&lowerSeam);
		line.setUpperBound(&upperSeam);
		removeLine(energy_map, upperSeam, lowerSeam);
		ImageTools::displayFresh("Energy map", energy_map);
		text_lines.push_back(&line);
		waitKey(0);
	}
}

/**
 * calculateDistanceMap - calculate the distance map of the image by binarizing it first.
 * Input: 
 *    component_list - The components of the distance map will be returned here
 */
DImage* TextLineExtractorGraySeam::calculateDistanceMap(vector<ConnectedComponent*> &component_list) {
	ComponentExtractorBinary component_extractor;

	BinarizerGlobal binarizer(128);
	DImage* binary = _image->binarize(binarizer);
	binary->display(" Binary Image ...");

	DImage* negative = binary->negate();
	negative->extractComponents(component_extractor, component_list);
	DistanceTransformSigned transform(binary);
	transform.setComponents(&component_list);
	return transform.transform();
}

vector<cv::Point> TextLineExtractorGraySeam::getLowerSeam(Mat energy_map, Mat neg_energy_map, vector<cv::Point> medSeam) {
	int index = 0, x = 0, y = 0, delta = 0, finalDelta = 0;
	vector<cv::Point> avgSeams[NUM_OF_AVG_PTS];
	int buffer = (energy_map.cols - 1) / NUM_OF_AVG_PTS;
	Point startingPoints[NUM_OF_AVG_PTS];

	for (int i = 0; i < NUM_OF_AVG_PTS; i++) {
		int x = medSeam.at(index).x;
		int y = medSeam.at(index).y + m_rowHeight / 2.0;
		if (y >= energy_map.rows) {
			y = energy_map.rows - 1;
		}
		startingPoints[i] = Point(x, y);
		avgSeams[i] = getSeam(neg_energy_map, startingPoints[i]);
	}

	vector<cv::Point> final;
	for (int i = 0; i < avgSeams[0].size(); i++) {
		int sum = 0;
		for (int j = 0; j < NUM_OF_AVG_PTS; j++) {
			sum += avgSeams[j].at(i).y;
		}
		final.push_back(Point(avgSeams[0].at(i).x, sum / NUM_OF_AVG_PTS));
	}
	std::sort(final.begin(), final.end(), customLess);
	return final;
}

vector<cv::Point> TextLineExtractorGraySeam::getUpperSeam(Mat energy_map, Mat neg_energy_map, vector<cv::Point> medSeam) {
	int index = 0, x = 0, y = 0, delta = 0, finalDelta = 0;
	double max = 0;
	vector<cv::Point> avgSeams[NUM_OF_AVG_PTS];
	int buffer = (energy_map.cols - 1) / NUM_OF_AVG_PTS;
	Point startingPoints[NUM_OF_AVG_PTS];

	for (int i = 0; i < NUM_OF_AVG_PTS; i++) {
		int x = medSeam.at(index).x;
		int y = medSeam.at(index).y - m_rowHeight / 2.0;
		if (y < 0) {
			y = 0;
		}
		startingPoints[i] = Point(x, y);
		avgSeams[i] = getSeam(neg_energy_map, startingPoints[i]);
	}

	vector<cv::Point> final;
	for (int i = 0; i < avgSeams[0].size(); i++) {
		int sum = 0;
		for (int j = 0; j < NUM_OF_AVG_PTS; j++) {
			sum += avgSeams[j].at(i).y;
		}
		final.push_back(Point(avgSeams[0].at(i).x, sum / NUM_OF_AVG_PTS));
	}
	std::sort(final.begin(), final.end(), customLess);
	return final;
}

int TextLineExtractorGraySeam::removeLine(Mat energy_map, vector<cv::Point> upperSeam, vector<cv::Point> lowerSeam){
	double mx, mn;
	minMaxLoc(energy_map, &mn, &mx);
	for (int col = 0; col < upperSeam.size(); col++){
		for (int row = upperSeam.at(col).y; row <= lowerSeam.at(col).y; row++) {
			energy_map.at<float>(row, col) = mx;
		}
	}
	return 0;
}

vector<cv::Point> TextLineExtractorGraySeam::getNextSeam(Mat energy_map){
	Point min_pt;
	double min_val, max_val;
	minMaxLoc(energy_map.col(energy_map.cols - 1), &min_val, &max_val, &min_pt);
	min_pt.x = energy_map.cols - 1;

	vector<Point> seam;
	int row = min_pt.y;
	seam.push_back(min_pt);
	for (int col = min_pt.x - 1; col >= 0; col--){
		row = getMinRow(energy_map, row, col, 1);
		seam.push_back(Point(col, row));
	}
	return seam;
}

/**
* getSeamComponents - transfer all components from component_list that intersect seam to seam_component_list.
* Input:
*	component_list-	all remaining components not assigned to rows.
*   seam_component_list - list of all components that intersect seam.
*	seam - list of points that represents a seam (line) passing through the Heat Map minimum.
*/
vector<ConnectedComponent*> TextLineExtractorGraySeam::getSeamComponents(vector<Point> seam, vector<ConnectedComponent*> &component_list){
	vector<ConnectedComponent*> seam_components;
	vector<ConnectedComponent*> rect_components;
	Rect seam_rect = boundingRect(seam);
	
	for each (ConnectedComponent* comp in component_list){
		if ((seam_rect & comp->getBoundRect()).area() > 0){
			rect_components.push_back(comp);
		}
	}
	sort(rect_components.begin(), rect_components.end(), comp);

	for (int i = 0; i < seam.size(); i++){
		vector<ConnectedComponent*>::iterator iter;
		for (iter = rect_components.begin(); iter != rect_components.end(); iter++){
		
			if ((*iter)->getContour().getBoundRect().contains(seam[i]) && 
				(*iter)->getContour().inside(seam[i]) >= 0 ){
				seam_components.push_back((*iter));
				rect_components.erase(iter);
				break;
			}
		}
	}
	return seam_components;
}



int TextLineExtractorGraySeam::getMinRow(Mat energy_map, int row, int col, int range){
	float min_val = energy_map.at<float>(row, col);
	int min_row = row;
	for (int i = -range; i <= range ; i++)
		if ((row + i >= 0) && (row + i < energy_map.rows) && energy_map.at<float>(row + i, col) < min_val){
				min_row = row + i;
				min_val = energy_map.at<float>(row + i, col);
			}
	return min_row;
}

vector<Point> TextLineExtractorGraySeam::getSeam(Mat energy_map, Point start){
	vector<Point> seam;
	int row = start.y;
	seam.push_back(Point(start.x, start.y));
	for (int col = start.x - 1; col >= 0 ; col--){
		row = getMinRow(energy_map, row, col, 1);
		seam.push_back(Point(col, row));
	}
	row = start.y;
	for (int col = start.x + 1; col < energy_map.cols; col++){
		row = getMinRow(energy_map, row, col, 1);
		seam.push_back(Point(col, row));
	}
	return seam;
}

Mat TextLineExtractorGraySeam::getEnergyMapFromDistance(Mat dst_map) {
	int last_row = dst_map.rows - 1;
	int last_col = dst_map.cols - 1;

	Mat energy_map_left = Mat::zeros(dst_map.size(), CV_32FC1);
	dst_map.col(0).copyTo(energy_map_left.col(0));
	dst_map.row(0).copyTo(energy_map_left.row(0));
	
	vector<float> w(3);
	w[0] = w[2] = 1 / sqrt(2);
	w[1] = 1;

	for (int col = 1; col < energy_map_left.cols; col++)
		for (int row = 1; row < energy_map_left.rows - 1; row++) {
			float min_val = minValueOfThreeRows(energy_map_left, row, col - 1, w);
			energy_map_left.at<float>(row, col) = 2.0*dst_map.at<float>(row, col) + min_val;
		}

	// Last Row 
	for (int col = 1; col < energy_map_left.cols; col++){
		float min_val = min(w[0]*energy_map_left.at<float>(last_row-1, col-1), w[1]*energy_map_left.at<float>(last_row, col - 1));
		energy_map_left.at<float>(last_row, col) = 2.0*dst_map.at<float>(last_row, col) + min_val;
	}

	Mat energy_map_right = Mat::zeros(dst_map.size(), CV_32FC1);
	dst_map.col(last_col).copyTo(energy_map_right.col(last_col));
	dst_map.row(last_row).copyTo(energy_map_right.row(last_row));

	for (int col = last_col - 1; col >= 0; col--)
		for (int row = last_row - 1; row > 1; row--) {
			float min_val = minValueOfThreeRows(energy_map_right, row, col + 1, w);
			energy_map_right.at<float>(row, col) = 2.0*dst_map.at<float>(row, col) + min_val;
		}

	// Last Row
	for (int col = last_col - 1; col >= 0; col--) {
		float min_val = min(w[0] * energy_map_right.at<float>(0, col + 1), w[1] * energy_map_right.at<float>(1, col + 1));
		energy_map_right.at<float>(0, col) = 2.0*dst_map.at<float>(0, col) + min_val;
	}

	//cout << "Distance: \n " << dst_map << endl;	
	//cout << "Enregy: \n " << energy_map << endl;

	Mat energy_map_left_norm = Mat::zeros(dst_map.size(), CV_32FC1);
	Mat energy_map_right_norm = Mat::zeros(dst_map.size(), CV_32FC1);
	normalize(energy_map_left, energy_map_left_norm, 0, 1, NORM_MINMAX, CV_32F);
	normalize(energy_map_right, energy_map_right_norm, 0, 1, NORM_MINMAX, CV_32F);
	ImageTools::display("Energy map left", energy_map_left_norm);
	ImageTools::display("Energy map right", energy_map_right_norm);

	Mat energy_map = Mat::zeros(dst_map.size(), CV_32FC1);
	for (int col = 0; col <= last_col; col++)
		for (int row = 0; row <= last_row; row++)
			energy_map.at<float>(row, col) = energy_map_right_norm.at<float>(row, col) + energy_map_left_norm.at<float>(row, col);

	return energy_map;
}

Mat TextLineExtractorGraySeam::negateEnergyMap(Mat energy_map) {
	Mat neg_energy_map = Mat::zeros(energy_map.size(), CV_32FC1);
	energy_map.copyTo(neg_energy_map);
	for (int i = 0; i < neg_energy_map.cols; i++) {
		for (int j = 0; j < neg_energy_map.rows; j++) {
			neg_energy_map.col(i).row(j) = -neg_energy_map.col(i).row(j);
		}
	}
	return neg_energy_map;
}

/**
* getMinRow - Return row where the last column is the minimum.
* Input:
*	EnergyMap.
*/

int TextLineExtractorGraySeam::getMinRow(Mat EnergyMap){
	int minRow = 0;
	float minValue = std::numeric_limits<float>::max();
	for (int i = 0; i < EnergyMap.rows; i++){
		float val = EnergyMap.at<float>(i, EnergyMap.cols - 1);
		if (val < minValue){
			minRow = i;
			minValue = EnergyMap.at<float>(i, EnergyMap.cols - 1);
		}
	}
	return minRow;
}

void TextLineExtractorGraySeam::drawDisplay(vector<Point> seam){
	vector < vector < Point >> polys;
	
	polys.push_back(seam);
	Mat img;

	cvtColor(_image->getMat(), img, CV_GRAY2RGB);
	polylines(img, seam, false, cv::Scalar(255, 0, 0));
	ImageTools::displayFresh("Polyline ", img);
}