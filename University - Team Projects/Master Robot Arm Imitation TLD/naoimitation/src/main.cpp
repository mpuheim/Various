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

// OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
// Aldebaran
#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>
// OpenTLD
#include "TLD.h"
// System
#include <iostream>
#include <sstream>
#include <string>
// Program
#include "auxiliary.h"
#include "imacquisition.h"
#include "tldinit.h"
#include "trajectory.h"
#include "evolution.h"
#include "motion.h"

#define PROXY_CLIENT_NAME "imitation1234"
#define MODEL_FILENAME "model.tld"
#define TRAJECTORY_FILENAME "trajectory.trj"
#define POPULATION_FILENAME "population.pop"

//evolution menu options
void printMenu4()
{
    std::cout << std::endl << "Evolution menu:" << std::endl;
    std::cout << "1. Start evolution with new random population." << std::endl;
    std::cout << "2. Continue evolution with previous population." << std::endl;
    std::cout << "3. Show best solution." << std::endl;
    std::cout << "4. Show best solutions from all generations." << std::endl;
    std::cout << "5. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//evolution menu
void menu4(ImAcquisition *acquisition, tld::TLD *detector, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy, Trajectory *trajectory)
{
    printMenu4();
    char option;
    Population *population = NULL;
    Population *generation = NULL;
    std::string gen_filename;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Evolve new population." << std::endl;
                //init new random population
                population = new Population(acquisition, motionproxy, memoryproxy, detector, trajectory);
                population->writeToFile(POPULATION_FILENAME);
                population->evolve();
                population->writeToFile(POPULATION_FILENAME);
                printMenu4();
                break;
            case '2':
                std::cout << "Option 2: Continue with evolution." << std::endl;
                //continue evolution
                if (!population)
                {
                    //if population is not initialized, load it from file
                    population = new Population(acquisition, motionproxy, memoryproxy, detector, trajectory, POPULATION_FILENAME);
                }
                if (population && population->size > 0)
                {
                    //if population is properly initialized (size > 0), start evolution
                    population->evolve();
                    population->writeToFile(POPULATION_FILENAME);
                }
                else
                {
                    //if population is not properly initialised, do not use it anymore
                    delete population;
                    population = NULL;
                }
                printMenu4();
                break;
            case '3':
                std::cout << "Option 3: Show best solution." << std::endl;
                //show best solution
                if (population)
                {
                    //if we have loaded population, just show best individual
                    std::cout << "Showing best individual from " << population->generation << ". generation. (ID" << population->individual[0].id << ")" <<std::endl;
                    population->individual[0].show(acquisition, motionproxy, detector, trajectory);
                }
                else
                {
                    //else load population from file
                    std::cout << "Loading population from file." << std::endl;
                    population = new Population(acquisition, motionproxy, memoryproxy, detector, trajectory, POPULATION_FILENAME);
                    if (population->size > 0)
                    {
                        //if population is properly loaded (size > 0), show best individual
                        std::cout << "Showing best individual from " << population->generation << ". generation. (ID" << population->individual[0].id << ")" <<std::endl;
                        population->individual[0].show(acquisition, motionproxy, detector, trajectory);
                    }
                    else
                    {
                        //if population is not properly initialised, do not use it anymore
                        delete population;
                        population = NULL;
                    }
                }
                printMenu4();
                break;
            case '4':
                std::cout << "Option 4: Show best solutions from all generations." << std::endl;
                //show best solutions
                if (!population)
                {
                    //if we do not have loaded population, load it from file
                    std::cout << "Loading population from file." << std::endl;
                    population = new Population(acquisition, motionproxy, memoryproxy, detector, trajectory, POPULATION_FILENAME);
                    if (population->size < 1)
                    {
                        //if population is not properly initialised (size < 1), do not use it anymore
                        delete population;
                        population = NULL;
                    }
                }
                if (population)
                {
                    //if we have loaded population, show best individual for every previous generation
                    for (int i = 0; i < population->generation; i++)
                    {
                        //load generation
                        gen_filename = static_cast<std::ostringstream*>( &(std::ostringstream() << "generation" << i << ".pop") )->str();
                        generation = new Population(acquisition, motionproxy, memoryproxy, detector, trajectory, gen_filename.c_str());
                        if (generation && generation->size > 0)
                        {
                            //if generation file was successfully loaded, show best individual
                            std::cout << "Showing best individual from " << i << ". generation. (ID" << generation->individual[0].id << ")" <<std::endl;
                            generation->individual[0].show(acquisition, motionproxy, detector, trajectory);
                        }
                        //free pointer to used generation
                        delete generation;
                    }
                    //show best individual from current generation
                    std::cout << "Showing best individual from " << population->generation << ". generation. (ID" << population->individual[0].id << ")" <<std::endl;
                    population->individual[0].show(acquisition, motionproxy, detector, trajectory);
                }
                printMenu4();
                break;
            case '5':
                std::cout << "Option 5: Exit." << std::endl;
                delete population;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//reference motion trajectory menu
void menu3(ImAcquisition *acquisition, tld::TLD *detector, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy)
{
    std::cout << std::endl << "Reference motion trajectory menu:" << std::endl;
    std::cout << "1. Show new motion to robot." << std::endl;
    std::cout << "2. Load reference motion from file." << std::endl;
    std::cout << "3. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
    char option;
    Trajectory *trajectory = new Trajectory;
    detector->learningEnabled = false;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Show new motion to robot." << std::endl;
                trajectory->initByMouse(acquisition);
                trajectory->saveWithPrompt(TRAJECTORY_FILENAME);
                menu4(acquisition, detector, motionproxy, memoryproxy, trajectory);
                delete trajectory;
                return;
            case '2':
                std::cout << "Option 2: Load reference motion from file." << std::endl;
                trajectory->readFromFile(TRAJECTORY_FILENAME);
                std::cout << "Done." << std::endl;
                menu4(acquisition, detector, motionproxy, memoryproxy, trajectory);
                delete trajectory;
                return;
            case '3':
                std::cout << "Option 3: Exit." << std::endl;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//tld after initialization menu options
void printMenu2()
{
    std::cout << std::endl << "TLD after initialization menu:" << std::endl;
    std::cout << "1. Run and train TLD." << std::endl;
    std::cout << "2. Save TLD model to file." << std::endl;
    std::cout << "3. Set object trajectory." << std::endl;
    std::cout << "4. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//tld after initialization options
void menu2(ImAcquisition *acquisition, tld::TLD *detector, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy)
{
    printMenu2();
    char option;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Run and train TLD." << std::endl;
                std::cout << std::endl << "Training TLD." << std::endl;
                runAndTrainTLD(acquisition, detector, motionproxy);
                std::cout << "Training finished." << std::endl;
                printMenu2();
                break;
            case '2':
                std::cout << "Option 2: Save TLD model to file." << std::endl;
                detector->writeToFile(MODEL_FILENAME);
                std::cout << "Done." << std::endl;
                printMenu2();
                break;
            case '3':
                std::cout << "Option 3: Set object trajectory." << std::endl;
                menu3(acquisition, detector, motionproxy, memoryproxy);
                delete detector;
                return;
            case '4':
                std::cout << "Option 4: Exit." << std::endl;
                delete detector;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//tld initialization options
void menu1(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy)
{
    std::cout << std::endl << "TLD initialization menu:" << std::endl;
    std::cout << "1. Initalize TLD by selecting new object." << std::endl;
    std::cout << "2. Initalize TLD by loading object model from file." << std::endl;
    std::cout << "3. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
    char option;
    tld::TLD *detector;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Manual selection." << std::endl;
                detector = initTLD_fromBB(acquisition, motionproxy);
                std::cout << "Done." << std::endl;
                menu2(acquisition, detector, motionproxy, memoryproxy);
                return;
            case '2':
                std::cout << "Option 2: Loading TLD model from file." << std::endl;
                detector = initTLD_fromFile(MODEL_FILENAME, acquisition);
                std::cout << "Done." << std::endl;
                menu2(acquisition, detector, motionproxy, memoryproxy);
                return;
            case '3':
                std::cout << "Option 3: Exit." << std::endl;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//entry point function
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage 'naoimitation robotIp'" << std::endl;
        return 1;
    }

    const std::string robotIp(argv[1]);
    ImAcquisition *acquisition;
    AL::ALMotionProxy *motionproxy;
    AL::ALMemoryProxy *memoryproxy;

    try
    {
        //load components
        acquisition = new ImAcquisition(robotIp, PROXY_CLIENT_NAME);
        motionproxy = new AL::ALMotionProxy(robotIp);
        memoryproxy = new AL::ALMemoryProxy(robotIp);
        //temperature check
        cooldown(motionproxy, memoryproxy, false);
        //set robot to initial position
        initServos(motionproxy);
        motionproxy->setStiffnesses("Body", 0.8f);
        initPose(motionproxy);
        //initialize random number generator
        srand(time(NULL));
        //start user interface
        menu1(acquisition, motionproxy, memoryproxy);
        //turn off robot
        motionproxy->setStiffnesses("Body", 0.01f);
        //unload components
        delete memoryproxy;
        delete motionproxy;
        delete acquisition;
    }
    catch (const AL::ALError& err)
    {
        std::cerr << "Naoqi exception " << err.what() << std::endl;
    }


    return 0;
}