/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                            evolution.cpp                           * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#include "evolution.h"
#include "timer.h"
#include <math.h>
#include <algorithm>
#include <ctime>
#include <iostream> //TEMP

#define MAX_NUM     RAND_MAX/2
#define MIN_NUM     -RAND_MAX/2

//compare individuals using fitness
bool compare(const Individual &x, const Individual &y)
{
    if (x.fitness == -1)
        return false;
    if (y.fitness == -1)
        return true;
	return x.fitness < y.fitness;
}

/*creates unique new ID*/
unsigned int assign_new_id(unsigned int value)
{
    static unsigned int ID = 0;
    if (value == 0)
    {
        ID++;
        return ID;
    }
    if (value > ID)
        ID = value;
    return value;
}
unsigned int assign_new_id()
{
    return assign_new_id(0);
}

Individual::Individual(int lenght)
{
    //set member variables
    id = assign_new_id();
    chromosomeLength = lenght;
    fitness = -1;
    //generate random genotype
    chromosome.reserve(chromosomeLength);
    for (int i=0; i<chromosomeLength; i++)
    {
        //random number from (- RAND_MAX/2) to (+ RAND_MAX/2)
        float init_value = (float)rand() - RAND_MAX/2;
        chromosome.push_back(init_value);
    }
}

Individual::Individual(Individual parent1, Individual parent2, int mutation)
{
    //set member variables
    id = assign_new_id();
    chromosomeLength = parent1.chromosomeLength;
    fitness = -1;
    //generate random genotype
    chromosome.reserve(chromosomeLength);
        
    //***********crossover***********
    
    //inicialization of random crosspoints
    int cross1 = rand() % chromosomeLength + 1;
    int cross2 = rand() % chromosomeLength + 1;
    if (cross1 > cross2)
    {
      int temp = cross1;
      cross1 = cross2;
      cross2 = temp;
    }    
    for (int i=0; i<cross1; i++)
      chromosome.push_back(parent1.chromosome[i]);
    for (int i=cross1; i<cross2; i++)
      chromosome.push_back(parent2.chromosome[i]);
    for (int i=cross2; i<chromosomeLength; i++)
      chromosome.push_back(parent1.chromosome[i]);
      
    //***********mutation***********
    
    int roll;
    int diff;
    float new_value;
    for (int i=0; i<chromosomeLength; i++)
    {
        //mutate roll component
        roll = rand() % 101;
        if (roll < mutation)
        {
            diff = rand() % 10;
            switch(diff)
            {
                case 0:
                    //increase coefficient value (*2)
                    new_value = 2*chromosome[i];
                    if (new_value < MAX_NUM && new_value > MIN_NUM)
                        chromosome[i] = new_value;
                    break;
                case 1:
                    //decrease coefficient value (/2)
                    chromosome[i] *= 0.5;
                    break;
                case 2:
                    //invert coefficient sign
                    chromosome[i] = -chromosome[i];
                    break;
                case 3:
                    //set coefficient to zero
                    chromosome[i] = 0;
                    break;
                default:
                    //change value to one from interval (value-5,value+5)
                    roll = rand() % 2;
                    if (roll == 0)
                        chromosome[i] = chromosome[i] + 5 * (float)rand() / RAND_MAX;
                    else
                        chromosome[i] = chromosome[i] - 5 * (float)rand() / RAND_MAX;
            }
        }
    }
}

void Individual::evaluate(std::vector<Sample> *dataVector)
{
    //data are provided as series of (x,y)
    //individual represents a set of polynomial coefficients [a_0, a_1, ..., a_(N-1), a_N].
    //f(x)= a_N*x^N + a_(N-1)*x^(N-1) + ... + a_2*x^2 + a_1*x + a0;
    //fitness is calculated as difference betveen f(x) and y
    fitness = 0;
    for (unsigned int i = 0; i < dataVector->size(); i++)
    {
        float fx = 0;
        for(int j = 0; j < chromosomeLength; j++)
        {
            //calculate j-th term of f(x) as a_j*x^j
            fx += chromosome[j] * pow((*dataVector)[i].x, j);
        }
        fitness += abs(fx-(*dataVector)[i].y);
    }
    fitness = fitness/dataVector->size();
}

Individual::~Individual()
{
    /*cleanup*/
}


Population::Population(int individualLength, Sample *data, int dataCount)
{
    //set population size and current generation
    size = POP_SIZE;
    generation = 0;
    //create data vector
    dataVector.reserve(dataCount);
    for (int i=0; i < dataCount; i++)
        dataVector.push_back(data[i]);
    //initialize random number generator
    srand((unsigned int)time(NULL));
    //generate individuals
    individual.reserve(size);
    for (int i=0; i < size; i++)
    {
        Individual ind(individualLength);
        ind.evaluate(&dataVector);
        individual.push_back(ind);
    }
    //sort population by fitness
    std::sort(individual.begin(),individual.end(),compare);
}

void Population::selection(int parent_count)
{
    //selection (parent pick) - requires sorted population
    parent.clear();
    for (int i=0; i < parent_count; i++)
        parent.push_back(individual[i]);
}

void Population::reproduction(int children_count, int mutation)
{
    //create children
    child.clear();
    int parent_count = (int)parent.size();
    for (int i=0; i < children_count; i++)
    {
        Individual new_child(parent[i%parent_count], parent[rand()%parent_count], mutation);
        //std::cout << std::endl << generation << ". generation, " << i+1 << ". child, ID" << new_child.id << std::endl;
        new_child.evaluate(&dataVector);
        //std::cout << "Fitness = " << new_child.fitness << std::endl;
        child.push_back(new_child);
    }
}

void Population::acceptation()
{
    //remove worst individuals in population - requires sorted population
    for (unsigned int i=0; i < child.size(); i++)
        individual.pop_back();
    //add children to population
    for (unsigned int i=0; i < child.size(); i++)
        individual.push_back(child[i]);
}

void Population::evolve()
{
    evolve((float)DEF_MAX_ERROR,(double)DEF_TIMELIMIT);
}

void Population::evolve(float fitnessGoal, double timeLimit)
{
    int parent_count = PARENT_COUNT;
    int children_count = CHILDREN_COUNT;
    int mutation = MUTATION;
    //start evolution
    //std::cout << "Starting evolution..." << std::endl;
    //set time limiter
    Timer timeLimiter(timeLimit);
    double elapsed_ms = timeLimit;
    //evolution
    while (timeLimiter.isTime() == false)
    {
        //sort population
        std::sort(individual.begin(),individual.end(),compare);
        //stop evolution if fitness is good enough
        if (individual[0].fitness < fitnessGoal)
        {
            elapsed_ms = timeLimiter.getElapsedTime();
            break;
        }
        //pick parents
        selection(parent_count);
        //create children
        reproduction(children_count, mutation);
        //replace worst individuals
        acceptation();
        //increase generation counter
        generation++;
    }
    std::sort(individual.begin(),individual.end(),compare);
    std::cout << "Evolution finished after " << (int)elapsed_ms << "ms in " << generation << ". generation." << std::endl;
    std::cout << "Best fitness: " << individual[0].fitness << std::endl;
}

Population::~Population()
{
    /*cleanup*/;
}

void polyfit(float *poly_coeffs, int poly_degree, Sample *data, int dataCount)
{
    polyfit(poly_coeffs, poly_degree, data, dataCount, (float)DEF_MAX_ERROR,(double)DEF_TIMELIMIT);
}
void polyfit(float *poly_coeffs, int poly_degree, Sample *data, int dataCount, float errorTolerance, double timeLimit)
{
    Population pop(poly_degree, data, dataCount);
    pop.evolve(errorTolerance,timeLimit);
    for (int i=0; i < poly_degree; i++)
    {
        poly_coeffs[i] = pop.individual[0].chromosome[i];
    }
}
