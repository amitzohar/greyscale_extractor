#include "stdafx.h"
#include "ImageTools.h"
#include "cv_macros.h"


void ImageTools::shift(std::vector<cv::Point>& pts, cv::Point offset){
	std::vector<cv::Point>::iterator iter;
	for (iter = pts.begin(); iter != pts.end(); iter++)
		*iter += offset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Return ths center of mass of a given image. </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageTools::getPixelEightNeighbors(Mat img, int row, int col, int radius, uchar p[]){
	p[0] = img.at<uchar>(row - radius, col - radius);
	p[1] = img.at<uchar>(row - radius, col);
	p[2] = img.at<uchar>(row - radius, col + radius);
	p[3] = img.at<uchar>(row, col + radius);
	p[4] = img.at<uchar>(row + radius, col + radius);
	p[5] = img.at<uchar>(row + radius, col);
	p[6] = img.at<uchar>(row + radius, col - radius);
	p[7] = img.at<uchar>(row, col - radius);
}

void ImageTools::getPixelEightNeighbors(Mat img, int row, int col, int radius, float p[]){
	p[0] = img.at<float>(row - radius, col - radius);
	p[1] = img.at<float>(row - radius, col);
	p[2] = img.at<float>(row - radius, col + radius);
	p[3] = img.at<float>(row, col + radius);
	p[4] = img.at<float>(row + radius, col + radius);
	p[5] = img.at<float>(row + radius, col);
	p[6] = img.at<float>(row + radius, col - radius);
	p[7] = img.at<float>(row, col - radius);
}
cv::Point2f ImageTools::centerOfMass(cv::Mat mat){
	cv::Point2f center(0, 0);
	float sum = 0;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++){
			float s = mat.at<uchar>(i, j);
			float s2 = s*s;
			sum += s2;
			center.y += (i * s2);
			center.x += (j * s2);
		}
	}
	center.x /= sum;
	center.y /= sum;
	return center;
}

cv::Mat ImageTools::varianceAroundPoint(cv::Mat m, cv::Point2f point){
	cv::Mat variance(m.cols * m.rows, 2, CV_32F);
	int idx = 0;
	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++){
		float s = m.at<uchar>(i, j);
		variance.at<float>(idx, 0) = (float)(i - point.y) * s;
		variance.at<float>(idx, 1) = (float)(j - point.x) * s;
		idx++;
		}
	return variance;
}

Orientation ImageTools::orientation(cv::Mat mat, cv::Point2f &com){
	std::pair<cv::Point2f, cv::Point2f> orient;
	cv::Mat covariance, variance, eigen_vals, eigen_vects;
	com = centerOfMass(mat);
	variance = varianceAroundPoint(mat, com);
	covariance = variance.t() * variance;
	cv::eigen(covariance, eigen_vals, eigen_vects);
	orient.first = cv::Point2f(eigen_vects.at<float>(0, 0), eigen_vects.at<float>(0, 1));
	orient.second = cv::Point2f(eigen_vects.at<float>(1, 0), eigen_vects.at<float>(1, 1));
	return orient;
}

void  ImageTools::smoothColumn(cv::Mat mat, cv::Mat filter, int col_idx){
	cv::Mat dest = cv::Mat(mat);
	int offset = (filter.rows - 1) / 2;
	for (int i = offset; i < mat.rows - offset; i++){
		cv::Mat m = mat(cv::Range(i - offset, i + offset + 1), cv::Range(col_idx, col_idx + 1));
		double sum = m.dot(filter);
		if (mat.depth() == 1)
			dest.at<uchar>(i, col_idx) = (uchar)sum;
		else
			dest.at<float>(i, col_idx) = (float)sum;
	}
	dest.copyTo(mat);
}

void ImageTools::fillComponents(cv::Mat mat, std::vector<ConnectedComponent*>& components, float filler, float boundary){
	vector < vector < Point >> plys;

	for each (ConnectedComponent* comp in components){
		plys.push_back(comp->getContour().getPoints());
	}

	fillPoly(mat, plys,  filler);
	polylines(mat, plys, true, boundary);
}

cv::Mat ImageTools::draw(cv::Mat mat, std::vector<ConnectedComponent*>& components, cv::Scalar clr){
	vector < vector < Point >> polys;
	for each (ConnectedComponent* comp in components){
		polys.push_back(comp->getContour().getPoints());
	}

	Mat img;
	cvtColor(mat, img, CV_GRAY2RGB);
	polylines(img, polys, true, clr);
	ImageTools::display("Polyline ", img);
	return img;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Compute the histogram of a gray scale image and store it into an array of 256 integers </summary>
///
/// <remarks>	El Sana, 2/15/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageTools::histogramGrayImage(Mat img, int histogram[], int bins ){

	memset(histogram, 0, bins*sizeof(int));
	for ( int row = 0 ; row < img.rows; row++)
		for ( int col = 0 ; col < img.cols; col++ )
			histogram[(int)(img.atuchar(row, col)/bins)]++ ;
}

cv::Mat ImageTools::histogramImage(Mat histogram){
	double min, max;
	Point start, end;
	minMaxLoc(histogram, &min, &max);
	cv::Mat img = cv::Mat::zeros(256, histogram.rows, CV_8U);
	start.y = 255;
	for (int col = 0; col < histogram.rows; col++){
		start.x = end.x = col;
		end.y = 255 - (int)(255.0 * histogram.at<float>(col) / max);
		cv::line(img, start, end, Scalar(255));
	}
	return img;
}

void ImageTools::remapColor(cv::Mat map, Vec3b color, cv::Point org, cv::Mat img){
	Mat roi = img.adjustROI(org.y, org.y + map.rows, org.x, org.x + map.cols);
	for (int row = 0; row < map.rows; row++)
		for (int col = 0; col < map.cols; col++)
			img.at<Vec3b>(row, col) = color * map.at<float>(row, col);
}


void ImageTools::display(String win, DImage& img){
	namedWindow(win, cv::WINDOW_NORMAL);
	imshow(win, img.getMat());
}

void ImageTools::display(String win, Mat& img){
	namedWindow(win, cv::WINDOW_NORMAL);
	imshow(win, img);
}

void ImageTools::display(String win, int idx, DImage& img){
	char buf[32];
	sprintf(buf, "-[%d]", idx);
	namedWindow(win + String(buf), cv::WINDOW_NORMAL);
	imshow(win + String(buf), img.getMat());
}

void ImageTools::display(String win, int idx, Mat& img){
	char buf[32];
	sprintf(buf, "-[%d]", idx);
	namedWindow(win + String(buf), cv::WINDOW_NORMAL);
	imshow(win + String(buf), img);
}

void ImageTools::displayFresh(String win, Mat& img){
	display(win, Index++, img);
}