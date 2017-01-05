#ifndef _CONNECTEDCOMPONENT_H 
#define _CONNECTEDCOMPONENT_H

#include "dll_export_import.h"

#include <vector>
#include <string>
#include "Contour.h" 


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Connected component. </summary>
///
/// <remarks>	El Sana, 2/9/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_API ConnectedComponent {
	const static int ABOVE =  1 ;
	const static int BELOW = -1 ;
	const static int BORDER = 1 ;
	const static int VERTEX = 2 ;
	const static int ON_EDGE = 3 ;

protected:
	cv::Mat                   _image   ;
	ConnectedComponent*       _parent  ;
	Contour                   _contour ;
	string                    _string  ;
	vector<ConnectedComponent*> _children ;
public:
	ConnectedComponent(vector<cv::Point>& contour);
	ConnectedComponent(void);
	~ConnectedComponent(void);

	void setImage(cv::Mat img)       { _image = img;  }
	cv::Mat  getImage()              { return _image ; } 
	Contour& getContour()        { return _contour ;}

	void addChild(ConnectedComponent* component) ;
	cv::Rect                          getBoundRect()   { return _contour.getBoundRect(); }
	ConnectedComponent*               getParent()      { return _parent; }
	vector<ConnectedComponent*>&      getChildren()    { return _children; }

	int  borderType(cv::Mat mat, int row, int col, char mask, int& trace );
	void setParent(ConnectedComponent* parent)        { _parent   = parent; }
	void fillComponentOnMat(cv::Mat mat, char mask, char filler);
	void draw(cv::Mat img, cv::Scalar clr, bool isclosed, int thickness, int line_type);
};

#endif 