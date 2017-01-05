
#ifndef _TextLineExtractorBinarySeam_H 
#define _TextLineExtractorBinarySeam_H 


#include <opencv/cv.h>
#include <opencv/ml.h>
#include <math.h>
#include "TextLineExtractor.h"

#include "dll_export_import.h"


class MANUSCRIPT_API TextLineExtractorBinarySeam : public TextLineExtractor {
	/* Class variables declaration */
	int theRows;
	int _border;
	int _diverge;

public:
	//Constructor
	TextLineExtractorBinarySeam(int rows);
	~TextLineExtractorBinarySeam();

	//Overridden methods
	void extract(std::vector<TextLine*>& text_lies);

private:

	Mat getEnergyMapFromDistance(Mat dst_map);
	int getMinRow(Mat energy_map, int row, int col, int range);
	vector<cv::Point> getSeam(Mat energy_map, cv::Point init);
	vector<cv::Point> getNextSeam(Mat energy_map);
	vector<ConnectedComponent*> getSeamComponents(vector<Point> seam, vector<ConnectedComponent*> &component_list);
	void removeTextLineFromEnergeyMap(Mat energy_map, TextLine& text_line);
	int getMinRow(Mat EnergyMap);
	void drawDisplay(vector<Point> seam);
	float minValueOfThreeRows(cv::Mat img, int row, int col){
		return min(img.at<float>(row - 1, col), min(img.at<float>(row, col), img.at<float>(row + 1, col)));
	}
};

#endif 