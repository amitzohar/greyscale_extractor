#ifndef _IMAGE_FILTER_H_ 
#define _IMAGE_FILTER_H_

#include "dll_export_import.h"

#include "imageoperator.h"

class MANUSCRIPT_API ImageFilter : public ImageOperator {

public:
	ImageFilter(void) {
	}

	~ImageFilter(void){
	}
	
	virtual DImage* filter(){
		return _image ;
	}

	
};

#endif 
