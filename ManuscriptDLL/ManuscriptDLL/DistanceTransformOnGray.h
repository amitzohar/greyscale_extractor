#ifndef _DISTANCE_TRANSFORMTION_ONGRAY_H_ 
#define _DISTANCE_TRANSFORMTION_ONGRAY_H_ 

#include "dll_export_import.h"
#include "imagetransformation.h"

class MANUSCRIPT_API DistanceTransformOnGray : public ImageTransformation {
public:
	DistanceTransformOnGray(void);
	~DistanceTransformOnGray(void);

	DImage* transform() ;
};

#endif 