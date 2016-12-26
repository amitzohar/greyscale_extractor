/* 
 * File:   BinarizerKabir.cpp
 * Author: Arnon Benshahar
 * This class was implemented on the base of the article "Binarization of degraded document image based on feature
 * space partitioning and classification" by Morteza Valizadeh and Ehsanollah Kabir 
 * 
 * This class is dealing with binarization of degraded document images.
 * 
 * The algorithm pipeline: 
 *   1. Feature Extraction  
 *   2.Feature space partitioning 
 *   3. Running NiBlack algorithm  
 *   4. Partition classification 
 *   5. Pixel classification 
 * 
 * Created on June 17, 2015, 10:43 AM
 */

#include "stdafx.h"
#include <iostream>
#include <math.h>
#include "BinarizerNiblack.h"
#include "ImageTools.h" 
#include "BinarizerKabir.h"

BinarizerKabir::BinarizerKabir() {
	_strokeWidth = 0 ;
    _k = 0 ;
}

BinarizerKabir::BinarizerKabir(const BinarizerKabir& org) {
	_strokeWidth = org._strokeWidth ;
    _k = org._k ;
}

BinarizerKabir::BinarizerKabir(int stroke_width,int k){
    _strokeWidth = stroke_width;
    _k = k;
}

BinarizerKabir::~BinarizerKabir() {
}

void BinarizerKabir::setStrokeWidth(int strokeWidth){
    this->_strokeWidth = strokeWidth;
}

void BinarizerKabir::setK(int k){
    _k = k;
}

DImage* BinarizerKabir::binarize(){  
	if ( _strokeWidth == 0 || _k == 0 )
		return NULL ;

	long* partition_count = new long[_k] ;
	memset(partition_count, 0, _k*sizeof(long));
    featureExtraction();
    partitionFeatureSpace();  
    runNiBlackAlgorithm();
    partitionClassification(partition_count);
    return new DImage(pixelClassification(partition_count));   
}
/*
 * First step in the pipeline: Feature Extraction.
 * We try to map the document image into a feature space in which 
 * the text and background pixels are separable. 
 * We propose a new feature named structural contrast (will be referred as SC)  
 * and use it together with gray level  (will be referred as GL) in this application.
 * 
 * Structural contrast (SC): This feature is based on the stroke width and gray 
 * level of the image. For each pixel, eight logical values are generated.
 * 
 * For every pixel (x,y), it is mapped into feature space,
 * f = [ f1, f2], where f1 = SC(x, y) and f2 = GL(x, y)
 * 
 * */
void BinarizerKabir::featureExtraction() {
    _structuralContrast = _image->getMat().clone();
    _grayLevel          = _image->getMat().clone();
    _meanStrokeWidth    = _image->getMat().clone();
    calculateMeanStrokeWidth();
	for (int i = _strokeWidth; i < _grayLevel.rows - _strokeWidth; i++) {
		for (int j = _strokeWidth; j < _grayLevel.cols - _strokeWidth; j++) {
            int max_neighbor = 0;
            uchar  p[8];
			ImageTools::getPixelEightNeighbors(_meanStrokeWidth, i, j, _strokeWidth, p); 
            for (int k = 0; k < 3; k++) {
                int kp = (k+4)%8;
				int min_neighbor = min(p[k], min(p[k + 1], min(p[kp], min(p[kp + 1], (uchar)255))));
				if (min_neighbor > max_neighbor)
					max_neighbor = min_neighbor ;
            }
			_structuralContrast.at<uchar>(i, j) = max_neighbor;
        }
    }
    calculateFeatureSpace();
    
}

void BinarizerKabir::calculateMeanStrokeWidth() {
    for (int i = 0; i < _grayLevel.rows; i++) {
        for (int j = 0; j < _grayLevel.cols; j++) {
            int sum = 0;
            int count = 0;
            for (int l = i-_strokeWidth; l <= i+_strokeWidth; l++) {
                for (int k = j-_strokeWidth; k <= j+_strokeWidth; k++) {
                    if(l > 0 && l < _grayLevel.rows && k > 0 && k < _grayLevel.cols){
                        sum = sum +_grayLevel.at<uchar>(l, k);
                        count++;
                    }
                }
            }
            _meanStrokeWidth.at<uchar>(i,j) = sum / count;
        }
    }
}

void BinarizerKabir::calculateFeatureSpace(){
    _featureSpace = Mat(_grayLevel.cols*_grayLevel.rows, 6, CV_32F);
    int index = 0;
    for (int i = 0; i < _grayLevel.rows; i++) {
        for (int j = 0; j < _grayLevel.cols; j++) {
            _featureSpace.at<int>(index,0) = (int)_grayLevel.at<uchar>(i,j);
            _featureSpace.at<int>(index,1) = (int)_structuralContrast.at<uchar>(i,j);
            _featureSpace.at<int>(index,2) = i;
            _featureSpace.at<int>(index,3) = j;
            index++;
        }
    }
}

/*
 *Second step in the pipeline: Feature space partitioning.
 *We partition the feature space using the Kmean clustering technique into k clusters.
 *
 */
void BinarizerKabir::partitionFeatureSpace(){

    Mat points(_featureSpace.rows,2, CV_32F,Scalar(10));
    Mat centers(10, 1, points.type());

    for(int row = 0; row < _featureSpace.rows; row++)
		for(int col = 0; col < 2; col++)
			points.at<float>(row, col)=_featureSpace.at<int>(row, col);
	
    kmeans(points, _k, _labels, TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, centers);
    
    for (int i = 0; i < _labels.rows; i++) 
        _featureSpace.at<int>(i,4) = (int)_labels.at<int>(i);

}

/*
 * Third step in the pipeline: Run auxiliary binarization algorithm
 * 
 * Our region classification method performs well when we use
 * an auxiliary binarization algorithm that correctly labels more
 * than 50% of the pixels inside each region. We use
 * Niblack’s method because it appropriate for this application.
 */

void BinarizerKabir::runNiBlackAlgorithm(){
    BinarizerNiblack binarizer;
    DImage* dimage = new DImage(_grayLevel);  
    DImage* result = dimage->binarize(binarizer);
    _binaryNiblack = result->getMat();
}

/*
 * Fourth step in the pipeline: Partition classification
 * 
 * After partitioning the feature space into small regions, we use 
 * the result of an auxiliary binarization algorithm to classify 
 * each region as either text or background.
 * 
 */

void BinarizerKabir::partitionClassification(long partition_count[]){
    int count =0;  

    for (int i = 0; i < _binaryNiblack.rows; i++) 
        for (int j = 0; j < _binaryNiblack.cols; j++) {
            int index = _labels.at<int>(count);
            if(_binaryNiblack.at<uchar>(i,j) == 255)
                partition_count[index]++;
            else
                partition_count[index]--;
            _featureSpace.at<int>(count,5) = (int)_binaryNiblack.at<uchar>(i,j);
            count++;
        }
}

/*
 * Fifth step in the pipeline: pixelClassification
 * 
 * We use the classification results of regions to binarize the document image. 
 * Suppose G(x, y) is mapped into [ f1, f2] in the feature space where [ f1, f2] ∈ Ri, Ri is a region.
 * The binary image, B(x, y), is obtained as follows:
 * B(x, y) =  0 if class(Ri) = text , 1 if class(Ri) = background
 * 
 */
Mat BinarizerKabir::pixelClassification(long* countPartition){
    Mat final = _grayLevel.clone();
    int count = 0;
    for (int i = 0; i < _featureSpace.rows; i++) {
        int group = _featureSpace.at<int>(i,4);
        int row = _featureSpace.at<int>(i,2);
        int col = _featureSpace.at<int>(i,3);
        if(countPartition[group] > 0)
            final.at<uchar>(row,col) = 255;
        else
            final.at<uchar>(row,col) = 0;
    }     
   return final;
}

//The next methods are for debugging 

void BinarizerKabir::printArea(long countPartition[]){
    Mat regions (255,400,CV_8U,Scalar(255));
    for (int i = 0; i < _featureSpace.rows; i++) {
        int group = _featureSpace.at<int>(i,4);
        int gl = _featureSpace.at<int>(i,0);
        int sc = _featureSpace.at<int>(i,1);
        regions.at<uchar>(gl,sc) = uchar(group)*5+40;

    }
    imshow("regions", regions);
}

void BinarizerKabir::printAreaBinary(long countPartition[]){
    Mat regions (255,400,CV_8U,Scalar(255));
    for (int i = 0; i < _featureSpace.rows; i++) {
        int group = _featureSpace.at<int>(i,4);
        int gl = _featureSpace.at<int>(i,0);
        int sc = _featureSpace.at<int>(i,1);
        if(countPartition[group] > 0)
            regions.at<uchar>(gl,sc) = 50;
        else
            regions.at<uchar>(gl,sc) = 150;

    }
    imshow("regionsBinary", regions);
}

void BinarizerKabir::printAreaBinaryNiblack(long countPartition[]){
    Mat regions (255,400,CV_8U,Scalar(255));
    int count = 0;
    for (int i = 0; i < _binaryNiblack.rows; i++) {
        for (int j = 0; j < _binaryNiblack.cols; j++) {
            int gl = _featureSpace.at<int>(count,0);
            int sc = _featureSpace.at<int>(count,1);
        if(_binaryNiblack.at<uchar>(i,j) == 0)
            regions.at<uchar>(gl,sc) = 150;
        else
            regions.at<uchar>(gl,sc) = 50;
        count++;    
        }

    }
    imshow("binaryNi", regions);
}
