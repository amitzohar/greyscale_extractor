#ifndef _IMAGE_ENHANCER_H 
#define _IMAGE_ENHANCER_H 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include "ImageOperator.h"

class DImage ;

class MANUSCRIPT_API ImageEnhancer: public ImageOperator {
public:
	
	virtual DImage* enhance(){
		return _image ;
	}
};

#endif 
