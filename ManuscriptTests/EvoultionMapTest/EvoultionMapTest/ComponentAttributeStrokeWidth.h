#ifndef _COMPONENT_ATTRIBUTE_STROKE_WIDTH_H 
#define _COMPONENT_ATTRIBUTE_STROKE_WIDTH_H

#include <ConnectedComponent.h>
#include "ComponentAttributeGetter.h"

class ComponentAttributeStrokeWidth: public ComponentAttributeGetter {

public:
	int getAttributeValue(ConnectedComponent* com) {
		return com->getContour().getBoundRect().height;
	}
};

#endif 
     