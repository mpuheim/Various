/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                             motion.cpp                             * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "motion.h"
#include "timer.h"
#include "auxiliary.h"
#include <iostream>
#include <math.h>

// ----------------------- //
// MOTION CONTROLLER CLASS //
// ----------------------- //

MotionController::MotionController(const std::string& robotIp, AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition)
{
    proxy = new AL::ALMotionProxy(robotIp);
    handController = new HandController(proxy,memoryProxy,acquisition);
    headController = new HeadController(proxy);
}

MotionController::~MotionController()
{
    delete proxy;
    delete handController;
    delete headController;
}

void MotionController::initialPosition()
{
    proxy->wakeUp();
    proxy->moveInit();
}

void MotionController::rest()
{
    proxy->rest();
}

// --------------------- //
// HEAD CONTROLLER CLASS //
// --------------------- //

HeadController::HeadController(AL::ALMotionProxy *motionProxy)
{
    proxy = motionProxy;
}

HeadController::~HeadController()
{
    //cleanup
}

void HeadController::initialPosition()
{
    AL::ALValue joints = AL::ALValue::array("HeadYaw","HeadPitch");
    AL::ALValue angles = AL::ALValue::array(HEADYAW_INIT,HEADPITCH_INIT);
    proxy->setAngles(joints, angles, 0.25);
}

void HeadController::trackObject(Object *object)
{
    double curr_yaw = proxy->getAngles("HeadYaw", true)[0];
    double curr_pitch = proxy->getAngles("HeadPitch", true)[0];
    if (object->partial)
    {
        if (abs(curr_yaw - object->yaw) > 0.05)
            proxy->changeAngles("HeadYaw", 0.5 * object->yaw, 0.025f);
        if (abs(curr_pitch - object->pitch) > 0.05)
            proxy->changeAngles("HeadPitch", 0.5 * object->pitch, 0.025f);
    }
    else
    {
        proxy->changeAngles("HeadYaw", 0, 0.5f);
        proxy->changeAngles("HeadPitch", 0, 0.25f);
    }
}

// --------------------- //
// HAND CONTROLLER CLASS //
// --------------------- //

HandController::HandController(AL::ALMotionProxy *motionProxy, AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition)
{
    proxy = motionProxy;
    memory = memoryProxy;
    nn = 0; //NULL
    initLimits(memoryProxy,acquisition);
    initialized = false;
}

HandController::~HandController()
{
    delete nn;
    delete limits;
}

void HandController::initLimits(AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition)
{
    limits = new Limits();
    //Shoulder Pitch
    limits->shoulderPitch.min = (double)memoryProxy->getData("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Min");
    limits->shoulderPitch.max = (double)memoryProxy->getData("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Max");
    //Shoulder Roll
    limits->shoulderRoll.min = (double)memoryProxy->getData("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Min");
    limits->shoulderRoll.max = (double)memoryProxy->getData("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Max");
    //Elbow Yaw
    limits->elbowYaw.min = (double)memoryProxy->getData("Device/SubDeviceList/LElbowYaw/Position/Actuator/Min");
    limits->elbowYaw.max = (double)memoryProxy->getData("Device/SubDeviceList/LElbowYaw/Position/Actuator/Max");
    //Elbow Roll
    limits->elbowRoll.min = (double)memoryProxy->getData("Device/SubDeviceList/LElbowRoll/Position/Actuator/Min");
    limits->elbowRoll.max = (double)memoryProxy->getData("Device/SubDeviceList/LElbowRoll/Position/Actuator/Max");
    //Wrist Yaw
    limits->wristYaw.min = (double)memoryProxy->getData("Device/SubDeviceList/LWristYaw/Position/Actuator/Min");
    limits->wristYaw.max = (double)memoryProxy->getData("Device/SubDeviceList/LWristYaw/Position/Actuator/Max");
    //Vertical field of view
    limits->viewVertical.min = (double)memoryProxy->getData("Device/SubDeviceList/HeadPitch/Position/Actuator/Min") - acquisition->verticalFov/2;
    limits->viewVertical.max = (double)memoryProxy->getData("Device/SubDeviceList/HeadPitch/Position/Actuator/Max") + acquisition->verticalFov/2;
    //Horizontal field of view
    limits->viewHorizontal.min = (double)memoryProxy->getData("Device/SubDeviceList/HeadYaw/Position/Actuator/Min") - acquisition->horizontFov/2;
    limits->viewHorizontal.max = (double)memoryProxy->getData("Device/SubDeviceList/HeadYaw/Position/Actuator/Max") + acquisition->horizontFov/2;
    //Distance
    limits->distance.min = 0;
    limits->distance.max = MAX_DISTANCE;
}

//convert value x from interval [min,max] to interval [0,1]
double HandController::normalize(double value, double min, double max)
{
    return (value-min)/(max-min);
}

//convert value x from interval [0,1] to interval [min,max]
double HandController::denormalize(double value, double min, double max)
{
    return (max-min)*value+min;
}

double HandController::getTemperature()
{
    double temp1 = memory->getData("Device/SubDeviceList/LShoulderRoll/Temperature/Sensor/Value");
    double temp2 = memory->getData("Device/SubDeviceList/LShoulderPitch/Temperature/Sensor/Value");
    if (temp1 > temp2)
        return temp1;
    else
        return temp2;
}

void HandController::cooldown()
{
    //decrease stifness (to put hand down slowly)
    proxy->rest();
    proxy->setStiffnesses("LElbowYaw", 0.6f);
    proxy->setStiffnesses("LWristYaw", 0.6f);
    proxy->setAngles("LElbowYaw", ELBOWYAW_REST, 0.25);
    proxy->setAngles("LWristYaw", WRISTYAW_REST, 0.25);
    wait(5000);
    proxy->setStiffnesses("LElbowYaw", 0.0f);
    proxy->setStiffnesses("LWristYaw", 0.0f);
    double current_temp = getTemperature();
    //wait to cool down
    while (current_temp > GOOD_TEMPERATURE)
    {
        //print info
        std::cout << "Shoulder temperature (" << current_temp << ") is above acceptable levels. Waiting to cool down." << std::endl;
        //wait for 1 minute
        wait(60000);
        //check temperature again
        current_temp = getTemperature();
    }
    //print info about end of cooldown
    std::cout << "Shoulder temperature (" << current_temp << ") is OK. Proceeding..." << std::endl;
    //turn stiffness on
    proxy->wakeUp();
    //go to init pose
    proxy->moveInit();
    frontPosition();
    wait(5000);
}

void HandController::init(const char *path)
{
    //load neural network from file
    nn = new NeuralNetwork(path);
    //check for proper initializatin
    if (nn->layerCount == 0)
    {
        initialized = false;
        delete nn;
        delete limits;
        return;
    }
    //enable anticollision system
    proxy->setCollisionProtectionEnabled("Arms",true);
    //update initialization status
    initialized = true;
}

void HandController::init(DataSet *trainDataset)
{
    //set neuron count in layers of controller's neural network
    int neuronCount[4];
    neuronCount[0] = trainDataset->inputCount; //input layer
    neuronCount[3] = trainDataset->outputCount; //output layer
    neuronCount[1] = (int)(1.6 * (neuronCount[0] + neuronCount[3]) / 2); //1st hidden layer
    neuronCount[2] = neuronCount[1]; //2nd hidden layer
    //init neural network
    nn = new NeuralNetwork(4,neuronCount);
    //train neural network
    trainOnDataset(trainDataset);
    //enable anticollision system
    proxy->setCollisionProtectionEnabled("Arms",true);
    //update initialization status
    initialized = true;
}

void HandController::trainOnDataset(DataSet *trainDataset)
{
    std::cout << std::endl << "Hand controller training." << std::endl;
    bool continueTraining = true;
    while (continueTraining)
    {
        //enter desired network precision
        double desiredPrecision;
        std::cout << std::endl << "Enter desired controller precision (in %): ";
        while(!(std::cin >> desiredPrecision) || desiredPrecision <= 0.0 || desiredPrecision > 100.0)
        {
            std::cout << "Wrong input. Please enter number between 0 and 100." << std::endl;
            std::cout << "Enter desired network precision (in %): ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        desiredPrecision = desiredPrecision / 100; //convert percentages to [0;1] interval
        //enter learning time limit in seconds
        double learnTime;
        std::cout << "Enter maximum learning time (in seconds): ";
        while(!(std::cin >> learnTime) || learnTime <= 0.0)
        {
            std::cout << "Wrong input. Please enter positive number." << std::endl;
            std::cout << "Enter maximum learning time (in seconds): ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        learnTime = 1000 * learnTime; //convert to miliseconds
        //enter learning parameter gamma
        double gamma;
        std::cout << "Enter learning rate (gamma): ";
        while(!(std::cin >> gamma) || gamma <= 0.0 || gamma > 1.0)
        {
            std::cout << "Wrong input. Please enter number between 0 and 1." << std::endl;
            std::cout << "Enter learning rate: ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        //set timers
        Timer learnTimer(learnTime);
        Timer announceTimer(5000); //announce every 5 seconds
        //start learning
        std::cout << "Neural network training has started." << std::endl;
        int learningCycle = 0;
        double precision = 0;
        double duration = learnTime;
        while (learnTimer.isTime() == false && precision < desiredPrecision)
        {
            learningCycle++;
            nn->train(trainDataset,(float)gamma);
            precision = (double)nn->test(trainDataset);
            if (announceTimer.isTime())
            {
                std::cout << learningCycle << ". learning cycle. Elapsed time is " << duration/1000 << " seconds of " << learnTime/1000 << " total." << std::endl;
                std::cout << " - Current network precision is " << 100*precision << "%." << std::endl;
            }
            duration = learnTimer.getElapsedTime();
        }
        //print results info
        std::cout << "Learning has ended at " << learningCycle << ". learning cycle after " << duration/1000 << " seconds." << std::endl;
        std::cout << "Network precision on training dataset is " << 100*precision << "%." << std::endl;
        //ask to continue if precision is lower than expected
        if (precision < desiredPrecision)
        {
            std::cout << std::endl << "Precision of the network is lower than initially expected." << std::endl;
            std::cout << "Do you want to continue learning (Y/N): ";
            char option = 0;
            while (option == 0)
            {
                option = getKeyFromInput();
                switch (option)
                {
                    case 'Y': case 'y':
                        break;
                    case 'N': case 'n':
                        continueTraining = false;
                        break;
                    default:
                        std::cout << "Wrong input (" << option << ")." << std::endl;
                        std::cout << "Do you want to continue learning (Y/N): ";
                        option = 0;
                }
            }
        }
        else continueTraining = false;
    }
}

DataSet *HandController::createDataset(StereoTLD *stereotld, HeadController *head)
{
    //ask user to enter maximum number of samples
    int maxSamples;
    std::cout << "Enter maximum number of training samples: ";
    while(!(std::cin >> maxSamples) || maxSamples < 10)
    {
        std::cout << "Wrong input. Please enter positive number larger than 10." << std::endl;
        std::cout << "Enter maximum number of training samples: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set robot to initial position
    proxy->wakeUp();
    proxy->moveInit();
    //get possible joint positions with cooperation of user
    AL::ALValue jointNames = AL::ALValue::array("LElbowYaw","LWristYaw");
    AL::ALValue jointAngles =  AL::ALValue::array(ELBOWYAW_FRONT,WRISTYAW_FRONT);
    proxy->setAngles(jointNames, jointAngles, 0.25); //fix unmovable hand joints into position
    jointNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowRoll");
    AL::ALValue jointStiffness = AL::ALValue::array(0.0,0.0,0.0);
    proxy->setStiffnesses(jointNames,jointStiffness); //free other joints so user can move them
    std::cout << std::endl << "Move robot's left hand until enough joint positions are gathered for data gathering." << std::endl;
    wait(10000);
    Pattern *handPositions = new Pattern[maxSamples];
    int s = 0;
    float samplingDistance = SAMPLING_DISTANCE + (DEFAULT_SAMP_COUNT / maxSamples) * SAMPLING_DISTANCE;
    Timer decreaseDistance(20000); //decrease sampling distance every 20 sec
    while (s < maxSamples)
    {
        //get current servo positions
        jointAngles = proxy->getAngles(jointNames,true);
        handPositions[s].value = new float[3];
        handPositions[s].value[0] = (float)jointAngles[0];
        handPositions[s].value[1] = (float)jointAngles[1];
        handPositions[s].value[2] = (float)jointAngles[2];
        //check if current position is far enough from other already gathered positions
        float min = sqrt(
            pow(handPositions[s].value[0] - handPositions[0].value[0],2) +
            pow(handPositions[s].value[1] - handPositions[0].value[1],2) +
            pow(handPositions[s].value[2] - handPositions[0].value[2],2));
        for (int i = 0; i < s; i++)
        {
            float tmp = sqrt(
            pow(handPositions[s].value[0] - handPositions[i].value[0],2) +
            pow(handPositions[s].value[1] - handPositions[i].value[1],2) +
            pow(handPositions[s].value[2] - handPositions[i].value[2],2));
            if (tmp < min)
                min = tmp;
        }
        if (min > samplingDistance || s == 0)
        {
            if (s > 0)
            {
                std::cout << std::endl << "Picking " << s << ". sample." << std::endl;
                std::cout << " - output 1 (shoulder pitch) = " << handPositions[s].value[0] << std::endl;
                std::cout << " - output 2 (shoulder roll ) = " << handPositions[s].value[1] << std::endl;
                std::cout << " - output 3 (elbow roll    ) = " << handPositions[s].value[2] << std::endl;
            }
            decreaseDistance.reset(20000);
            s++;
        }
        //decrease sampling distance if no samples were gathered for a long time
        if (s > 1 && decreaseDistance.isTime())
            samplingDistance = (float)0.9 * samplingDistance;
    }
    int handPositionsCount = s;
    std::cout << std::endl << "Positions for sampling are gathered. Thank you." << std::endl;
    //set hand stiffness
    jointNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowRoll");
    jointStiffness = AL::ALValue::array(HAND_STIFFNESS,HAND_STIFFNESS,HAND_STIFFNESS);
    proxy->setStiffnesses(jointNames,jointStiffness);
    wait(5000);
    //move hand to initial position
    frontPosition();
    //move head to initial position
    head->initialPosition();
    //ask user to put object in robot hand
    proxy->openHand("LHand");
    std::cout << std::endl << "Now put object into robot's hand..." << std::endl;
    wait(2000);
    std::cout << "Press 'Enter' when done. The robot will close it's hand.";
    std::cin.ignore(10000,'\n');
    proxy->closeHand("LHand");
    std::cout << "Press 'Enter' to confirm object position in the hand.";
    std::cin.ignore(10000,'\n');
    //set window to display video
    stereotld->initGui();
    //set head movement to every 50 miliseconds
    bool moveHead = true;
    Timer headTimer(50);
    //prepare variables
    Pattern *inputs = new Pattern[maxSamples];
    Pattern *outputs = new Pattern[maxSamples];
    int inputCount = 3;
    int outputCount = 3;
    Timer moveTimer(20000);
    //take samples
    s = 0;
    for (int i=1; i < handPositionsCount; i++)
    {
        double radianShoulderPitch = (double)handPositions[i].value[0];
        double normalizedShoulderPitch = normalize(radianShoulderPitch, limits->shoulderPitch.min, limits->shoulderPitch.max);
        double radianShoulderRoll = (double)handPositions[i].value[1];
        double normalizedShoulderRoll = normalize(radianShoulderRoll, limits->shoulderRoll.min, limits->shoulderRoll.max);
        double radianElbowRoll = (double)handPositions[i].value[2];
        double normalizedElbowRoll = normalize(radianElbowRoll, limits->elbowRoll.min, limits->elbowRoll.max);
        //process images with tld
        stereotld->processImages();
        //wait to cool down if temperature is high
        if (getTemperature() > MAX_TEMPERATURE)
        {
            stereotld->destroyGui();
            cooldown();
            head->initialPosition();
            stereotld->initGui();
        }
        //store outputs per sample
        outputs[s].value = new float[3];
        outputs[s].value[0] = (float)normalizedShoulderPitch;
        outputs[s].value[1] = (float)normalizedShoulderRoll;
        outputs[s].value[2] = (float)normalizedElbowRoll;
        //move servos into position
        jointNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowRoll");
        jointAngles = proxy->getAngles(jointNames,true);
        jointAngles[0] = radianShoulderPitch - (double)jointAngles[0];
        jointAngles[1] = radianShoulderRoll - (double)jointAngles[1];
        jointAngles[2] = radianElbowRoll - (double)jointAngles[2];
        proxy->changeAngles(jointNames, jointAngles, MOTOR_SPEED);
        //wait until servos get into position
        moveTimer.reset(20000);
        do
        {
            //process images with tld
            stereotld->processImages();
            //refresh gui
            stereotld->refreshGui();
            //handle gui events
            cv::waitKey(2);
            //move head to track object
            if (headTimer.isTime()) head->trackObject(stereotld->object);
            //get current angles
            AL::ALValue jointAngles = proxy->getAngles(jointNames,true);
            jointAngles[0] = radianShoulderPitch - (double)jointAngles[0];
            jointAngles[1] = radianShoulderRoll - (double)jointAngles[1];
            jointAngles[2] = radianElbowRoll - (double)jointAngles[2];
        }
        while (!moveTimer.isTime() && ((double)jointAngles[0] > MOVEMENT_PRECISION || (double)jointAngles[1] > MOVEMENT_PRECISION || (double)jointAngles[2] > MOVEMENT_PRECISION));
        //move to initial position if object is not seen or movement is not precise enough
        if (stereotld->object->valid == false)
        {
            //move servos
            frontPosition();
            //wait until servos move to position
            moveTimer.reset(4000);
            while (!moveTimer.isTime())
            {
                //process images with tld
                stereotld->processImages();
                //refresh gui
                stereotld->refreshGui();
                //handle gui events
                cv::waitKey(2);
                //move head to initial position
                if (headTimer.isTime()) head->initialPosition();
            }
            //print info
            std::cout << std::endl << "Failed to pick sample." << std::endl;
        }
        //else store outputs and increase sample counter
        else
        {
            //normalize object coordinates and store inputs per sample
            AL::ALValue headJoints = AL::ALValue::array("HeadPitch","HeadYaw");
            AL::ALValue headAngles = proxy->getAngles(headJoints,true);
            inputs[s].value = new float[3];
            inputs[s].value[0] = (float)normalize(stereotld->object->dist, limits->distance.min, limits->distance.max);
            inputs[s].value[1] = (float)normalize(stereotld->object->pitch + (double)headAngles[0], limits->viewVertical.min, limits->viewVertical.max);
            inputs[s].value[2] = (float)normalize(stereotld->object->yaw + (double)headAngles[1], limits->viewHorizontal.min, limits->viewHorizontal.max);
            //print info
            std::cout << std::endl << "Picking " << s+1 << ". sample." << std::endl;
            std::cout << " - input  1 (distance      ) = " << stereotld->object->dist << " (normalized = " << inputs[s].value[0] << ")" << std::endl;
            std::cout << " - input  2 (head pitch    ) = " << stereotld->object->pitch + (double)headAngles[0] << " (normalized = " << inputs[s].value[1] << ")" << std::endl;
            std::cout << " - input  3 (head yaw      ) = " << stereotld->object->yaw + (double)headAngles[1] << " (normalized = " << inputs[s].value[2] << ")" << std::endl;
            std::cout << " - output 1 (shoulder pitch) = " << radianShoulderPitch << " (normalized = " << normalizedShoulderPitch << ")" << std::endl;
            std::cout << " - output 2 (shoulder roll ) = " << radianShoulderRoll << " (normalized = " << normalizedShoulderRoll << ")" << std::endl;
            std::cout << " - output 3 (elbow roll    ) = " << radianElbowRoll << " (normalized = " << normalizedElbowRoll << ")" << std::endl;
            //increase sample counter
            s++;
        }
    }
    //close gui
    stereotld->destroyGui();
    //put samples into dataset
    DataSet *trainDataset = new DataSet(s,3,3);
    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            trainDataset->input[i].value[j] = inputs[i].value[j];
            trainDataset->output[i].value[j] = outputs[i].value[j];
        }
        delete [] inputs[i].value;
        delete [] outputs[i].value;
    }
    //cleanup
    delete [] inputs;
    delete [] outputs;
    for (int i = 0; i < handPositionsCount; i++)
    {
        delete [] handPositions[i].value;
    }
    delete [] handPositions;
    //turn stiffness off
    proxy->rest();
    //return dataset
    return trainDataset;
}

void HandController::save(const char *path)
{
    nn->saveToFile(path);
}

void HandController::frontPosition()
{
    AL::ALValue joints = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowYaw","LElbowRoll","LWristYaw");
    AL::ALValue angles = AL::ALValue::array(SHOULDERPITCH_FRONT,SHOULDERROLL_FRONT,ELBOWYAW_FRONT,ELBOWROLL_FRONT,WRISTYAW_FRONT);
    proxy->setAngles(joints, angles, 0.15);
}

void HandController::initialPosition()
{
    AL::ALValue joints = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowYaw","LElbowRoll","LWristYaw");
    AL::ALValue angles = AL::ALValue::array(SHOULDERPITCH_INIT,SHOULDERROLL_INIT,ELBOWYAW_INIT,ELBOWROLL_INIT,WRISTYAW_INIT);
    proxy->setAngles(joints, angles, 0.15);
}

void HandController::grabObject(Object *object)
{
    //get head joint angles
    AL::ALValue headJoints = AL::ALValue::array("HeadPitch","HeadYaw");
    AL::ALValue headAngles = proxy->getAngles(headJoints,true);
    //get information about object spatial location
    double distance = object->dist;
    double pitch = object->pitch + (double)headAngles[0];
    double yaw = object->yaw +  (double)headAngles[1];
    /*TEMP*/std::cout << "distance = " << distance <<std::endl;
    /*TEMP*/std::cout << "pitch = " << pitch <<std::endl;
    /*TEMP*/std::cout << "yaw = " << yaw <<std::endl<<std::endl;
    /*TEMP*/FILE *file = fopen("EXPERIMENT.txt", "a");
    /*TEMP*/fprintf(file, "%lf #distance\n", distance);
    /*TEMP*/fprintf(file, "%lf #pitch\n", pitch);
    /*TEMP*/fprintf(file, "%lf #yaw\n", yaw);
    //normalize values
    distance = normalize(distance, limits->distance.min, limits->distance.max);
    pitch = normalize(pitch, limits->viewVertical.min, limits->viewVertical.max);
    yaw = normalize(yaw, limits->viewHorizontal.min, limits->viewHorizontal.max);
    //build input array
    float *input = new float[3];
    input[0] = (float)distance;
    input[1] = (float)pitch;
    input[2] = (float)yaw;
    //run neural network
    nn->run(input);
    //get outputs
    double shoulderPitch = (double)nn->layer[nn->layerCount-1]->neuron[0]->output;
    double shoulderRoll = (double)nn->layer[nn->layerCount-1]->neuron[1]->output;
    double elbowRoll = (double)nn->layer[nn->layerCount-1]->neuron[2]->output;
    //denormalize outputs
    shoulderPitch = denormalize(shoulderPitch, limits->shoulderPitch.min, limits->shoulderPitch.max);
    shoulderRoll = denormalize(shoulderRoll, limits->shoulderRoll.min, limits->shoulderRoll.max);
    elbowRoll = denormalize(elbowRoll, limits->elbowRoll.min, limits->elbowRoll.max);
    /*TEMP*/std::cout << "shoulderPitch = " << shoulderPitch <<std::endl;
    /*TEMP*/std::cout << "shoulderRoll = " << shoulderRoll <<std::endl;
    /*TEMP*/std::cout << "elbowRoll = " << elbowRoll <<std::endl<<std::endl<<std::endl;
    /*TEMP*/fprintf(file, "%lf #shoulderPitch\n", shoulderPitch);
    /*TEMP*/fprintf(file, "%lf #shoulderRoll\n", shoulderRoll);
    /*TEMP*/fprintf(file, "%lf #elbowRoll\n\n", elbowRoll);
    /*TEMP*/fclose(file);
    //move servos to position
    AL::ALValue jointNames = AL::ALValue::array("LShoulderPitch","LShoulderRoll","LElbowRoll","LElbowYaw","LWristYaw");
    AL::ALValue jointAngles =  AL::ALValue::array(shoulderPitch,shoulderRoll,elbowRoll,ELBOWYAW_FRONT,WRISTYAW_FRONT);
    proxy->setAngles(jointNames, jointAngles, 0.25);
    //cleanup
    delete [] input;
}   
    