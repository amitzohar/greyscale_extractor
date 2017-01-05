#ifndef _OTSUGRAYBINARIZER_H 
#define _OTSUGRAYBINARIZER_H 

#include "dll_export_import.h"

#include "ImageBinarizer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Otsul binarizer class implements the Otsu binarization algorithm </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API BinarizerOtsuGray : public ImageBinarizer {

public:
	DImage* binarize();
};

#endif 