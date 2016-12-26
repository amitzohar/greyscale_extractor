#ifndef __ComponentExtractorBinary_H_
#define __ComponentExtractorBinary_H_

#include "dll_export_import.h"

#include "ComponentExtractor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Binary component extractor class </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API ComponentExtractorBinary : public ComponentExtractor {

public:
	ComponentExtractorBinary()  { ; }
	ComponentExtractorBinary(DImage* img) :ComponentExtractor(img){ ; }
	~ComponentExtractorBinary(void) { ; }

	void extract(vector<ConnectedComponent*>&) ;
	void extract();
protected:
	void collectComponents(vector<vector<cv::Point>>& contours, vector<cv::Vec4i>& hierarchy, vector<ConnectedComponent*>& component);
};

#endif 
