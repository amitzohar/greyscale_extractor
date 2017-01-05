#ifndef _COMPONENT_ATTRIBUTE_GETTER_H 
#define _COMPONENT_ATTRIBUTE_GETTER_H

#include "ConnectedComponent.h"

class ComponentAttributeGetter {
public:
	virtual int getAttributeValue(ConnectedComponent* com)=0 ;
};

#endif 

