/*
* starter_video.cpp
*
*  Created on: Nov 23, 2010
*      Author: Ethan Rublee
*
*  Modified on: April 17, 2013
*      Author: Kevin Hughes
*
* A starter sample for using OpenCV VideoCapture with capture devices, video files or image sequences
* easy as CV_PI right?
*/

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <sstream>
#include <vector>
#include "opencv2/core/core.hpp"
#include <algorithm>
#include <unistd.h>

using namespace cv;
using namespace std;

//hide the local functions in an anon namespace
namespace {
	int quantidade_aguardo = 100; //provavelmente vá usar isso depois
	int threshold = 40;
	int contour_length_threshold= 45;
	
	
	int morph_elem = 0;
	int morph_size = 0;
	int morph_operator = 0;
	int const max_operator = 4;
	int const max_elem = 2;
	int const max_kernel_size = 21;
	
	unsigned long long int mostrar;
	
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	
	void CallBackFunc(int event, int x, int y, int flags, void* userdata){
		if  ( event == EVENT_LBUTTONDOWN ){
			cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
			threshold+=5;
			printf("\n\n\n\n\n\n\nmostrar vale: %lld\n\n\n\n\n\n\n", mostrar);
		}else if  ( event == EVENT_RBUTTONDOWN ){
			cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
			if(threshold > 0) threshold--;			
		}else if  ( event == EVENT_MBUTTONDOWN ){
			cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		}else if ( event == EVENT_MOUSEMOVE){
			cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
		}
	}

	
    int process(VideoCapture& capture) {
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow("original", CV_WINDOW_NORMAL);
        namedWindow(window_name, WINDOW_KEEPRATIO); //resizable window;
        namedWindow("substraction", CV_WINDOW_NORMAL);
        namedWindow("canny", WINDOW_KEEPRATIO); //resizable window;
        Mat frame, frame_gray, fundo_gray, substraction, resultado_canny, frame_original;
        CvArr* arrayx;
		setMouseCallback(window_name, CallBackFunc, NULL);
		
		//carregar o fundo
		fundo_gray = imread("./fundo.png", CV_BGR2GRAY);
		int pi = 0;
		
		pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
		
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		RNG rng(12345);
		
        for (;;) {
			//getchar();
			int largest_area = 0;
			int largest_contour_index = 0;
			Rect bounding_rect;
			
			//mostrar++;
            capture >> frame >> frame_original;
            if (frame.empty())
                break;
            arrayx =  &capture;
			
            

            // Converte a imagem para cinza e aplica uma mascara de media
		    cvtColor(frame, frame_gray, CV_BGR2GRAY);
		    blur(frame_gray, frame_gray, Size(3,3)); //operação gaussiana
		    
		    //fundo_gray = frame_gray;
		    /*if(mostrar==97){
				image2 = frame_gray.clone();
				pi++;
				printf("\ncapturou quadro");
			}else{
				if(pi==0){
					image2 = frame_gray.clone();
				}
			} */
			
		    // Cria uma janela
		    char source_window[7];
		    strcpy(source_window, "Source");

			//faz o background substraction
			pMOG2->apply(frame_gray, substraction);
			
			int operation = morph_operator + 2;			
			Mat element = getStructuringElement(2, Size(5,5), Point(1,1));
						
			morphologyEx(substraction, substraction, MORPH_OPEN, element );
			morphologyEx(substraction, substraction, MORPH_CLOSE, element);			
			element = getStructuringElement(2, Size(20,20), Point(1,1));
			dilate(substraction, substraction, element);						
			//morphologyEx(substraction, substraction, MORPH_TOPHAT, element );
			//dilate(substraction, substraction, 0, Point(0,0), 2, 1, 1);
			
			//fastNlMeansDenoising(substraction, substraction);
			
			//cv::absdiff(frame_gray, fundo_gray, substraction); // Absolute differences between the 2 images
			//cv::threshold(substraction, substraction, 5, 255, CV_THRESH_BINARY); // set threshold to ignore small differences you can also use inrange function
			Canny(substraction, resultado_canny, 20, 5, 3);
			//Canny(frame, frame_gray, 100, 200, 3); // => fica doidão
			
			
			
		
			findContours(resultado_canny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			/// Draw contours
			/*Mat drawing = Mat::zeros(frame_gray.size(), CV_8UC3 );
			for( int i = 0; i< contours.size(); i++ )
			{
				Scalar color = Scalar(0,255,0);
				drawContours(frame, contours, i, color, 2, 8, hierarchy, 0, Point() );
			} */
			
			/*for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); )
			{
				if (it->size()<contour_length_threshold)
					it=contours.erase(it);
				else
					++it;
			} */
			
			// iterate through each contour.
			for( int i = 0; i< contours.size(); i++ )
			{
				//  Find the area of contour
				double a = arcLength(contours[i],true); //contourArea é a área e arcLength é o perímetro
				if(a >= threshold){
					//if(a > largest_area){
						largest_area=a;
						//cout << i << " area  " << a << endl;
						// Store the index of largest contour
						largest_contour_index=i;              
						// Find the bounding rectangle for biggest contour
						bounding_rect = boundingRect(contours[i]);
					//} 
					//Scalar color( 255,255,255);  // color of the contour in the
					//Draw the contour and rectangle
					//drawContours( frame, contours,largest_contour_index, color, CV_FILLED,8,hierarchy);
					rectangle(frame, bounding_rect,  Scalar(255,0,0),2, 8,0);				
				}
			}
			//imshow( "Display window", src );    
			
			
			
			//imshow("", image2);

			//cv::line(substraction, cv::Point(160, 200), cv::Point(472, 220), cv::Scalar(220,0,0), 10, 4); //traçar a linha
			imshow("original", frame_original);
			imshow(window_name, frame);
		    imshow("substraction", substraction);
		    imshow("canny", resultado_canny);


            char key = (char) waitKey(quantidade_aguardo); //delay N millis, usually long enough to display and capture input  -fa

            switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                return 0;
            case ' ': //Save an image
                sprintf(filename,"filename%.3d.jpg",n++);
                imwrite(filename,frame);
                cout << "Saved " << filename << endl;
                break;
            default:
                break;
            }            
        }
        return 0;
    }
}

int main(int ac, char** av) {

    if (ac != 2) {
        return 1;
    }
    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file or image sequence
    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));
    if (!capture.isOpened()) {
        cerr << "Failed to open the video device, video file or image sequence!\n" << endl;
        return 1;
    }
    return process(capture);
}
