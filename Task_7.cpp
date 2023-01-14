/***********************************Project 4: Calibration and Augmented Reality**********************************
 Created by Samruddhi Raut.
  
    File Name: Task7.cpp
	This file Detects the harris corner

    Instructions to run the file:
    make
    ./Project_4 "Samruddhi"
    

*/

#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/calib3d/calib3d_c.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/saturate.hpp>
#include <cstring>
#include <cstdlib>
#include <cmath>


void video() { }
using namespace cv;
using namespace std;
int main() {
	VideoCapture* capdev;

	// open the video device
	capdev = new VideoCapture(0);
	// capdev = new VideoCapture(1);//uncoment to use mobile camera

	if (!capdev->isOpened()) {
		printf("Unable to open video device\n");
		return (-1);
	}

	// get some properties of the image
	Size refS((int)capdev->get(CAP_PROP_FRAME_WIDTH),
	              (int)capdev->get(CAP_PROP_FRAME_HEIGHT));

	namedWindow("Video", 1); // identifies a window
	Mat frame;
	Mat gray;

	while (true) {
		*capdev >> frame; // get a new frame from the camera, treat as a stream
		if (frame.empty()) {
			printf("frame is empty\n");
			break;
		}

		cvtColor(frame, gray, COLOR_BGR2GRAY);

		vector<Point2f> haris_corners;
		// It finds N strongest corners in the image by  Harris Corner Detection
		goodFeaturesToTrack(gray, haris_corners, 100, 0.1, 10);

		for (int idx = 0; idx < haris_corners.size(); idx++) {
			circle(frame, haris_corners.at(idx), 2, Scalar(48, 213, 200), 2);
		}

		cout << haris_corners.size() << endl;


		char key = waitKey(10);

		if (key == 'q') { break; }

		imshow("video", frame);
	}
	return 0;
}

