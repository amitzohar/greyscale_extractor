
#include "stdafx.h"
#include "DImage.h"
#include "ImageTools.h"
#include "stdext.h"
#include "TextLineExtractorGraySeam.h"
#include "DistanceTransformSigned.h"
#include "ComponentExtractorBinary.h"
#include "TextLine.h"

using namespace cv;
using namespace std;

struct {
	bool operator()(ConnectedComponent* a, ConnectedComponent* b){
		return a->getContour().getCenter().x < b->getContour().getCenter().x;
	}
} comp;


/**
* TextLineExtractorGraySeam Constructor
* Input:
*	rows- amount of rows to extract.
*/
TextLineExtractorGraySeam::TextLineExtractorGraySeam(int rows){
	theRows = rows;
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
	ComponentExtractorBinary component_extractor;
	vector<ConnectedComponent*> component_list;

	// Extarct the connected components from the binary image 
	DImage* negative = _image->negate();
	negative->extractComponents(component_extractor, component_list);
	DistanceTransformSigned transform(_image);
	transform.setComponents(&component_list);
	DImage* dist_map = transform.transform();

	Mat vis = Mat(dist_map->getMat().size(), CV_32FC1);
	normalize(dist_map->getMat(), vis, 0, 1, NORM_MINMAX, CV_32F);
	ImageTools::display(" Distance ", vis);

	Mat energy_map = getEnergyMapFromDistance(dist_map->getMat());
	normalize(energy_map, vis, 0, 1, NORM_MINMAX, CV_32F);
	ImageTools::display(" Energy Map ", vis);
	//return;
	
	for (int i = 0; i <= theRows; i++) {
		vector<cv::Point> seam = getNextSeam(energy_map);
		drawDisplay(seam);
		waitKey(0);
		vector<ConnectedComponent*> seam_components = getSeamComponents(seam, component_list);
		cout << "Component List: " << component_list.size() << " Seam Components: " << seam_components.size() << endl;
		if (seam_components.size() > 0){
			TextLine* text_line = new TextLine(_image->getMat());
			text_line->setLineBoundary(seam_components);
			text_lines.push_back(text_line);
			stdext::subtract(component_list, seam_components);
			removeTextLineFromEnergyMap(energy_map, *text_line);
			normalize(energy_map, vis, 0, 1, NORM_MINMAX, CV_32F);
			//ImageTools::displayFresh(" Energy Map ", vis);
		}
	}
}

vector<cv::Point> TextLineExtractorGraySeam::getNextSeam(Mat energy_map){
	Point min_pt;
	double min_val, max_val;
	//cout << "Col: " << energy_map.col(energy_map.cols - 2) << endl ;
	minMaxLoc(energy_map.col(energy_map.cols - 2), &min_val, &max_val, &min_pt);
	min_pt.x = energy_map.cols - 2;

	//cout << "Min: " << min_pt << " MinVal: " << min_val << " MaxVal: " << max_val << endl;
	vector<Point> seam;
	int row = min_pt.y;
	seam.push_back(min_pt);
	for (int col = min_pt.x; col > 0; col--){
		//cout << "Row: " << row << " Col: " << col << endl;
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

/*
*
*/

vector<Point> TextLineExtractorGraySeam::getSeam(Mat energy_map, Point start){
	vector<Point> seam;
	int row = start.y;
	seam.push_back(start);
	for (int col = start.x; col > 0 ; col--){
		row = getMinRow(energy_map, row, col, 1);
		seam.push_back(Point(col, row));
	}
	return seam;
}


/**
* getEnergyMapFromDistance - Create Energy Map From Signed Distance Map.
* Input:
*	dstMap - Distance Map built by SignedDistanceTransform.transform
*	energyMap- Energy map of image, (Built from Signed Distance Transform).
*/
/*
Mat TextLineExtractorBinarySeam::getEnergyMapFromDistance(Mat dst_map){
	Mat energy_map = Mat::zeros(dst_map.size(), CV_32FC1);
	copyMakeBorder(dst_map, dst_map, 2, 2, 2, 2, BORDER_REPLICATE);//BORDER_CONSTANT, Scalar(255)

	for (int j = 2; j < energy_map.cols - 2; j++)
		for (int i = 2; i < energy_map.rows - 2; i++){
			float before = dst_map.at<float>(i, j);
			float min_val = min(energy_map.at<float>(i - 2, j - 1),
				min(energy_map.at<float>(i - 1, j - 1), min(energy_map.at<float>(i, j - 1),
				min(energy_map.at<float>(i + 1, j - 1),	energy_map.at<float>(i + 2, j - 1)))));
			float val = 2.0f * dst_map.at<float>(i, j) + min_val;
			energy_map.at<float>(i, j) = val;
		}
	Rect roi(2, 2, energy_map.cols - 4, energy_map.rows - 4);
	//return energy_map(roi);
	return energy_map;
}
*/

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


void TextLineExtractorGraySeam::removeTextLineFromEnergyMap(Mat energy_map, TextLine& text_line){
	double mx, mn;
	minMaxLoc(energy_map, &mn, &mx);
	cout << "Boundary: " << text_line.getBoundary().size() << endl;
	Rect rect = boundingRect(text_line.getBoundary());
	//ImageTools::display(rect);
	rect.x = 0;
	rect.width = energy_map.cols-1;
	cout << "Rect: " << rect << endl;
	cv::rectangle(energy_map, rect, (float)mx, CV_FILLED);
	//cv::fillPoly(energy_map, text_line.getBoundary(), 10000.0f);
}

void TextLineExtractorGraySeam::drawDisplay(vector<Point> seam){
	vector < vector < Point >> polys;
	
	polys.push_back(seam);
	Mat img;

	cvtColor(_image->getMat(), img, CV_GRAY2RGB);
	polylines(img, seam, false, cv::Scalar(255, 0, 0));
	ImageTools::displayFresh("Polyline ", img);
}