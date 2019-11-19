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

#include "evolution.h"
#include "motion.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

//joint limits
#define ROLL_MIN    -0.944
#define ROLL_MAX    -0.118
#define ROLL_LIM     ROLL_MAX - ROLL_MIN
#define PITCH_MIN   -0.610    //-0.242
#define PITCH_MAX    0.377
#define PITCH_LIM    PITCH_MAX - PITCH_MIN

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

cv::Point bboxCenter(cv::Rect *BB)
{
    int x = BB->x + (BB->width/2);
    int y = BB->y + (BB->height/2);
    return cv::Point(x,y);
}

cv::Mat createTrajectoryMask(cv::Mat img, Trajectory *trajectory)
{
    cv::Mat mask = cv::Mat::zeros(img.rows,img.cols,CV_8U);
    for (int i = 1; i <trajectory->sampleCount; i++)
    {
        //draw sample point
        cv::circle(mask, trajectory->sample[i], 5, cv::Scalar(1), -1);
        //connect points with line
        cv::line(mask, trajectory->sample[i-1], trajectory->sample[i], cvScalar(1, 1, 1), 2);
    }
    cv::circle(mask, trajectory->sample[0], 5, cv::Scalar(1), -1);
    return mask;
}

JointPos::JointPos(double new_roll, double new_pitch)
{
    roll = new_roll;
    pitch = new_pitch;
}

JointPos::~JointPos()
{
    /*cleanup*/;
}

Individual::Individual(int lenght, float period, float duration)
{
    //set member variables
    id = assign_new_id();
    chromosomeLength = lenght;
    samplePeriod = period;
    motionDuration = duration;
    fitness = -1;
    //generate random genotype
    genotype.reserve(chromosomeLength);
    for (int i=0; i<chromosomeLength; i++)
    {
        double roll = (ROLL_LIM * ( (double)rand()/((double)RAND_MAX) ) ) + ROLL_MIN;
        double pitch = (PITCH_LIM * ( (double)rand()/((double)RAND_MAX) ) ) + PITCH_MIN;
        //std::cout << "Generated Roll: " << roll << std::endl;
        //std::cout << "Generated Pitch: " << pitch << std::endl;
        positionFix(&roll, &pitch);
        //std::cout << "Fixed Roll: " << roll << std::endl;
        //std::cout << "Fixed Pitch: " << pitch << std::endl;
        genotype.push_back(JointPos(roll,pitch));
        //std::cout << "Saved Roll: " << genotype[i].roll << std::endl;
        //std::cout << "Saved Pitch: " << genotype[i].pitch << std::endl;
    }
}

Individual::Individual(Individual parent1, Individual parent2, int mutation)
{
    //set member variables
    id = assign_new_id();
    chromosomeLength = parent1.chromosomeLength;
    samplePeriod = parent1.samplePeriod;
    motionDuration = parent1.motionDuration;
    fitness = -1;
    //generate random genotype
    genotype.reserve(chromosomeLength);
        
    //***********crossover***********
    
    //inicialization random crosspoints
    int cross1 = rand() % chromosomeLength + 1;
    int cross2 = rand() % chromosomeLength + 1;
    if (cross1 > cross2)
    {
      int temp = cross1;
      cross1 = cross2;
      cross2 = temp;
    }
    //std::cout << "1. crosspoint at " << cross1 << std::endl;
    //std::cout << "2. crosspoint at " << cross2 << std::endl;    
    for (int i=0; i<cross1; i++)
      genotype.push_back(parent1.genotype[i]);
    for (int i=cross1; i<cross2; i++)
      genotype.push_back(parent2.genotype[i]);
    for (int i=cross2; i<chromosomeLength; i++)
      genotype.push_back(parent1.genotype[i]);
      
    //***********mutation***********
    
    int ran;
    int mutate_roll;
    int mutate_pitch;
    double roll;
    double pitch;
    for (int i=0; i<chromosomeLength; i++)
    {
      //mutate roll component
      mutate_roll = rand() % 101;
      if (mutate_roll < mutation)
      {
        if (mutate_roll < mutation / 2)
        {
          ran = rand() % 2;
          if(ran ==0)
            roll = genotype[i].roll*1.1;
          else
            roll = genotype[i].roll*0.9;
        }
        else
        {
          ran = rand() % 2;
          if(ran ==0)
            roll = genotype[i].roll*1.05;
          else
            roll = genotype[i].roll*0.95;
        }
      }
      //mutate pitch component
      mutate_pitch = rand() % 101;
      if (mutate_pitch < mutation)
      {
        if (mutate_pitch < mutation / 2)
        {
          ran = rand() % 2;
          if(ran ==0)
            pitch = genotype[i].pitch*1.1;
          else
            pitch = genotype[i].pitch*0.9;
        }
        else
        {
          ran = rand() % 2;
          if(ran ==0)
            pitch = genotype[i].pitch*1.05;
          else
            pitch = genotype[i].pitch*0.95;
        }
      }
      //position fix of new roll and pitch if changed
      if(mutate_roll < mutation || mutate_pitch < mutation)
      {
        positionFix(&roll, &pitch);
        genotype[i].pitch = pitch;
        genotype[i].roll = roll;
      }
    }
}

Individual::~Individual()
{
    /*cleanup*/
}

int Individual::decode(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, tld::TLD *detector, Trajectory *trajectory)
{
    //get current image
    cv::Mat frame = acquisition->getImage();
    //set reference trajectory mask
    cv::Mat ref_traj_mask = createTrajectoryMask(frame, trajectory);
    //show trajectory on image
    frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
    //set window to display video
	std::string winname = "Decoding individual...";
    cv::namedWindow(winname);
    //show image
    cv::imshow(winname, frame);
    //set current trajectory mask
    cv::Mat cur_traj_mask = cv::Mat::zeros(frame.rows,frame.cols,CV_8U);
    //set initial pose
    moveToPosition(motionproxy, genotype[0].roll, genotype[0].pitch);
    //set wait marker (to get to initial pose)
    bool wait = true;
    //init fenotype
    fenotype.clear();
    fenotype.reserve(chromosomeLength);
    //prepare timers
    std::clock_t current_clock;
    std::clock_t start_clock;
    double elapsed_secs;
    //sample counter
    int sample_move = 0;
    int sample_scan = 1;
    //decode loop
    start_clock = std::clock();
    while (1)
    {
        //get frame
        frame = acquisition->getImage();
        //process image with TLD
        detector->processImage(frame);
        //show current bounding box
        if (detector->currBB)
        {
		    cv::Point A;
		    A.x = detector->currBB->x;
		    A.y = detector->currBB->y;
		    cv::Point B;
		    B.x = detector->currBB->x + detector->currBB->width;
		    B.y = detector->currBB->y;
		    cv::Point C;
		    C.x = detector->currBB->x + detector->currBB->width;
		    C.y = detector->currBB->y + detector->currBB->height;
		    cv::Point D;
		    D.x = detector->currBB->x;
		    D.y = detector->currBB->y + detector->currBB->height;
		    //draw bb to image
		    cv::line(frame, A, B, cvScalar(0, 255, 0), 2);
		    cv::line(frame, B, C, cvScalar(0, 255, 0), 2);
		    cv::line(frame, C, D, cvScalar(0, 255, 0), 2);
		    cv::line(frame, D, A, cvScalar(0, 255, 0), 2);
        }
        //apply trajectory overlays
        frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
        frame.setTo(cv::Scalar(0,255,255),cur_traj_mask);
        //display image
        cv::imshow(winname, frame);
        //handle events
        cv::waitKey(2);
        //update timers
        current_clock = std::clock();
        elapsed_secs = double(current_clock - start_clock) / CLOCKS_PER_SEC;
        //wait for initial pose
        if (wait)
        {
            double current_pitch = (double)motionproxy->getAngles("RShoulderPitch", true)[0];
            double current_roll = (double)motionproxy->getAngles("RShoulderRoll", true)[0];
            if ((std::abs(current_roll - genotype[0].roll) < 0.1f && std::abs(current_pitch - genotype[0].pitch) < 0.1f) || elapsed_secs > 8)
            {
                //restart start time and begin decoding
                wait = false;
                start_clock = std::clock();
            }
            //do not start motion while not in initial position
            continue;
        }
        //convert joint position to object location on frame
        if (elapsed_secs > sample_scan * samplePeriod)
        {
            if (!detector->currBB)
            {
                std::cerr << "Error while decoding. Bounding box not detected." << std::endl;
                fenotype.clear();
                for (int i = 0; i < chromosomeLength; i++)
                    fenotype.push_back(cv::Point(-1,-1));
                cv::destroyWindow(winname);
                return 1;
            }
            fenotype.push_back(bboxCenter(detector->currBB));
            cv::circle(cur_traj_mask, fenotype[sample_scan-1], 5, cv::Scalar(1), -1);
            if (sample_scan > 1)
                cv::line(cur_traj_mask, fenotype[sample_scan-1], fenotype[sample_scan-2], cvScalar(1), 2);
            std::cout << "Taking motion sample [" << fenotype[sample_scan-1].x << ", " << fenotype[sample_scan-1].y << "] at time " << elapsed_secs << "." << std::endl;
            sample_scan++;
        }
        //end if time is greater than movement time
        if (sample_scan > chromosomeLength)
        {
            //apply trajectory overlays
            frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
            frame.setTo(cv::Scalar(0,255,255),cur_traj_mask);
            //display image
            cv::imshow(winname, frame);
            cv::waitKey(50);
            cv::destroyWindow(winname);
            return 0;
        }
        //move if time is up
        if (elapsed_secs > sample_move * samplePeriod)
        {
            moveToPosition(motionproxy, genotype[sample_move].roll, genotype[sample_move].pitch);
            sample_move++;
        }
    }
}

float Individual::evaluate(Trajectory *refTrajectory)
{
    fitness = 0;
    for (int i = 0; i < chromosomeLength; i++)
    {
        fitness += sqrt (
            pow((float)(fenotype[i].x - refTrajectory->sample[i].x), 2) +
            pow((float)(fenotype[i].y - refTrajectory->sample[i].y), 2));
    }
    fitness = fitness/chromosomeLength;
    return fitness;
}

void Individual::show(ImAcquisition *acquisition, AL::ALMotionProxy *motionproxy, tld::TLD *detector, Trajectory *trajectory)
{
    //get current image
    cv::Mat frame = acquisition->getImage();
    //set reference trajectory mask
    cv::Mat ref_traj_mask = createTrajectoryMask(frame, trajectory);
    //show trajectory on image
    frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
    //set window to display video
	std::string winname = "Decoding individual...";
    cv::namedWindow(winname);
    //show image
    cv::imshow(winname, frame);
    //set current trajectory mask
    cv::Mat cur_traj_mask = cv::Mat::zeros(frame.rows,frame.cols,CV_8U);
    //set fenotype trajectory mask
    cv::Mat fen_traj_mask = cv::Mat::zeros(frame.rows,frame.cols,CV_8U);
    //set initial pose
    moveToPosition(motionproxy, genotype[0].roll, genotype[0].pitch);
    //set wait marker (to get to initial pose)
    bool wait = true;
    //positions
    cv::Point curr_pos(-1,-1);
    cv::Point last_pos(-1,-1);
    //prepare timers
    std::clock_t current_clock;
    std::clock_t start_clock;
    double elapsed_secs;
    //sample counter
    int sample_move = 0;
    int sample_scan = 1;
    //decode loop
    start_clock = std::clock();
    while (1)
    {
        //get frame
        frame = acquisition->getImage();
        //process image with TLD
        detector->processImage(frame);
        //show current bounding box
        if (detector->currBB)
        {
		    cv::Point A;
		    A.x = detector->currBB->x;
		    A.y = detector->currBB->y;
		    cv::Point B;
		    B.x = detector->currBB->x + detector->currBB->width;
		    B.y = detector->currBB->y;
		    cv::Point C;
		    C.x = detector->currBB->x + detector->currBB->width;
		    C.y = detector->currBB->y + detector->currBB->height;
		    cv::Point D;
		    D.x = detector->currBB->x;
		    D.y = detector->currBB->y + detector->currBB->height;
		    //draw bb to image
		    cv::line(frame, A, B, cvScalar(0, 255, 0), 2);
		    cv::line(frame, B, C, cvScalar(0, 255, 0), 2);
		    cv::line(frame, C, D, cvScalar(0, 255, 0), 2);
		    cv::line(frame, D, A, cvScalar(0, 255, 0), 2);
        }
        //apply trajectory overlays
        frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
        frame.setTo(cv::Scalar(0,255,255),cur_traj_mask);
        frame.setTo(cv::Scalar(255,0,0),fen_traj_mask);
        //display image
        cv::imshow(winname, frame);
        //handle events
        cv::waitKey(2);
        //update timers
        current_clock = std::clock();
        elapsed_secs = double(current_clock - start_clock) / CLOCKS_PER_SEC;
        //wait for initial pose
        if (wait)
        {
            double current_pitch = (double)motionproxy->getAngles("RShoulderPitch", true)[0];
            double current_roll = (double)motionproxy->getAngles("RShoulderRoll", true)[0];
            if ((std::abs(current_roll - genotype[0].roll) < 0.1f && std::abs(current_pitch - genotype[0].pitch) < 0.1f) || elapsed_secs > 8)
            {
                //restart start time and begin decoding
                wait = false;
                start_clock = std::clock();
            }
            //do not start motion while not in initial position
            continue;
        }
        //convert joint position to object location on frame
        if (elapsed_secs > sample_scan * samplePeriod)
        {
            //draw fenotype trajectory
            cv::circle(fen_traj_mask, fenotype[sample_scan-1], 5, cv::Scalar(1), -1);
            if (sample_scan > 1)
                cv::line(fen_traj_mask, fenotype[sample_scan-1], fenotype[sample_scan-2], cvScalar(1), 2);
            //draw current trajectory
            if (detector->currBB)
            {
                curr_pos = bboxCenter(detector->currBB);
                cv::circle(cur_traj_mask, curr_pos, 5, cv::Scalar(1), -1);
                if (last_pos.x != -1)
                    cv::line(cur_traj_mask, last_pos, curr_pos, cvScalar(1), 2);
            }
            else
                curr_pos = cv::Point (-1,-1);
            last_pos = curr_pos;
            //increase sample counter
            sample_scan++;
        }
        //end if time is greater than movement time
        if (sample_scan > chromosomeLength)
        {
            //apply trajectory overlays
            frame.setTo(cv::Scalar(0,0,255),ref_traj_mask);
            frame.setTo(cv::Scalar(0,255,255),cur_traj_mask);
            frame.setTo(cv::Scalar(255,0,0),fen_traj_mask);
            //display image
            cv::imshow(winname, frame);
            //show result and exit
            cv::waitKey(2000);
            cv::destroyWindow(winname);
            return;
        }
        //move if time is up
        if (elapsed_secs > sample_move * samplePeriod)
        {
            moveToPosition(motionproxy, genotype[sample_move].roll, genotype[sample_move].pitch);
            sample_move++;
        }
    }
}

Population::Population(ImAcquisition *p_acquisition, AL::ALMotionProxy *p_motionproxy, AL::ALMemoryProxy *p_memoryproxy, tld::TLD *p_detector, Trajectory *p_trajectory)
{
    //set resources
    acquisition = p_acquisition;
    motionproxy = p_motionproxy;
    memoryproxy = p_memoryproxy;
    detector = p_detector;
    trajectory = p_trajectory;
    //generate new population
    generateNew();
}

Population::Population(ImAcquisition *p_acquisition, AL::ALMotionProxy *p_motionproxy, AL::ALMemoryProxy *p_memoryproxy, tld::TLD *p_detector, Trajectory *p_trajectory, const char *filename)
{
    //set resources
    acquisition = p_acquisition;
    motionproxy = p_motionproxy;
    memoryproxy = p_memoryproxy;
    detector = p_detector;
    trajectory = p_trajectory;
    //open file
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        std::cout << "Error: Population file not found: " << filename << std::endl;
        size = -1;
    }
    if(file != NULL)
    {
        size = -1;
        try
        {
            int MAX_LEN = 255;
            char str_buf[255];
            unsigned int id;
            
            fgets(str_buf, MAX_LEN, file); //skip line
            fscanf(file, "%d \n", &size); //get population size
            fgets(str_buf, MAX_LEN, file); //skip rest of line
            fscanf(file, "%d \n", &generation); //get generation number
            fgets(str_buf, MAX_LEN, file); //skip rest of line
            
            //get individuals
            for(int i = 0; i < size; i++)
            {
                //create individual object
                Individual ind(trajectory->sampleCount,trajectory->samplePeriod,trajectory->duration);
                fgets(str_buf, MAX_LEN, file); //skip line
                //get individual ID
                fscanf(file, "%d \n", &id); ind.id = assign_new_id(id);
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get chromosome length
                fscanf(file, "%d \n", &(ind.chromosomeLength));
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get sample period
                fscanf(file, "%f \n", &(ind.samplePeriod));
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get motion duration
                fscanf(file, "%f \n", &(ind.motionDuration));
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get fitness
                fscanf(file, "%f \n", &(ind.fitness));
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get genotype
                fgets(str_buf, MAX_LEN, file); //skip line
                for (int j = 0; j < ind.chromosomeLength; j++)
                    fscanf(file, "%lf %lf", &(ind.genotype[j].pitch), &(ind.genotype[j].roll));
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //get fenotype
                fgets(str_buf, MAX_LEN, file); //skip line
                ind.fenotype.reserve(ind.chromosomeLength);
                for (int j = 0; j < ind.chromosomeLength; j++)
                {
                    cv::Point pos;
                    fscanf(file, "%d %d", &(pos.x), &(pos.y));
                    ind.fenotype.push_back(pos);
                }
                fgets(str_buf, MAX_LEN, file); //skip rest of line
                //put individual to population
                individual.push_back(ind);
            }
            //sort population by fitness
            std::sort(individual.begin(),individual.end(),compare);
            //close file
            fclose(file);
        }
        catch (...)
        {
            size = -1;
        }
        if (size == -1)
            std::cout << "Error occured while loading file." << std::endl;
    }
}

void Population::generateNew()
{
    std::cout << "Creating new population." << std::endl;
    //set population size
    std::cout << "Set population size: ";
    while(!(std::cin >> size) || size <= 0)
    {
        std::cout << "Wrong input. Please enter positive number." << std::endl;
        std::cout << "Set population size: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set generation
    generation = 0;
    std::cout << "Initializing population..." << std::endl;
    //check joint temperature
    cooldown(motionproxy, memoryproxy);
    //generate individuals
    individual.reserve(size);
    for (int i=0; i < size; i++)
    {
        std::cout << "Initializing " << i+1 << ". individual ";
        Individual ind(trajectory->sampleCount,trajectory->samplePeriod,trajectory->duration);
        std::cout << "(ID" << ind.id << ")." << std::endl;
        ind.decode(acquisition,motionproxy,detector,trajectory);
        if (ind.fenotype[0].x != -1)
        {
            ind.evaluate(trajectory);
            std::cout << "Fitness = " << ind.fitness << std::endl;
        }
        individual.push_back(ind);
        std::cout << i+1 << ". individual done." << std::endl;
        cooldown(motionproxy, memoryproxy, false);
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
        std::cout << std::endl << generation << ". generation, " << i+1 << ". child, ID" << new_child.id << std::endl;
        new_child.decode(acquisition,motionproxy,detector,trajectory);
        if (new_child.fenotype[0].x != -1)
        {
            new_child.evaluate(trajectory);
            std::cout << "Fitness = " << new_child.fitness << std::endl;
        }
        child.push_back(new_child);
        cooldown(motionproxy, memoryproxy, false);
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
    std::cout << "Set evolution parameters:" << std::endl;
    //set number of generations
    int generations = -1;
    std::cout << "Generations: ";
    while(!(std::cin >> generations) || generations <= 0)
    {
        std::cout << "Wrong input. Please enter positive number." << std::endl;
        std::cout << "Generations: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set parents count
    int parent_count = -1;
    std::cout << "Parents count: ";
    while(!(std::cin >> parent_count) || parent_count > size || parent_count < 1)
    {
        std::cout << "Wrong input. Please enter number in range 1 to " << size << "." << std::endl;
        std::cout << "Parents count: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set children count
    int children_count = -1;
    std::cout << "Children count: ";
    while(!(std::cin >> children_count) || children_count > size || children_count < 1)
    {
        std::cout << "Wrong input. Please enter number in range 1 to " << size << "." << std::endl;
        std::cout << "Children count: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set mutation percentage
    int mutation = -1;
    std::cout << "Mutation percentage: ";
    while(!(std::cin >> mutation) || mutation > 100 || mutation < 0)
    {
        std::cout << "Wrong input. Please enter number in range 0 to 100." << std::endl;
        std::cout << "Mutation percentage: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //start evolution
    std::cout << "Starting evolution..." << std::endl;
    //check joint temperature
    cooldown(motionproxy, memoryproxy);
    //evolution
    for (int i=0; i < generations; i++)
    {
        //sort population
        std::sort(individual.begin(),individual.end(),compare);
        //write current population to file
        std::string path = static_cast<std::ostringstream*>( &(std::ostringstream() << "generation" << generation << ".pop") )->str();
        writeToFile(path.c_str());
        //increase generation counter
        generation++;
        //pick parents
        selection(parent_count);
        //create children
        reproduction(children_count, mutation);
        //replace worst individuals
        acceptation();
        //check joints temperature
        cooldown(motionproxy, memoryproxy);
    }
    std::sort(individual.begin(),individual.end(),compare);
    std::cout << "Evolution finished. Best fitness: " << individual[0].fitness << std::endl;
}

void Population::writeToFile(const char *path)
{
    FILE *file = fopen(path, "w");
    fprintf(file, "#Population for Nao motion evolution\n");
    fprintf(file, "%d #size\n", size);
    fprintf(file, "%d #generation\n", generation);
    for(int i = 0; i < size; i++)
    {
        fprintf(file, "#%d. Individual\n", i);
        fprintf(file, "%d #ID\n", individual[i].id);
        fprintf(file, "%d #chromosome length\n", individual[i].chromosomeLength);
        fprintf(file, "%f #sample period\n", individual[i].samplePeriod);
        fprintf(file, "%f #motion duration\n", individual[i].motionDuration);
        fprintf(file, "%f #fitness\n", individual[i].fitness);
        fprintf(file, "#Genotype:\n");
        for (int j = 0; j < individual[i].chromosomeLength; j++)
        {
            fprintf(file, "%lf %lf ", individual[i].genotype[j].pitch, individual[i].genotype[j].roll);
        }
        fprintf(file, "\n");
        fprintf(file, "#Fenotype:\n");
        for (int j = 0; j < individual[i].chromosomeLength; j++)
        {
            fprintf(file, "%d %d ", individual[i].fenotype[j].x, individual[i].fenotype[j].y);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

Population::~Population()
{
    /*cleanup*/;
}

//TODO Population member functions