/***********************************Project 4: Calibration and Augmented Reality**********************************
 Created by Samruddhi Raut.
  
    File Name: Extension.cpp
	This file contain self implemented Haris corner detection and ShiTomashi detector

    Instructions to run the file:
    make
    ./Project_4 
    

*/


#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>


using namespace cv;
using namespace std;


Mat Image;
Mat Image_gray;
Mat Harris_dst;
Mat Harris_copy;
Mat Mc;
Mat ShiTomasi_dst;
Mat ShiTomasi_copy;

int ShiTomasi_qualityLevel = 50;
int Harris_qualityLevel = 50;
int max_qualityLevel = 100;

double Harris_minVal, Harris_maxVal;
double ShiTomasi_minVal, ShiTomasi_maxVal;

RNG rng(12345);

const char* Harris_corner_detectore = " Harris corner detector";
const char* ShiTomasi_corner_detectore = " Shi Tomasi corner detector";
void ShiTomasi_function( int, void* );
void Harris_function( int, void* );


int main( int argc, char** argv ){

    // VideoCapture* capdev;

	// // open the video device
	// capdev = new VideoCapture(0);
	// if (!capdev->isOpened()) {
	// 	printf("Unable to open video device\n");
	// 	return (-1);
	// }

	// // get some properties of the image
	// Size refS((int)capdev->get(CAP_PROP_FRAME_WIDTH),
	//               (int)capdev->get(CAP_PROP_FRAME_HEIGHT));

	// namedWindow("Video", 1); // identifies a window
	// // namedWindow("Video", 0); // identifies a mobile camera window 
	// Mat frame;
	// Mat gray;

	// while (true) {
	// 	*capdev >> frame; // get a new frame from the camera, treat as a stream
	// 	if (frame.empty()) {
	// 		printf("frame is empty\n");
	// 		break;
	// 	}

    Image = imread("/home/samruddhi/CV/Project_4/building.jpg");
    if ( Image.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        cout << "Usage: " << argv[0] << " <Input image>" << endl;
        return -1;
    }
    cvtColor(Image, Image_gray, COLOR_BGR2GRAY );//convert IMage to gray scale
    int blockSize = 3, apertureSize = 3;

    // to find the eigenvalues and eigenvectors to determine if a pixel is a corner.
    cornerEigenValsAndVecs( Image_gray, Harris_dst, blockSize, apertureSize );

    /* calculate Mc */
    Mc = Mat( Image_gray.size(), CV_32FC1 );
    for( int i = 0; i < Image_gray.rows; i++ )
    {
        for( int j = 0; j < Image_gray.cols; j++ )
        {
            float lambda_1 = Harris_dst.at<Vec6f>(i, j)[0];
            float lambda_2 = Harris_dst.at<Vec6f>(i, j)[1];
            Mc.at<float>(i, j) = lambda_1*lambda_2 - 0.04f*((lambda_1 + lambda_2) * (lambda_1 + lambda_2));
        }
    }
    minMaxLoc( Mc, &Harris_minVal, &Harris_maxVal );

    /* Create Window and Trackbar */
    namedWindow( Harris_corner_detectore );
    createTrackbar( "Quality Level:", Harris_corner_detectore, &Harris_qualityLevel, max_qualityLevel, Harris_function );
    Harris_function( 0, 0 );

    //to find the minimum eigenvalues for corner detection.
    cornerMinEigenVal( Image_gray, ShiTomasi_dst, blockSize, apertureSize );

    // minmaxLoc()  is one of the  builtin function provided OpenCV to find the global minimum and maximum in an array.
    minMaxLoc( ShiTomasi_dst, &ShiTomasi_minVal, &ShiTomasi_maxVal );

    /* Create Window and Trackbar */
    namedWindow( ShiTomasi_corner_detectore );
    createTrackbar( "Quality Level:", ShiTomasi_corner_detectore, &ShiTomasi_qualityLevel, max_qualityLevel, ShiTomasi_function );
    ShiTomasi_function( 0, 0 );
    waitKey();
    return 0;
}


//ShiTomasi- corners can be detected by looking for significant change in all direction

void ShiTomasi_function( int, void* )
{
    ShiTomasi_copy = Image.clone();
    ShiTomasi_qualityLevel = MAX(ShiTomasi_qualityLevel, 1);
    for( int i = 0; i < Image_gray.rows; i++ )
    {
        for( int j = 0; j < Image_gray.cols; j++ )
        {
            if( ShiTomasi_dst.at<float>(i,j) > ShiTomasi_minVal + ( ShiTomasi_maxVal - ShiTomasi_minVal )*ShiTomasi_qualityLevel/max_qualityLevel )
            {
                circle( ShiTomasi_copy, Point(j,i), 4, Scalar( rng.uniform(0,256), rng.uniform(0,256), rng.uniform(0,256) ), FILLED );
            }
        }
    }
    imshow( ShiTomasi_corner_detectore, ShiTomasi_copy );
}

/*Harris Corner Detector is a corner detection operator that is
 commonly used in computer vision algorithms to extract corners and infer features of an image.
*/


void Harris_function( int, void* )
{
    Harris_copy = Image.clone();
    Harris_qualityLevel = MAX(Harris_qualityLevel, 1);
    for( int i = 0; i < Image_gray.rows; i++ )
    {
        for( int j = 0; j < Image_gray.cols; j++ )
        {
            if( Mc.at<float>(i,j) > Harris_minVal + ( Harris_maxVal - Harris_minVal )*Harris_qualityLevel/max_qualityLevel )
            {
                circle( Harris_copy, Point(j,i), 4, Scalar( rng.uniform(0,256), rng.uniform(0,256), rng.uniform(0,256) ), FILLED );
            }
        }
    }
    imshow( Harris_corner_detectore, Harris_copy );
}