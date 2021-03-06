#ifndef _IMAGE_BINARIZER_H 
#define _IMAGE_BINARIZER_H 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include "ImageOperator.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Binarizer class implements a binarization algorithm. This is a base class that each 
/// 			new algorithm should use and implement the virtual function binarize </summary>
///
/// <remarks>	El Sana, 2/9/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class DImage ;

class MANUSCRIPT_API ImageBinarizer: public ImageOperator {

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Binarize is the virtual function to implement a binarization algorithm in the 
	/// 			extended class </summary>
	///
	/// <remarks>	El Sana, 2/15/2012. </remarks>
	///
	/// <returns>	null if it fails, else. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual DImage* binarize() {
		return _image ;
	}
};

#endif 