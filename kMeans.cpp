#include "opencv2/opencv.hpp"
#include "time.h"
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
Mat kMeans(Mat src, String * primary, String * second);
String findColor(int H, int S, int V);

Mat kMeans(Mat src, String * primary, String * second)
{
	String colors;
	Mat samples(src.rows * src.cols, 3, CV_32F);
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x*src.rows, z) = src.at<Vec3b>(y, x)[z];

	int clusterCount = 2;
	cv::Scalar cluster_values1[2];
	cv::Scalar cluster_values2[2];
	int cc1 = 0;
	int cc2 = 0;
	int cc3 = 0;
	int clusterNum = 4;
	Mat labels;
	int attempts = 5;
	Mat centers;
	double compactness = kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	Mat new_image(src.size(), src.type());

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*src.rows, 0);
			if (cluster_idx == 0)
				cc1++;
			else if (cluster_idx == 1)
				cc2++;
			cluster_values1[cluster_idx] = Scalar(centers.at<float>(cluster_idx, 0) - 1.0, centers.at<float>(cluster_idx, 1) - 1.0, centers.at<float>(cluster_idx, 2) - 1.0);
			cluster_values2[cluster_idx] = Scalar(centers.at<float>(cluster_idx, 0) + 1.0, centers.at<float>(cluster_idx, 1) + 1.0, centers.at<float>(cluster_idx, 2) + 1.0);
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
		}
	Mat RGB = new_image(Rect(0, 0, 1, 4));
	RGB.at<Vec3b>(0, 0)[0] = centers.at<float>(0, 0);
	RGB.at<Vec3b>(0, 0)[1] = centers.at<float>(0, 1);
	RGB.at<Vec3b>(0, 0)[2] = centers.at<float>(0, 2);
	RGB.at<Vec3b>(1, 0)[0] = centers.at<float>(1, 0);
	RGB.at<Vec3b>(1, 0)[1] = centers.at<float>(1, 1);
	RGB.at<Vec3b>(1, 0)[2] = centers.at<float>(1, 2);

	Mat HSV;
	cvtColor(RGB, HSV, CV_BGR2HSV);
	int p[3];
	int s[3];
	String s_color;
	String p_color;
	int i;
	printf("Compactness:%f\n", compactness);
	if (cc1 > cc2)
	{
		for (i = 0; i < 3; i++)
			p[i] = HSV.at<Vec3b>(0, 0)[i];
		for (i = 0; i < 3; i++)
			s[i] = HSV.at<Vec3b>(1, 0)[i];
		//printf("Primary Color: %i,%i,%i\n", HSV.at<Vec3b>(0, 0)[0], HSV.at<Vec3b>(0, 0)[1], HSV.at<Vec3b>(0, 0)[2]);
		//printf("Secondary Color: %i,%i,%i\n", HSV.at<Vec3b>(1, 0)[0], HSV.at<Vec3b>(1, 0)[1], HSV.at<Vec3b>(1, 0)[2]);
	}
	else
	{
		for (i = 0; i < 3; i++)
			s[i] = HSV.at<Vec3b>(0, 0)[i];
		for (i = 0; i < 3; i++)
			p[i] = HSV.at<Vec3b>(1, 0)[i];
		//printf("Primary Color: %i,%i,%i\n", HSV.at<Vec3b>(1, 0)[0], HSV.at<Vec3b>(1, 0)[1], HSV.at<Vec3b>(1, 0)[2]);
		//printf("Secondary Color: %i,%i,%i\n", HSV.at<Vec3b>(0, 0)[0], HSV.at<Vec3b>(0, 0)[1], HSV.at<Vec3b>(0, 0)[2]);
	}
	*(primary) = findColor(2*p[0], p[1], p[2]);
	*(second) = findColor(2*s[0], s[1], s[2]);
	if ((abs(p[0] - s[0]) + abs(p[1] - s[1]) + abs(p[2] - s[2])) < 20) {
		*(primary) = "";
		*(second) = "";
	}
	//cout << *(primary) << "\n";
	//cout << *(second) << "\n";
	return new_image;
}

String findColor(int H, int S, int V) {
	String color = "";
	if (V < 100)
		color = "Black";
	else if (S < 25)
		color = "White";
	else
	{
		if (H < 25)
			color = "Red";
		else if (H < 45)
			color = "Orange";
		else if (H < 75)
			color = "Yellow";
		else if (H < 140)
			color = "Green";
		else if (H < 170)
			color = "Teal";
		else if (H < 262)
			color = "Blue";
		else if (H < 290)
			color = "Purple";
		else if (H < 312)
			color = "Pink";
		else
			color = "Red";
	}
	return color;
}