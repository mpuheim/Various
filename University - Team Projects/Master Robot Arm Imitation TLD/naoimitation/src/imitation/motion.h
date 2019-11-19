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

#ifndef MOTION_H_
#define MOTION_H_

#include <alproxies/almemoryproxy.h>
#include <alproxies/almotionproxy.h>

void teleoperate(AL::ALMotionProxy *motion, char c);
void printControls(void);
void positionFix(double *roll, double *pitch);
void moveToPosition(AL::ALMotionProxy *motion, double roll, double pitch);
void moveToPosition(AL::ALMotionProxy *motion, double roll, double pitch, bool relative);
void initPose(AL::ALMotionProxy *motion);
void initServos(AL::ALMotionProxy *motion);
void cooldown(AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy);
void cooldown(AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy, bool verbose);

#endif // MOTION_H_