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

#ifndef IMACQUISITION_H_
#define IMACQUISITION_H_

#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>
#include <opencv2/core/core.hpp>

class ImAcquisition
{
public:
    bool initialized;
    std::string robotIp;
    AL::ALVideoDeviceProxy *proxy;
    std::string proxyClient;
    
    ImAcquisition();
    ImAcquisition(const std::string& robotIp, const std::string& clientName);
    virtual ~ImAcquisition();
    void init(const std::string& robotIp, const std::string& clientName);
    cv::Mat getImage();
};

#endif // IMACQUISITION_H_