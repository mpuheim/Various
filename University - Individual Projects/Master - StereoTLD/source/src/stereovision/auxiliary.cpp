/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                            auxiliary.cpp                           * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "auxiliary.h"
#include <iostream>

char getKeyFromInput()
{
	char c = '\n';
    std::cin >> c;
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    return c;
}

double round(double num)
{
    int rounded = (int)(num+0.5);
    return (double)rounded;
}

// normalize angle between -pi and +pi
double normalizeAngle(double angle) {
  while (angle > 3.14159265) angle -= 2.0 * 3.14159265;
  while (angle < -3.14159265) angle += 2.0 * 3.14159265;
  return angle;
}