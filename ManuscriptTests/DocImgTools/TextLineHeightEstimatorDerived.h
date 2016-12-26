
#ifndef TEXTLINEHEIGHTESTIMATORDERIVED_H_
#define TEXTLINEHEIGHTESTIMATORDERIVED_H_

#include "ManuscriptExport.h"
#include "TextLineHeightEstimator.h"
#include "MorphologyTextLineExtractor.h"

//class DImage ;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Text Line Height Estimator Derived is a class that implements the estimate function
///             which calculates the average line height of an image   </summary>
///
/// <remarks>	Inna Kr, 16/08/2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_EXPORT TextLineHeightEstimatorDerived : public TextLineHeightEstimator {

public:

	TextLineHeightEstimatorDerived();

	~TextLineHeightEstimatorDerived();

	float estimate();
};

#endif /* TEXTLINEHEIGHTESTIMATORDERIVED_H_ */
