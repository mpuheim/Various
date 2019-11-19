/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                              robot.h                               * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef ROBOT_H_
#define ROBOT_H_

#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include "imacquisition.h"
#include "motion.h"
#include "stereotld.h"

class Robot
{
public:
    //identity
    std::string robotIp;
    std::string clientName;
    //modules
    MotionController *motion;
    AL::ALMemoryProxy *memory;
    ImAcquisition *acquisition;
    StereoTLD *stereotld;
    //constructor & destructor
    Robot(const std::string& ip, const std::string& proxyClientName);
    virtual ~Robot();
    //member functions
    void init();
};

#endif // ROBOT_H_