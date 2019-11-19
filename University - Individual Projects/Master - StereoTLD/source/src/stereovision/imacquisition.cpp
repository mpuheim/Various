/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                          imacquisition.cpp                         * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "imacquisition.h"
#include "auxiliary.h"
#include "evolution.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

// ------------------- //
// AUXILIARY VARIABLES //
// ------------------- //

int left_x;
int right_x;

// ------------------ //
// AUXILIARY FUNCTONS //
// ------------------ //

void select_points_callback( int event, int x, int y, int flags, void* param ) // mouse callback
{
	IplImage* image = (IplImage*) param;
    int curr_x = -1;

	if (event == CV_EVENT_LBUTTONDOWN)
    {
		if ((right_x == -1) && (left_x == -1))
        {
            //set point
            curr_x = x;
            //draw point
            cvCircle(image, cvPoint(x,y), 3, cv::Scalar(0,0,255), -1);
            //set left or right according to coordinates
            if (x < image->width/2)
            {
                left_x = curr_x;
            }
            else
            {
                right_x = curr_x;
                right_x -= image->width/2;
            }
            
        }
        else if ((left_x == -1) && (x < image->width/2))
        {
            //set left point
            left_x = x;
            //draw point
            cvCircle(image, cvPoint(x,y), 3, cv::Scalar(0,0,255), -1);
        }
        else if ((right_x == -1) && (x >= image->width/2))
        {
            //set right point
            right_x = x;
            //draw point
            cvCircle(image, cvPoint(x,y), 3, cv::Scalar(0,0,255), -1);
            //fix offset
            right_x -= image->width/2;
        }
    }
}

double focal_dist_calib(ImAcquisition *acquisition, double camdist, double objdist)
{
    //set window name
	const char* name = "Select corresponding points";
    //set initial points
	left_x = -1;
    right_x = -1;
    //set image parameters
    int cols = (acquisition->leftImage).cols;
    int rows = (acquisition->leftImage).rows;
    int type = (acquisition->leftImage).type();
    //set background image (which will contain images from stereo stream)
    cv::Mat *bgimage = new cv::Mat(rows, 2*cols, type);
    //set pointers to actual images as subareas of background image
    cv::Mat *limage = new cv::Mat(*bgimage, cv::Range(0,rows), cv::Range(0,cols));
    cv::Mat *rimage = new cv::Mat(*bgimage, cv::Range(0,rows), cv::Range(cols,2*cols));
    //update images
    acquisition->updateImages();
    //copy images to background image
    (acquisition->leftImage).copyTo(*limage);
    (acquisition->rightImage).copyTo(*rimage);
    //convert to IplImage
    IplImage* image = cvCloneImage(&((IplImage)(*bgimage)));
    //display window
	cvNamedWindow(name);
    cvShowImage(name,image);
	//set up the callback
	cvSetMouseCallback( name, select_points_callback, (void*) image);
	//main loop
    char c;
	while( 1 ){
        //update images only until first point is selected
        if( (left_x == -1) && (right_x == -1) )
        {
            //release previous image
            cvReleaseImage(&image);
            //update images
            acquisition->updateImages();
            //copy images to background image
            (acquisition->leftImage).copyTo(*limage);
            (acquisition->rightImage).copyTo(*rimage);
            //convert to IplImage
            image = cvCloneImage(&((IplImage)(*bgimage)));
        }
        //show image
        cvShowImage( name, image );
        //stop loop if both points are set
        if( (left_x != -1) && (right_x != -1) ) 
            break;
        //handle gui events
        c = (char)cvWaitKey(15);
	}
    c = (char)cvWaitKey(1000);
    //calculate focal distance
    double focal_distance = (objdist/camdist) * ((double)(left_x - right_x));
    //release resources
    delete bgimage;
    delete limage;
    delete rimage;
    cvReleaseImage(&image);
    cvDestroyWindow(name);
    //return focal distance
    return focal_distance;
}

double distance_correction_calib(ImAcquisition *acquisition, double camdist, double focaldist)
{
    //set window name
	const char* name = "Select corresponding points";
    //set initial points
	left_x = -1;
    right_x = -1;
    //set image parameters
    int cols = (acquisition->leftImage).cols;
    int rows = (acquisition->leftImage).rows;
    int type = (acquisition->leftImage).type();
    //set background image (which will contain images from stereo stream)
    cv::Mat *bgimage = new cv::Mat(rows, 2*cols, type);
    //set pointers to actual images as subareas of background image
    cv::Mat *limage = new cv::Mat(*bgimage, cv::Range(0,rows), cv::Range(0,cols));
    cv::Mat *rimage = new cv::Mat(*bgimage, cv::Range(0,rows), cv::Range(cols,2*cols));
    //update images
    acquisition->updateImages();
    //copy images to background image
    (acquisition->leftImage).copyTo(*limage);
    (acquisition->rightImage).copyTo(*rimage);
    //convert to IplImage
    IplImage* image = cvCloneImage(&((IplImage)(*bgimage)));
    //display window
	cvNamedWindow(name);
    cvShowImage(name,image);
	//set up the callback
	cvSetMouseCallback( name, select_points_callback, (void*) image);
	//main loop
    char c;
	while( 1 ){
        //update images only until first point is selected
        if( (left_x == -1) && (right_x == -1) )
        {
            //release previous image
            cvReleaseImage(&image);
            //update images
            acquisition->updateImages();
            //copy images to background image
            (acquisition->leftImage).copyTo(*limage);
            (acquisition->rightImage).copyTo(*rimage);
            //convert to IplImage
            image = cvCloneImage(&((IplImage)(*bgimage)));
        }
        //show image
        cvShowImage( name, image );
        //stop loop if both points are set
        if( (left_x != -1) && (right_x != -1) ) 
            break;
        //handle gui events
        c = (char)cvWaitKey(15);
	}
    c = (char)cvWaitKey(1000);
    //calculate distance
    double distance = (camdist*focaldist)/((double)(left_x-right_x));
    //release resources
    delete bgimage;
    delete limage;
    delete rimage;
    cvReleaseImage(&image);
    cvDestroyWindow(name);
    //return focal distance
    return distance;
}

// ---------------------------------- //
// IMACQUISITION CLASS IMPLEMENTATION //
// ---------------------------------- //

ImAcquisition::ImAcquisition()
{
    initialized = false;
}

ImAcquisition::ImAcquisition(const std::string& robotIp, const std::string& clientName)
{
    //create a proxy to ALVideoDevice on the robot
	proxy = new AL::ALVideoDeviceProxy(robotIp, 9559);
    //try to unsubscribe possible previous subscription to proxy
    try {proxy->unsubscribe(clientName);} catch(...) {;}
    //subscribe client for left and right camera requiring 320*240 and BGR colorspace
    AL::ALValue IDs = AL::ALValue::array (0, 1);
    AL::ALValue Resolutions = AL::ALValue::array (RESOLUTION, RESOLUTION);
    AL::ALValue Colorspaces = AL::ALValue::array (COLORSPACE, COLORSPACE);
    proxyClient = proxy->subscribeCameras(clientName, IDs, Resolutions, Colorspaces, 30);
    //configure acquisition
    configure();
    //initialization done
    initialized = true;
}

ImAcquisition::~ImAcquisition()
{
    try {proxy->unsubscribe(proxyClient);} catch(...) {;}
    delete proxy;
}

void ImAcquisition::init(const std::string& robotIp, const std::string& clientName)
{
    if (initialized==false)
    {
        //create a proxy to ALVideoDevice on the robot
        proxy = new AL::ALVideoDeviceProxy(robotIp, 9559);
        //try to unsubscribe possible previous subscription to proxy
        try {proxy->unsubscribe(clientName);} catch(...) {;}
        //subscribe a client image requiring 320*240 and BGR colorspace
        AL::ALValue IDs = AL::ALValue::array (0, 1);
        AL::ALValue Resolutions = AL::ALValue::array (RESOLUTION, RESOLUTION);
        AL::ALValue Colorspaces = AL::ALValue::array (COLORSPACE, COLORSPACE);
        proxyClient = proxy->subscribeCameras(clientName, IDs, Resolutions, Colorspaces, 30);
        //configure acquisition
        configure();
        //initialization done
        initialized = true;
    }
    else
        std::cerr << "Error. ImAcquisition object is already initialized." << std::endl;
}

void ImAcquisition::updateImages()
{
    //retrieve an image from the camera.
    AL::ALValue img = proxy->getImagesRemote(proxyClient);
    //create an iplimage header to wrap into an opencv image.
    IplImage* imgLeftHeader = cvCreateImageHeader(CVIMAGEPARAMS);
    IplImage* imgRightHeader = cvCreateImageHeader(CVIMAGEPARAMS);
    //assign image data to the opencv image container.
    char* data = (char*)img[2].GetBinary();
    int linesize = imgLeftHeader->width * imgLeftHeader->nChannels;
    int datasize = imgLeftHeader->height * linesize;
    cvSetData(imgLeftHeader, data, linesize); //imgLeftHeader->imageData = (char*)img[6].GetBinary();
    cvSetData(imgRightHeader, data+datasize, linesize);
    //tell ALVideoDevice that it can give back the image buffer to the driver.
    proxy->releaseImages(proxyClient);
    //convert to Mat
    cv::Mat leftFrame = cv::Mat(imgLeftHeader).clone();
    cv::Mat rightFrame = cv::Mat(imgRightHeader).clone();
    //cleanup
    cvReleaseImageHeader(&imgLeftHeader);
    cvReleaseImageHeader(&imgRightHeader);
    //flip images to actual orientation and save them as acquisition object instance variable (obj.leftImage, obj.rightImage)
    cv::flip(leftFrame, leftImage, -1);
    cv::flip(rightFrame, rightImage, -1);
}

void ImAcquisition::configure()
{
    //update images
    updateImages();
    //try to load configuration parameters from file
    int success = configureFromFile();
    //else set up configuration manually or load defaults
    if (success == 0)
    {
        char option;
        std::cout << std::endl << "Acquisition configuration from file has failed." << std::endl;
        std::cout << "File may be corrupted or missing." << std::endl;
        std::cout << "What do you want to do?" << std::endl;
        std::cout << "1. Set up acquisition parameters manually." << std::endl;
        std::cout << "2. Load defaults." << std::endl;
        std::cout << std::endl << "Enter your option: ";
        while (1)
        {
            option = getKeyFromInput();
            switch (option)
            {
                case '1':
                    std::cout << "Setting up acquisition parameters manually:" << std::endl;
                    configureManually();
                    std::cout << "Setting up acquisition parameters is done." << std::endl;
                    saveConfig();
                    return;
                case '2':
                    std::cout << "Loading default acquisition configuration... ";
                    configureUsingDefaults();
                    std::cout << "Done." << std::endl;
                    saveConfig();
                    return;
                default:
                    std::cout << "Wrong input (" << option << ")." << std::endl;
                    std::cout << std::endl << "Enter your option: ";
            }
        }
    }
}

int ImAcquisition::configureFromFile()
{
    FILE *file = fopen(CONFIGPATH, "r");
    if(file == NULL)
        return 0;
    int MAX_LEN = 255;
    char str_buf[255];
    fgets(str_buf, MAX_LEN, file); //skip line
    if (fscanf(file, "%lf \n", &focalDist) != 1) //get focalDist
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &camDist) != 1) //get camDist
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &horizontFov) != 1) //get horizontal fov
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &verticalFov) != 1) //get vertical fov
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &(distCorrection.a0)) != 1) //get correction parameter a0
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &(distCorrection.a1)) != 1) //get correction parameter a1
    {
        fclose(file);
        return 0;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%lf \n", &(distCorrection.a2)) != 1) //get correction parameter a2
    {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

void ImAcquisition::configureUsingDefaults()
{
    focalDist = FOCALDIST;
    camDist = CAMERADIST;
    horizontFov = HORIZONT_FOV;
    verticalFov = VERTICAL_FOV;
    distCorrection.a0 = CORR_A0;
    distCorrection.a1 = CORR_A1;
    distCorrection.a2 = CORR_A2;
}

void ImAcquisition::configureManually()
{
    //set field of view
    AL::ALValue img = proxy->getImagesRemote(proxyClient); //retrieve images from the camera.
    horizontFov = 2*(double)img[0][8];
    verticalFov = 2*(double)img[0][9];
    //set distance between cameras
    std::cout << "Set distance between cameras (in meters): ";
    while(!(std::cin >> camDist) || camDist <= 0)
    {
        std::cout << "Wrong input. Please enter positive number." << std::endl;
        std::cout << "Set distance between cameras (in meters): ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set arbitrary object real distance
    std::cout << "Pick an arbitrary object in the vision field of the robot and measure its real" << std::endl;
    std::cout << "world distance from robots head." << std::endl;
    std::cout << "When you are done, press 'Escape' in the Video window." << std::endl;
    showVideo();
    std::cout << "Set distance of the object (in meters): ";
    double objDist;
    while(!(std::cin >> objDist) || objDist <= 0)
    {
        std::cout << "Wrong input. Please enter positive number." << std::endl;
        std::cout << "Set distance of the object (in meters): ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set focal distance
    std::cout << "Select corresponding points of the object in both left and right image." << std::endl;
    focalDist = focal_dist_calib(this, camDist, objDist);
    //set distance correction parameters
    std::cout << "To improve distance measurement it is necessary to gather another training" << std::endl;
    std::cout << "samples of object. Try to move object in different distances than in previous" << std::endl;
    std::cout << "measurements. Take another 5 measurements please." << std::endl;
    float real_dist[5];
    float measured_dist[5];
    for (int i=0; i < 5; i++)
    {
        std::cout << std::endl << "Measurement " << i+1 << ":" << std::endl;
        std::cout << "Set distance of the object (in meters): ";
        double objDist;
        while(!(std::cin >> real_dist[i]) || real_dist[i] <= 0)
        {
            std::cout << "Wrong input. Please enter positive number." << std::endl;
            std::cout << "Set distance of the object (in meters): ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        measured_dist[i] = distance_correction_calib(this, camDist, focalDist);
    }
    Sample data[5];
    for (int i=0; i < 5; i++)
    {
        data[i].x = measured_dist[i];
        data[i].y = real_dist[i];
        /*TEMP*/std::cout << i+1 << ". measurement: measured = " << measured_dist[i] << " ; real = " << real_dist[i] << std::endl;
    }
    float poly_coeffs[3];
    std::cout << "Calculating correction parameters. Please wait a few more seconds... ";
    polyfit(poly_coeffs, 3, data, 5);
    std::cout << "Done." << std::endl;
    distCorrection.a0 = (double )poly_coeffs[0];
    distCorrection.a1 = (double )poly_coeffs[1];
    distCorrection.a2 = (double )poly_coeffs[2];
    std::cout << std::endl << "Thank you for your patience. Gathering configuration information is done." << std::endl;
    //print configuration
    std::cout << std::endl << "Calculated parameters are as follows:" << std::endl;
    std::cout << "Distance between cameras is " << camDist << " m" << std::endl;
    std::cout << "Focal distance is " << focalDist << " pix" << std::endl;
    std::cout << "Horizontal field of view is " << horizontFov << " rad" << std::endl;
    std::cout << "Vertical field of view is " << verticalFov << " rad" << std::endl;
    std::cout << "Correction function is f(x) = " << distCorrection.a2 << "*x^2 + " << distCorrection.a1 << "*x + " << distCorrection.a0 << std::endl;
}

void ImAcquisition::showVideo()
{
    //get image
    updateImages();
    cv::Mat frame = leftImage.clone();
    //set window to display video
    cv::namedWindow("Video");
    //set loop to display video from camera
    while(1)
    {
        //get frame
        updateImages();
        frame = leftImage.clone();
        //display image
        cv::imshow("Video", frame);
        //stop if Escape key is pressed
        char c = (char)cvWaitKey(2);
        if(c==27 )
        {
            //close window
            cv::destroyWindow("Video");
            return;
        }
    }
}

void ImAcquisition::saveConfig()
{
    FILE *file = fopen(CONFIGPATH, "w");
    fprintf(file, "#Nao stereohead acquisition configuration\n");
    fprintf(file, "%f #Focal distance\n", focalDist);
    fprintf(file, "%f #Distance between cameras\n", camDist);
    fprintf(file, "%f #Horizontal field of view\n", horizontFov);
    fprintf(file, "%f #Vertical field of view\n", verticalFov);
    fprintf(file, "%f #Correction A0\n", distCorrection.a0);
    fprintf(file, "%f #Correction A1\n", distCorrection.a1);
    fprintf(file, "%f #Correction A2\n", distCorrection.a2);
    fclose(file);
}
