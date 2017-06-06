#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
String getShape(Mat src, int debug);
double angle(int x1, int y1, int x2, int y2, int x3, int y3);

String getShape(Mat src, int debug) {
	//cout << debug;
	Mat dst, cdst, src_gray;
	blur(src, src, Size(3, 3));
	Canny(src, dst, 0, 100, 3);

	cvtColor(dst, cdst, COLOR_GRAY2BGR);
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	Mat im_in = dst;
	Mat im_th;
	Mat im_out;
	// Display images*/

#if 0
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
	}
#else
	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	vector<Vec3f> circles;
	HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, 60, 200, 20, 0, 0);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 6, CV_AA);
	}
#endif
	Size kernalSize(5, 5);
	Mat element = getStructuringElement(MORPH_RECT, kernalSize, Point(1, 1));
	morphologyEx(cdst, cdst, MORPH_CLOSE, element);
	//imshow("Morph" + std::to_string(debug), cdst);
	//imshow("source", src);
	//imshow("detected lines", dst);

	cvtColor(cdst, im_in, CV_BGR2GRAY);
	im_out = dst;
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(im_out, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	String shape = "None";

	//imshow("Morph" + std::to_string(debug), im_out);

	vector<vector<Point>> contours_poly(contours.size());
	RNG rng(12345);
	//printf("%i\n", contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], arcLength(contours[i], true)*.02, true);
		//printf("%i\n", contours_poly[i].size());
	}

	double maxRadius = 0.0;
	double circleArea = 0.0;
	if (circles.size() > 0)
	{
		for (int j = 0; j < circles.size(); j++) {
			int radius = cvRound(circles[j][2]);
			if (radius > maxRadius)
				maxRadius = radius;
		}
		if (maxRadius > 12 && maxRadius < 80) {
			circleArea = maxRadius * maxRadius * 3.14159265;
			//cout << "Circle Area:" << circleArea << " ";
		}
	}

	Mat drawing = Mat::zeros(im_out.size(), CV_8UC3);
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	for (int i = 0; i < contours.size(); i++)
	{
		int numSides = contours_poly[i].size();
		double area = contourArea(contours_poly[i]);
		double arclength = arcLength(contours_poly[i], true);
		double circ = 12.57 * area / arclength / arclength;
		if (area > 600 && area < 20000)
		{
			drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			//imshow("Contours" + std::to_string(debug) + " " + std::to_string(i), drawing);
			double percentArea;
			//printf("Area:%f ArcLength:%f\n", area, arclength);
			//printf("Circularity: %f\n", 12.57*area / arclength / arclength);
			if (circleArea > 0.1 && (circ > 0.65))
			{
				percentArea = area / circleArea;
				//cout << "Percent" << percentArea;
				if (percentArea < 0.6 && percentArea > 0.4 && circ > 0.65) {
					shape = "Semicircle";
					return shape;
				}
				else if (percentArea > 0.9 && circ > 0.9) {
					shape = "Circle";
					return shape;
				}
			}
			if ((numSides <= 4) || (numSides >= 5 && numSides <= 8 && circ > .8) || ((numSides == 10) && (circ < 0.6 && circ > 0.3)) || (numSides == 12) && (circ < 0.45 && circ > 0.2)){
				double angleSum1 = 0;
				double angleSum2 = 0;
				for (int j = 0; j < numSides; j++) {
					Point p0, p1, p2;
					p0 = contours_poly[i][(j - 1 + numSides) % numSides];
					p1 = contours_poly[i][j];
					p2 = contours_poly[i][(j + 1 + numSides) % numSides];
					double a = angle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
					if (j % 2 == 0)
						angleSum1 = angleSum1 + a;
					else
						angleSum2 = angleSum2 + a;
				}
				if (numSides == 3)
					shape = "Triangle";
				if (numSides == 4)
				{
					double angles[4];
					double lengths[4];
					for (int j = 0; j < numSides; j++) {
						Point p0, p1, p2;
						p0 = contours_poly[i][(j - 1 + numSides) % numSides];
						p1 = contours_poly[i][j];
						p2 = contours_poly[i][(j + 1 + numSides) % numSides];
						angles[j] = angle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
						lengths[j] = pow(pow(p0.x-p1.x,2)+pow(p0.y-p1.y,2),0.5);
						cout << angles[j] << " " << lengths[j] << "\n";
					}
					if (angles[0] < 100.0 && angles[0] > 80.0 && angles[1] < 100.0 && angles[1] > 80.0 && angles[2] < 100.0 && angles[2] > 80.0 && angles[3] < 100.0 && angles[3] > 80.0)
					{
						if (abs(lengths[0] - lengths[1]) < lengths[0] * 0.1)
							shape = "Square";
						else
							shape = "Rectangle";
					}
					else if (((angles[1] - angles[3]) < 0.2*angles[1] && (angles[0] - angles[2] < 0.2*angles[0])))
						shape = "Parallelogram";
					else if ((abs(angles[0] + angles[1] - 180) < 18.0 && abs(angles[2] + angles[3] - 180) < 18.0) || (abs(angles[1] + angles[2] - 180) < 18.0 && abs(angles[0] + angles[3] - 180) < 18.0))
						shape = "Trapezoid";
					else
						shape = "Quadrilateral";
				}
				if (numSides == 5)
					shape = "Pentagon";
				if (numSides == 6 || numSides == 7)
					shape = "Hexagon";
				if (numSides == 8 || numSides == 9)
					shape = "Octagon";
				if ((numSides == 10) && (circ < 0.6 && circ > 0.3))
				{
					double mean1 = angleSum1 / 5.0;
					double mean2 = angleSum2 / 5.0;
					double diff1 = 0;
					double diff2 = 0;
					for (int j = 0; j < numSides && diff1 < mean1*.2; j = j+2) {
						Point p0, p1, p2;
						p0 = contours_poly[i][(j - 1 + numSides) % numSides];
						p1 = contours_poly[i][j];
						p2 = contours_poly[i][(j + 1 + numSides) % numSides];
						double a = angle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
						diff1 = abs(mean1 - a);
					}
					for (int j = 1; j < numSides && diff2 < mean2*.2; j = j + 2) {
						Point p0, p1, p2;
						p0 = contours_poly[i][(j - 1 + numSides) % numSides];
						p1 = contours_poly[i][j];
						p2 = contours_poly[i][(j + 1 + numSides) % numSides];
						double a = angle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
						diff2 = abs(mean2 - a);
					}
					if (diff1 < mean1 * .2 && diff2 < mean2 * .2)
						shape = "Star";
					/*else
						shape = "???";*/
				}
				if ((numSides == 12) && (circ < 0.45 && circ > 0.2))
				{
					double mean = 90.0;
					double diff = 0;
					for (int j = 0; j < numSides && diff < 10.0; j++) {
						Point p0, p1, p2;
						p0 = contours_poly[i][(j - 1 + numSides) % numSides];
						p1 = contours_poly[i][j];
						p2 = contours_poly[i][(j + 1 + numSides) % numSides];
						double a = angle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
						diff = abs(mean - a);
					}
					if (diff < 10.0)
						shape = "Cross";
					/*else
						shape = "???";*/
				}
			}
		}
	}
	return shape;
}

double angle(int x1, int y1, int x2, int y2, int x3, int y3) {
	int xr1 = x1 - x2;
	int yr1 = y1 - y2;
	int xr2 = x3 - x2;
	int yr2 = y3 - y2;
	double theta1 = atan2(yr1, xr1);
	double theta2 = atan2(yr2, xr2);
	/*if (theta1 < 0)
		theta1 = theta1 + 2 * 3.14195265;
	if (theta2 < 0)
		theta2 = theta2 + 2 * 3.14195265;*/
	double theta = abs(theta1-theta2);
	theta = theta * 180.0 / 3.14159265;
	if (theta > 180)
		theta = 360 - theta;
	//printf("%f\n", theta);
	return theta;
}