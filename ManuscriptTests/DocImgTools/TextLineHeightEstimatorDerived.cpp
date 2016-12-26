#include "stdafx.h"
#include "TextLineHeightEstimatorDerived.h"
#include "DImage.h"

using namespace cv;

TextLineHeightEstimatorDerived::TextLineHeightEstimatorDerived(){;}

TextLineHeightEstimatorDerived::~TextLineHeightEstimatorDerived(){;}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the average text line height in an image  </summary>
///
/// <returns>	A float - the average height  </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float TextLineHeightEstimatorDerived::estimate(){
	float avgHeight=0;
	MorphologyTextLineExtractor* tle = new MorphologyTextLineExtractor();
	vector<TextLine*> tl;
	tle->setImage(_image->getMat());
	tle->extract(tl);

	if((int)tl.size()>0){
		for(int i=0;i<(int)tl.size();i++){
			RotatedRect rect = tl[i]->getRect();
			Size rect_size = rect.size;
			if (rect.angle < -45.) {
				swap(rect_size.width, rect_size.height);
			}
			avgHeight += rect_size.height;
		}
		avgHeight /= (int)tl.size();
	}
	return avgHeight;
}
