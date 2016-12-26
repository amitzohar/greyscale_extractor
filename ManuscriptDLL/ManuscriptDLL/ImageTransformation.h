#ifndef _IMAGE_TRANSFORMATION_H_ 
#define _IMAGE_TRANSFORMATION_H_ 

#include <opencv/cv.h>
#include "imageoperator.h"
#include "dll_export_import.h"

class MANUSCRIPT_API ImageTransformation : public ImageOperator {
public:

	virtual DImage* transform() = 0 ;
};

#endif 