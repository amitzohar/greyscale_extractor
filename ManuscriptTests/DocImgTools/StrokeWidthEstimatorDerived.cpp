#include "stdafx.h"
#include <math.h>
#include "StrokeWidthEstimatorDerived.h"
#include "DImage.h"

StrokeWidthEstimatorDerived::StrokeWidthEstimatorDerived(){;}

StrokeWidthEstimatorDerived::~StrokeWidthEstimatorDerived(){;}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Calculates the height and widths of the color changes  </summary>
///
/// <param name="imgPxls">			[in] vectors of pixels </param>
/// <param name="heights">			[out] The heights </param>
/// <param name="widths">			[out] The widths </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void calcHeightsAndWidths(vector <vector<uchar> > imgPxls, vector<uchar>* heights, vector<int>* widths){
	int pxl, acc=0;
	for(int i=0; i<(int)imgPxls.size();i++){
		for(int j=1; j<(int)imgPxls[i].size(); j++){
			if((imgPxls[i][j]-imgPxls[i][j-1]) > 10){
				pxl=imgPxls[i][j-1];
				j++;
				acc++;
				while(j<(int)imgPxls[i].size() && imgPxls[i][j-1]<imgPxls[i][j]){
					j++;
					acc++;
				}
				heights->push_back(imgPxls[i][j-1]-pxl);
				while(j<(int)imgPxls[i].size() && (imgPxls[i][j]-pxl > 10)){
					j++;
					acc++;
				}
				widths->push_back(acc);
			}
			else{
				if((imgPxls[i][j-1]-imgPxls[i][j]) > 10){
					pxl=imgPxls[i][j-1];
					j++;
					acc++;
					while(j<(int)imgPxls[i].size() && imgPxls[i][j-1]>imgPxls[i][j]){
						j++;
						acc++;
					}
					heights->push_back(pxl-imgPxls[i][j-1]);
					while(j<(int)imgPxls[i].size() && (pxl-imgPxls[i][j] > 10)){
						j++;
						acc++;
					}
					widths->push_back(acc);
				}
			}
			acc=0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Estimates the average stroke width in the image, also prints the average
///             height of the color changes, the standard deviation of the heights and the median
///             of the stroke widths   </summary>
///
/// <returns>   A float- the average width </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

float StrokeWidthEstimatorDerived::estimate(){
	int i=0, j=0;
	Mat img =_image->getMat();
	int rows= img.rows;
	int cols= img.cols;
	vector<vector<uchar> > vertical(cols, vector<uchar>(rows,0));
	vector<vector<uchar> > horizontal(rows, vector<uchar>(cols,0));
	vector<vector<uchar> > plus45(rows+cols-1, vector<uchar>(0));
	vector<vector<uchar> > minus45(rows+cols-1, vector<uchar>(0));
	vector<uchar> heights;
	vector<int> widths;

	//horizontal and vertical
	for(i=0; i<rows;i++){
		for(j=0; j<cols; j++){
			horizontal[i][j]=img.at<uchar>(i,j);
			vertical[j][i]=horizontal[i][j];
		}
	}

	//-45 and +45
	int k;
	for(k=0; k<rows;k++){
		for(i=k, j=0; i<rows && j<cols ;i++, j++){
			minus45[k].push_back(horizontal[i][j]);
			plus45[k].push_back(horizontal[i][cols-1-j]);
		}
	}
	int idx=k-1;
	for(k=1; k<cols;k++){
		for(i=0, j=k; i<rows && j<cols ;i++, j++){
			minus45[idx+k].push_back(horizontal[i][j]);
			plus45[idx+k].push_back(horizontal[i][cols-1-j]);
		}
	}

	calcHeightsAndWidths(horizontal, &heights, &widths);
	calcHeightsAndWidths(vertical, &heights, &widths);
	calcHeightsAndWidths(plus45, &heights, &widths);
	calcHeightsAndWidths(minus45, &heights, &widths);
	float avgH=0, stnDev=0;
	int sizeH=heights.size();
	if(sizeH>0){
		float sum=0;
		for(i=0;i<sizeH;i++){
			sum=sum+heights[i];
		}
		avgH=sum/sizeH;

		float sumOfSquares=0;
		for(i=0;i<sizeH;i++){
			sumOfSquares=sumOfSquares+pow((heights[i]-avgH), 2);
		}
		stnDev=sqrt(sumOfSquares/sizeH);
	}
	cout<<"The average height is: "<< avgH<<endl;
	cout<<"The standard deviation of the heights is: "<< stnDev<< endl;

	vector<int> newWidths;
	float avgW=0, median=0;
	int size= widths.size();
	if(size>0){
		float sum2=0;
		for(i=0;i<size;i++){
			if(heights[i] > avgH-(stnDev+5)){
				newWidths.push_back(widths[i]);
				sum2=sum2+widths[i];
			}
		}
		int sizeW=newWidths.size();
		avgW=sum2/sizeW;

		sort(newWidths.begin(), newWidths.end());
		int medIdx=sizeW/2;
		if(sizeW%2==0){
			median = (newWidths[medIdx]+newWidths[medIdx+1])/2;
		}
		else
			median = newWidths[medIdx+1];
	}
	cout << "The median of the stroke widths is: " << median << endl<< endl;

	return avgW;
}
