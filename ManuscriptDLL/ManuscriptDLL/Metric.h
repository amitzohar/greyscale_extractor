#ifndef _METRIC_H_ 
#define _METRIC_H_

#include "dll_export_import.h"

#include <vector>
#include "Feature.h"

using namespace std;

class MANUSCRIPT_API Metric {
public:
	Metric(void){};
	~Metric(void){ ; }

	virtual double distance(Feature* a, Feature* b) = 0;
	virtual double distance(vector<Feature*>& a, vector<Feature*>& b) = 0;
};

#endif 