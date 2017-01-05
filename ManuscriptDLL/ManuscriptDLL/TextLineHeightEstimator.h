#ifndef _TEXTLINE_HEIGHT_ESTIMATOR_H_ 
#define _TEXTLINE_HEIGHT_ESTIMATOR_H_ 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include <tuple>
#include "imageoperator.h"

class DImage ;

class MANUSCRIPT_API TextLineHeightEstimator : public ImageOperator {
	float _height ;
public:
	TextLineHeightEstimator(){
		_height = -1 ;
	}
	
	virtual float estimate() = 0 ;
	~TextLineHeightEstimator(){;}
};

#endif 