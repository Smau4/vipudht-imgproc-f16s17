#include "opencv2/opencv.hpp"
#include "time.h"
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
void imgProc(String fileLoc, String fileDst);

void imgProc(String fileLoc, String fileDst)
{
	//Timers to track time program takes to run.
	time_t start_time = clock();
	time_t im_read_time, blob_detect_time;

	//Determines how scaled the image for blob detection will be.  Used for efficiency.  Greater scaling factor is faster but lowers image density for blob detection
	static double SCALING_FACTOR = 4.08;

	//Determines scale factor of cropping.
	static double CROP_SCALE = 1.5;

	////////////////////////////////////////////////////////////////////PREPARING IMAGES////////////////////////////////////////////////////////////////////

	// Read images in color, grayscale, and inverted grayscale
	Mat im = imread(fileLoc + ".jpg", IMREAD_GRAYSCALE);
	Mat im_small;
	Mat im_color = imread(fileLoc + ".jpg");
	Mat im_inv;
	int col, row;
	col = im.cols;
	row = im.rows;
	int SIZE_X = (int)((float)im.cols / SCALING_FACTOR);
	int SIZE_Y = (int)((float)im.rows / SCALING_FACTOR);


	//Resize the image temporarily for faster processing time.
	Size size(SIZE_X, SIZE_Y);
	resize(im, im, size);
	resize(im_color, im_small, size);
	
	//Set Inverted Greyscale image
	bitwise_not(im, im_inv);
	im_read_time = clock();
	//printf("Took %i seconds to prepare images\n", (im_read_time - start_time));


	////////////////////////////////////////////////////////////////////BLOB DETECTION////////////////////////////////////////////////////////////////////


	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 255;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = (int)(600 / SCALING_FACTOR / SCALING_FACTOR);
	params.maxArea = (int)(5000 / SCALING_FACTOR / SCALING_FACTOR);

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.3;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.3;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.3;

	params.minDistBetweenBlobs = 10;

	// Storage for blobs
	vector<KeyPoint> keypoints;
	vector<KeyPoint> keypoints_inv;

	// Set up detector with params
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	// Detect blobs
	detector->detect(im, keypoints);
	detector->detect(im_inv, keypoints_inv);

	blob_detect_time = clock();
	/*printf("Detect blob time: %i\n", blob_detect_time - im_read_time);
	printf("Color Rows: %i, Color Col: %i\n", im_small.rows, im_small.cols);
	printf("Color Rows: %i, Color Col: %i\n", im_color.rows, im_color.cols);*/

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	//Mat im_with_keypoints;
	//drawKeypoints(im_small, keypoints, im_with_keypoints, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//namedWindow("keypoints", WINDOW_NORMAL);
	//cv::resizeWindow("keypoints", 600, 900);
	//Show blobs
	//imshow("keypoints", im_with_keypoints);


	////////////////////////////////////////////////////////////////////CROPPING TARGETS////////////////////////////////////////////////////////////////////

	const int numTargets = 500; // Replace with a vector later
	cv::Mat targets[numTargets];
	cv::Mat imageROI;
	cv::Mat smallROI;
	int i;
	double x, y, w, h;
	String shape = "None";
	String primary, secondary;

	for (i = 0; i < keypoints.size(); i++) {
		x = SCALING_FACTOR*(keypoints[i].pt.x - keypoints[i].size * CROP_SCALE / 2);
		y = SCALING_FACTOR*(keypoints[i].pt.y - keypoints[i].size * CROP_SCALE / 2);
		w = SCALING_FACTOR*(keypoints[i].size * CROP_SCALE);
		h = SCALING_FACTOR*(keypoints[i].size * CROP_SCALE);
		if (x <= 0)
			x = 0;
		if (y <= 0)
			y = 0;
		if (x + w >= col)
			w = col - x;
		if (y + h >= row)
			h = row - y;
		//printf("%f,%f,%f,%f\n", x, y, w, h);
		imageROI = im_color(cv::Rect(x, y, w, h));
		targets[i] = imageROI;
		//imshow("target" + std::to_string(i), imageROI);
		shape = getShape(targets[i], i);
		if (shape.compare("None") != 0) {
			cout << fileLoc << " has target with shape " << shape << "\n";
			//cout << "Size:" << keypoints[i].size << "\n";
			x = SCALING_FACTOR*(keypoints[i].pt.x - keypoints[i].size / 4);
			y = SCALING_FACTOR*(keypoints[i].pt.y - keypoints[i].size / 4);
			w = SCALING_FACTOR*(keypoints[i].size / 2);
			h = SCALING_FACTOR*(keypoints[i].size / 2);
			if (x <= 0)
				x = 0;
			if (y <= 0)
				y = 0;
			if (x + w >= col)
				w = col - x;
			if (y + h >= row)
				h = row - y;
			smallROI = im_color(cv::Rect(x, y, w, h));
			kMeans(smallROI, &primary, &secondary);
			//imshow("kmeans" + std::to_string(i), kMeans(smallROI, &primary, &secondary));
			if (primary != "" && secondary != ""){
				printf("Target:%i ", i);
				cout << "Primary Color: " << primary << " Secondary Color: " << secondary << "\n";
				imwrite(fileDst + primary + secondary + shape + std::to_string(i) + ".jpg", imageROI);
			}
		}
	}

	for (i = 0; i < keypoints_inv.size(); i++) {
		x = SCALING_FACTOR*(keypoints_inv[i].pt.x - keypoints_inv[i].size * CROP_SCALE / 2);
		y = SCALING_FACTOR*(keypoints_inv[i].pt.y - keypoints_inv[i].size * CROP_SCALE / 2);
		w = SCALING_FACTOR*(keypoints_inv[i].size * CROP_SCALE);
		h = SCALING_FACTOR*(keypoints_inv[i].size * CROP_SCALE);
		if (x <= 0)
			x = 0;
		if (y <= 0)
			y = 0;
		if (x + w >= col)
			w = col - x;
		if (y + h >= row)
			h = row - y;
		//printf("%f,%f,%f,%f\n", x, y, w, h);
		imageROI = im_color(cv::Rect(x, y, w, h));
		targets[i + keypoints.size()] = imageROI;
		//imshow("target" + std::to_string(i + keypoints.size()), imageROI);
		shape = getShape(targets[i + keypoints.size()], i + keypoints.size());
		if (shape.compare("None") != 0) {
			cout << fileLoc << " has target with shape " << shape << "\n";
			//cout << "Size:" << keypoints_inv[i].size << "\n";
			x = SCALING_FACTOR*(keypoints_inv[i].pt.x - keypoints_inv[i].size / 4);
			y = SCALING_FACTOR*(keypoints_inv[i].pt.y - keypoints_inv[i].size / 4);
			w = SCALING_FACTOR*(keypoints_inv[i].size / 2);
			h = SCALING_FACTOR*(keypoints_inv[i].size / 2);
			if (x <= 0)
				x = 0;
			if (y <= 0)
				y = 0;
			if (x + w >= col)
				w = col - x;
			if (y + h >= row)
				h = row - y;
			smallROI = im_color(cv::Rect(x, y, w, h));
			kMeans(smallROI, &primary, &secondary);
			//imshow("kmeans" + std::to_string(i+keypoints.size()),kMeans(smallROI, &primary, &secondary));
			if (primary != "" && secondary != "") {
				printf("Target:%i ", i);
				cout << "Primary Color: " << primary << " Secondary Color: " << secondary << "\n";
				imwrite(fileDst + primary + secondary + shape + std::to_string(i) + ".jpg", imageROI);
			}
		}
	}
}