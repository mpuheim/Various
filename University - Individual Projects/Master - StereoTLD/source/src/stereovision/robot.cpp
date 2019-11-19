/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                             robot.cpp                              * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "robot.h"

Robot::Robot(const std::string& ip, const std::string& proxyClientName)
{
    robotIp = ip;
    clientName = proxyClientName;
}

void Robot::init()
{
    acquisition = new ImAcquisition(robotIp, clientName);
    memory = new AL::ALMemoryProxy(robotIp);
    stereotld = new StereoTLD(acquisition);
    motion = new MotionController(robotIp,memory,acquisition);
}

Robot::~Robot()
{
    delete stereotld;
    delete memory;
    delete motion;
    delete acquisition;
}