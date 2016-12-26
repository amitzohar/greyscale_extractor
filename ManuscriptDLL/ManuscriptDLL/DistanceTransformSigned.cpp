#include "stdafx.h"
#include "DistanceTransformSigned.h"
#include "ImageTools.h" 

void fillComponentsOnMat(vector<ConnectedComponent*> component_list, Mat &mat, float filler, float border_filler);
void fillComponentsOnMat(vector<ConnectedComponent*> component_list, Mat &mat, uchar filler, uchar border_filler);

void DistanceTransformSigned::updateSignedDistaceMap(Mat map, int row, int col, int step, int delta){
	float up = map.at<float>(row - step, col);
	float left = map.at<float>(row, col - step);
	float center = map.at<float>(row, col);
	if ( map.at<float>(row, col) > 0 ){
		if ( map.at<float>(row-step, col) > map.at<float>(row, col-step) ){
			if ( map.at<float>(row, col-step) < map.at<float>(row, col)) {
				map.at<float>(row, col) = map.at<float>(row, col-step) + delta ;
				if ( map.at<float>(row, col) < map.at<float>(row-step, col))
					map.at<float>(row-step, col) = map.at<float>(row, col) + delta ;
			}
		}
		else {
			if ( map.at<float>(row-step, col) < map.at<float>(row, col)) {
				map.at<float>(row, col) = map.at<float>(row-step, col) + delta ;
				if ( map.at<float>(row, col) < map.at<float>(row, col-step))
					map.at<float>(row, col-step) = map.at<float>(row, col) + delta ;
			}
		}
	}

	if ( map.at<float>(row, col) < 0 ){
	    if ( map.at<float>(row-step, col) > map.at<float>(row, col-step) ){
			if ( map.at<float>(row-step, col) > map.at<float>(row, col)) {
				map.at<float>(row, col) = map.at<float>(row-step, col) - delta ;
				if ( map.at<float>(row, col) > map.at<float>(row, col-step))
					map.at<float>(row, col-step) = map.at<float>(row, col) - delta ;
			}
		}
		else {
			if ( map.at<float>(row, col-step) > map.at<float>(row, col)) {
				map.at<float>(row, col) = map.at<float>(row, col-step) - delta ;
				if ( map.at<float>(row, col) > map.at<float>(row-step, col))
					map.at<float>(row-step, col) = map.at<float>(row, col) - delta ;
			}
		}
	}
}

DImage* DistanceTransformSigned::transform(){
	
	float limit = _image->getMat().total();
	cv::Mat map(this->_image->getMat().size(), CV_32F, limit);
	ImageTools::fillComponents(map, *_components, -limit, 0.0f);

	int col, row;
	// Forward traversal 
	for ( col = 1 ; col < map.cols ; col++ )
		for ( row = 1 ; row < map.rows ; row++ )
			updateSignedDistaceMap(map, row, col, 1, 1);

	// Backward traversal 
	for ( col = map.cols-2 ; col >= 0  ; col-- )
		for ( row = map.rows-2 ; row >= 0 ; row-- )
			updateSignedDistaceMap(map, row, col, -1, 1);

	return new DImage(map);
}

void fillComponentsOnMat(vector<ConnectedComponent*> component_list, Mat &mat, uchar filler, uchar border_filler)
{
	vector<ConnectedComponent*>::iterator iter = component_list.begin();
	while (iter != component_list.end()){
		//This is how you go over the points of a shape.
		Rect boundingRect = (*iter)->getContour().getBoundRect();
		for (int row = boundingRect.y; row < boundingRect.height + boundingRect.y; ++row)
			for (int col = boundingRect.x; col < boundingRect.width + boundingRect.x; ++col){
				if ((*iter)->getContour().inside(Point(col, row)))
				{
					uchar curVal = mat.at<uchar>(row, col);
					mat.at<uchar>(row, col) = filler;
				}

			}

		vector<Point>::iterator pntIter = (*iter)->getContour().getPoints().begin();
		vector<Point>::iterator endpntIter = (*iter)->getContour().getPoints().end();
		while (pntIter != endpntIter){
			mat.at<uchar>(pntIter->y, pntIter->x) = border_filler;

			pntIter++;
		}
		iter++;
	}

}

void fillComponentsOnMat(vector<ConnectedComponent*> component_list, Mat &mat, float filler, float border_filler){
	vector<ConnectedComponent*>::iterator iter = component_list.begin();
	while (iter != component_list.end()){
		//This is how you go over the points of a shape.
		Rect boundingRect = (*iter)->getContour().getBoundRect();
		for (int row = boundingRect.y; row < boundingRect.height + boundingRect.y; ++row)
			for (int col = boundingRect.x; col < boundingRect.width + boundingRect.x; ++col){
				if ((*iter)->getContour().inside(Point(col, row)))
				{
					float curVal = mat.at<float>(row, col);
					mat.at<float>(row, col) = filler;
				}

			}

		vector<Point>::iterator pntIter = (*iter)->getContour().getPoints().begin();
		vector<Point>::iterator endpntIter = (*iter)->getContour().getPoints().end();
		while (pntIter != endpntIter){
			mat.at<float>(pntIter->y, pntIter->x) = border_filler;

			pntIter++;
		}
		iter++;
	}
}