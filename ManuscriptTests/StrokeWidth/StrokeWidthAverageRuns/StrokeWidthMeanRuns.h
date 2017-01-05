#pragma once
#include <StrokeWidthEstimator.h>

class StrokeWidthMeanRuns : public StrokeWidthEstimator {

public:
	StrokeWidthMeanRuns();
	~StrokeWidthMeanRuns();

	float estimate();
};

