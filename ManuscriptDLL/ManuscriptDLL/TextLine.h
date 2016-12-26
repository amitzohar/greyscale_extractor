#ifndef _TEXTLINE_H_ 
#define _TEXTLINE_H_ 

#include <iostream>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <math.h>
#include "ConnectedComponent.h"

#include "dll_export_import.h"

using namespace cv ;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Text line stores the properties of a text line of a document image  </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API TextLine {
protected :
	cv::Mat   _image ;
	cv::Rect  _rect  ;
	vector<cv::Point>   _boundary ;
	vector<cv::Point>* _upperBound ;
	vector<cv::Point>* _lowerBound ;

public:
	TextLine()  {;}
	TextLine(cv::Mat img){ 
		_image = img;
	}

	~TextLine(void) {;}
	//TextLine(cv::Mat img, std::vector<ConnectedComponent> components);
	
	
	void setLineBoundary(std::vector<cv::Rect>& brects);
	void setLineBoundary(std::vector<ConnectedComponent*>& components);

	void setImage(cv::Mat& img) { _image = img; }
	void setUpperBound(vector<Point>* bound){
		_upperBound = bound;
	}

	void setLowerBound(vector<Point>* bound){
		_lowerBound = bound;
	}

	vector<cv::Point>& getBoundary(){
		return _boundary;
	}

	Point getCenter(cv::Rect rec) {
		return Point(rec.x + rec.width / 2, rec.y - rec.height / 2);
	}

	// delta is a percentage already divided by 100
	

	vector<Point> getConvexHull(vector<Point> a, vector<Point> b);
	vector<int>   getIntersectionIndices(vector<Point> a, vector<Point> b);
	int           getFurthestPointIndex(vector<Point> v, int a, int da, int b, int db);
	void addRecPoints(Point center, int height, int width);
	void rotateRec(Point center, Size recSize, double angle);

	double getAngle(Rect left, Rect right);
	void addPointsBetween(Point right, Point left, vector<Point> hull, bool bigger);
	void drawLine(Scalar color);
	void drawLine();

	void printLine();
};

#endif 