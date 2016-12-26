
#include "stdafx.h"
#include "TextLineExtractorGrid.h"
#include "DImage.h"
#include "ProjectionProfile.h"

TextLineExtractorGrid::TextLineExtractorGrid(Mat img, int lineHeight) :TextLineExtractor(){
	_image = new DImage(img);
	_lineHeight = lineHeight;
	_allowedLineDistance = 50;
	_splitRatio = 1.5;
	_peakSimilarity = 0.5;
}

TextLineExtractorGrid::TextLineExtractorGrid(Mat img, int lineHeight, double peakSimilarity) :TextLineExtractor(){
	_image = new DImage(img);
	_lineHeight = lineHeight;
	_allowedLineDistance = 50;
	_splitRatio = 1.5;
	_peakSimilarity = peakSimilarity;
}


void TextLineExtractorGrid::extract(vector<TextLine*>& textLines){

	cv::Mat invSrc = cv::Scalar::all(255) - _image->getMat();
	DImage* invSrcDimg = new DImage(invSrc);


	// 0. Calculate lines
	std::vector<std::vector<Point2i>> lines = calculateByProjection(invSrcDimg, _lineHeight);

	// 8. Convert structure to Vector of TextLines
	for (int i = 0; i < lines.size(); i++){
		std::vector<Point>* upperBound = new vector < Point >;
		std::vector<Point>* lowerBound = new vector < Point >;
		for (int j = 0; j < lines[i].size() / 2; j++){
			Point2i start = lines[i][j * 2];
			Point2i end = lines[i][j * 2 + 1];
			(*upperBound).insert((*upperBound).end(), *(new Point(start.x, start.y + _lineHeight / 2)));
			(*upperBound).insert((*upperBound).end(), *(new Point(end.x, end.y + _lineHeight / 2)));
			(*lowerBound).insert((*lowerBound).end(), *(new Point(start.x, start.y - _lineHeight / 2)));
			(*lowerBound).insert((*lowerBound).end(), *(new Point(end.x, end.y - _lineHeight / 2)));
		}
		TextLine* textLine = new TextLine();
		textLine->setImage(_image->getMat());
		textLine->setUpperBound(upperBound);
		textLine->setLowerBound(lowerBound);
		textLines.insert(textLines.end(), textLine);
	}

	return;
}

std::vector<std::vector<Point2i>> TextLineExtractorGrid::calculateByProjection(DImage* img, int line_height){

	// 1. Compute projection profile in many directions for current image
	int maxPeakIndex = 0;
	double maxPeak = 0;
	double sumPeaks = 0;
	double not_zero = 0;
	double peakAVG = 0;
	std::vector<std::pair<double, int>> peaks;
	for (int i = 0; i < 178; i++){
		ProjectionProfile* proj = new ProjectionProfile(0, i * 1);
		proj->setImage(img);
		DImage* profile = proj->project();
		int peak = proj->getWeight(profile, 1);
		if (peak > 0){
			sumPeaks += peak;
			not_zero++;
		}
		peaks.insert(peaks.end(), std::make_pair(peak, i * 1));
		if (peak > maxPeak){
			maxPeak = peak;
			maxPeakIndex = peaks.size() - 1;
		}
	}
	peakAVG = sumPeaks / not_zero;

	// 2. Decide if the max peak is significantly higher than rest
	int similar = 0;
	for (int i = 0; i < peaks.size(); i++){
		if ((maxPeak - peaks[i].first) < (maxPeak * _peakSimilarity) && (abs(i - maxPeakIndex) > 10) && (abs(abs(i - 178) - maxPeakIndex) > 10)){
			similar++;
		}
	}


	if (similar == 0 || (img->getMat().rows < 50 || img->getMat().cols < 50)){
		// 3.1 Calculate lines from the max projection that was accepted
		//cout << "Accepting:" << " [Skew: " << peaks[maxPeakIndex].second << ", Max Peak: " << maxPeak << ", Amount similar: " << similar << "]\n";
		std::vector<std::vector<Point2i>> lines = findTextLinesFromPeaks(peaks[maxPeakIndex], img, line_height);
		return lines;
	}
	else {
		// 3.2 Projection is not good enough. divide image
		//cout << "Not Accepting (dividing picture):" << " [Skew: " << peaks[maxPeakIndex].second << ", Max Peak: " << maxPeak << ", Amount similar: " << similar << "]\n";

		// 4. Decide how to divide the page
		if (img->getMat().cols > _splitRatio * img->getMat().rows){
			// 4.1 case 2: image is very wide
			DImage* cell_1 = new DImage(cv::Mat(img->getMat(), cv::Rect(img->getMat().cols / 2, 0, img->getMat().cols / 2, img->getMat().rows)));
			DImage* cell_2 = new DImage(cv::Mat(img->getMat(), cv::Rect(0, 0, img->getMat().cols / 2, img->getMat().rows)));

			// 5. recurssion
			std::vector<std::vector<Point2i>> cell_1_lines = calculateByProjection(cell_1, line_height);
			std::vector<std::vector<Point2i>> cell_2_lines = calculateByProjection(cell_2, line_height);

			// 6. Get original cords
			getOriginalCordsWidth(cell_1_lines, cell_1, cell_2_lines, cell_2);

			// 7. Unify returned lines into new
			return unifyLinesByWidth(cell_2_lines, cell_1_lines);
		}
		else if (img->getMat().rows > _splitRatio * img->getMat().cols){
			// 4.2 case 3: image is very high
			DImage* cell_1 = new DImage(cv::Mat(img->getMat(), cv::Rect(0, 0, img->getMat().cols, img->getMat().rows / 2)));
			DImage* cell_2 = new DImage(cv::Mat(img->getMat(), cv::Rect(0, img->getMat().rows / 2, img->getMat().cols, img->getMat().rows / 2)));

			// 5. recurssion
			std::vector<std::vector<Point2i>> cell_1_lines = calculateByProjection(cell_1, line_height);
			std::vector<std::vector<Point2i>> cell_2_lines = calculateByProjection(cell_2, line_height);

			// 6. Get original cords
			getOriginalCordsHeight(cell_1_lines, cell_1, cell_2_lines, cell_2);

			// 7. Unify returned lines into new
			return unifyLinesByWidth(cell_1_lines, cell_2_lines);
		}
		else {
			// 4.3 case 1: square
			DImage* cell_1 = new DImage(cv::Mat(img->getMat(), cv::Rect(img->getMat().cols / 2, 0, img->getMat().cols / 2, img->getMat().rows / 2)));
			DImage* cell_2 = new DImage(cv::Mat(img->getMat(), cv::Rect(0, 0, img->getMat().cols / 2, img->getMat().rows / 2)));
			DImage* cell_3 = new DImage(cv::Mat(img->getMat(), cv::Rect(0, img->getMat().rows / 2, img->getMat().cols / 2, img->getMat().rows / 2)));
			DImage* cell_4 = new DImage(cv::Mat(img->getMat(), cv::Rect(img->getMat().cols / 2, img->getMat().rows / 2, img->getMat().cols / 2, img->getMat().rows / 2)));

			// 5. recurssion
			std::vector<std::vector<Point2i>> cell_1_lines = calculateByProjection(cell_1, line_height);
			std::vector<std::vector<Point2i>> cell_2_lines = calculateByProjection(cell_2, line_height);
			std::vector<std::vector<Point2i>> cell_3_lines = calculateByProjection(cell_3, line_height);
			std::vector<std::vector<Point2i>> cell_4_lines = calculateByProjection(cell_4, line_height);

			// 6. Get original cords
			getOriginalCords(cell_1_lines, cell_1, cell_2_lines, cell_2, cell_3_lines, cell_3, cell_4_lines, cell_4);

			// 7. Unify returned lines into new
			std::vector<std::vector<Point2i>> temp1 = unifyLinesByWidth(cell_2_lines, cell_3_lines);
			std::vector<std::vector<Point2i>> temp2 = unifyLinesByWidth(cell_1_lines, cell_4_lines);
			std::vector<std::vector<Point2i>> final = unifyLinesByHeight(temp1, temp2);
			return final;
		}

	}

}

void TextLineExtractorGrid::getOriginalCords(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2, std::vector<std::vector<Point2i>> &cell_3_lines, DImage* cell_3, std::vector<std::vector<Point2i>> &cell_4_lines, DImage* cell_4){
	for (int i = 0; i < cell_1_lines.size(); i++){
		for (int j = 0; j < cell_1_lines[i].size(); j++){
			cell_1_lines[i][j].x += cell_1->getMat().cols;
		}
	}

	for (int i = 0; i < cell_3_lines.size(); i++){
		for (int j = 0; j < cell_3_lines[i].size(); j++){
			cell_3_lines[i][j].y += cell_1->getMat().rows;
		}
	}

	for (int i = 0; i < cell_4_lines.size(); i++){
		for (int j = 0; j < cell_4_lines[i].size(); j++){
			cell_4_lines[i][j].x += cell_3->getMat().cols;
			cell_4_lines[i][j].y += cell_1->getMat().rows;
		}
	}
}

void TextLineExtractorGrid::getOriginalCordsWidth(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2){
	for (int i = 0; i < cell_1_lines.size(); i++){
		for (int j = 0; j < cell_1_lines[i].size(); j++){
			cell_1_lines[i][j].x += cell_1->getMat().cols;
		}
	}
}

void TextLineExtractorGrid::getOriginalCordsHeight(std::vector<std::vector<Point2i>> &cell_1_lines, DImage* cell_1, std::vector<std::vector<Point2i>> &cell_2_lines, DImage* cell_2){
	for (int i = 0; i < cell_2_lines.size(); i++){
		for (int j = 0; j < cell_2_lines[i].size(); j++){
			cell_2_lines[i][j].y += cell_1->getMat().rows;
		}
	}
}

std::vector<std::vector<Point2i>> TextLineExtractorGrid::unifyLinesByWidth(std::vector<std::vector<Point2i>> cell_1_lines, std::vector<std::vector<Point2i>> cell_2_lines) {
	std::vector<int> indexes_to_push;

	if (cell_1_lines.size() == 0){
		return cell_2_lines;
	}

	// from top to bottom
	for (int i = 0; i < cell_1_lines.size(); i++){
		int lastIndex = cell_1_lines[i].size() - 1;
		Point2i point_end_1 = cell_1_lines[i][lastIndex];
		Point2i point_start_1 = cell_1_lines[i][0];

		// check with bottom cell
		for (int j = 0; j < cell_2_lines.size(); j++){
			int lastIndex = cell_2_lines[j].size() - 1;
			Point2i point_end_2 = cell_2_lines[j][lastIndex];
			Point2i point_start_2 = cell_2_lines[j][0];

			if (point_start_1.y == point_start_2.y && abs(point_start_1.x - point_start_2.x) < _allowedLineDistance){
				for (int k = 0; k < cell_2_lines[j].size(); k++){
					cell_1_lines[i].insert(cell_1_lines[i].end(), cell_2_lines[j][k]);
				}
			}
			if (point_start_1.y == point_end_2.y && abs(point_start_1.x - point_end_2.x) < _allowedLineDistance){
				for (int k = 0; k < cell_2_lines[j].size(); k++){
					cell_1_lines[i].insert(cell_1_lines[i].end(), cell_2_lines[j][k]);
				}
			}
			if (point_end_1.y == point_start_2.y && abs(point_end_1.x - point_start_2.x) < _allowedLineDistance){
				for (int k = 0; k < cell_2_lines[j].size(); k++){
					cell_1_lines[i].insert(cell_1_lines[i].end(), cell_2_lines[j][k]);
				}
			}
			if (point_end_1.y == point_end_2.y && abs(point_end_1.x - point_end_2.x) < _allowedLineDistance){
				for (int k = 0; k < cell_2_lines[j].size(); k++){
					cell_1_lines[i].insert(cell_1_lines[i].end(), cell_2_lines[j][k]);
				}
			}
			else {
				// lines does not collide
				if (std::find(indexes_to_push.begin(), indexes_to_push.end(), j) == indexes_to_push.end()){
					indexes_to_push.insert(indexes_to_push.end(), j);
				}
			}
		}
	}

	// push cells that could not be attached
	for (int i = 0; i < indexes_to_push.size(); i++){
		cell_1_lines.insert(cell_1_lines.end(), cell_2_lines[indexes_to_push[i]]);
	}

	return cell_1_lines;
}

std::vector<std::vector<Point2i>> TextLineExtractorGrid::unifyLinesByHeight(std::vector<std::vector<Point2i>> cell_1_lines, std::vector<std::vector<Point2i>> cell_2_lines) {
	std::vector<int> indexes_to_push;

	if (cell_1_lines.size() == 0){
		return cell_2_lines;
	}

	// from left to right
	for (int i = 0; i < cell_1_lines.size(); i++){
		int lastIndex = cell_1_lines[i].size() - 1;
		Point2i point_end = cell_1_lines[i][lastIndex];

		// check with right cell
		for (int j = 0; j < cell_2_lines.size(); j++){
			Point2i point_start = cell_2_lines[j][0];
			if (point_start.x == point_end.x && abs(point_start.y - point_start.y) < _allowedLineDistance){
				for (int k = 0; k < cell_2_lines[j].size(); k++){
					cell_1_lines[i].insert(cell_1_lines[i].end(), cell_2_lines[j][k]);
				}
			}
			else {
				// lines does not collide
				if (std::find(indexes_to_push.begin(), indexes_to_push.end(), j) == indexes_to_push.end()){
					indexes_to_push.insert(indexes_to_push.end(), j);
				}
			}
		}
	}

	// push cells that could not be attached
	for (int i = 0; i < indexes_to_push.size(); i++){
		cell_1_lines.insert(cell_1_lines.end(), cell_2_lines[indexes_to_push[i]]);
	}

	return cell_1_lines;
}

std::vector<std::vector<Point2i>> TextLineExtractorGrid::findTextLinesFromPeaks(std::pair<double, int> &maxPeak, DImage* &invSrcDimg, int line_height){
	std::vector<std::vector<Point2i>> lines;

	int angle = maxPeak.second;
	int cols = invSrcDimg->getMat().cols;
	int rows = invSrcDimg->getMat().rows;


	ProjectionProfile* proj = new ProjectionProfile(0, angle);
	proj->setImage(invSrcDimg);
	DImage* profile = proj->project();

	std::vector<double> profile_values;
	Mat mat = profile->getMat();
	for (int i = 0; i < mat.rows; i++){
		profile_values.insert(profile_values.end(), (int)mat.at<float>(i, 0));
	}

	Mat dst;
	Mat invertMat;
	dst.create(cols, rows, CV_8U);
	Mat rot_mat = cv::getRotationMatrix2D(cv::Point2f(cols / 2.0f, rows / 2.0f), angle, 1);
	warpAffine(invSrcDimg->getMat(), dst, rot_mat, Size(cols, rows));
	invertAffineTransform(rot_mat, invertMat);


	//find minimum value of the picture
	int min_value = _MAX_INT_DIG;
	int max_value = 0;
	for (int i = 0; i < profile_values.size(); i++){
		if (profile_values[i] < min_value)
			min_value = profile_values[i];
		if (profile_values[i] > max_value)
			max_value = profile_values[i];
	}

	int i = 0;
	while (i < profile_values.size()){

		if (profile_values[i] <  min_value + 150){			//to low value, not considered as a line
			i++;
			continue;
		}


		int middle_of_line_value = 0;
		int counter = 0;
		while (i < profile_values.size() && counter < line_height){
			if (profile_values[i] > middle_of_line_value){
				middle_of_line_value = profile_values[i];
			}
			else if (profile_values[i] < min_value + 150){
				break;
			}
			counter++;
			i++;
		}

		if (counter < line_height / 4){			//this can't be a line
			continue;
		}

		// take the middle of the line
		int middle_of_line_index = i - (counter / 2);

		Point2i start;
		double x_start = 0;
		double y_start = middle_of_line_index;
		start.x = invertMat.at<double>(0, 0)*x_start + invertMat.at<double>(0, 1)*y_start + invertMat.at<double>(0, 2);
		start.y = invertMat.at<double>(1, 0)*x_start + invertMat.at<double>(1, 1)*y_start + invertMat.at<double>(1, 2);

		Point2i end;
		double x_end = cols;
		double y_end = middle_of_line_index;
		end.x = invertMat.at<double>(0, 0)*x_end + invertMat.at<double>(0, 1)*y_end + invertMat.at<double>(0, 2);
		end.y = invertMat.at<double>(1, 0)*x_end + invertMat.at<double>(1, 1)*y_end + invertMat.at<double>(1, 2);

		if (start.x < end.x){
			fixCoordinatesTopPoint(start, angle);
			fixCoordinatesBottomPoint(end, angle, rows, cols);
			std::vector<Point2i> line;
			line.insert(line.end(), start);
			line.insert(line.end(), end);
			lines.insert(lines.end(), line);
		}
		else {
			fixCoordinatesTopReversedPoint(start, angle, rows, cols);
			fixCoordinatesBottomReversedPoint(end, angle, rows, cols);
			std::vector<Point2i> line;
			line.insert(line.end(), end);
			line.insert(line.end(), start);
			lines.insert(lines.end(), line);
		}
	}

	return lines;
}

void TextLineExtractorGrid::fixCoordinatesTopPoint(Point2i &point, int angle)
{
	if (point.y < 0 && point.x > 0) {
		double a = -point.y;
		double b = (a / tan(angle * PI / 180.0));
		point.y = 0;
		point.x = point.x + b;
	}
	else if (point.y < 0 && point.x < 0){
		double b = -point.x;
		double a = (b * tan(angle * PI / 180.0));
		if (a > -point.y){
			point.x = 0;
			point.y = point.y + a;
		}
		else {
			point.x = (-point.y / tan(angle * PI / 180.0));
			point.y = 0;
		}
	}
	else if (point.y > 0 && point.x < 0){
		double b = -point.x;
		double a = (b * tan(angle * PI / 180.0));
		point.x = 0;
		point.y = point.y + a;
	}
	else if (point.y > 0 && point.x > 0){
		double b = point.x;
		double a = (b * tan(angle * PI / 180.0));
		if (a > point.y){
			point.x = point.x - (point.y / tan(angle * PI / 180.0));
			point.y = 0;
		}
		else {
			point.y = point.y - (point.x * tan(angle * PI / 180.0));
			point.x = 0;
		}
	}
}

void TextLineExtractorGrid::fixCoordinatesBottomPoint(Point2i &point, int angle, int rows, int cols)
{
	if (point.y > rows && point.x < cols) {
		double a = point.y - rows;
		double b = (a / tan(angle * PI / 180.0));
		point.x = point.x - b;
		point.y = rows;
	}
	else if (point.y > rows && point.x > cols){
		double b = rows - point.x;
		double a = (b * tan(angle * PI / 180.0));
		if ((point.y - a) > rows) {
			point.x = point.x - ((point.y - rows) / tan(angle * PI / 180.0));
			point.y = rows;
		}
		else {
			point.y = point.y - ((point.x - cols) * tan(angle * PI / 180.0));
			point.x = cols;
		}
	}
	else if (point.y < rows && point.x < cols){
		double b = cols - point.x;
		double a = b * tan(angle * PI / 180.0);
		if ((point.y + a) > rows) {
			point.x = point.x + ((rows - point.y) / tan(angle * PI / 180.0));
			point.y = rows;
		}
		else {
			point.y = point.y + ((cols - point.x) * tan(angle * PI / 180.0));
			point.x = cols;
		}
	}
	else if (point.y < rows && point.x > cols){
		double a = point.x - cols;
		double b = a * tan(angle * PI / 180.0);
		point.x = cols;
		point.y = point.y - b;
	}
}

void TextLineExtractorGrid::fixCoordinatesTopReversedPoint(Point2i &point, int angle, int rows, int cols)
{
	angle = 180 - angle;

	if (point.x < cols && point.y < 0){
		double a = -point.y;
		double b = a / tan(angle * PI / 180.0);
		point.y = 0;
		point.x = point.x - b;
	}
	else if (point.x > cols && point.y < 0){
		double a = -point.y;
		double b = a / tan(angle * PI / 180.0);
		if (b >(point.x - cols)){
			point.x = point.x - b;
			point.y = 0;
		}
		else{
			point.y = point.y + ((point.x - cols) * tan(angle * PI * 180.0));
			point.x = cols;
		}
	}
	else if (point.x > cols && point.y < rows){
		double a = point.x - cols;
		double b = a * tan(angle * PI / 180.0);
		point.x = cols;
		point.y = point.y + b;
	}
	else if (point.x < cols && point.y < rows){
		double a = cols - point.x;
		double b = a * tan(angle * PI / 180.0);
		if ((point.y - b) > 0){
			point.y = point.y - b;
			point.x = cols;
		}
		else {
			point.x = point.x + (point.y / tan(angle * PI / 180.0));
			point.y = 0;
		}
	}
}

void TextLineExtractorGrid::fixCoordinatesBottomReversedPoint(Point2i &point, int angle, int rows, int cols)
{
	angle = 180 - angle;

	if (point.x < 0 && point.y < rows){
		double a = -point.x;
		double b = a * tan(angle * PI / 180.0);
		point.x = 0;
		point.y = point.y - b;
	}
	else if (point.x > 0 && point.y > rows){
		double a = point.y - rows;
		double b = a / tan(angle * PI / 180.0);
		point.y = rows;
		point.x = point.x + b;
	}
	else if (point.x > 0 && point.y < rows){
		double a = rows - point.y;
		double b = a / tan(angle * PI / 180.0);
		if ((point.x - b) > 0){
			point.y = rows;
			point.x = point.x - b;
		}
		else {
			point.y = point.y + (point.x * tan(angle * PI / 180.0));
			point.x = 0;
		}
	}
	else if (point.x < 0 && point.y > rows){
		double a = -point.x;
		double b = a * tan(angle * PI / 180.0);
		if (point.y - b > rows){
			point.x = point.x + ((point.y - rows) / tan(angle * PI / 180.0));
			point.y = rows;
		}
		else {
			point.y = point.y - (-point.x * tan(angle * PI / 180.0));
			point.x = 0;
		}
	}
}