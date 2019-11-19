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

#include "imacquisition.h"
#include <opencv/cv.h>

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
    //subscribe a client image requiring 320*240 and BGR colorspace
	proxyClient = proxy->subscribe(clientName, AL::kQVGA, AL::kBGRColorSpace, 30);
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
        proxyClient = proxy->subscribe(clientName, AL::kQVGA, AL::kBGRColorSpace, 30);
        initialized = true;
    }
    else
        std::cerr << "Error. ImAcquisition object is already initialized." << std::endl;
}

cv::Mat ImAcquisition::getImage()
{
    //retrieve an image from the camera.
    AL::ALValue img = proxy->getImageRemote(proxyClient);
    //create an iplimage header to wrap into an opencv image.
	IplImage* imgHeader = cvCreateImageHeader(cvSize(320, 240), 8, 3);
    //assign image data to the opencv image container.
    cvSetData(imgHeader,(char*)img[6].GetBinary(),imgHeader->width * imgHeader->nChannels); //imgHeader->imageData = (char*)img[6].GetBinary();
    //tell to ALVideoDevice that it can give back the image buffer to the driver.
    proxy->releaseImage(proxyClient);
    //convert to Mat
    cv::Mat frame = cv::Mat(imgHeader).clone();
    //cleanup
	cvReleaseImageHeader(&imgHeader);
    //return image
    return frame;
}
