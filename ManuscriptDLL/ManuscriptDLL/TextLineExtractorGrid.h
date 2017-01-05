#ifndef _TextLineExtractorGrid_H_ 
#define _TextLineExtractorGrid_H_

#include <opencv\cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

#include "TextLineExtractor.h"
#include "ImageFilter.h"
#include "ImageBinarizer.h"
#include "ImageEnhancer.h"
#include "ImageConverter.h"
#include "ImageProjector.h"
#include "ComponentExtractorBinary.h"
#include "FeatureExtractor.h"
#include "Metric.h"
#include "TextLine.h"

#include "dll_export_import.h"

using namespace std;
using namespace cv;
#define PI 3.14159265

class DImage;

class MANUSCRIPT_API TextLineExtractorGrid : TextLineExtractor {
	int _lineHeight;
	int _allowedLineDistance;
	double _splitRatio;
	double _peakSimilarity;

public:
	TextLineExtractorGrid(Mat img, int lineHeight);
	TextLineExtractorGrid(Mat img, int lineHeight, double peakSimilarity);

	~TextLineExtractorGrid(void){}



	void extract(vector<TextLine*>&);

	void fixCoordinatesTopPoint(Point2i &point, int angle);
	void fixCoordinatesBottomPoint(Point2i &point, int angle, int rows, int cols);
	void fixCoordinatesTopReversedPoint(Point2i &point, int angle, int rows, int cols);
	void fixCoordinatesBottomReversedPoint(Point2i &point, int angle, int rows, int cols);
	std::vector<std::vector<Point2i>> calculateByProjection(DImage* img, int line_height);
	std::vector<std::vector<Point2i>> findTextLinesFromPeaks(std::pair<double, int> &maxPeak, DImage* &invSrcDimg, int line_height);
	std::vector<std::vector<Point2i>> unifyLinesByWidth(std::vector<std::vector<Point2i>> cell_1_lines, std::vector<std::vector<Point2i>> cell_2_lines);
	std::vector<std::vector<Point2i>> unifyLinesByHeight(std::vector<std::vector<Point2i>> cell_1_lines, std::vector<std::vector<Point2i>> cell_2_lines);
	void getOriginalCords(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2, std::vector<std::vector<Point2i>> &cell_3_lines, DImage* cell_3, std::vector<std::vector<Point2i>> &cell_4_lines, DImage* cell_4);
	void getOriginalCordsWidth(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2);
	void getOriginalCordsHeight(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2);

};

#endif 