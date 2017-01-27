#include "process_image.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void process_image(char *image, unsigned long size)
{
	
	// Read image
	Mat rawImage = Mat( 1, size, CV_8UC1, image );
	Mat im = imdecode( rawImage, IMREAD_GRAYSCALE );
	if ( im.data == NULL ) {
		printf("OpenCV: Failed to read in jpg file from memory.");
		return;
	}
	Mat im_color = imdecode( rawImage, IMREAD_COLOR );
	Mat im_inv;
	bitwise_not(im, im_inv);
	
	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 255;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1200;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.3;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.3;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.3;

	params.minDistBetweenBlobs = 100;

	// Storage for blobs
	vector<KeyPoint> keypoints;
	vector<KeyPoint> keypoints_inv;


#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

	// Set up detector with params
	SimpleBlobDetector detector(params);

	// Detect blobs
	detector.detect(im, keypoints);
#else 

	// Set up detector with params
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	// Detect blobs
	detector->detect(im, keypoints);
	detector->detect(im_inv, keypoints_inv);
#endif 

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	Mat im_with_keypoints;
	drawKeypoints(im, keypoints_inv, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	namedWindow("keypoints", WINDOW_NORMAL);
	cv::resizeWindow("keypoints", 1200, 900);
	//Show blobs
	imshow("keypoints", im_with_keypoints);
	cv::Mat imageROI;
	int i;
	/*
	for (i = 0; i < keypoints.size(); i++) {
		imageROI = im_color(cv::Rect(keypoints[i].pt.x - keypoints[i].size / 2, keypoints[i].pt.y - keypoints[i].size / 2, keypoints[i].size, keypoints[i].size));
		imshow("cropped_image" + i, imageROI);
	}
	for (i = 0; i < keypoints_inv.size(); i++) {
		imageROI = im_color(cv::Rect(keypoints_inv[i].pt.x - keypoints_inv[i].size / 2, keypoints_inv[i].pt.y - keypoints_inv[i].size / 2, keypoints_inv[i].size, keypoints_inv[i].size));
		imshow("cropped_image" + (i+keypoints.size()), imageROI);
	}*/
	waitKey(0);
	
	
	// write file to disk
	imwrite("processed.jpg", im);
}