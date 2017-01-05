#ifndef _TEXTLINEEXTRACTOR_H 
#define _TEXTLINEEXTRACTOR_H 

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>

#include "dll_export_import.h"

class TextLine ;
class DImage;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Text line extractor is a base class for text line extraction algorithms. Each 
/// 			should implement the virtual funxtion extract  </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API TextLineExtractor {
protected :
	DImage* _image ;

public:
	TextLineExtractor(void)  {;}
	~TextLineExtractor(void) {;}

	void setImage(DImage* img) { _image = img ;}
	virtual void extract(std::vector<TextLine*>& ) = 0;
};

#endif 