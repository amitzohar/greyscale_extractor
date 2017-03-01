
#ifndef _TextLineExtractorGraySeam_H 
#define _TextLineExtractorGraySeam_H 


#include <opencv/cv.h>
#include <opencv/ml.h>
#include <math.h>
#include "TextLineExtractor.h"


class TextLineExtractorGraySeam : public TextLineExtractor {
	int m_rows;
	double m_rowHeight;

public:
	//Constructor
	TextLineExtractorGraySeam(int rows);
	~TextLineExtractorGraySeam();

	//Overridden methods
	void extract(std::vector<TextLine*>& text_lies);

private:
	int removeLine(Mat energy_map, vector<cv::Point> upperSeam, vector<cv::Point> lowerSeam);
	DImage* calculateDistanceMap(vector<ConnectedComponent*> &component_list);
	Mat getEnergyMapFromDistance(Mat dst_map);
	Mat negateEnergyMap(Mat energy_map);
	int getMinRow(Mat energy_map, int row, int col, int range);
	int TextLineExtractorGraySeam::calculateLineHeight(vector<cv::Point> lowerSeam, vector<cv::Point> upperSeam);
	vector<cv::Point> getSeam(Mat energy_map, cv::Point init);
	vector<cv::Point> getNextSeam(Mat energy_map);
	vector<cv::Point> getUpperSeam(Mat energy_map, Mat neg_energy_map, vector<cv::Point> medSeam);
	vector<cv::Point> getLowerSeam(Mat energy_map, Mat neg_energy_map, vector<cv::Point> medSeam);
	vector<ConnectedComponent*> getSeamComponents(vector<Point> seam, vector<ConnectedComponent*> &component_list);
	int getMinRow(Mat EnergyMap);
	void drawDisplay(vector<Point> seam);
	float minValueOfThreeRows(cv::Mat img, int row, int col, vector<float> w){
		return min(w[0]*img.at<float>(row - 1, col), min(w[1]*img.at<float>(row, col), w[2]*img.at<float>(row + 1, col)));
	}

	static const unsigned int NUM_OF_AVG_PTS;

};

#endif 