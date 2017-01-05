
#ifndef STROKEWIDTHESTIMATORDERIVED_H_
#define STROKEWIDTHESTIMATORDERIVED_H_

#include "ManuscriptExport.h"
#include "StrokeWidthEstimator.h"

class DImage ;
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Stroke Width Estimator Derived is a class that implements the estimate function
///             which calculates the average stroke width of an image   </summary>
///
/// <remarks>	Inna Kr, 16/08/2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////


class MANUSCRIPT_EXPORT StrokeWidthEstimatorDerived : public StrokeWidthEstimator {

public:

	StrokeWidthEstimatorDerived();
	~StrokeWidthEstimatorDerived();
	//works with a binary Mat
	float estimate();

};

#endif /* STROKEWIDTHESTIMATORDERIVED_H_ */
