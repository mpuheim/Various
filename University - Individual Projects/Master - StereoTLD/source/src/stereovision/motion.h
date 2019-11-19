/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                              motion.h                              * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef MOTION_H_
#define MOTION_H_

#include <opencv2/highgui/highgui.hpp>
#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include "neuralnet.h"
#include "stereotld.h"

#define MOTOR_SPEED         0.01
#define MOVEMENT_PRECISION  0.05
#define SAMPLING_DISTANCE   0.1
#define DEFAULT_SAMP_COUNT  50
#define MAX_TEMPERATURE     75
#define GOOD_TEMPERATURE    50
#define HAND_STIFFNESS      0.7

#define MAX_DISTANCE        0.5 //half meter

#define SHOULDERPITCH_FRONT  0.000000
#define SHOULDERROLL_FRONT   0.000000
#define ELBOWYAW_FRONT      -0.184122
#define ELBOWROLL_FRONT     -0.470896
#define WRISTYAW_FRONT      -1.603650

#define SHOULDERPITCH_INIT  1.439000
#define SHOULDERROLL_INIT   0.276000
#define ELBOWYAW_INIT      -1.376000
#define ELBOWROLL_INIT     -1.000000
#define WRISTYAW_INIT       0.000000

#define WRISTYAW_REST      -1.400000
#define ELBOWYAW_REST      -1.555555

#define HEADYAW_INIT        0.000000
#define HEADPITCH_INIT      0.200000

struct MinMax
{
    double max;
    double min;
};

struct Limits
{
    MinMax shoulderPitch;
    MinMax shoulderRoll;
    MinMax elbowYaw;
    MinMax elbowRoll;
    MinMax wristYaw;
    MinMax viewVertical;
    MinMax viewHorizontal;
    MinMax distance;
};

class HeadController
{
    AL::ALMotionProxy *proxy;
public:
    void initialPosition();
    void trackObject(Object *object);
    
    HeadController(AL::ALMotionProxy *motionProxy);
    virtual ~HeadController();
};

class HandController
{
    AL::ALMotionProxy *proxy;
    AL::ALMemoryProxy *memory;
    NeuralNetwork *nn;
    Limits *limits;
    
    void initLimits(AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition);
    double normalize(double value, double min, double max);
    double denormalize(double value, double min, double max);
    double getTemperature();
    void cooldown();
public:
    bool initialized;

    void init(const char *path); //load neural network
    void init(DataSet *trainDataset); //train new neural network
    void trainOnDataset(DataSet *trainDataset); //train current neural network
    DataSet *createDataset(StereoTLD *stereotld, HeadController *head); //generate training data
    void save(const char *path); //save neural network
    void frontPosition(); //move hand to position in front of robot
    void initialPosition(); //move hand to default position
    void grabObject(Object *object); //move hand to grab object
    
    HandController(AL::ALMotionProxy *motionProxy, AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition);
    virtual ~HandController();
};

class MotionController
{
public:
    AL::ALMotionProxy *proxy;
    HandController *handController;
    HeadController *headController;
    
    void initialPosition();
    void rest();

    MotionController(const std::string& robotIp, AL::ALMemoryProxy *memoryProxy, ImAcquisition *acquisition);
    virtual ~MotionController();
};

#endif // MOTION_H_