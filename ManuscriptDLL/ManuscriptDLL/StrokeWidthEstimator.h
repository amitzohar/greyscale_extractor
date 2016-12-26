#ifndef _STROKE_WIDTH_ESTIMATOR_H_ 
#define _STROKE_WIDTH_ESTIMATOR_H_ 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include <tuple>
#include "imageoperator.h"

class DImage ;

class MANUSCRIPT_API StrokeWidthEstimator : public ImageOperator {

public:
	virtual float estimate() = 0 ;
};

#endif 