#ifndef _PROFILE_SEAM_TEXTLINE_EXTRACTOR_H_ 
#define _PROFILE_SEAM_TEXTLINE_EXTRACTOR_H_ 

#include "TextLineExtractor.h"	
#include "ProjectionProfile.h"
#include "dll_export_import.h"

class DImage ;


class MANUSCRIPT_API TextLineExtractorProfileSeam :public TextLineExtractor {
	ProjectionProfile _projector;
	DImage*          _profile;
	std::vector<DImage> createVerticalDevision(int k, int projectionMethod);
	Mat computeDerivative();
	
public:
	vector<vector<int>> computeSeperatingSeams();
	void MedialSeamDrawing(); // for testing purposes.
	TextLineExtractorProfileSeam(ProjectionProfile& proj, DImage* prof, DImage* img);
	~TextLineExtractorProfileSeam(){ ; }
	void extract();
};

#endif