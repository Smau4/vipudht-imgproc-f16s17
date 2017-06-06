
#include "opencv2/opencv.hpp"
#include "time.h"
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "getShape.cpp"
#include "kMeans.cpp"
#include "imgProc.cpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	time_t start_time;
	time_t im_read_time, blob_detect_time;
	String srcDir, dstDir, photoName;
	int start = 7478;
	int end = 7478;
	int num;
	srcDir = "C:/Users/Smau2/Desktop/Test/May1/";
	dstDir = "C:/Users/Smau2/Desktop/Test/May1/Targets/";
	for (int i = start; i <= end; i++) {
		start_time = clock();
		photoName = "DSC_" + std::to_string(i);
		imgProc(srcDir + photoName, dstDir + photoName);
		cout << "\n";
		cout << "Analyzing photo " << i - start + 1 << "  of " << end - start + 1 << "\n";
		im_read_time = clock();
		printf("Took %i seconds to process images\n", (im_read_time - start_time));
	}
	waitKey(0);
	system("pause");
	return 0;
}