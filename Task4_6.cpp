/***********************************Project 4: Calibration and Augmented Reality**********************************
 Created by Samruddhi Raut.
  
    File Name: Task4_6.cpp
	This file Draw 3d Objects on a checkboard.

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


/*
Task4: Calculate Current Position of the Camera
Task5: Project Outside Corners or 3D Axes
Task6: Create a Virtual Object          */


using namespace cv;
using namespace std;

Mat cam_matrix = Mat::zeros(3,3, CV_32FC1);
vector<float> distortLine;

//Read file and save camera matrix and distortion coeff in a vector
void readFile(string filename){
    string line;
	ifstream nameFileout;
    Mat cam_matrix = Mat::zeros(3,3, CV_32FC1);
    vector<float> distort_line;
    

	nameFileout.open(filename);
	if (!nameFileout){
		std::cout << "unable to open\n";
		exit(1);
	}

    bool loopMat = true;
    int i = 0;
    int j = 0;
	while (getline(nameFileout, line))
	{
		stringstream string_strm(line);        
        float value;
       
        string_strm.ignore(); 
        
        while ((string_strm >> value)){
            if (loopMat){ 
                cam_matrix.at<float>(j,i) = value;
                if (string_strm.peek() == ',' || string_strm.peek() == ' '){
                    string_strm.ignore();
                    i++;
                }
                else if (string_strm.peek() == ';'){ 
                    string_strm.ignore();
                    j++;
                    i=0;
                }
                else if (string_strm.peek() == ']'){ 
                    loopMat = false;
                }
            }
            
            else{ 
                distort_line.push_back(value);
                if (string_strm.peek() == ',' || string_strm.peek() == ' ')
                    string_strm.ignore();
            }

        };

        cout << cam_matrix << "\n";
        cout << "distortion:\n";
        for (i=0;i<distort_line.size();i++){
            cout << distort_line[i] << ", ";
        }
        cout << "\n";

	    
	}
	nameFileout.close();

}
//Task6: Create a Virtual Object 

//Drawing a 3D pyramid
Mat drawPyramid(Mat Image, float x, float y, float z){
	Mat grayImg;
// initialize point set and corner set
	vector<Point2f> corner_set_pyramid;/*Point2f/Point3f-Represents the two/three coordinates of a point in two/three dimensional space, 
	 									using Single-precision floating point numbers.*/
	vector<Point3f> point_set_pyramid;

    Size patternSize = Size(9,6);//Define the size of pattern on checkboard
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			point_set_pyramid.push_back(Point3f(x,y,0));
		}
	}

	cvtColor(Image, grayImg, COLOR_BGR2GRAY);//converting Img to Gray scale

	//find the corners of check board
	bool patternFound = findChessboardCorners(grayImg, patternSize, corner_set_pyramid);

	if (patternFound){
		cornerSubPix(grayImg,corner_set_pyramid,Size(11,11),Size(-1,-1),//find the sub-pixel
				TermCriteria(TermCriteria::EPS+ TermCriteria::MAX_ITER, 30, 0.1));
	}

	Mat tvec;//translataion vec
	Mat rvec;//Rotaion vec
// initializing below parameters
	vector<Point2f> outsidecorner_set_pyramid;
	vector<Point> points;
	vector<Point2f> dst_corner_set_pyramid;
	vector<Point2f> dst_extra_pyramid;
	vector<Point2f> dst_Axis_pyramid;

//Defineing points to draw the stucture of pyramid
	vector<Point3f> externalcorner_set_pyramid{
	Point3f(-1,1,0), Point3f(-1,-6,0), Point3f(9,-6,0), Point3f(9,1,0)
	};
	vector<Point3f> ref_axis_pyramid{
	Point3f(0,0,0), Point3f(2,0,0), Point3f(0,2,0), Point3f(0,0,2)
	};
	vector<Point3f> pyramid_points{
	Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0), Point3f(x+6,y-4.5,z+7)
	};
	vector<Point3f> extra_points_pyramid{
	Point3f(x+6,y-4.5,z+7), Point3f(x+6,y-4.5,z+0), Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0)
	};
	
	if (corner_set_pyramid.size() == point_set_pyramid.size() && patternFound){

/*solvePnP: This function returns the rotation and the translation vectors that transform a 
3D point expressed in the object coordinate frame to the camera coordinate frame*/

		solvePnP(point_set_pyramid, corner_set_pyramid, ::cam_matrix , ::distortLine, rvec, tvec);
		std::cout << "\ntvec\n" << tvec << "\nrvec\n" << rvec << "\n";
//Projects 3D points to an image plane. 
		projectPoints(externalcorner_set_pyramid, rvec, tvec, ::cam_matrix , ::distortLine, outsidecorner_set_pyramid);
		
//drawing circle at each external corner
		for (int i=0; i<outsidecorner_set_pyramid.size(); i++){
			circle(Image, outsidecorner_set_pyramid.at(i), 5, (0,0,0));
			points.push_back(outsidecorner_set_pyramid.at(i));
		}

//drawing a black background rectangle so that it doesn't detect the pattern in the next recursion
//The function cv::fillConvexPoly draws a filled convex polygon.
		fillConvexPoly( Image, points, Scalar({ 255, 255, 255 }));

//recursion:a recursive function is a particular function that calls itself repeatedly until a certain condition is met.
		drawPyramid(Image, x, y, z);
		
//projecting the points on coordinate
		projectPoints(extra_points_pyramid, rvec, tvec, ::cam_matrix , ::distortLine, dst_extra_pyramid);   
		projectPoints(ref_axis_pyramid, rvec, tvec, ::cam_matrix , ::distortLine, dst_Axis_pyramid);           
		projectPoints(pyramid_points, rvec, tvec, ::cam_matrix , ::distortLine, dst_corner_set_pyramid);

		line(Image, dst_Axis_pyramid.at(0), dst_Axis_pyramid.at(1), Scalar({48,213,200}), 4);
		line(Image, dst_Axis_pyramid.at(0), dst_Axis_pyramid.at(2), Scalar({213,48,200}), 4);
		line(Image, dst_Axis_pyramid.at(0), dst_Axis_pyramid.at(3), Scalar({213,200,48}), 4);

//drawing all the relevent points of pyramid
		for (int i=0; i<dst_corner_set_pyramid.size()-1; i++){
			line(Image, dst_corner_set_pyramid.at(i), dst_corner_set_pyramid.at(i+1), Scalar({47,79,79}), 4);
		}
		line(Image, dst_corner_set_pyramid.at(0), dst_corner_set_pyramid.at(3), Scalar({47,79,79}), 4);

		for (int i=2;i<6;i++){
			line(Image,dst_extra_pyramid.at(1), dst_extra_pyramid.at(i), Scalar({0,255,255}), 4);
		}
		line(Image, dst_extra_pyramid.at(0), dst_extra_pyramid.at(1), Scalar({0,255,255}), 4);

		
		for (int i=0; i<dst_corner_set_pyramid.size()-1; i++){
			line(Image, dst_corner_set_pyramid.at(i), dst_corner_set_pyramid.at(4), Scalar({47,79,79}), 4);
		}
	}
	return Image;
}

// Drawing a cube 
Mat drawCube(Mat Image, float x, float y, float z){
	Mat grayImg;
	// initialize point set and corner set
	/*Point2f/Point3f-Represents the two/three coordinates of a point in two/three dimensional space, 
	using Single-precision floating point numbers.*/
	vector<Point2f> corners;
	vector<vector<Point2f>> corner_list;
	vector<Point3f> point_set;
	vector<vector<Point3f>> point_list; 

	
    Size patternSize = Size(9,6);
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			point_set.push_back(Point3f(x,y,0));
		}
	}

	cvtColor(Image, grayImg, COLOR_BGR2GRAY);

//Drawing the chessboard corner
	bool patternFound = findChessboardCorners(grayImg, patternSize, corners);

	if (patternFound){
		cornerSubPix(grayImg,corners,Size(11,11),Size(-1,-1),
				TermCriteria(TermCriteria::EPS+ TermCriteria::MAX_ITER, 30, 0.1));
	}

	Mat tvec; // Translation vec
	Mat rvec;// rotation Vec
	vector<Point2f> outsideCorners;
	vector<Point> points;
	vector<Point2f> dst_Corners;
	vector<Point2f> dst_Extra;
	vector<Point2f> dst_Axis;
	vector<Point2f> dst_External;
	
// Defining points to draw structure of Cube
	vector<Point3f> externalCorners{
	Point3f(-1,1,0), Point3f(-1,-6,0), Point3f(9,-6,0), Point3f(9,1,0)
	};
	vector<Point3f> refaxis{
	Point3f(0,0,0), Point3f(2,0,0), Point3f(0,2,0), Point3f(0,0,2)
	};
	vector<Point3f> cubePoints{
	Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0)
	};
	vector<Point3f> extraPoints{
	Point3f(x+4,y-3,z+4), Point3f(x+4,y-6,z+4), Point3f(x+8,y-6,z+4), Point3f(x+8,y-3,z+4)
	};

	if (corners.size() == point_set.size() && patternFound){
/*solvePnP: This function returns the rotation and the translation vectors that transform a 
3D point expressed in the object coordinate frame to the camera coordinate frame*/

		solvePnP(point_set, corners, ::cam_matrix , ::distortLine, rvec, tvec);
		std::cout << "\ntvec\n" << tvec << "\nrvec\n" << rvec << "\n";
//Projects 3D points to an image plane. 
		projectPoints(externalCorners, rvec, tvec, ::cam_matrix , ::distortLine, outsideCorners);
		
//drawing circles at external corner
		for (int i=0; i<outsideCorners.size(); i++){
			circle(Image, outsideCorners.at(i), 5, (0,0,0));
			points.push_back(outsideCorners.at(i));
		}
//drawing a black background rectangle so that it doesn't detect the pattern in the next recursion
//The function cv::fillConvexPoly draws a filled convex polygon.
		fillConvexPoly( Image, points, Scalar({ 0, 0, 0 }));
		
//recursion:a recursive function is a particular function that calls itself repeatedly until a certain condition is met.	
		drawCube(Image, x, y, z);
		
//project points to image coordinates so that we can draw them and they'll make sense
		projectPoints(externalCorners, rvec, tvec, ::cam_matrix , ::distortLine, dst_External);
		projectPoints(refaxis, rvec, tvec, ::cam_matrix , ::distortLine, dst_Axis);  
		projectPoints(extraPoints, rvec, tvec, ::cam_matrix , ::distortLine, dst_Extra);            
		projectPoints(cubePoints, rvec, tvec, ::cam_matrix , ::distortLine, dst_Corners);

//drawing  3d axes
		line(Image, dst_Axis.at(0), dst_Axis.at(1), Scalar({255,0,0}), 4);
		line(Image, dst_Axis.at(0), dst_Axis.at(2), Scalar({0,255,0}), 4);
		line(Image, dst_Axis.at(0), dst_Axis.at(3), Scalar({0,0,255}), 4);
		
//drawing all the relevent points of pyramid

		for (int i=0; i<dst_Corners.size()-1; i++){
			line(Image, dst_Corners.at(i), dst_Corners.at(i+1), Scalar({48,213,200}), 4);
		}
		line(Image, dst_Corners.at(0), dst_Corners.at(3), Scalar({48,213,200}), 4);

		for (int i=0;i<dst_Extra.size() - 1;i++){
			line(Image, dst_Extra.at(i), dst_Extra.at(i+1), Scalar({48,213,200}), 4);
		}
		line(Image, dst_Extra.at(0), dst_Extra.at(3), Scalar({48,213,200}), 4);

		
		for (int i=0; i<dst_Corners.size(); i++){
			line(Image, dst_Corners.at(i), dst_Extra.at(i), Scalar({48,213,200}), 4);
		}

	}
	return Image;
}

// Executing the above functions to plot the 3d objects
int main(int argc, char *argv[]) {
//Reading the cameraM_distortionC.txt file
    readFile("cameraM_distortionC.txt");
    std::cout << "cam_matrix \n" << ::cam_matrix  << "\n";
            
    cv::VideoCapture *capdev;
	char Samruddhi[256];
	int quit = 0;
	int Imageid = 0;
	char buffer[400];
	std::vector<int> pars;

	pars.push_back(5);

	if( argc < 2 ) {
	    printf("Usage: %s <Samruddhi>\n", argv[0]);
	    exit(-1);
	}

	// open the video device
	capdev = new cv::VideoCapture(0);
	// capdev = new VideoCapture(1); //uncoment to use mobile camera
	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	strcpy(Samruddhi, argv[1]);

	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	printf("Expected size: %d %d\n", refS.width, refS.height);

    cv::Mat Image;
	
//Initilizing the variables 
	int x = 0, y = 0, z = 0;
            
	for(;!quit;) {
		*capdev >> Image; 

		if( Image.empty() ) {
		  printf("Image is empty\n");
		  break;
		}
		
		// Image = drawPyramid(Image, float(x), float(y), float(z));
		// Mat Image1;
		Image = drawCube(Image, float(x), float(y), float(z));
		// imshow("Video 1", Image);
		
		imshow("Video 2", Image);

		int key = cv::waitKey(10);
		std::cout << x <<"\n"<< y << "\n";
		
		switch(key) {
			case 'q':
				quit = 1;
				break;
			
//For the movement of cube/pyramid around the check board
			case '2':
			{
				y++;
				break;
			}
			case '3':
			{
				y--;
				break;
			}
			case '4':
			{
				x--;
				break;
			}
			case '5':
			{
				x++;
				break;
			}
			case '6':
			{				
				z--;
				break;
			}
			case '7':
			{
				z++;
				break;
			}		
			
		}

	}
	printf("Terminating\n");// teminating the Video Capture.
	delete capdev;

	return(0);
}
