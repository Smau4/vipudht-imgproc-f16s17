#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{

	// Read image
	Mat im = imread("Images/test1b.jpg", IMREAD_GRAYSCALE);

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 255;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1000;

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
#endif 

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	Mat im_with_keypoints;
	drawKeypoints(im, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	namedWindow("keypoints", WINDOW_NORMAL);
	cv::resizeWindow("keypoints", 1200, 600);
	// Show blobs
	imshow("keypoints", im_with_keypoints);
	waitKey(0);

}