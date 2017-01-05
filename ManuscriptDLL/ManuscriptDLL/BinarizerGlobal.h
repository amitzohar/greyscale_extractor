#ifndef _GLOBALBINARIZER_H 
#define _GLOBALBINARIZER_H 

#include "dll_export_import.h"

#include "ImageBinarizer.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Global binarizer implment a global binarization algorithm </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API BinarizerGlobal : public ImageBinarizer {
	float  _thershold ;

public:
	BinarizerGlobal(void);
	BinarizerGlobal(float thershold);
	~BinarizerGlobal(void);
	
	void setThershold(float threshold){
		_thershold = threshold ;
	}

	DImage* binarize();
};

#endif 