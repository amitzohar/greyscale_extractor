#include "stdafx.h"
#include "stdext.h"
#include "ImageTools.h"
#include "ComponentExtractorBinary.h"
#include "TextLine.h"

using namespace cv;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Text line stores the properties of a text line of a document image  </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////


void TextLine::setLineBoundary(vector<cv::Rect>& brects) {
	if (brects.empty())
		return;

	_upperBound->push_back(brects[0].tl());
	_upperBound->push_back(brects[0].tl() + Point(brects[0].width, 0));
	_lowerBound->push_back(brects[0].br() - Point(brects[0].width, 0));
	_lowerBound->push_back(brects[0].br());
	
	for (int i = 1; i < brects.size(); i++){
		if (!brects[i].contains(*(_upperBound->end())))
			_upperBound->push_back(brects[i].tl());
		_upperBound->push_back(brects[i].tl() + Point(brects[i].width, 0));

		if (!brects[i].contains(*(_lowerBound->end())))
			_lowerBound->push_back(brects[i].br() - Point(brects[i].width, 0));
		_lowerBound->push_back(brects[i].br());
	}
}

// Algorithm: The convex hull of the fisrt component is the line contour, l, then at each iteration we 
// perform:
// 1. Compute the convex hull, h1 of the newly added component, c1 
// 2. Compute the convex hull, h,  of h1+h0, where h0 is the previous component's convex hull
// 3. Merge h with l 

void TextLine::setLineBoundary(vector<ConnectedComponent*>& components) {
	if (components.size() > 0) {
		Rect brect = components[0]->getBoundRect();
		for (int i = 0; i < components.size(); i++){
			brect |= components[i]->getBoundRect();
		}

		Mat mask = Mat::zeros(brect.size(), CV_8UC1);
		vector<ConnectedComponent*>::iterator iter = components.begin();
		vector<Point> previousPlygon = (*(iter++))->getContour().getConvexHull(false);
		ImageTools::shift(previousPlygon, Point(-brect.x, -brect.y));
		cv::fillConvexPoly(mask, previousPlygon, 255);

		while (iter != components.end()){
			vector<Point> plygon = (*(iter++))->getContour().getConvexHull(false);
			ImageTools::shift(plygon, Point(-brect.x, -brect.y));
			vector<Point> ply = getConvexHull(plygon, previousPlygon);
			cv::fillConvexPoly(mask, ply, 255);
			stdext::copy(plygon, previousPlygon);
		}

		DImage dimg(mask);
		ComponentExtractorBinary extractor;
		vector<ConnectedComponent*> comps;
		dimg.extractComponents(extractor, comps);
		stdext::copy(comps[0]->getContour().getPoints(), _boundary) ;
		ImageTools::shift(_boundary, Point(brect.x, brect.y));
	}
}

vector<Point> TextLine::getConvexHull(vector<Point> a, vector<Point> b){
	vector<Point> contour;
	vector<Point> hull;

	contour.insert(contour.end(), a.begin(), a.end());
	contour.insert(contour.end(), b.begin(), b.end());
	convexHull(Mat(contour), hull, false);

	return hull;
}

vector<int> TextLine::getIntersectionIndices(vector<Point> a, vector<Point> b){
	vector<int> pts;
	for (int i = 0; i < a.size(); i++){
		for (int j = 0; i < b.size(); j++){
			if (a[i] == b[j])
				pts.push_back(j);
		}
	}
	return pts;
}	

int TextLine::getFurthestPointIndex(vector<Point> v, int a, int da, int b, int db){
	float dista = 0;
	float distb = 0;

	int ia = a;
	int ib = b;

	while (ia != ib){
		if (dista > distb){
			dista += norm(v[ia] - v[ia + da]);
			ia += da;
		}
		else {
			distb += norm(v[ib] - v[ib + db]);
			ib += db;
		}
	}
	return ia;
}

void TextLine::drawLine(Scalar color){
		for (int i = 0; i < _upperBound->size() / 2; i++){
			line(_image, (*_upperBound)[i * 2], (*_upperBound)[i * 2 + 1], color);
		}
		for (int i = 0; i < _lowerBound->size() / 2; i++){
			line(_image, (*_lowerBound)[i * 2], (*_lowerBound)[i * 2 + 1], color);
		}
	}

void TextLine::drawLine(){
		for (int i = 0; i < _upperBound->size() / 2; i++){
			line(_image, (*_upperBound)[i * 2], (*_upperBound)[i * 2 + 1], Scalar(0, 0, 255));
		}
		for (int i = 0; i < _lowerBound->size() / 2; i++){
			line(_image, (*_lowerBound)[i * 2], (*_lowerBound)[i * 2 + 1], Scalar(0, 0, 255));
		}
}

void TextLine::printLine(){
		std::string print_line_upper = "";
		std::string print_line_lower = "";

		for (int i = 0; i < _upperBound->size() / 2; i++){
			print_line_upper += "(" + std::to_string((*_upperBound)[i * 2].x) + "," + std::to_string((*_upperBound)[i * 2].y) + ") " + "(" + std::to_string((*_upperBound)[i * 2 + 1].x) + "," + std::to_string((*_upperBound)[i * 2 + 1].y) + ") ";
		}
		for (int i = 0; i < _lowerBound->size() / 2; i++){
			print_line_lower += "(" + std::to_string((*_lowerBound)[i * 2].x) + "," + std::to_string((*_lowerBound)[i * 2].y) + ") " + "(" + std::to_string((*_lowerBound)[i * 2 + 1].x) + "," + std::to_string((*_lowerBound)[i * 2 + 1].y) + ") ";
		}

		cout << "Line:\n";
		cout << "\tUpper: " << print_line_upper << "\n";
		cout << "\tLower: " << print_line_lower << "\n";
}

