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