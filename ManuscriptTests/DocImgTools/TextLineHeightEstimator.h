#ifndef TEXTLINEHEIGHTESTIMATOR_H_
#define TEXTLINEHEIGHTESTIMATOR_H_

#include <opencv/cv.h>
#include <tuple>
#include "ManuscriptExport.h"
#include "imageoperator.h"

//class DImage ;

class MANUSCRIPT_EXPORT TextLineHeightEstimator : public ImageOperator {

public:

	TextLineHeightEstimator(void){
	}

	~TextLineHeightEstimator(void){
	}

	virtual float estimate() = 0 ;
};

#endif /* TEXTLINEHEIGHTESTIMATOR_H_ */
