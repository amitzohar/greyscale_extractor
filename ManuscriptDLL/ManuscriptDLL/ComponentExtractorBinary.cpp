#include "stdafx.h"
#include <iostream>
#include "DImage.h"
#include "ImageTools.h"
#include "ComponentExtractorBinary.h"

using namespace cv;
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Collect components. </summary>
///
/// <remarks>	El Sana, 2/9/2012. </remarks>
///
/// <param name="contours">  	[in] The contours. </param>
/// <param name="hierarchy"> 	[in] The hierarchy. </param>
/// <param name="components">	[out] the collected components. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void ComponentExtractorBinary::collectComponents(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy,
	vector<ConnectedComponent*>& components){

	if ( contours.size() > 0 ) {
		// Generate all the components 
		vector<vector<Point>>::iterator iter; 
		for ( iter = contours.begin(); iter != contours.end(); iter++ ){
			ConnectedComponent* component = new ConnectedComponent(*iter);
			components.push_back(component);
		}

		// Scan the the component and build the hierarchy 
		for ( unsigned int i = 0 ; i < components.size(); i++ ){
			ConnectedComponent* component = components[i];
			// Child
			if (hierarchy[i][2] >= 0 ){
				component->addChild(components[hierarchy[i][2]]);
			}

			// Parent 
			if (hierarchy[i][3] >= 0 )
				component->setParent(components[hierarchy[i][3]]);	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Extracts the given components. </summary>
///
/// <remarks>	El Sana, 2/9/2012. </remarks>
///
/// <param name="components">	[in,out] the extracted components. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void  ComponentExtractorBinary::extract(vector<ConnectedComponent*>& components){
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<ConnectedComponent*> tmp_components;

	Mat mat = _image->getMat().clone();
	findContours(mat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0,0) );
	collectComponents(contours, hierarchy, tmp_components);
	for each (ConnectedComponent* comp in tmp_components)
		if (!comp->getParent())
			components.push_back(comp);
}

void ComponentExtractorBinary::extract(){
	Mat gray;
	Mat bin;
	RNG rng(12345);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat mat = Mat::zeros(Size(100, 100), CV_8U);
	findContours(mat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
}