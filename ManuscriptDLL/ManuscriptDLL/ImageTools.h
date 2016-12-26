#ifndef _IMAGE_TOOLS_H 
#define _IMAGE_TOOLS_H

#include "dll_export_import.h"

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include "DImage.h"

typedef std::pair<cv::Point2f, cv::Point2f>   Orientation;

namespace ImageTools {
	static int  Index;

	void MANUSCRIPT_API getPixelEightNeighbors(Mat img, int row, int col, int radius, uchar p[]);
	void MANUSCRIPT_API getPixelEightNeighbors(Mat img, int row, int col, int radius, float p[]);

	cv::Point2f MANUSCRIPT_API centerOfMass(cv::Mat mat);
	cv::Mat     MANUSCRIPT_API varianceAroundPoint(cv::Mat m, cv::Point2f point);
	Orientation MANUSCRIPT_API orientation(cv::Mat m, cv::Point2f &com);
	void MANUSCRIPT_API smoothColumn(cv::Mat mat, cv::Mat filter, int col_idx);
	void MANUSCRIPT_API remapColor(cv::Mat map, Vec3b color, cv::Point org, cv::Mat img);
	void MANUSCRIPT_API histogramGrayImage(Mat img, int histogram[], int bins = 256);

	void MANUSCRIPT_API shift(std::vector<cv::Point>& pts, cv::Point offset);

	void fillComponents(cv::Mat mat, std::vector<ConnectedComponent*>& components, float filler, float boundary);
	cv::Mat MANUSCRIPT_API draw(cv::Mat mat, std::vector<ConnectedComponent*>& components, cv::Scalar clr );
	// Debug 
	cv::Mat  MANUSCRIPT_API histogramImage(Mat histogram);

	void MANUSCRIPT_API display(String win, DImage& img);
	void MANUSCRIPT_API display(String win, Mat& img);

	void MANUSCRIPT_API display(String win, int idx, DImage& img);
	void MANUSCRIPT_API display(String win, int idx, Mat& img);
	void MANUSCRIPT_API displayFresh(String win, Mat& img);
}

#endif 
