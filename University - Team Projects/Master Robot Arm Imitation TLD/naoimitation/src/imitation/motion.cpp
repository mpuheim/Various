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

#include "motion.h"
#include <iostream>
#include <ctime>

#define MOTION_SPEED 0.025f

double checkPitch(double roll, double pitch);
double checkRoll(double roll, double pitch);

void teleoperate(AL::ALMotionProxy *motion, char c)
{
    AL::ALValue sensorAngles;
    switch ((char)c)
    {
      case 'i': //initial pose
        initPose(motion);
        break;
      case 'p': //print joint angles
        sensorAngles = motion->getAngles("RShoulderPitch", true);
        std::cout << "Pitch: " << sensorAngles << std::endl;
        sensorAngles = motion->getAngles("RShoulderRoll", true);
        std::cout << "Roll: " << sensorAngles << std::endl;
        break;
      case 'w': // move up
        moveToPosition(motion, 0, -0.06f, true);
        break;
      case 'd': // move right
        moveToPosition(motion, -0.06f, 0, true);
        break;
      case 'a': // move left
        moveToPosition(motion, 0.06f, 0, true);
        break;
      case 's': // move down
        moveToPosition(motion, 0, 0.06f, true);
        break;
      break;
    }
}

void printControls()
{
    std::cout << "Teleoperation controls: " << std::endl;
    std::cout << "w - move up" << std::endl;
    std::cout << "A - move left" << std::endl;
    std::cout << "S - move down" << std::endl;
    std::cout << "D - move right" << std::endl;
    std::cout << "I - move to initial position" << std::endl;
    std::cout << "P - print joint angles" << std::endl;
}

double checkRoll(double roll, double pitch)
{
  double upraveny_roll;
  if (roll >= ((-0.46) * pitch - 0.767))
  {
    if (roll <= ((-0.22) * pitch - 0.143))
    {
      upraveny_roll = roll;
    }
    else 
    {
      upraveny_roll = ((-0.22) * pitch - 0.143);
      //std::cout << "Upraveny roll-> Pohyb dolava mimo hranic: " << upraveny_roll << std::endl;
    }
  }
  else 
  {
    upraveny_roll = ((-0.46) * pitch - 0.767);
    //std::cout << "Upraveny roll-> Pohyb doprava mimo hranic: " << upraveny_roll << std::endl;
  }
  //std::cout << upraveny_roll << std::endl;
  return upraveny_roll;
}

double checkPitch(double roll, double pitch)
{
  double upraveny_pitch;
  if (pitch >= ((roll - 0.368) / (4.23)))
  {
    if (pitch <= ((roll - 1.069) / (-5.34)))
    {
      upraveny_pitch = pitch;
    }
    else 
    {
      upraveny_pitch = ((roll - 1.069) / (-5.34));
      //std::cout << "Upraveny pitch-> Pohyb dole mimo hranic: " << upraveny_pitch << std::endl;
    }
  }
  else 
  {
    upraveny_pitch = ((roll - 0.368) / (4.23));
    //std::cout << "Upraveny pitch-> Pohyb hore mimo hranic: " << upraveny_pitch << std::endl;
  }
  //std::cout << upraveny_pitch << std::endl;
  return upraveny_pitch;
}

void positionFix(double *roll, double *pitch)
{
    *roll = checkRoll(*roll, *pitch);
    *pitch = checkPitch(*roll, *pitch);
}

void moveToPosition(AL::ALMotionProxy *motion, double roll, double pitch)
{
    moveToPosition(motion, roll, pitch, false);
}

void moveToPosition(AL::ALMotionProxy *motion, double roll, double pitch, bool relative)
{
    //get current values
    double curr_roll = motion->getAngles("RShoulderRoll", true)[0];
    double curr_pitch = motion->getAngles("RShoulderPitch", true)[0];
    //declare changes
    double roll_diff;
    double pitch_diff;
    //set roll and pitch for relative move (using only change in joint values)
    if (relative)
    {
        roll += curr_roll;
        pitch += curr_pitch;
        positionFix(&roll, &pitch);
        roll_diff = roll - curr_roll;
        pitch_diff = pitch - curr_pitch;
    }
    //set roll and pitch for absolute move (using absolute joint values)
    else
    {
        positionFix(&roll, &pitch);
        roll_diff = roll - curr_roll;
        pitch_diff = pitch - curr_pitch;
    }
    //move
    motion->changeAngles("RShoulderRoll", roll_diff, MOTION_SPEED);
    motion->changeAngles("RShoulderPitch", pitch_diff, MOTION_SPEED);
}

void initPose(AL::ALMotionProxy *motion)
{
    motion->setAngles("HeadYaw", -0.8f, MOTION_SPEED);
    motion->setAngles("HeadPitch", -0.05f, MOTION_SPEED);
    motion->setAngles("RShoulderRoll", -0.47f, MOTION_SPEED);
    motion->setAngles("RShoulderPitch", 0.02f, MOTION_SPEED);
    motion->setAngles("RElbowYaw", -0.04f, MOTION_SPEED);
    motion->setAngles("RElbowRoll", 0.0f, MOTION_SPEED);
    motion->setAngles("RWristYaw", 1.70f, MOTION_SPEED);
}

double getTemperature(AL::ALMemoryProxy *memoryproxy)
{
    double temp1 = memoryproxy->getData("Device/SubDeviceList/RShoulderRoll/Temperature/Sensor/Value");
    double temp2 = memoryproxy->getData("Device/SubDeviceList/RShoulderPitch/Temperature/Sensor/Value");
    if (temp1 > temp2)
        return temp1;
    else
        return temp2;
}

void pause_ms(int miliseconds)
{
    std::clock_t start_clock = std::clock();
    std::clock_t current_clock = start_clock;
    double elapsed_msecs = 1000 * double(current_clock - start_clock) / CLOCKS_PER_SEC;
    while(miliseconds > (int)elapsed_msecs)
    {
        current_clock = std::clock();
        elapsed_msecs = 1000 * double(current_clock - start_clock) / CLOCKS_PER_SEC;
    }
}

//calling this function before setting stiffness prevents damage to robot because of rapid motion when servos are trying to get fast into last position
void initServos(AL::ALMotionProxy *motion)
{
    AL::ALValue startup_pitch;
    AL::ALValue startup_roll;
    startup_pitch = motion->getAngles("RShoulderPitch", true);
    startup_roll = motion->getAngles("RShoulderRoll", true);
    motion->setAngles("RShoulderPitch", startup_pitch, 1);
    motion->setAngles("RShoulderRoll", startup_roll, 1);
    pause_ms(500);
}

void cooldown(AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy)
{
    cooldown(motionproxy, memoryproxy, true);
}

void cooldown(AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy, bool verbose)
{
    double current_temp = getTemperature(memoryproxy);
    if (verbose)
        std::cout << "Shoulder temperature: " << current_temp << std::endl;
    if (current_temp > 75)
    {
        std::cout << "Need to cool down..." << std::endl;
        //decrease stifness (to put hand down slowly)
        motionproxy->setStiffnesses("RShoulderRoll", 0.01f);
        motionproxy->setStiffnesses("RShoulderPitch", 0.01f);
        pause_ms(6000);
        //wait to cool down
        while (current_temp > 55)
        {
            //print info
            std::cout << "Shoulder temperature (" << current_temp << ") is above acceptable levels. Waiting to cool down." << std::endl;
            //turn stiffness off
            motionproxy->setStiffnesses("RShoulderRoll", 0.0f);
            motionproxy->setStiffnesses("RShoulderPitch", 0.0f);
            //wait for 1 minute
            pause_ms(60000);
            //check temperature again
            current_temp = getTemperature(memoryproxy);
        }
        //print info about end of cooldown
        std::cout << "Shoulder temperature (" << current_temp << ") is OK. Proceeding..." << std::endl;
        //turn stiffness on
        motionproxy->setStiffnesses("RShoulderRoll", 0.8f);
        motionproxy->setStiffnesses("RShoulderPitch", 0.8f);
        //go to init pose
        initPose(motionproxy);
        pause_ms(5000);
    }
}
