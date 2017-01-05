#ifndef _IMAGE_CONVERTER_H_ 
#define _IMAGE_CONVERTER_H_ 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include "ImageOperator.h"
#include "ImageOperator.h"


class MANUSCRIPT_API ImageConverter : public ImageOperator {
public:
	ImageConverter(void);
	~ImageConverter(void);

	virtual DImage* convert(){
		return _image ;
	}
};

#endif 