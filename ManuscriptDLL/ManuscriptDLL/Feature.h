#ifndef _FEATURE_H_ 
#define _FEATURE_H_ 

#include "dll_export_import.h"

#include <vector>
#include <opencv/cv.h> 
#include "stdafx.h"


using namespace cv ;
using namespace std ;

class Metric ;

class MANUSCRIPT_API Feature {
public:
	Feature(void){;}
	~Feature(void){;}

	virtual void  sample2Row(Mat mat, int row) = 0 ;
	virtual vector<float> vectorize() = 0 ; 
	virtual double distance(Feature* a, Metric* m) = 0 ;
};
#endif
