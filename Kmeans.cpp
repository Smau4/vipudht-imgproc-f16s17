#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>

using namespace cv;

int main(int argc, char** argv)
{
	Mat src = imread("Images/KMeansTests/test4.JPG", 1);
	Mat samples(src.rows * src.cols, 3, CV_32F);
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			for (int z = 0; z < 3; z++)
				samples.at<float>(y + x*src.rows, z) = src.at<Vec3b>(y, x)[z];


	int clusterCount = 6;
	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	int clusterArray[6+1];
	for (int a = 0; a < clusterCount + 1; a++)
		clusterArray[a] = 0;
	float clusterRGB[6][3];

	Mat new_image(src.size(), src.type());
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*src.rows, 0);
			clusterArray[clusterCount]++;
			clusterArray[cluster_idx]++;
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			clusterRGB[cluster_idx][0]= centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			clusterRGB[cluster_idx][1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
			clusterRGB[cluster_idx][2] = centers.at<float>(cluster_idx, 2);
		}
	imshow("clustered image", new_image);
	for (int i = 0; i < clusterCount; i++) {
		printf("Percentage %f: ", (float)(100*clusterArray[i])/(float)(clusterArray[clusterCount]));
		printf("R: %f, G: %f, B: %f\n", clusterRGB[i][2], clusterRGB[i][1], clusterRGB[i][0]);
	}
	printf("%i", clusterArray[clusterCount]);
	waitKey(0);
}
