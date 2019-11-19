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

#include "auxiliary.h"
#include "trajectory.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <ctime>

Trajectory::Trajectory()
{
    sampleCount = 0;
    samplePeriod = 0;
    duration = 0;
    sample.reserve(10);
}

Trajectory::~Trajectory()
{
    /*cleanup*/;
}

cv::Mat canvas;
std::clock_t start_clock;
double elapsed_secs;
bool drawing;
bool take_sample;
std::vector<cv::Point> *p_sample;

static void onMouse( int event, int x, int y, int flags, void* param)
{
    static int i = 0;
    switch( event )
    {
		case CV_EVENT_MOUSEMOVE: 
			if(drawing)
            {
				cv::circle(canvas, cv::Point(x,y), 1, cv::Scalar(0, 255, 0), -1);
                if(take_sample)
                {
                    cv::circle(canvas, cv::Point(x,y), 5, cv::Scalar(0, 255, 255), -1);
                    i+=1;
                    std::cout << "Taking " << i << ". sample [" << x << "," << y << "] at time " << elapsed_secs << "." << std::endl;
                    p_sample->push_back(cv::Point(x,y));
                    take_sample = false;
                }
			}
			break;

		case CV_EVENT_LBUTTONDOWN:
            if(!drawing)
            {
                drawing = true;
                start_clock = std::clock();
                i = 0;
            }
            break;
	}
}

void Trajectory::initByMouse(ImAcquisition *acquisition)
{
    //set sample period
    std::cout << "Set sample period: ";
    while(!(std::cin >> samplePeriod))
    {
        std::cout << "Wrong input. Please enter numeric characters only." << std::endl;
        std::cout << "Set sample period: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set sample count
    std::cout << "Set sample count: ";
    while(!(std::cin >> sampleCount))
    {
        std::cout << "Wrong input. Please enter numeric characters only." << std::endl;
        std::cout << "Set sample count: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //calculate duration
    duration = (float)sampleCount * samplePeriod;
    //print info
    std::cout << "Left click with mouse on video to start drawing." << std::endl;
    std::cout << "Then move your mouse for " << duration << " seconds to create motion trajectory." << std::endl;
    //set window to display video
	std::string winname = "Draw trajectory";
    cv::namedWindow(winname);
    //prepare images
    cv::Mat frame = acquisition->getImage();
    cv::Mat merge = frame.clone();
    //set up mouse callback
    drawing = false;
    take_sample = false;
    p_sample = &sample;
    cv::setMouseCallback(winname, onMouse, (void*) &canvas);
    //prepare timers
    std::clock_t current_clock;
    double sampled_time = samplePeriod;
    //sample counter
    int sample = 1;
    //clear drawing canvas
    canvas = cv::Mat::zeros(frame.rows,frame.cols,frame.type());
    //drawing loop
    while (1)
    {
        //merge image with canvas
        cv::Mat merge = frame.clone();
        merge += canvas;
        //display image
        cv::imshow(winname, merge);
        //draw
        if (drawing)
        {
            current_clock = std::clock();
            elapsed_secs = double(current_clock - start_clock) / CLOCKS_PER_SEC;
            if (!take_sample && sample > sampleCount)
            {
                drawing = false;
                std::cout << "Done. Press any key to close window..." << std::endl;
                cvWaitKey(0);
                cv::destroyWindow(winname);
                break;
            }
            if (!take_sample && elapsed_secs >= sampled_time)
            {
                take_sample = true;
                sample += 1;
                sampled_time = (double)sample * (double)samplePeriod;
            }
        }
        else
        {
            //get image from camera
            frame = acquisition->getImage();
        }
        //handle mouse events
        cvWaitKey(2);
    }
}

void Trajectory::saveWithPrompt(const char *path)
{
    char option;
    std::cout << "Do you want to save trajectory? (Y/N): ";
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case 'y':
            case 'Y':
                std::cout << "Saving trajectory to file '" << path << "'... ";
                writeToFile(path);
                std::cout << "Done." << std::endl;
                return;
            case 'n':
            case 'N':
                std::cout << "Save cancelled." << std::endl;
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Do you want to save trajectory? (Y/N): ";
        }
    }
}

void Trajectory::writeToFile(const char *path)
{
    FILE *file = fopen(path, "w");
    fprintf(file, "#Nao motion trajectory\n");
    fprintf(file, "%d #sample count\n", sampleCount);
    fprintf(file, "%f #sampling period\n", samplePeriod);
    fprintf(file, "%f #motion duration\n", duration);
    fprintf(file, "Samples(x,y):\n", duration);
    for(int i = 0; i < sampleCount; i++)
        fprintf(file, "%d %d\n", sample[i].x, sample[i].y);
    fclose(file);
}

void Trajectory::readFromFile(const char *path)
{
    FILE *file = fopen(path, "r");
    
    if(file == NULL)
    {
        std::cout << "Error: Trajectory file not found: " << path << std::endl;
        exit(1);
    }
    
    int MAX_LEN = 255;
    char str_buf[255];
    
    fgets(str_buf, MAX_LEN, file); //skip line
    fscanf(file, "%d \n", &sampleCount);
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    fscanf(file, "%f \n", &samplePeriod);
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    fscanf(file, "%f \n", &duration);
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    fgets(str_buf, MAX_LEN, file); //skip line
    //read data
    int x,y;
    for(int i = 0; i < sampleCount; i++)
    {
        fscanf(file, "%d %d", &x, &y);
        fgets(str_buf, MAX_LEN, file); //skip rest of line
        sample.push_back(cv::Point(x,y));
    }
    
    fclose(file);
}
