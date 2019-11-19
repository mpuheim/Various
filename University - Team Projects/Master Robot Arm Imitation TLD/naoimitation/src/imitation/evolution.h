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

#ifndef EVOLUTION_H_
#define EVOLUTION_H_

#include "imacquisition.h"
#include "trajectory.h"
#include "motion.h"
#include "TLD.h"
#include <alproxies/almemoryproxy.h>
#include <opencv2/core/core.hpp>
#include <vector>

void evolution(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, tld::TLD *detector, Trajectory *trajectory);

class JointPos
{
public:
    double roll;
    double pitch;
    JointPos(double new_roll, double new_pitch);
    virtual ~JointPos();
};

class Individual
{
public:
    unsigned int id;
    int chromosomeLength;
    float samplePeriod;
    float motionDuration;
    float fitness;
    std::vector<JointPos> genotype;
    std::vector<cv::Point> fenotype;
    
    Individual(int lenght, float period, float duration);
    Individual(Individual parent1, Individual parent2, int mutation);
    virtual ~Individual();
    int decode(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, tld::TLD *detector, Trajectory *trajectory); //decode genotype into fenotype
    float evaluate(Trajectory *refTrajectory); //calculate fitness of fenotype
    void show(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, tld::TLD *detector, Trajectory *trajectory);
};

class Population
{
    std::vector<Individual> parent;
    std::vector<Individual> child;
    
    ImAcquisition *acquisition;
    AL::ALMotionProxy *motionproxy;
    AL::ALMemoryProxy *memoryproxy;
    tld::TLD *detector;
    Trajectory *trajectory;
    
    void generateNew(); //generate new population
    void selection(int parent_count); //select parents
    void reproduction(int children_count, int mutation); //produce children
    void acceptation(); //accept children into population
    
public:
    int size;
    int generation;
    std::vector<Individual> individual;
    
    Population(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy, tld::TLD *detector, Trajectory *trajectory);
    Population(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, AL::ALMemoryProxy *memoryproxy, tld::TLD *detector, Trajectory *trajectory, const char *filename);
    virtual ~Population();
    void evolve();
    void writeToFile(const char *path);
};

#endif // EVOLUTION_H_