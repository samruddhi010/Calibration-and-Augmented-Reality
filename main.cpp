/***********************************Project 4: Calibration and Augmented Reality**********************************
 Created by Samruddhi Raut.
  
    File Name: main.cpp
    This file contain Task 1 to Task3

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



using namespace cv;
using namespace std;

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

        std::cout << cam_matrix << "\n";
        std::cout << "distortion:\n";
        for (i=0;i<distort_line.size();i++){
            std::cout << distort_line[i] << ", ";
        }
        std::cout << "\n";

	    
	}
	nameFileout.close();

}

// To get a Camera matrix, distortion coeff and Error

/*
Task1: Detect and Extract Chessboard Corners
Task2: Select Calibration Images
Task3: Calibrate the Camera

*/
int main(int argc, char *argv[]) {


	Mat Image;
	Mat grayImg;

	VideoCapture *capdev;
	char Samruddhi[256];
	int stop = 0;
	int Imageid = 0;
	char buffer[256];
	vector<int> pars;

	pars.push_back(10);

	if( argc < 2 ) {
	    printf("Usage: %s <Samruddhi>\n", argv[0]);
	    exit(-1);
	}

	// open the video device
	capdev = new VideoCapture(0);
	// capdev = new VideoCapture(1);//uncoment to use mobile camera

	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	strcpy(Samruddhi, argv[1]);

	Size refS( (int) capdev->get(CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(CAP_PROP_FRAME_HEIGHT));

	printf("Expected size: %d %d\n", refS.width, refS.height);

	namedWindow("Video", 1); 

// initialize point set and corner set for camera calibration and AR
	/*Point2f/Point3f-Represents the two/three coordinates of a point in two/three dimensional space, 
	 using Single-precision floating point numbers.
	 */
	vector<Point2f> corner_set;
	vector<vector<Point2f>> corner_list;
	vector<Point3f> point_set;
	vector<vector<Point3f>> point_list; 

    Size patternSize = Size(9,6);
	
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			point_set.push_back(Point3f(x,y,0));
			
		}
	}
		
	for(;!stop;) {
// storing the Images of the video capture device into a mat object and loop over to create a video
		*capdev >> Image; 

		if( Image.empty() ) {
		  printf("Image is empty\n");
		  break;
		}

		cvtColor(Image, grayImg, COLOR_BGR2GRAY);//converting Img to Gray scale img.


//********************************Task1:Detect and Extract Chessboard Corners****************************************

/* The function attempts to determine whether the input image 
is a view of the chessboard pattern and locate the internal chessboard corners.*/
		bool patternFound = findChessboardCorners(grayImg, patternSize, corner_set);

		if (patternFound){
//The function iterates to find the sub-pixel accurate location of corners or radial saddle points
		    cornerSubPix(grayImg,corner_set,Size(11,11),Size(-1,-1),
		            TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		}
		Mat ImageCopy= Image.clone();//clone the img

//Draw corners:The function draws individual chessboard corners detected either as red circles 
// if the board was not found, or as colored corners connected with lines if the board was found. 
		drawChessboardCorners(Image, patternSize, Mat(corner_set), patternFound);
		imshow("Video", Image);
		int key = waitKey(10);
		
		switch(key) {
			case 'q':
				stop = 1;
				break;
				
//Capturing the frame: if user presses 1
			case '1': 
				sprintf(buffer, "../data/%s.%03d.png", Samruddhi, Imageid++);
				imwrite(buffer, ImageCopy, pars);
				printf("frame_capture: %s\n", buffer);
				break;


//*********************************Task 2:Select Calibration Images*****************

// if the user types '2', then store the vector of corners (camera matrix and distortion coeff) 
			case '2':
				{
					if (corner_set.size() != 54)
						break;
// Pushing all the corners and point set into a vector
					corner_list.push_back(corner_set);
					point_list.push_back(point_set);
					sprintf(buffer, "../data/Calibrate.%s.%03d.png", Samruddhi, Imageid++);
					imwrite(buffer, Image, pars);//Writing the calbiration image
					printf("Cal_frame_capture: %s\n", buffer);
					vector<double> distortion_coeff;
					vector<Mat> rvecs; // Rotation Vector
					vector<Mat> tvecs; //Translation Vector

//Input 5 corners to create a matrix
					if (corner_list.size() >= 5){
						float data[3][3] = {{1,0,float(Image.cols)/2},{0,1,float(Image.rows)/2},{0,0,1}};
						Mat camera_matrix = Mat( 3,3 , CV_32FC1, data);// CV_32FC1 defines both the depth of each element and the number of channels.
						Size imageSize = Size(Image.cols, Image.rows);


//********************************Task3:Calibrate the Camera******************************************


/*calibrateCamera function :Finds the camera intrinsic and extrinsic parameters from several views of a calibration pattern. 
 returns:The overall RMS re-projection error.*/						
 						double error = calibrateCamera( point_list, corner_list, imageSize, camera_matrix, distortion_coeff, rvecs, tvecs, CV_CALIB_FIX_ASPECT_RATIO);
						cout << "camera_matrix: " << camera_matrix << "\n" << "distortion_coeff: ";//getting camera matrix and distrortin coeff
						for (int i=0; i<distortion_coeff.size(); i++){
							cout << distortion_coeff.at(i) << ", ";
						}
						cout << "\nerror: " << error << "\n";
					}
					break;
				}

//writing the camera matrix and the distortion coeffecients to the file
			case '3':
				{
				vector<double> distortion_coeff;
				vector<Mat> rvecs;// Rotation Vector
				vector<Mat> tvecs;//Translation Vector				
				if (corner_list.size() >= 5){ //at least 5 corners should be availabel
					float data[3][3] = {{1,0,float(Image.cols)/2},{0,1,float(Image.rows)/2},{0,0,1}};
					Mat camera_matrix = Mat( 3,3 , CV_32FC1, data);
					Size imageSize = Size(Image.cols, Image.rows);
					double error = calibrateCamera( point_list, corner_list, imageSize, camera_matrix, distortion_coeff, rvecs, tvecs, CV_CALIB_FIX_ASPECT_RATIO);
					cout << "camera_matrix: " << camera_matrix << "\n" << "distortion_coeff: ";
					
					for (int i=0; i<distortion_coeff.size(); i++){
						cout << distortion_coeff.at(i) << ", ";
					}
					
					cout << "\nerror: " << error << "\n";
					string filename = "cameraM_distortionC.txt";
					ofstream myfile;
					myfile.open (filename);
					myfile << camera_matrix;//writing camera_matrix to  the file
					myfile << "\n";
					for (int  i=0;i<distortion_coeff.size(); i++){//writing distortion coefficients to the file
						myfile <<  distortion_coeff.at(i);
						if (i!=distortion_coeff.size()-1){
							myfile << ", ";
						}
					}
					myfile << "\n";
					myfile.close();
					
				}  	
				break;
			}
		}
	}	
	printf("Terminating\n");// teminating the Video Capture.
	delete capdev;
	return(0);
}
