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

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "imacquisition.h"
#include <opencv2/core/core.hpp>
#include <vector>

class Trajectory
{
public:
    int sampleCount;
    float samplePeriod;
    float duration;
    std::vector<cv::Point> sample;
    
    Trajectory();
    virtual ~Trajectory();
    void initByMouse(ImAcquisition *acquisition);
    void writeToFile(const char *path);
    void readFromFile(const char *path);
    void saveWithPrompt(const char *path);
};

#endif // TRAJECTORY_H_
