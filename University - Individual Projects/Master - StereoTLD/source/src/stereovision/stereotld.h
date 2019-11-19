/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                            stereotld.h                             * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef STEREOTLD_H_
#define STEREOTLD_H_

#include <opencv2/core/core.hpp>
#include "TLD.h"
#include "imacquisition.h"
#include "timer.h"

class Object
{
private:
    ImAcquisition *acquisition;
    double correctDistance(double measuredDist);
public:
    bool valid;
    bool partial;
    double x;
    double y;
    double z;
    double dist;
    double pitch;
    double yaw;
    Object(ImAcquisition *acquisition);
    virtual ~Object();
    void update(cv::Rect *leftBB, cv::Rect *rightBB);
};

class StereoTLD
{
private:
    Timer *sync;
    void synchroniseDetectors();
    cv::Mat *bgimage;
    cv::Mat *limage;
    cv::Mat *rimage;
    cv::Mat *bimage;
    ImAcquisition *acquisition;
public:
    //detectors
    tld::TLD *leftDetector;
    tld::TLD *rightDetector;
    //object coordinates
    Object *object;
    //constructor & destructor
    StereoTLD(ImAcquisition *acquisition);
    virtual ~StereoTLD();
    //detector synchronization flag
    bool syncEnabled;
    //member functions
    void initFromBB();
    void initFromFile(const char *path);
    void runAndTrainTLD();
    void saveToFile(const char *path);
    void processImages();
    void initGui();
    void refreshGui();
    void destroyGui();
    void setSyncTime(double ms);
};

#endif // STEREOTLD_H_