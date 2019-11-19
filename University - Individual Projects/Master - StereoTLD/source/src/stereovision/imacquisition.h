/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                           imacquisition.h                          * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef IMACQUISITION_H_
#define IMACQUISITION_H_

#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>
#include <opencv2/core/core.hpp>

//image parameters
#define RESOLUTION      AL::kQVGA
#define COLORSPACE      AL::kBGRColorSpace
#define CVIMAGEPARAMS   cvSize(320, 240), 8, 3
//camera parameters
#define FOCALDIST       288.0000 //m
#define CAMERADIST      0.050000 //m
#define HORIZONT_FOV    1.062367 //rad
#define VERTICAL_FOV    0.830356 //rad
#define CORR_A0         0.000000
#define CORR_A1         0.972930
#define CORR_A2         0.849330
//config filename
#define CONFIGPATH      "acquisition.cfg"

struct CorrParams
{
    double a0;
    double a1;
    double a2;
};

class ImAcquisition
{
private:
    void configure();
    int configureFromFile();
    void configureUsingDefaults();
    void configureManually();
    void saveConfig();
public:
    bool initialized;
    std::string robotIp;
    AL::ALVideoDeviceProxy *proxy;
    std::string proxyClient;
    
    cv::Mat leftImage;
    cv::Mat rightImage;
    
    double focalDist;
    double camDist;
    double horizontFov;
    double verticalFov;
    CorrParams distCorrection;
    
    ImAcquisition();
    ImAcquisition(const std::string& robotIp, const std::string& clientName);
    virtual ~ImAcquisition();
    void init(const std::string& robotIp, const std::string& clientName);
    void updateImages();
    void showVideo();
};

#endif // IMACQUISITION_H_