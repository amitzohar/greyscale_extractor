#ifndef _BINARIZER_NIBLACK_H_ 
#define _BINARIZER_NIBLACK_H_ 

#include "dll_export_import.h"

#include "cv_macros.h"
#include "imagebinarizer.h"

using namespace std ;

class MANUSCRIPT_API BinarizerNiblack : public ImageBinarizer{
	cv::Mat _mean ;
	cv::Mat _std  ;
	cv::Mat _threshold ;
	cv::Size   _window ;
	float  _k       ;
	double _max_std ;

public:
	BinarizerNiblack(void);
	BinarizerNiblack(int width, int height);
	~BinarizerNiblack(void);

	void   setWindow(cv::Size w )  { _window = w ; }
	void   setK(float k)       { _k = k ; }
	double setMeanStdMaps(cv::Mat img, cv::Size window);
	void   setThreshold(cv::Size size, float k);

	DImage* binarize();
};

#endif