/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                              main.cpp                              * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

// OpenCV includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
// Aldebaran includes
#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>
// OpenTLD includes
#include "TLD.h"
// Program includes
#include "auxiliary.h"
#include "robot.h"
#include "imacquisition.h"
#include "timer.h"
// Configuration
#define PROXY_CLIENT_NAME   "stereovision"
#define MODEL_FILENAME      "model.tld"
#define HAND_CONFIG_FILE    "handnn.net"
#define TRAIN_DATA_FILE     "traindata.pat"

void runStereo(Robot* robot)
{
    //set stiffness
    robot->motion->initialPosition();
    robot->motion->headController->initialPosition();
    //set window to display video
    robot->stereotld->initGui();
    //move head every 200 miliseconds
    bool moveHead = true;
    Timer moveTimer(200);
    //main loop
    while (1)
    {
        //process images with tld
        robot->stereotld->processImages();
        //refresh gui
        robot->stereotld->refreshGui();
        //move head to track object
        if (moveHead && moveTimer.isTime())
        {
            robot->motion->headController->trackObject(robot->stereotld->object);
        }
        //handle gui events
        char c = (char)cv::waitKey(2);
        //enable/disable head movement if 'm' is pressed
        if(c=='m')
        {
            if (moveHead)
            {
                robot->motion->proxy->setStiffnesses("HeadYaw", 0.0f);
                robot->motion->proxy->setStiffnesses("HeadPitch", 0.0f);
                moveHead = false;
                std::cout << "Head movement disabled." << std::endl;
            }
            else
            {
                robot->motion->proxy->changeAngles("HeadYaw", 0, 0.025f);
                robot->motion->proxy->changeAngles("HeadPitch", 0, 0.025f);
                robot->motion->proxy->setStiffnesses("HeadYaw", 0.8f);
                robot->motion->proxy->setStiffnesses("HeadPitch", 1.0f);
                moveHead = true;
                std::cout << "Head movement enabled." << std::endl;
            }
        }
        //enable/disable detector sync if 's' is pressed
        if(c=='s')
        {
            if (robot->stereotld->syncEnabled)
            {
                robot->stereotld->setSyncTime(0); //disable sync
                std::cout << "Detector sync disabled." << std::endl;
            }
            else
            {
                robot->stereotld->setSyncTime(15000); //set sync to 15 sec
                std::cout << "Detector sync enabled (15 sec)." << std::endl;
            }
        }
        //try to grab object if 'g' is pressed
        if(c=='g')
        {
            if (robot->stereotld->object->valid)
                robot->motion->handController->grabObject(robot->stereotld->object);
            else
                std::cout << "Object is not visible. It cannot be grabbed." << std::endl;
        }
        //move hand to initial position if 'i' is pressed
        if(c=='i')
        {
            robot->motion->handController->initialPosition();
        }
        //stop if Escape key is pressed
        if(c==27)
        {
            robot->stereotld->destroyGui();
            robot->motion->rest();
            return;
        }
    }
}

//hand controller initialization menu options
void handInitMenuInfo()
{
    std::cout << std::endl << "Hand controller initialization menu:" << std::endl;
    std::cout << "1. Create new dataset." << std::endl;
    std::cout << "2. Train new neural network." << std::endl;
    std::cout << "3. Load neural network configuration from file." << std::endl;
    std::cout << "4. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//hand controller initialization menu
void handInitMenu(Robot* robot)
{
    handInitMenuInfo();
    char option;
    DataSet *dataset = NULL;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Create new dataset." << std::endl;
                delete dataset;
                std::cout << std::endl << "Training dataset creation." << std::endl;
                dataset = robot->motion->handController->createDataset(robot->stereotld,robot->motion->headController);
                std::cout << std::endl << "Training dataset creation is done." << std::endl;
                std::cout << std::endl << "Saving dataset to file... ";
                dataset->saveToFile(TRAIN_DATA_FILE);
                std::cout << "Done." << std::endl;
                handInitMenuInfo();
                break;
            case '2':
                std::cout << "Option 2: Train new neural network." << std::endl;
                if (dataset == NULL)
                {
                    std::cout << std::endl << "Loading dataset from file... ";
                    dataset = new DataSet(TRAIN_DATA_FILE);
                    if (dataset->patternCount == 0)
                    {
                        std::cout << "Could not load dataset. Try to create new one." << std::endl;
                        delete dataset;
                        dataset = NULL;
                        handInitMenuInfo();
                        break;
                    }
                    std::cout << "Done." << std::endl;
                }
                robot->motion->handController->init(dataset);
                robot->motion->handController->save(HAND_CONFIG_FILE);
                delete dataset;
                runStereo(robot);
                return;
            case '3':
                std::cout << "Option 3: Load neural network configuration from file." << std::endl;
                std::cout << std::endl << "Loading network configuration... ";
                robot->motion->handController->init(HAND_CONFIG_FILE);
                if (robot->motion->handController->initialized == false)
                {
                    std::cout << "Could not load neural network from file. Try to train new one." << std::endl;
                    handInitMenuInfo();
                    break;
                }
                std::cout << "Done." << std::endl;
                delete dataset;
                runStereo(robot);
                return;
            case '4':
                std::cout << "Option 4: Exit." << std::endl;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//tld after initialization menu options
void tldTrainMenuInfo()
{
    std::cout << std::endl << "TLD after initialization menu:" << std::endl;
    std::cout << "1. Run and train TLD." << std::endl;
    std::cout << "2. Save TLD model to file." << std::endl;
    std::cout << "3. Init hand controller." << std::endl;
    std::cout << "4. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//tld after initialization menu
void tldTrainMenu(Robot* robot)
{
    tldTrainMenuInfo();
    char option;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Run and train TLD." << std::endl;
                std::cout << std::endl << "Training TLD." << std::endl;
                robot->stereotld->runAndTrainTLD();
                std::cout << "Training finished." << std::endl;
                tldTrainMenuInfo();
                break;
            case '2':
                std::cout << "Option 2: Save TLD model to file." << std::endl;
                robot->stereotld->saveToFile(MODEL_FILENAME);
                std::cout << "Done." << std::endl;
                tldTrainMenuInfo();
                break;
            case '3':
                std::cout << "Option 3: Hand controller initialization." << std::endl;
                handInitMenu(robot);
                return;
            case '4':
                std::cout << "Option 4: Exit." << std::endl;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//tld initialization options
void tldInitMenu(Robot *robot)
{
    std::cout << std::endl << "TLD initialization menu:" << std::endl;
    std::cout << "1. Initalize TLD by selecting new object." << std::endl;
    std::cout << "2. Initalize TLD by loading object model from file." << std::endl;
    std::cout << "3. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
    char option;
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Manual selection." << std::endl;
                robot->stereotld->initFromBB();
                std::cout << "Done." << std::endl;
                tldTrainMenu(robot);
                return;
            case '2':
                std::cout << "Option 2: Loading TLD model from file." << std::endl;
                robot->stereotld->initFromFile(MODEL_FILENAME);
                std::cout << "Done." << std::endl;
                tldTrainMenu(robot);
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

//main working loop
void start(std::string robotIp)
{
    //connect to robot and initialize modules
    Robot *robot = new Robot(robotIp, PROXY_CLIENT_NAME);
    robot->init();
    
    //DataSet dataset(10,3,3);
    //for (int i = 0; i < 10; i++)
    //{
    //    dataset.input[i].value[0] = i;
    //    dataset.input[i].value[1] = 2*i;
    //    dataset.input[i].value[2] = 3*i;
    //    dataset.output[i].value[0] = i;
    //    dataset.output[i].value[1] = 2*i;
    //    dataset.output[i].value[2] = 3*i;
    //}
    //dataset.saveToFile("test.pat");
    //return;
    
    // Timer wtime(5000);
    // while (!wtime.isTime()) {;};
    // robot->motion->initialPosition();
    // //while (!wtime.isTime()) {;};
    // //robot->motion->handController->frontPosition();
    // while (!wtime.isTime()) {;};
    // AL::ALValue sensorNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowYaw","LElbowRoll","LWristYaw");
    // AL::ALValue sensorAngles = robot->motion->proxy->getAngles(sensorNames, true);
    // std::cout << "LShoulderPitch = " << sensorAngles[0] << std::endl;
    // std::cout << "LShoulderRoll  = " << sensorAngles[1] << std::endl;
    // std::cout << "LElbowYaw     = " << sensorAngles[2] << std::endl;
    // std::cout << "LElbowRoll     = " << sensorAngles[3] << std::endl;
    // std::cout << "LWristYaw     = " << sensorAngles[4] << std::endl;
    // std::cout << std::endl;
    // robot->motion->rest();
    // while (!wtime.isTime()) {;};
    // return;
    
    // AL::ALValue joints = AL::ALValue::array("HeadYaw","HeadPitch");
    // AL::ALValue angles = robot->motion->proxy->getAngles(joints,true);
    // std::cout << "HeadYaw = " << angles[0] << std::endl;
    // std::cout << "HeadPitch  = " << angles[1] << std::endl;
    
    // return;
    
    /*TEMP*/
    //robot->motion->proxy->setStiffnesses("LArm", 0.0f);
    
    //std::cout << robot->memory->getDataListName();
    //return;
    
    //AL::ALValue sensorNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowYaw","LElbowRoll","LWristYaw");
    //AL::ALValue sensorAngles;
    //Timer announce(2000);
    //while (1)
    //{
    //    if (announce.isTime())
    //    {
    //        sensorAngles = robot->motion->proxy->getAngles(sensorNames, true);
    //        std::cout << "LShoulderPitch = " << sensorAngles[0] << std::endl;
    //        std::cout << "LShoulderRoll  = " << sensorAngles[1] << std::endl;
    //        std::cout << "LElbowYaw     = " << sensorAngles[2] << std::endl;
    //        std::cout << "LElbowRoll     = " << sensorAngles[3] << std::endl;
    //        std::cout << "LWristYaw     = " << sensorAngles[4] << std::endl;
    //        std::cout << std::endl;
    //    }
    //}
    //return;
    
    //std::cout << "Limit: "<< robot->memory->getData("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Limit") << std::endl;
    //std::cout << "Min  : "<< robot->memory->getData("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Min") << std::endl;
    //std::cout << "Max  : "<< robot->memory->getData("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Max") << std::endl;
    /*TEMP*/
    
    //show tld initialization menu
    tldInitMenu(robot);
    //cleanup
    delete robot;
}

//entry point function
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage 'stereovision robotIp'" << std::endl;
        return 1;
    }

    const std::string robotIp(argv[1]);

    try
    {
        start(robotIp);
    }
    catch (const AL::ALError& err)
    {
        std::cerr << "Naoqi exception " << err.what() << std::endl;
    }

    return 0;
}