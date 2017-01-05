#ifndef _COMPONENEXTRACTOR_H 
#define _COMPONENEXTRACTOR_H 

#include "dll_export_import.h"

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include "ConnectedComponent.h"


class DImage;

class MANUSCRIPT_API ComponentExtractor{
protected:
	DImage* _image;

public:
	ComponentExtractor()  { ; }
	ComponentExtractor(DImage* image)  {
		_image = image;
	}

	~ComponentExtractor(void) { ; }

	void setImage(DImage* img) { _image = img; }

	virtual void extract(vector<ConnectedComponent*>&) = 0;
};

#endif 