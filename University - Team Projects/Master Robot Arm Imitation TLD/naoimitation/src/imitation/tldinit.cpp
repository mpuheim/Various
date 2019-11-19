/**********************************************/
/*                                            */
/*           Program Naoimitation             */
/*                version 1.0                 */
/*                                            */
/*               Michal Puheim                */
/*             Jaroslav Vraštiak              */
/*                                            */
/*              KKUI, FEI, TUKE               */
/*                 2012/2013                  */
/*                                            */
/**********************************************/

#include "tldinit.h"
#include "motion.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
#include <iostream>

CvRect box;
bool drawing_box = false;
bool endRect = false;

void draw_box( IplImage* img, CvRect rect )
{
	cvRectangle( img, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height), cvScalar(0, 255, 0), 2);
}

// mouse callback
void select_box_callback( int event, int x, int y, int flags, void* param )
{
	IplImage* image = (IplImage*) param;

	switch( event )
    {
		case CV_EVENT_MOUSEMOVE: 
			if( drawing_box )
            {
				box.width = x-box.x;
				box.height = y-box.y;
			}
			break;

		case CV_EVENT_LBUTTONDOWN:
			drawing_box = true;
			box = cvRect( x, y, 0, 0 );
			break;

		case CV_EVENT_LBUTTONUP:
			drawing_box = false;
			if( box.width < 0 )
            {
				box.x += box.width;
				box.width *= -1;
			}
			if( box.height < 0 )
            {
				box.y += box.height;
				box.height *= -1;
			}
			draw_box( image, box );
            endRect = true;
			break;
	}
}

cv::Rect select_box(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy)
{
    //print controlls info
    printControls();
    std::cout << "Mouse - drag and select initial bounding box" << std::endl;
    //set window name
	const char* name = "Select bounding box";
    //set initial bb
	box = cvRect(-1,-1,0,0);
    //set images
    IplImage* image = cvCloneImage(&(IplImage)acquisition->getImage());
	IplImage* temp = cvCloneImage( image );
    //display window
	cvNamedWindow( name );
	// Set up the callback
	cvSetMouseCallback( name, select_box_callback, (void*) image);
	// Main loop
    char c;
	while( 1 ){
        if( !drawing_box )
        {
            cvReleaseImage( &image );
            image = cvCloneImage(&(IplImage)acquisition->getImage());
            cvShowImage( name, image );
        }
        else
        {
            cvCopyImage( image, temp ); 
			draw_box( temp, box );
            cvShowImage( name, temp );
        }
        if( endRect == true )
            break;
        c = (char)cvWaitKey(15);
        teleoperate(motionproxy, c);
	}

	cvReleaseImage( &image );
	cvReleaseImage( &temp );
	cvDestroyWindow( name );

	return cv::Rect(box.x,box.y,box.width,box.height);
}

tld::TLD *initTLD_fromBB(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy)
{
    //get image
    cv::Mat frame = acquisition->getImage();
    //convert image to grayscale
    cv::Mat grey(frame.cols, frame.rows, CV_8UC1);
    cv::cvtColor(frame, grey, CV_BGR2GRAY);
    //initialize TLD object
    tld::TLD *tld_object = new tld::TLD();
    //set detector properties
    tld_object->detectorCascade->imgWidth = grey.cols;
    tld_object->detectorCascade->imgHeight = grey.rows;
    tld_object->detectorCascade->imgWidthStep = grey.step;
    //initialize tld
    cv::Rect bb_object = select_box(acquisition, motionproxy);
    tld_object->selectObject(grey, &bb_object);
    //return pointer to initialized TLD
    return tld_object;
}

tld::TLD *initTLD_fromFile(const char *path, ImAcquisition *acquisition)
{
    //get image
    cv::Mat frame = acquisition->getImage();
    //convert image to grayscale
    cv::Mat grey(frame.cols, frame.rows, CV_8UC1);
    cv::cvtColor(frame, grey, CV_BGR2GRAY);
    //initialize TLD object
    tld::TLD *tld_object = new tld::TLD();
    //set detector properties
    tld_object->detectorCascade->imgWidth = grey.cols;
    tld_object->detectorCascade->imgHeight = grey.rows;
    tld_object->detectorCascade->imgWidthStep = grey.step;
    //initialize tld
    tld_object->readFromFile(path);
    //return pointer to initialized TLD
    return tld_object;
}

void runAndTrainTLD(ImAcquisition *acquisition, tld::TLD *detector, AL::ALMotionProxy *motionproxy)
{
    //print controlls info
    printControls();
    std::cout << "Escape - stop training" << std::endl;
    //get image
    cv::Mat frame = acquisition->getImage();
    //set window to display video
    cv::namedWindow("video");
    //set loop to display video from camera
    while(1)
    {
        //get frame
        frame = acquisition->getImage();
        //process image with TLD
        detector->processImage(frame);
        //return current bounding box
        if (detector->currBB)
        {
		    //detector->currBB
		    cv::Point A;
		    A.x = detector->currBB->x;
		    A.y = detector->currBB->y;
		    cv::Point B;
		    B.x = detector->currBB->x + detector->currBB->width;
		    B.y = detector->currBB->y;
		    cv::Point C;
		    C.x = detector->currBB->x + detector->currBB->width;
		    C.y = detector->currBB->y + detector->currBB->height;
		    cv::Point D;
		    D.x = detector->currBB->x;
		    D.y = detector->currBB->y + detector->currBB->height;
		    //draw bb to image
		    cv::line(frame, A, B, cvScalar(0, 255, 0), 2);
		    cv::line(frame, B, C, cvScalar(0, 255, 0), 2);
		    cv::line(frame, C, D, cvScalar(0, 255, 0), 2);
		    cv::line(frame, D, A, cvScalar(0, 255, 0), 2);
        }
        //display image
        cv::imshow("video", frame);
        //stop if Escape key is pressed
        char c = (char)cvWaitKey(2);
        if(c==27 )
        {
            cv::destroyWindow("video");
            return;
        }
        teleoperate(motionproxy, c);
    }
}