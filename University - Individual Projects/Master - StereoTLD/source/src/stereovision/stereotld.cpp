/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                            stereotld.cpp                           * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "stereotld.h"
#include "auxiliary.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

// ------------------- //
// AUXILIARY VARIABLES //
// ------------------- //

CvRect box;
bool drawing_box = false;
bool endRect = false;

// ------------------ //
// AUXILIARY FUNCTONS //
// ------------------ //

void draw_box( IplImage* img, CvRect rect )
{
	cvRectangle( img, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height), cvScalar(0, 255, 0), 2);
}

void select_box_callback( int event, int x, int y, int flags, void* param ) // mouse callback
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

cv::Rect select_box(ImAcquisition *acquisition)
{
    //print controlls info
    std::cout << "Click and drag to select initial bounding box." << std::endl;
    //set window name
	const char* name = "Select bounding box";
    //set initial bb
	box = cvRect(-1,-1,0,0);
    //set images
    acquisition->updateImages();
    IplImage* image = cvCloneImage(&(IplImage)acquisition->leftImage);
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
            acquisition->updateImages();
            image = cvCloneImage(&(IplImage)acquisition->leftImage);
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
	}

	cvReleaseImage( &image );
	cvReleaseImage( &temp );
	cvDestroyWindow( name );
    endRect = false;

	return cv::Rect(box.x,box.y,box.width,box.height);
}

cv::Point bboxCenter(cv::Rect *BB)
{
    int x = BB->x + (BB->width/2);
    int y = BB->y + (BB->height/2);
    return cv::Point(x,y);
}

tld::TLD *copy(tld::TLD *original) //TODO - bypass unnecessary file I/O operations (maybe implement clone() member function for TLD class???)
{
    //save original to temporary file
    original->writeToFile("temp.tld");
    //initialize duplicate
    tld::TLD *duplicate = new tld::TLD();
    //set image properties
    duplicate->detectorCascade->imgWidth = original->detectorCascade->imgWidth;
    duplicate->detectorCascade->imgHeight = original->detectorCascade->imgHeight;
    duplicate->detectorCascade->imgWidthStep = original->detectorCascade->imgWidthStep;
    //load duplicate from file
    duplicate->readFromFile("temp.tld");
    //return pointer to duplicate
    return duplicate;
}

// ------------------------------- //
// STEREO-TLD CLASS IMPLEMENTATION //
// ------------------------------- //

StereoTLD::StereoTLD(ImAcquisition *acq)
{
    syncEnabled = false;
    sync = NULL;
    leftDetector = NULL;
    rightDetector = NULL;
    object = new Object(acq);
    acquisition = acq;
}

StereoTLD::~StereoTLD()
{
    delete sync;
    delete leftDetector;
    delete rightDetector;
    delete object;
}

void StereoTLD::initFromBB()
{
    //get image
    acquisition->updateImages();
    cv::Mat frame = (acquisition->leftImage).clone();
    //convert image to grayscale
    cv::Mat grey(frame.cols, frame.rows, CV_8UC1);
    cv::cvtColor(frame, grey, CV_BGR2GRAY);
    //initialize left detector
    leftDetector = new tld::TLD();
    //set image properties
    leftDetector->detectorCascade->imgWidth = grey.cols;
    leftDetector->detectorCascade->imgHeight = grey.rows;
    leftDetector->detectorCascade->imgWidthStep = grey.step;
    //initialize tld by user defined bounding box
    cv::Rect bbox = select_box(acquisition);
    leftDetector->selectObject(grey, &bbox);
    //clone left detector to right detector
    rightDetector = copy(leftDetector);
}

void StereoTLD::initFromFile(const char *path)
{
    //get image
    acquisition->updateImages();
    cv::Mat frame = (acquisition->leftImage).clone();
    //convert image to grayscale
    cv::Mat grey(frame.cols, frame.rows, CV_8UC1);
    cv::cvtColor(frame, grey, CV_BGR2GRAY);
    //initialize left detector
    leftDetector = new tld::TLD();
    //set detector properties
    leftDetector->detectorCascade->imgWidth = grey.cols;
    leftDetector->detectorCascade->imgHeight = grey.rows;
    leftDetector->detectorCascade->imgWidthStep = grey.step;
    //initialize tld from file
    leftDetector->readFromFile(path);
    //initialize right detector
    rightDetector = new tld::TLD();
    //set detector properties
    rightDetector->detectorCascade->imgWidth = grey.cols;
    rightDetector->detectorCascade->imgHeight = grey.rows;
    rightDetector->detectorCascade->imgWidthStep = grey.step;
    //initialize tld from file
    rightDetector->readFromFile(path);
}

void StereoTLD::runAndTrainTLD()
{
    //print info
    std::cout << "Press 'Escape' to stop training." << std::endl;
    //get image
    acquisition->updateImages();
    cv::Mat frame = (acquisition->leftImage).clone();
    //set window to display video
    cv::namedWindow("video");
    //set loop to display video from camera
    while(1)
    {
        //get frame
        acquisition->updateImages();
        frame = (acquisition->leftImage).clone();
        //process image with TLD
        leftDetector->processImage(frame);
        //draw current bounding box
        if (leftDetector->currBB)
            cv::rectangle(frame, *(leftDetector->currBB), cvScalar(0, 255, 0), 2);
        //display image
        cv::imshow("video", frame);
        //stop if Escape key is pressed
        char c = (char)cvWaitKey(2);
        if(c==27 )
        {
            //close window
            cv::destroyWindow("video");
            //clone left detector to right detector
            delete rightDetector;
            rightDetector = copy(leftDetector);
            return;
        }
    }
}

void StereoTLD::saveToFile(const char *path)
{
    leftDetector->writeToFile(path);
}

void StereoTLD::processImages()
{
    //sync detectors
    synchroniseDetectors();
    //update to latest images
    acquisition->updateImages();
    cv::Mat leftFrame = (acquisition->leftImage).clone();
    cv::Mat rightFrame = (acquisition->rightImage).clone();
    //process images with TLD
    leftDetector->processImage(leftFrame);
    rightDetector->processImage(rightFrame);
    //update object coordinates
    object->update(leftDetector->currBB,rightDetector->currBB);
}

void StereoTLD::synchroniseDetectors()
{
    if (syncEnabled && sync->isTime())
    {
        delete rightDetector;
        rightDetector = copy(leftDetector);
    }
}

void StereoTLD::setSyncTime(double ms)
{
    if (ms <= 0)
        syncEnabled = false;
    else
    {
        syncEnabled = true;
        delete sync;
        sync = new Timer(ms);
    }
}

void StereoTLD::initGui()
{
    int cols = (acquisition->leftImage).cols;
    int rows = (acquisition->leftImage).rows;
    int type = (acquisition->leftImage).type();
    //set background image (which will contain images from stereo stream)
    bgimage = new cv::Mat(140 + rows, 2*cols, type);
    //set pointers to actual images as subareas of background image
    limage = new cv::Mat(*bgimage, cv::Range(20,20+rows), cv::Range(0,cols));
    rimage = new cv::Mat(*bgimage, cv::Range(20,20+rows), cv::Range(cols,2*cols));
    //set pointer to info panel image
    bimage = new cv::Mat(*bgimage, cv::Range(20+rows,140+rows), cv::Range(0,2*cols));
    //copy images to background image
    (acquisition->leftImage).copyTo(*limage);
    (acquisition->rightImage).copyTo(*rimage);
    //write text info
    putText(*bgimage, "LEFT CAMERA", cvPoint(limage->cols/3,15), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    putText(*bgimage, "RIGHT CAMERA", cvPoint(limage->cols + limage->cols/3,15), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    //show window
    cv::namedWindow("StereoTLD");
    //show background image on window
    cv::imshow("StereoTLD", *bgimage);
}

void StereoTLD::refreshGui()
{
    //copy images to background image
    (acquisition->leftImage).copyTo(*limage);
    (acquisition->rightImage).copyTo(*rimage);
    //draw left bounding box
    if (leftDetector->currBB)
        cv::rectangle(*limage, *(leftDetector->currBB), cvScalar(0, 255, 0), 2);
    //draw right bounding box
    if (rightDetector->currBB)
        cv::rectangle(*rimage, *(rightDetector->currBB), cvScalar(0, 255, 0), 2);
    //write info to bottom image
    bimage->setTo(cv::Scalar(0,0,0)); //clear image
    cv::Point lcenter, rcenter;
    std::string text;
    //left bounding box info
    putText(*bimage, "Left BB center:", cvPoint(5,15), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    if (leftDetector->currBB)
    {
        lcenter = bboxCenter(leftDetector->currBB);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "X = " << lcenter.x) )->str();
        putText(*bimage, text, cvPoint(5,30), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "Y = " << lcenter.y) )->str();
        putText(*bimage, text, cvPoint(5,45), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    else
    {
        putText(*bimage, "X = N/A", cvPoint(5,30), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        putText(*bimage, "Y = N/A", cvPoint(5,45), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    //right bounding box info
    putText(*bimage, "Right BB center:", cvPoint(5+limage->cols,15), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    if (rightDetector->currBB)
    {
        rcenter = bboxCenter(rightDetector->currBB);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "X = " << rcenter.x) )->str();
        putText(*bimage, text, cvPoint(5+limage->cols,30), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "Y = " << rcenter.y) )->str();
        putText(*bimage, text, cvPoint(5+limage->cols,45), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    else
    {
        putText(*bimage, "X = N/A", cvPoint(5+limage->cols,30), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        putText(*bimage, "Y = N/A", cvPoint(5+limage->cols,45), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    //object coordinates info (X,Y,Z)
    putText(*bimage, "Object coordinates (X,Y,Z):", cvPoint(5,70), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    if (rightDetector->currBB && leftDetector->currBB)
    {
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "X = " << object->x << " m") )->str();
        putText(*bimage, text, cvPoint(5,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "Y = " << object->y << " m") )->str();
        putText(*bimage, text, cvPoint(5,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "Z = " << object->z << " m") )->str();
        putText(*bimage, text, cvPoint(5,115), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    else
    {
        if (rightDetector->currBB || leftDetector->currBB)
        {
            text = static_cast<std::ostringstream*>( &(std::ostringstream() << "X = " << object->x << " m (approx.)") )->str();
            putText(*bimage, text, cvPoint(5,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
            text = static_cast<std::ostringstream*>( &(std::ostringstream() << "Y = " << object->y << " m (approx.)") )->str();
            putText(*bimage, text, cvPoint(5,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        }
        else
        {
            putText(*bimage, "X = N/A m", cvPoint(5,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
            putText(*bimage, "Y = N/A m", cvPoint(5,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        }
        putText(*bimage, "Z = N/A m", cvPoint(5,115), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    //object coordinates info (yaw, pitch, distance)
    putText(*bimage, "Object coordinates (polar):", cvPoint(5+limage->cols,70), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    if (rightDetector->currBB && leftDetector->currBB)
    {
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "yaw = " << object->yaw << " rad") )->str();
        putText(*bimage, text, cvPoint(5+limage->cols,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "pitch = " << object->pitch << " rad") )->str();
        putText(*bimage, text, cvPoint(5+limage->cols,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        text = static_cast<std::ostringstream*>( &(std::ostringstream() << "distance = " << object->dist << " m") )->str();
        putText(*bimage, text, cvPoint(5+limage->cols,115), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    else
    {
        if (rightDetector->currBB || leftDetector->currBB)
        {
            text = static_cast<std::ostringstream*>( &(std::ostringstream() << "yaw = " << object->yaw << " rad (approx.)") )->str();
            putText(*bimage, text, cvPoint(5+limage->cols,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
            text = static_cast<std::ostringstream*>( &(std::ostringstream() << "pitch = " << object->pitch << " rad") )->str();
            putText(*bimage, text, cvPoint(5+limage->cols,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        }
        else
        {
            putText(*bimage, "yaw = N/A rad", cvPoint(5+limage->cols,85), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
            putText(*bimage, "pitch = N/A rad", cvPoint(5+limage->cols,100), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
        }
        putText(*bimage, "distance = N/A m", cvPoint(5+limage->cols,115), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(200,200,200), 1, 4);
    }
    //show background image on window
    cv::imshow("StereoTLD", *bgimage);
}

void StereoTLD::destroyGui()
{
    delete bgimage;
    delete limage;
    delete rimage;
    delete bimage;
    cv::destroyWindow("StereoTLD");
}

// ------------------------------- //
//   OBJECT CLASS IMPLEMENTATION   //
// ------------------------------- //

Object::Object(ImAcquisition *P_acquisition)
{
    acquisition = P_acquisition;
    valid = false;
    partial = false;
    x = 0;
    y = 0;
    y = 0;
    dist = 0;
    pitch = 0;
    yaw = 0;
}

Object::~Object()
{
    //cleanup
}

double Object::correctDistance(double measuredDist)
{
    static double a0 = (acquisition->distCorrection).a0;
    static double a1 = (acquisition->distCorrection).a1;
    static double a2 = (acquisition->distCorrection).a2;
    return a2*measuredDist*measuredDist + a1*measuredDist + a0;
}

void Object::update(cv::Rect *leftBB, cv::Rect *rightBB)
{
    //Get acquisition parameters
    double imwidth = (double) (acquisition->leftImage).cols;
    double imheight = (double) (acquisition->leftImage).rows;
    double camera_dist = acquisition->camDist;
    double focal_dist = acquisition->focalDist;
    double horizont_fov = acquisition->horizontFov;
    double vertical_fov = acquisition->verticalFov;
    //Update all info if both bounding boxes are available
    if (leftBB && rightBB)
    {
        cv::Point lcenter = bboxCenter(leftBB);
        cv::Point rcenter = bboxCenter(rightBB);
        z = correctDistance((camera_dist*focal_dist)/((double)(lcenter.x-rcenter.x)));
        x = 0.5*camera_dist + z*((double)lcenter.x - imwidth/2)/focal_dist;
        y = -z*((double)lcenter.y - imheight/2)/focal_dist;
        dist = sqrt(x*x + y*y + z*z);
        pitch = normalizeAngle( ((double)lcenter.y / imheight - 0.5) * vertical_fov );
        yaw = normalizeAngle( -0.5 * ( ((double)lcenter.x / imwidth - 0.5) + ((double)rcenter.x / imwidth - 0.5) ) * horizont_fov );
        valid = true;
        partial = true;
    }
    //Update only part of the info if some bounding box is not available (this is not accurate)
    else
    {
        if (leftBB)
        {
            cv::Point lcenter = bboxCenter(leftBB);
            x = 0.5*camera_dist + z*((double)lcenter.x - imwidth/2)/focal_dist;
            y = -z*((double)lcenter.y - imheight/2)/focal_dist;
            dist = sqrt(x*x + y*y + z*z);
            pitch = normalizeAngle( ((double)lcenter.y / imheight - 0.5) * vertical_fov );
            yaw = normalizeAngle( -((double)lcenter.x / imwidth - 0.5) * horizont_fov );
            partial = true;
        }
        else if (rightBB)
        {
            cv::Point rcenter = bboxCenter(rightBB);
            x = -0.5*camera_dist + z*((double)rcenter.x - imwidth/2)/focal_dist;
            y = -z*((double)rcenter.y - imheight/2)/focal_dist;
            dist = sqrt(x*x + y*y + z*z);
            pitch = normalizeAngle( ((double)rcenter.y / imheight - 0.5) * vertical_fov );
            yaw = normalizeAngle( -((double)rcenter.x / imwidth - 0.5) * horizont_fov );
            partial = true;
        }
        else partial = false;
        valid = false;
    }
}