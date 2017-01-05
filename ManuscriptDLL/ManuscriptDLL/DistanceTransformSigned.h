#ifndef _SIGNED_DISTANCE_TRANSFORMATION_ 
#define _SIGNED_DISTANCE_TRANSFORMATION_

#include <vector>
#include "ImageTransformation.h"
#include "ConnectedComponent.h"
#include "DImage.h"

#include "dll_export_import.h"

class MANUSCRIPT_API DistanceTransformSigned : public ImageTransformation {
	
protected:
	vector<ConnectedComponent*>* _components;
	void updateSignedDistaceMap(Mat map, int row, int col, int step, int delta);


public:
	DistanceTransformSigned(void) {;}
	DistanceTransformSigned(DImage* img){
		setImage(img);
	}

	~DistanceTransformSigned(void){;}

	void setComponents(vector<ConnectedComponent*>* components){
		_components = components ;
	}

	vector<ConnectedComponent*>* getComponents(){
		return _components;
	}
	DImage * transform();
};

#endif 