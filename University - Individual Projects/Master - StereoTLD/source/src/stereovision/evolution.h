/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                             evolution.h                            * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef EVOLUTION_H_
#define EVOLUTION_H_

#include <vector>

#define POP_SIZE            100         //population size
#define PARENT_COUNT        50          //number of parents picked in generation
#define CHILDREN_COUNT      70          //number of children produced per generation
#define MUTATION            10          //integer number as percentage
#define DEF_TIMELIMIT       5000        //default maximum time of evolution in miliseconds
#define DEF_MAX_ERROR       0.01        //default error tolerance

struct Sample
{
    float x;
    float y;
};

class Individual
{
public:
    unsigned int id;
    float fitness;
    int chromosomeLength;
    std::vector<float> chromosome;
    
    Individual(int lenght);
    Individual(Individual parent1, Individual parent2, int mutation);
    void evaluate(std::vector<Sample> *dataVector); //calculate fitness
    virtual ~Individual();
};

class Population
{
    std::vector<Individual> parent;
    std::vector<Individual> child;
    std::vector<Sample> dataVector;
    
    void selection(int parent_count); //select parents
    void reproduction(int children_count, int mutation); //produce children
    void acceptation(); //accept children into population
    
public:
    int size;
    int generation;
    std::vector<Individual> individual;
    
    Population(int individualLength, Sample *data, int dataCount);
    void evolve();
    void evolve(float fitnessGoal, double timeLimit);
    virtual ~Population();
};

void polyfit(float *poly_coeffs, int poly_degree, Sample *data, int dataCount); //sets poly_coeffs as [a_0, a_1, ..., a_(N-1), a_N]
void polyfit(float *poly_coeffs, int poly_degree, Sample *data, int dataCount, float errorTolerance, double timeLimit);

#endif // EVOLUTION_H_
