#ifndef _TEXTLINE_H_ 
#define _TEXTLINE_H_ 

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include "ManuscriptExport.h"

using namespace cv ;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Text line stores the properties of a text line of a document image  </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_EXPORT TextLine {
protected :
	Mat   _image ;
	RotatedRect  _rect  ;
	vector<Point>* _upperBound ;
	vector<Point>* _lowerBound ;

public:
	TextLine(void)  {;}
	~TextLine(void) {;}

	void setImage(Mat& img) { _image = img ;}
	Mat getImage(){return _image;}
	void setRect(RotatedRect& rect){ _rect=rect;}
	RotatedRect getRect(){return _rect;}
	void setUpperBound(vector<Point>* bound){
		_upperBound = bound ;
	}

	void setLowerBound(vector<Point>* bound){
		_lowerBound = bound ;
	}

	void drawLine(Scalar color){
		for (int i = 0; i <(int) _upperBound->size() / 2; i++){
			line(_image, (*_upperBound)[i * 2], (*_upperBound)[i * 2 + 1], color);
		}
		for (int i = 0; i < (int)_lowerBound->size() / 2; i++){
			line(_image, (*_lowerBound)[i * 2], (*_lowerBound)[i * 2 + 1], color);
		}
	}

	void drawLine(){
		for (int i = 0; i < (int)_upperBound->size() / 2; i++){
			line(_image, (*_upperBound)[i * 2], (*_upperBound)[i * 2 + 1], Scalar(0, 0, 255));
		}
		for (int i = 0; i < (int)_lowerBound->size() / 2; i++){
			line(_image, (*_lowerBound)[i * 2], (*_lowerBound)[i * 2 + 1], Scalar(0, 0, 255));
		}
	}

	void printLine(){
		std::string print_line_upper = "";
		std::string print_line_lower = "";

		for (int i = 0; i < (int)_upperBound->size() / 2; i++){
			//print_line_upper += "(" + std::to_string((*_upperBound)[i * 2].x) + "," + std::to_string((*_upperBound)[i * 2].y) + ") " + "(" + std::to_string((*_upperBound)[i * 2 + 1].x) + "," + std::to_string((*_upperBound)[i * 2 + 1].y) + ") ";
		}
		for (int i = 0; i < (int)_lowerBound->size() / 2; i++){
			//print_line_lower += "(" + std::to_string((*_lowerBound)[i * 2].x) + "," + std::to_string((*_lowerBound)[i * 2].y) + ") " + "(" + std::to_string((*_lowerBound)[i * 2 + 1].x) + "," + std::to_string((*_lowerBound)[i * 2 + 1].y) + ") ";
		}

		cout << "Line:\n";
		cout << "\tUpper: " << print_line_upper << "\n";
		cout << "\tLower: " << print_line_lower << "\n";
	}
};

#endif 
