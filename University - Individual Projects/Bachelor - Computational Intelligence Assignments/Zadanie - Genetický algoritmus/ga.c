//Genetic algorithm - Sample version - M.PUHEIM 2011

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define POPULATION_SIZE 1000
#define MAX_GENERATIONS 10000
#define ELITES 5
#define MUTATION 10
#define EQUATION (A*sin(B)+C*cos(D)/(E*F))

//Chromozome definition
typedef struct
{
    double A;
    double B;
    double C;
    double D;
    double E;
    double F;
    double fittness;
} chromozome_t;

//Random number generator (from 0 to max)
double random_number(double max)
{
    if (max<1)
        max = 1;
    int integer_part = rand() % ((int)max);
    int fractional_part = rand() % 10001;
    return (double)integer_part + (double)fractional_part / 10000;
}

//Fittness of chromozome
double get_fittness(chromozome_t ch)
{
    double A,B,C,D,E,F,result;
    A = ch.A;
    B = ch.B;
    C = ch.C;
    D = ch.D;
    E = ch.E;
    F = ch.F;
    result = EQUATION;
    if (result > 0)
        return result;
    else
        return 0;
}

//Sort population by fittness
void sort_population(chromozome_t* ch)
{
    //Sort population table
    double value;
    int i,j;
    chromozome_t temp;
    int size = sizeof(chromozome_t);
    for (j = 1; j < POPULATION_SIZE; j++)
    {
        value = ch[j].fittness;
        memcpy(&temp,&(ch[j]),size);
        i = j - 1;
        while ((i >= 0) && (ch[i].fittness < value))
        {
            memcpy(&(ch[i+1]),&(ch[i]),size);
            i -= 1;
        }
        memcpy(&(ch[i+1]),&temp,size);
    }
}

//Initialization of population
void initialize_population(chromozome_t* ch)
{
    int i;
    
    //Initialize random number generator
    srand ( (unsigned)time ( NULL ) );
    //Build whole population
    for (i=0; i < POPULATION_SIZE;i++)
    {
        ch[i].A = random_number(1000);
        ch[i].B = random_number(1000);
        ch[i].C = random_number(1000);
        ch[i].D = random_number(1000);
        ch[i].E = random_number(1000);
        ch[i].F = random_number(1000);
        ch[i].fittness = get_fittness(ch[i]);
    }
    //sort population by fittness
    sort_population(ch);
}

//Find index of best individual in population
int best_individual(chromozome_t* ch)
{
    double max_fitness = ch[0].fittness;
    int max_index = 0;
    int i;
    for (i=1; i<POPULATION_SIZE; i++)
    {
        if (max_fitness < ch[i].fittness)
        {
            max_fitness = ch[i].fittness;
            max_index = i;
        }
    }
    return max_index;
}

//Total fittness of population
double total_fittness(chromozome_t* ch)
{
    int i;
    int sum = 0;
    for (i=0; i<POPULATION_SIZE; i++)
        sum += ch[i].fittness;
    return sum;
}

//Create new generation
void new_generation(chromozome_t* ch)
{
    //create new individuals
    int i,j;
    int parent_A, parent_B, crosspoint;
    int ch_size = sizeof(chromozome_t);
    chromozome_t new[POPULATION_SIZE - ELITES];
    int wheel_size = total_fittness(ch);
    double wheel_turn;
    for (i=0; i<POPULATION_SIZE - ELITES; i++)
    {
        //Pick parent A
        parent_A = -1;
        wheel_turn = random_number(wheel_size);
        do
        {
            parent_A++;
            wheel_turn -= ch[parent_A].fittness;  //Find member of population whose turn it is
        } while ((wheel_turn > 0) || (parent_A >= POPULATION_SIZE));
        //Pick parent B
        parent_B = -1;
        wheel_turn = random_number(wheel_size);
        do
        {
            parent_B++;
            wheel_turn -= ch[parent_B].fittness;  //Find member of population whose turn it is
        } while ((wheel_turn > 0) || (parent_B >= POPULATION_SIZE));
        //Crossover
        crosspoint = rand() % 7; //random number form 0 to 6
        switch (crosspoint)
        {
            case 0:
                memcpy(&(new[i]),&(ch[parent_B]),ch_size);
                break;
            case 1:
                memcpy(&(new[i]),&(ch[parent_B]),ch_size);
                new[i].A = ch[parent_A].A;
                break;
            case 2:
                memcpy(&(new[i]),&(ch[parent_B]),ch_size);
                new[i].A = ch[parent_A].A;
                new[i].B = ch[parent_A].B;
                break;
            case 3:
                memcpy(&(new[i]),&(ch[parent_B]),ch_size);
                new[i].A = ch[parent_A].A;
                new[i].B = ch[parent_A].B;
                new[i].C = ch[parent_A].C;
                break;
            case 4:
                memcpy(&(new[i]),&(ch[parent_A]),ch_size);
                new[i].E = ch[parent_B].E;
                new[i].F = ch[parent_B].F;
                break;
            case 5:
                memcpy(&(new[i]),&(ch[parent_A]),ch_size);
                new[i].F = ch[parent_B].F;
                break;
            case 6:
                memcpy(&(new[i]),&(ch[parent_A]),ch_size);
                break;
        }
        //Mutation
        if (rand() % 101 < MUTATION)
            new[i].A = new[i].A + random_number(2+abs(new[i].A)/2) - random_number(2+abs(new[i].A)/2);
        if (rand() % 101 < MUTATION)
            new[i].B = new[i].B + random_number(2+abs(new[i].B)/2) - random_number(2+abs(new[i].B)/2);
        if (rand() % 101 < MUTATION)
            new[i].C = new[i].C + random_number(2+abs(new[i].C)/2) - random_number(2+abs(new[i].C)/2);
        if (rand() % 101 < MUTATION)
            new[i].D = new[i].D + random_number(2+abs(new[i].D)/2) - random_number(2+abs(new[i].D)/2);
        if (rand() % 101 < MUTATION)
            new[i].E = new[i].E + random_number(2+abs(new[i].E)/2) - random_number(2+abs(new[i].E)/2);
        if (rand() % 101 < MUTATION)
            new[i].F = new[i].F + random_number(2+abs(new[i].F)/2) - random_number(2+abs(new[i].F)/2);
        //Set fittness
        new[i].fittness = get_fittness(new[i]);
    }
    //Put new individuals into population
    j=0;
    for (i=ELITES; i<POPULATION_SIZE; i++)
    {
        memcpy(&(ch[i]),&(new[j]),ch_size);
        j++;
    }
    //sort population by fittness
    sort_population(ch);
}

//Main function
main()
{
    //Create initial population
    chromozome_t population[POPULATION_SIZE];
    initialize_population(population);
    
    //Start genetic algorithm
    int generation = 0;
    printf("Starting genetic algorithm.\n");
    printf("%5d. generation: Best fittness: %0.2lf\n",generation,population[0].fittness);
    while ((generation < MAX_GENERATIONS) && (population[0].fittness < (double)INT_MAX))
    {
        new_generation(population);
        generation++;
        printf("%5d. generation: Best fittness: %0.2lf\n",generation,population[0].fittness);
    }
    printf("Genetic algorithm finished.\n");

    //Print results
    printf("\nBest solution has fittness of %0.2lf.\n",population[0].fittness);
    printf("Solution equation: %0.2lf*sin(%0.2lf)+%0.2lf*cos(%0.2lf)/(%0.2lf*%0.2lf).\n",population[0].A,population[0].B,population[0].C,population[0].D,population[0].E,population[0].F);
    getchar();
}

