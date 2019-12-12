// Program includes
#include "auxiliary.h"
#include "timer.h"
#include "neuralnet.h"
#include <iostream>
// Configuration
#define ANN_CONFIG_FILE     "annconfig.net"
#define TRAIN_DATA_FILE     "traindata.pat"
#define TEST_DATA_FILE      "testdata.pat"

//save neural network
void annSave(NeuralNetwork *nn)
{
    char option;
    std::cout << std::endl << "Would you like to save your ANN? (Y/N): ";
    while (1)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case 'Y':
            case 'y':
                std::cout << "Saving ANN to file '" << ANN_CONFIG_FILE << "'...";
                nn->saveToFile(ANN_CONFIG_FILE);
                std::cout << " Saved." << std::endl;
                return;
            case 'N':
            case 'n':
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << std::endl << "Would you like to save your ANN? (Y/N): ";
        }
    }
}

//load training dataset
DataSet *loadData(const char *path)
{
    std::cout << "Loading data from file '" << path << "'...";
    DataSet *data = new DataSet(path);
    if (data->patternCount == 0)
    {
        std::cout << std::endl << "Data not loaded." << std::endl;
        delete data;
        return NULL;
    }
    std::cout << " Loaded." << std::endl;
    return data;
}

//neural network runtime
void annRuntime(NeuralNetwork *nn)
{
    std::cout << std::endl << "ANN Runtime." << std::endl << std::endl;
    int input_count = nn->layer[0]->neuronCount;
    int output_count = nn->layer[nn->layerCount-1]->neuronCount;
    float *inputs = new float[input_count];
    float *outputs = new float[output_count];
    float input;
    //get inputs from the user
    for (int i = 0; i < input_count; i++)
    {
        std::cout << "Enter the " << i+1 << ". input: ";
        while(!(std::cin >> input) || input < 0.0 || input > 1.0)
        {
            std::cout << "Wrong input. Please enter number between 0 and 1." << std::endl;
            std::cout << "Enter the " << i+1 << ". input: ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        inputs[i] = input;
    }
    //calculate outputs
    nn->calculateOutput(inputs, outputs);
    //print outputs
    for (int i = 0; i < output_count; i++)
    {
        std::cout << i+1 << ". output = " << outputs[i] << std::endl;
    }
}

//initialization of neural network
NeuralNetwork *annInit(DataSet *trainDataset)
{
    //enter number of hidden layers
    int hidden;
    std::cout << "Enter number of hidden layers: ";
    while(!(std::cin >> hidden) || hidden < 0 || hidden > 10)
    {
        std::cout << "Wrong input. Please enter integer number between 0 and 10." << std::endl;
        std::cout << "Enter number of hidden layers: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //set number of neurons on input and output according to the training dataset
    int *neuronCount = new int[hidden+2];
    neuronCount[0] = trainDataset->inputCount; //input layer
    neuronCount[hidden+1] = trainDataset->outputCount; //output layer
    //enter neuron count for each hidden layer (if there are any)
    int neurons;
    for (int i=0; i < hidden; i++)
    {
        std::cout << "Enter number of neurons on " << i+1 << ". hidden layer: ";
        while(!(std::cin >> neurons) || neurons < 1 || neurons > 100)
        {
            std::cout << "Wrong input. Please enter integer number between 1 and 100." << std::endl;
            std::cout << "Enter number of neurons on " << i+1 << ". hidden layer: ";
            std::cin.clear();
            std::cin.ignore(10000,'\n');
        }
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        neuronCount[i+1] = neurons;
    }
    //initialize neural network
    NeuralNetwork *nn = new NeuralNetwork(hidden+2,neuronCount);
    //return pointer
    return nn;
}

//training of neural network
void annTraining(NeuralNetwork *nn,DataSet *trainDataset)
{
    bool continueTraining = true;
    while (continueTraining)
    {
        //enter desired network precision
        double desiredPrecision;
        std::cout << "Enter desired ANN precision (in %): ";
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
        std::cout << std::endl << "ANN training has started." << std::endl;
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
                std::cout << learningCycle << ". training cycle. Elapsed time is " << duration/1000 << " seconds of " << learnTime/1000 << " total." << std::endl;
                std::cout << " - Current ANN precision on training data is " << 100*precision << "%." << std::endl;
            }
            duration = learnTimer.getElapsedTime();
        }
        //print results info
        std::cout << "Training has ended at " << learningCycle << ". learning cycle after " << duration/1000 << " seconds." << std::endl;
        std::cout << "ANN precision on training dataset is " << 100*precision << "%." << std::endl;
        //ask to continue if precision is lower than expected
        if (precision < desiredPrecision)
        {
            std::cout << std::endl << "Precision of the ANN is lower than initially expected." << std::endl;
            std::cout << "Do you want to continue training (Y/N): ";
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

//new neural network training menu options
void annTrainMenuInfo()
{
    std::cout << std::endl << "ANN training:" << std::endl;
    std::cout << "1. Test ANN." << std::endl;
    std::cout << "2. Train ANN." << std::endl;
    std::cout << "3. Reinitialize ANN." << std::endl;
    std::cout << "4. Runtime." << std::endl;
    std::cout << "5. Return to previous menu." << std::endl;
    std::cout << "0. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//new neural network training menu
int annTrainMenu(NeuralNetwork *nn)
{
    if (nn != NULL)
        annTrainMenuInfo();
    DataSet *trainDataset = NULL;
    DataSet *testDataset = NULL;
    char option;
    double precision = 0;
    int ret = 1;
    while (ret)
    {
        //train if not trained yet
        if (nn == NULL)
        {
            std::cout << std::endl << "ANN training." << std::endl;
            //load training dataset if not done yet
            if (trainDataset == NULL)
                trainDataset = loadData(TRAIN_DATA_FILE);
            if (trainDataset == NULL)
            {
                wait(4000);
                return 0;
            }
            //initialize neural network
            nn = annInit(trainDataset);
            //train neural network
            annTraining(nn,trainDataset);
            //print menu info after training
            annTrainMenuInfo();
        }
        //training menu
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Test ANN." << std::endl;
                //load testing dataset if not done yet
                if (testDataset == NULL)
                    testDataset = loadData(TEST_DATA_FILE);
                if (testDataset == NULL)
                {
                    wait(4000);
                    return 0;
                }
                //test
                std::cout << std::endl << "ANN testing has started...";
                precision = (double)nn->test(testDataset);
                std::cout << " Done." << std::endl;
                std::cout << "ANN precision on testing data is " << 100*precision << "%." << std::endl;
                //wait a while for user to see results
                wait(4000);
                //print menu info after testing
                annTrainMenuInfo();
                break;
            case '2':
                std::cout << "Option 2: Further train ANN." << std::endl;
                //train neural network
                annTraining(nn,trainDataset);
                //print menu info
                annTrainMenuInfo();
                break;
            case '3':
                std::cout << "Option 3: Reinitialize ANN." << std::endl;
                delete nn;
                nn = NULL;
                break;
            case '4':
                std::cout << "Option 4: Runtime." << std::endl;
                annRuntime(nn);
                wait(4000);
                annTrainMenuInfo();
                break;
            case '5':
                std::cout << "Option 5: Return to previous menu." << std::endl;
                if (nn != NULL)
                    annSave(nn);
                delete nn;
                delete trainDataset;
                delete testDataset;
                return 1;
            case '0':
                std::cout << "Option 0: Exit." << std::endl;
                if (nn != NULL)
                    annSave(nn);
                delete nn;
                delete trainDataset;
                delete testDataset;
                return 0;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
    delete nn;
    delete trainDataset;
    delete testDataset;
    return 0;
}

//neural network initialization menu options
void annInitMenuInfo()
{
    std::cout << std::endl << "ANN initialization:" << std::endl;
    std::cout << "1. Create and train new ANN." << std::endl;
    std::cout << "2. Load ANN from file." << std::endl;
    std::cout << "0. Exit program." << std::endl;
    std::cout << std::endl << "Enter your option: ";
}

//neural network initialization menu
void annInitMenu(NeuralNetwork *nn)
{
    annInitMenuInfo();
    char option;
    int ret = 1;
    while (ret)
    {
        option = getKeyFromInput();
        switch (option)
        {
            case '1':
                std::cout << "Option 1: Create and train new ANN." << std::endl;
                delete nn;
                ret = annTrainMenu(nn);
                if (ret == 1) 
                    annInitMenuInfo();
                break;
            case '2':
                std::cout << "Option 2: Load ANN from file." << std::endl;
                delete nn;
                std::cout << std::endl << "Loading ANN from file... ";
                nn = new NeuralNetwork(ANN_CONFIG_FILE);
                if (nn->initialized == 0)
                {
                    std::cout << "Could not load ANN. Try to create new one." << std::endl;
                    delete nn;
                    nn = NULL;
                    annInitMenuInfo();
                    break;
                }
                std::cout << "Done." << std::endl;
                ret = annTrainMenu(nn);
                if (ret == 1)
                    annInitMenuInfo();
                break;
            case '0':
                std::cout << "Option 0: Exit." << std::endl;
                if (nn != NULL)
                    annSave(nn);
                return;
            default:
                std::cout << "Wrong input (" << option << ")." << std::endl;
                std::cout << "Enter your option: ";
        }
    }
}

//entry point function
int main()
{
    //initialize neural network pointer
    NeuralNetwork *nn = NULL;
    //start menu
    annInitMenu(nn);
    //cleanup
    delete nn;
    //end
    return 0;
}