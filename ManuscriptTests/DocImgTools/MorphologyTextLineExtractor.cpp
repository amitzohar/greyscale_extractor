#include "stdafx.h"
#include "MorphologyTextLineExtractor.h"
#include "DImage.h"

#define PI 3.14159265358979323846

using namespace cv ;
using namespace std ;

MorphologyTextLineExtractor::MorphologyTextLineExtractor()  {;}
MorphologyTextLineExtractor::~MorphologyTextLineExtractor() {;}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Converts the image to a gray scale image   </summary>
///
/// <returns>	A Mat - the converted image </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

Mat MorphologyTextLineExtractor::convertMat(){
	Mat gray;
	if (_image.channels() == 1){
		_image.convertTo(gray, CV_8U);
		return gray;
	}

	if (_image.channels() == 3) {
		cvtColor(_image, gray, CV_BGR2GRAY);
		return gray;
	}
	return _image.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the x-projection of a binary image  </summary>
///
/// <param name="img">			[in] The image </param>
///
/// <returns>  A vector of floats  </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<float> x_projection(Mat img){
	vector<float> x_proj(img.cols);
	int i=0, j=0, acc=0;
	for(i=0; i<img.cols;i++){
		for(j=0; j<img.rows; j++){
			if(img.at<uchar>(j,i) > 0){
				acc++;
			}
		}
		x_proj[i]=acc;
		acc=0;
	}
	return x_proj;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Checks if the valleys in a given x-projection satisfy the
///             algorithm requirements    </summary>
///
/// <param name="x_proj">		[in] The x-projection. </param>
/// <param name="height">       [in] The height of the bounding rectangle
///                                   of the text line candidate. </param>
///
/// <returns>	A boolean - true if the requirements were satisfied. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool checkValleys(vector<float> x_proj, float height){
	int acc=0;
	for(int i=0; i< (int) x_proj.size(); i++){
		if(x_proj[i]==0)
			acc++;
		else{
			if(acc > (height/8))
				return false;
			acc=0;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the average width of the characters in the text line candidate    </summary>
///
/// <param name="x_proj">		[in] The x-projection of the text line. </param>
///
/// <returns>	A float - the average width . </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float avgWidth(vector <float> x_proj){
	int acc=0, count=0, flag=0;
	for(int i=0; i< (int) x_proj.size(); i++){
		if(x_proj[i]>0){
			acc++;
			if(flag==0){
				flag=1;
				count++;
			}
		}
		else
			flag=0;
	}
	float ans=0;
	if(count>0)
		ans=acc/count;
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the width variance of characters in the text line candidate   </summary>
///
/// <param name="x_proj">		[in] The x-projection of the text line. </param>
/// <param name="avgW">		    [in] The average width of the characters. </param>
///
/// <returns>	A float - the width variance. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float varianceWidth(vector<float> x_proj, float avgW){
	int acc=0, count=0, flag=0;
	float sum=0;
	for(int i=0; i< (int) x_proj.size(); i++){
		if(x_proj[i]>0){
			acc++;
			if(flag==0){
				flag=1;
				count++;
			}
		}
		else{
			flag=0;
			sum = sum + pow((acc-avgW),2);
			acc=0;
		}
	}
	float ans=0;
	if(count>0)
		ans=sum/count;
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the average height of the characters in the text line candidate    </summary>
///
/// <param name="x_proj">		[in] The x-projection of the text line. </param>
///
/// <returns>	A float - the average height . </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float avgHeight(vector <float> x_proj){
	int acc=0, sumOfHeights=0;
	for(int i=0; i< (int) x_proj.size(); i++){
		if(x_proj[i]>0){
			acc++;
			sumOfHeights += x_proj[i];
		}
	}
	float ans=0;
	if(acc>0)
		ans=sumOfHeights/acc;
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the height variance of characters in the text line candidate   </summary>
///
/// <param name="x_proj">		[in] The x-projection of the text line. </param>
/// <param name="avgW">		    [in] The average height of the characters. </param>
///
/// <returns>	A float - the height variance. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float varianceHeight(vector<float> x_proj, float avgH){
	int acc=0, hSum=0, height=0,count=0, flag=0;
	float sum=0;
	for(int i=0; i< (int) x_proj.size(); i++){
		if(x_proj[i]>0){
			acc++;
			hSum += x_proj[i];
			if(flag==0){
				flag=1;
				count++;
			}
		}
		else{
			flag=0;
			if(acc>0)
				height = hSum/acc;
			else
				height=0;
			sum = sum + pow((height-avgH),2);
			acc=0;
			hSum=0;
		}
	}
	float ans=0;
	if(count>0)
		ans=sum/count;
	return ans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Checks whether two components should be merged- whether they satisfy the
///             algorithm requirements.   </summary>
///
/// <param name="cont1">	 	[in] The first component. </param>
/// <param name="cont2">	    [in] The second component. </param>
/// <param name="theta1">		[in] The first orientation. </param>
/// <param name="theta2">	    [in] The second orientation. </param>
/// <param name="mc1">		    [in] The first center of mass. </param>
/// <param name="mc2">		    [in] The second center of mass. </param>
/// <param name="rect1">		[in] The first bounding rectangle. </param>
/// <param name="rect2">	    [in] The second bounding rectangle. </param>
///
/// <returns>	A boolean - true if the requirements are satisfied. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool shouldMerge(vector<Point> cont1, vector<Point> cont2, double theta1, double theta2,
		                 Point2f mc1, Point2f mc2, RotatedRect rect1, RotatedRect rect2){
	bool dTheta = min(abs(theta1-theta2), min(abs(theta1-theta2+360), abs(theta1-theta2-360))) < 15;
	float angle1=rect1.angle;
	float angle2=rect2.angle;
	float h1 = rect1.size.height;
	float h2 = rect2.size.height;
	if (rect1.angle < -45.) {
		angle1 += 90.0;
		h1= rect1.size.width;
	}
	if (rect2.angle < -45.) {
		angle2 += 90.0;
		h2= rect2.size.width;
	}
	bool dHeights = (2*abs(h1-h2))/(h1+h2) < 0.2;
	bool dMc = sqrt(pow((mc2.x-mc1.x), 2) + pow((mc2.y-mc1.y),2)) < 4*(h1+h2);
	bool dAngle = abs(angle1-angle2)<1;
	Point2f points1[4];
	rect1.points(points1);
	float dy=0, dx=0;
	if(points1[0].y-points1[1].y==0)
		dy=abs(points1[1].y-points1[2].y);
	else
		dy=abs(points1[0].y-points1[1].y);
	if(points1[0].x-points1[1].x==0)
		dx=abs(points1[1].x-points1[2].x);
	else
		dx=abs(points1[0].x-points1[1].x);
	float mr1= dy/dx;
	Point2f points2[4];
	rect2.points(points2);
	if(points2[0].y-points2[1].y==0)
		dy=abs(points2[1].y-points2[2].y);
	else
		dy=abs(points2[0].y-points2[1].y);
	if(points2[0].x-points2[1].x==0)
		dx=abs(points2[1].x-points2[2].x);
	else
		dx=abs(points2[0].x-points2[1].x);
	float mr2=dy/dx;
	float xc1=rect1.center.x;
	float yc1=rect1.center.y;
	float br1=yc1-mr1*xc1;
	float xc2=rect2.center.x;
	float yc2=rect2.center.y;
	float br2=yc2-mr2*xc2;
	float dl= abs(yc2-mr1*xc2-br1)/(2*sqrt(1+pow(mr1,2)))+abs(yc1-mr2*xc1-br2)/(2*sqrt(1+pow(mr2,2)));
	bool dl5= dl<15;
	return (dAngle || dTheta) && dHeights && dMc  && dl5;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Merges two components   </summary>
///
/// <param name="cont1">		[in] The first component. </param>
/// <param name="cont2">		[in] The second component. </param>
///
/// <returns>	A vector of Points - the merged component. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<Point> mergeConts(vector<Point> cont1, vector<Point> cont2){
	vector<Point> newContour;
	for (int j = 0; j < (int)cont1.size(); j++) {
		newContour.push_back(cont1[j]);
	}
	for (int j = 0; j < (int)cont2.size(); j++) {
		newContour.push_back(cont2[j]);
	}
	return newContour;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Extracts text lines from the image according to the morphology-based text
///             line extraction algorithm   </summary>
///
/// <param name="tl">		[out] Vector of text lines extracted from the image. </param>///
////////////////////////////////////////////////////////////////////////////////////////////////////

void MorphologyTextLineExtractor::extract(vector<TextLine*>& tl){

	Mat src=convertMat();
	Mat blured, closed, opened, diff, dst;
	blur(src, blured, Size(3,3));
	Canny( blured, blured, 150, 400, 3 );
	Mat element = getStructuringElement( MORPH_RECT, Size(1, 7), Point(-1, -1));
	morphologyEx(blured,closed,MORPH_CLOSE,element);
	morphologyEx(blured,opened,MORPH_OPEN,element);
	absdiff(closed, opened, diff);
	element = getStructuringElement( MORPH_RECT, Size(5, 5), Point(-1, -1));
	morphologyEx(diff,closed,MORPH_CLOSE,element);
	threshold( closed, dst, 0, 255 ,0);
	//imshow("threshold", dst);
	Mat imgForCrop;
	dst.copyTo(imgForCrop);

	//Moment-based orientation estimation
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	int contSize=(int)contours.size();

	/// Get the moments
	vector<Moments> mu(contours.size() );
	for( int i = 0; i < (int)contours.size(); i++ )
	{
		mu[i] = moments( contours[i], false );
	}

	///  Get the mass centers and orientations:
	vector<Point2f> mc( contours.size() );
	vector<double> theta( contours.size() );
	vector<RotatedRect> box(contours.size());
	for( int i = 0; i < (int)contours.size(); i++ )
	{
		mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
		if((mu[i].m20 - mu[i].m02)!=0){
			theta[i] = 0.5 * atan(2 * mu[i].m11 /(mu[i].m20 - mu[i].m02) );
			theta[i] = (theta[i] / PI) * 180;
		}
		else
			theta[i] = 0;

		// Find the minimum area enclosing bounding box
		box[i] = minAreaRect(contours[i]);
	}

	//Text candidate selection
	float width=0, height=0;
	double area=0;
	for( int i = 0; i < (int)contours.size(); i++ ){
		Point2f vtx[4];
		box[i].points(vtx);
		width=box[i].size.width;
		height=box[i].size.height;
		if (box[i].angle < -45.) {
			swap(width, height);
		}

		area=contourArea(contours[i], false );

		if(area/(width*height)<=0.2 || width/height<=0.5 || area<=2.5){
			if(contSize>1){
				contours[i].clear();
				contSize--;
			}
			else
				contSize=0;
			box[i]=RotatedRect();

		}
	}

	Mat M, rotated, cropped;
	float tw=85 ,th=85;
	for( int i = 0; i < (int)contours.size(); i++ ){
		if((contSize>0) && !contours[i].empty()){
			// get angle and size from the bounding box
			float angle = box[i].angle;
			Size rect_size = box[i].size;
			if (box[i].angle < -45.) {
				angle += 90.0;
				swap(rect_size.width, rect_size.height);
			}
			// get the rotation matrix
			M = getRotationMatrix2D(box[i].center, angle, 1.0);
			// perform the affine transformation
			warpAffine(blured, rotated, M, imgForCrop.size(), INTER_CUBIC);
			// crop the resulting image
			getRectSubPix(rotated, rect_size, box[i].center, cropped);

			vector<float> x_proj(cropped.cols);
			x_proj=x_projection(cropped);
			bool vall=checkValleys(x_proj, rect_size.height);
			vall=true;
			if(!vall){
				box[i]=RotatedRect();
				if(contSize>1){
					contours[i].clear();
					contSize--;
				}
				else
					contSize=0;
			}
			else{
				float avgW = avgWidth(x_proj);
				float varW= varianceWidth(x_proj, avgW);
				float avgH = avgHeight(x_proj);
				float varH= varianceHeight(x_proj, avgH);
				if(varW>tw || varH>th){
					box[i]=RotatedRect();
					if(contSize>1){
						contours[i].clear();
						contSize--;
					}
					else
						contSize=0;
				}
			}
		}
	}

	//Text line merging
	if(contSize>1){
		for( int i = 0; i < (int)contours.size(); i++ ){
			if(!contours[i].empty()){
				for( int j = i+1; j < (int)contours.size(); j++ ){
					if(!contours[j].empty()){
						if(shouldMerge(contours[i], contours[j], theta[i], theta[j], mc[i], mc[j], box[i], box[j])){
							contours[j]=mergeConts(contours[i], contours[j]);
							contours[i].clear();
							contSize--;
							break;
						}
					}
				}
			}
		}

		for( int i = 0; i < (int)contours.size(); i++ ){
			if(!contours[i].empty()){
				box[i] = minAreaRect(contours[i]);
			}
			else
				box[i]=RotatedRect();
		}
	}

	for( int i = 0; i < (int)contours.size(); i++ ){
		if((contSize>0) && !contours[i].empty() && (box[i].size.width*box[i].size.height<600)){
			if(contSize>1){
				contours[i].clear();
				contSize--;
			}
			else
				contSize=0;
			box[i]=RotatedRect();
		}
	}

	if(contSize>0){
		for( int i = 0; i < (int)contours.size(); i++ ){
			if(!contours[i].empty()){
				TextLine* t= new TextLine();
				t->setImage(_image);
				t->setRect(box[i]);
				tl.push_back(t);
			}
		}

		RNG rng(12345);
		/// Draw contours
		Mat drawing = Mat::zeros( dst.size(), CV_8UC3 );
		for( int i = 0; i< (int)contours.size(); i++ )
		{
			if(!contours[i].empty()){
				Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
				drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
				circle( drawing, mc[i], 4, color, -1, 8, 0 );

				// Draw the bounding box
				Point2f vtx[4];
				box[i].points(vtx);
				for( int k = 0; k < 4; k++ )
					line(_image, vtx[k], vtx[(k+1)%4], Scalar(0, 255, 0), 1, 1);
			}
		}
		imshow( "Result",  _image);
		waitKey(0);
		/// Show in a window-Debag
		//imshow( "contours",  drawing);
	}
}
