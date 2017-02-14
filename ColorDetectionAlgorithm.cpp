#include "opencv2/opencv.hpp"
#include "time.h"
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//Timers to track time program takes to run.
	time_t start_time = clock();
	time_t im_read_time, blob_detect_time;

	//Determines scale factor of cropping.
	static double CROP_SCALE = 1.5;
	static double SCALING_FACTOR = 1.0;

	////////////////////////////////////////////////////////////////////PREPARING IMAGES////////////////////////////////////////////////////////////////////

	// Read images in color, grayscale, and inverted grayscale
	Mat im = imread("Images/sample3.jpg", IMREAD_GRAYSCALE);
	Mat im_color = imread("Images/sample3.jpg");
	Mat im_inv;


	//Resize the image for faster processing time.
	Size size((int)(900.0*SCALING_FACTOR), (int)(1200.0*SCALING_FACTOR);
	resize(im, im, size);
	resize(im_color, im_color, size);

	//Set Inverted Greyscale image
	bitwise_not(im, im_inv);
	im_read_time = clock();
	printf("Took %i seconds to prepare images\n", (im_read_time - start_time));


	////////////////////////////////////////////////////////////////////BLOB DETECTION////////////////////////////////////////////////////////////////////

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 255;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = (int)(216.0*SCALING_FACTOR*SCALING_FACTOR);

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
	printf("Detect blob time: %i\n", blob_detect_time - im_read_time);

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	Mat im_with_keypoints;
	drawKeypoints(im_color, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	namedWindow("keypoints", WINDOW_NORMAL);
	cv::resizeWindow("keypoints", 600, 800);
	//Show blobs
	imshow("keypoints", im_with_keypoints);


	////////////////////////////////////////////////////////////////////CROPPING TARGETS////////////////////////////////////////////////////////////////////

	const int numTargets = 50; // Replace with a vector later
	cv::Mat targets[numTargets];
	cv::Mat imageROI;
	int i;

	//Replace with Cropping_Factor constant

	for (i = 0; i < keypoints.size(); i++) {
		imageROI = im_color(cv::Rect(keypoints[i].pt.x - keypoints[i].size * CROP_SCALE / 2, keypoints[i].pt.y - keypoints[i].size * CROP_SCALE / 2, keypoints[i].size * CROP_SCALE, keypoints[i].size * CROP_SCALE));
		//imshow("cropped_image" + i, imageROI);
		targets[i] = imageROI;
	}
	for (i = 0; i < keypoints_inv.size(); i++) {
		imageROI = im_color(cv::Rect(keypoints_inv[i].pt.x - keypoints_inv[i].size * CROP_SCALE / 2, keypoints_inv[i].pt.y - keypoints_inv[i].size * CROP_SCALE / 2, keypoints_inv[i].size * CROP_SCALE, keypoints_inv[i].size * CROP_SCALE));
		//imshow("cropped_image" + (i + keypoints.size()), imageROI);
		targets[i + keypoints.size()] = imageROI;
	}

	for (i = 0; i < keypoints.size() + keypoints_inv.size(); i++)
	{
		Mat samples(targets[i].rows * targets[i].cols, 3, CV_32F);
		for (int y = 0; y < targets[i].rows; y++)
			for (int x = 0; x < targets[i].cols; x++)
				for (int z = 0; z < 3; z++)
					samples.at<float>(y + x*targets[i].rows, z) = targets[i].at<Vec3b>(y, x)[z];

		int clusterCount = 3;
		Mat labels;
		int attempts = 5;
		Mat centers;
		kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
		Mat new_image(targets[i].size(), targets[i].type());
		for (int y = 0; y < targets[i].rows; y++)
			for (int x = 0; x < targets[i].cols; x++)
			{
				int cluster_idx = labels.at<int>(y + x*targets[i].rows, 0);
				new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
				new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
				new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
			}
		imshow("12345678901234567890123456789012345678901234567890"+i, new_image);
	}

	waitKey(0);

}
