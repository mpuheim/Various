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

#ifndef TLDINIT_H_
#define TLDINIT_H_

#include "imacquisition.h"
#include "TLD.h"
#include <alproxies/almotionproxy.h>

tld::TLD *initTLD_fromBB(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy);
tld::TLD *initTLD_fromFile(const char *path, ImAcquisition *acquisition);
void runAndTrainTLD(ImAcquisition *acquisition, tld::TLD *detector, AL::ALMotionProxy *motionproxy);

#endif // TLDINIT_H_
