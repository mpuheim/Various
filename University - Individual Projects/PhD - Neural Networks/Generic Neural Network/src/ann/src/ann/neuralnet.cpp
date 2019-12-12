#include "neuralnet.h"
#include <math.h>
#include <ctime>
#include <iostream>

DataSet::DataSet(int patCount, int inCount, int outCount)
{
    patternCount = patCount;
    inputCount = inCount;
    outputCount = outCount;
    input = new Pattern[patternCount];
    output = new Pattern[patternCount];
    for (int i=0; i < patternCount; i++)
    {
        input[i].value = new float[inputCount];
        output[i].value = new float[outputCount];
    }
}

DataSet::DataSet(const char *path)
{
    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        std::cerr << "Error. Cannot open file '" << path << "'." << std::endl;
        patternCount = 0;
        return;
    }
    
    int MAX_LEN = 255;
    char str_buf[255];
    
    fgets(str_buf, MAX_LEN, file); //skip line
    fgets(str_buf, MAX_LEN, file); //skip line
    
    if (fscanf(file, "%d \n", &patternCount) != 1) //get patternCount
    {
        fclose(file);
        std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
        patternCount = 0;
        return;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%d \n", &inputCount) != 1) //get inputCount
    {
        fclose(file);
        std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
        patternCount = 0;
        return;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    if (fscanf(file, "%d \n", &outputCount) != 1) //get outputCount
    {
        fclose(file);
        std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
        patternCount = 0;
        return;
    }
    fgets(str_buf, MAX_LEN, file); //skip rest of line
    
    fgets(str_buf, MAX_LEN, file); //skip line
    fgets(str_buf, MAX_LEN, file); //skip line
    fgets(str_buf, MAX_LEN, file); //skip line
    fgets(str_buf, MAX_LEN, file); //skip line
    
    input = new Pattern[patternCount];
    output = new Pattern[patternCount];
    for (int i=0; i < patternCount; i++)
    {
        input[i].value = new float[inputCount];
        output[i].value = new float[outputCount];
        for (int j=0; j < inputCount; j++)
        {
            if (fscanf(file, "%f ", &(input[i].value[j])) != 1) //get j-th input
            {
                fclose(file);
                std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
                for (int e=0; e <= i; e++)
                {
                    delete [] input[e].value;
                    delete [] output[e].value;
                }
                delete [] input;
                patternCount = 0;
                return;
            }
        }
        for (int j=0; j < outputCount; j++)
        {
            if (fscanf(file, "%f ", &(output[i].value[j])) != 1) //get j-th output
            {
                fclose(file);
                std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
                for (int e=0; e <= i; e++)
                {
                    delete [] input[e].value;
                    delete [] output[e].value;
                }
                delete [] output;
                patternCount = 0;
                return;
            }
        }
    }
}

DataSet::~DataSet()
{
    for (int i=0; i < patternCount; i++)
    {
        delete [] input[i].value;
        delete [] output[i].value;
    }
    delete [] input;
    delete [] output;
}

void DataSet::saveToFile(const char *path)
{
    FILE *file = fopen(path, "w");
    fprintf(file, "#Input/output dataset\n\n");
    fprintf(file, "%d #Number of samples\n", patternCount);
    fprintf(file, "%d #Inputs per sample\n", inputCount);
    fprintf(file, "%d #Outputs per sample\n\n", outputCount);
    fprintf(file, "#Input data values\n");
    fprintf(file, "#Output data values\n\n");
    for (int i=0; i < patternCount; i++)
    {
        for (int j=0; j < inputCount; j++)
            fprintf(file, "%f ", input[i].value[j]);
        fprintf(file, "\n");
        for (int j=0; j < outputCount; j++)
        {
            fprintf(file, "%f ", output[i].value[j]);
        }
        fprintf(file, "\n\n");
    }
    fclose(file);
}

Neuron::Neuron()
{
    input = 0;
    activation = 0;
    output = 0;
    delta = 0;
    previousCount = 0;
    previousNeuron = NULL;
    weight = NULL;
    biasWeight = 0;
}

Neuron::Neuron(Neuron **previous, int count)
{
    input = 0;
    activation = 0;
    output = 0;
    delta = 0;
    previousCount = count;
    previousNeuron = previous;
    weight = new float[count];
    for (int i=0; i < count; i++)
        weight[i] = (float) (WEIGHT_INIT_MIN + (WEIGHT_INIT_MAX - WEIGHT_INIT_MIN) * (double)rand() / RAND_MAX);
    biasWeight = (float) (WEIGHT_INIT_MIN + (WEIGHT_INIT_MAX - WEIGHT_INIT_MIN) * (double)rand() / RAND_MAX);
}

Neuron::~Neuron()
{
    delete [] weight;
}

void Neuron::calculateOutput()
{
    //input function
    input = 0;
    for (int i = 0; i < previousCount; i++)
    {
        input += weight[i] * previousNeuron[i]->output;
    }
    input += biasWeight * (float)BIAS;
    //activation function
    activation = (float)AF(input);
    //output function
    output = activation;
}

Layer::Layer(int size)
{
    previous = 0;
    neuronCount = size;
    neuron = new Neuron*[size];
    for (int i = 0; i < neuronCount; i++)
    {
        neuron[i] = new Neuron();
    }
}

Layer::Layer(int size, Layer *previousLayer)
{
    previous = previousLayer;
    neuronCount = size;
    neuron = new Neuron*[size];
    for (int i = 0; i < neuronCount; i++)
    {
        neuron[i] = new Neuron(previous->neuron,previous->neuronCount);
    }
}

Layer::~Layer()
{
    for (int i = 0; i < neuronCount; i++)
    {
        delete neuron[i];
    }
    delete [] neuron;
}

void Layer::calculateOutput()
{
    if (previous != NULL)
    {
        for (int i = 0; i < neuronCount; i++)
            neuron[i]->calculateOutput();
    }
}

NeuralNetwork::NeuralNetwork(int p_layerCount, int *neuronCount)
{
    //initialize random number generator
    srand((unsigned int)time(NULL));
    //initialize layers
    layerCount = p_layerCount;
    layer = new Layer*[layerCount];
    //initialize input layer
    layer[0] = new Layer(neuronCount[0]);
    //initialize other layers
    for (int i=1; i < layerCount; i++)
    {
        layer[i] = new Layer(neuronCount[i],layer[i-1]);
    }
    //initialization done
    initialized = 1;
}

NeuralNetwork::NeuralNetwork(const char *path)
{
    initialized = 0;
    layerCount = 0;
    //initialize random number generator
    srand((unsigned int)time(NULL));
    //open file
    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        std::cerr << "Error. Cannot open file '" << path << "'." << std::endl;
        return;
    }
    //set read buffer for line skipping
    int MAX_LEN = 255;
    char str_buf[255];
    //start reading from file
    fgets(str_buf, MAX_LEN, file); //skip line #Neural network weights configuration file
    fgets(str_buf, MAX_LEN, file); //skip line (empty)
    //get layer count
    fgets(str_buf, MAX_LEN, file); //skip line #Number of layers
    if (fscanf(file, "%d \n", &layerCount) != 1)
    {
        fclose(file);
        std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
        layerCount = 0;
        return;
    }
    //initialize layers
    layer = new Layer*[layerCount];
    fgets(str_buf, MAX_LEN, file); //skip line #Number of neurons in each layer
    for (int i=0; i < layerCount; i++)
    {
        //get neuron count in each layer
        int count;
        if (fscanf(file, "%d ", &count) != 1)
        {
            fclose(file);
            std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
            if (i==0)
                layerCount = 0;
            else
                layerCount = i-1;
            return;
        }
        //initialize input layer
        if (i == 0)
            layer[0] = new Layer(count);
        //initialize other layers
        else
            layer[i] = new Layer(count,layer[i-1]);
    }
    //load weights from file
    for (int h=1; h < layerCount; h++)
    {
        //load weights between h-th and (h-1)-th layers.
        fgets(str_buf, MAX_LEN, file); //skip line #Weights between h. and (h+1). layer:
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            for (int j=0; j < layer[h]->neuron[i]->previousCount; j++)
            {
                if (fscanf(file, "%f ", &(layer[h]->neuron[i]->weight[j])) != 1)
                {
                    fclose(file);
                    std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
                    return;
                }
            }
        }
        //load bias on h-th layer.
        fgets(str_buf, MAX_LEN, file); //skip line #Bias on %d. layer:\n
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            if (fscanf(file, "%f ", &(layer[h]->neuron[i]->biasWeight)) != 1)
            {
                fclose(file);
                std::cerr << "Error occured while reading from file '" << path << "'." << std::endl;
                return;
            }
        }
    }
    //close file
    fclose(file);
    //done initialization
    initialized = 1;
}

NeuralNetwork::~NeuralNetwork()
{
    for (int i = 0; i < layerCount; i++)
    {
        delete layer[i];
    }
    if (layerCount > 0)
    {
        delete [] layer;
    }
}

void NeuralNetwork::run(float *inputData)
{
    //set neurons of the input layers to input data
    for (int i=0; i < layer[0]->neuronCount; i++)
    {
        layer[0]->neuron[i]->output = inputData[i];
    }
    //calculate output through other layers
    for (int i=1; i < layerCount; i++)
    {
        layer[i]->calculateOutput();
    }
}

void NeuralNetwork::calculateOutput(float *input, float *output)
{
    //run network
    run(input);
    //save output
    for (int i=0; i < layer[layerCount-1]->neuronCount; i++)
    {
        output[i] = layer[layerCount-1]->neuron[i]->output;
    }
}

void NeuralNetwork::trainOnSample(float *input, float *output, float gamma)
{
    //run network
    run(input);
    //calculate deltas of output layer
    for (int i=0; i < layer[layerCount-1]->neuronCount; i++)
    {
        float neuronInput = layer[layerCount-1]->neuron[i]->input;
        float neuronOutput = layer[layerCount-1]->neuron[i]->output;
        float desiredOutput = output[i];
        //delta of i-th neuron
        layer[layerCount-1]->neuron[i]->delta = (desiredOutput - neuronOutput) * (float)DF(neuronInput);
    }
    //calculate deltas of other layers (using backpropagation of error)
    for (int h=layerCount-2; h > 0; h--)
    {
        //for each i-th neuron on h-th layer
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            //calculate error S as sum of deltas of each j-th neuron on (h+1)-th layer powered by weight value between neuron i and j
            float S = 0.0;
            for (int j=0; j < layer[h+1]->neuronCount; j++)
            {
                S += layer[h+1]->neuron[j]->delta * layer[h+1]->neuron[j]->weight[i];
            }
            float neuronInput = layer[h]->neuron[i]->input;
            //delta of i-th neuron
            layer[h]->neuron[i]->delta = S * (float)DF(neuronInput);
        }
    }
    //weight update
    for (int h=layerCount-1; h > 0; h--)
    {
        //for each i-th neuron on h-th layer
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            //update weights leading to neurons on previous layer
            float i_th_delta = layer[h]->neuron[i]->delta;
            for (int j=0; j < layer[h-1]->neuronCount; j++)
            {
                float j_th_output = layer[h]->neuron[i]->previousNeuron[j]->output;
                layer[h]->neuron[i]->weight[j] += gamma * i_th_delta * j_th_output;
            }
            //update bias weight
            layer[h]->neuron[i]->biasWeight += gamma * i_th_delta * (float)BIAS;
        }
    }
}

void NeuralNetwork::train(DataSet *trainDataset, float gamma)
{
    //create shuffle index table
    int *shuffle = new int[trainDataset->patternCount];
    for (int i = 0; i < trainDataset->patternCount; i++)
    {
        int r = (int)rand() % trainDataset->patternCount;
        shuffle[i] = r;
        shuffle[r] = i;
    }
    //train network using training data
    for (int i = 0; i < trainDataset->patternCount; i++)
    {
        trainOnSample(trainDataset->input[shuffle[i]].value,trainDataset->output[shuffle[i]].value,gamma);
    }
    //cleanup
    delete [] shuffle;
}

float NeuralNetwork::testOnSample(float *input, float *output)
{
    //run network
    run(input);
    //sum of differences
    float cumulativeError = 0;
    for (int i=0; i < layer[layerCount-1]->neuronCount; i++)
    {
        cumulativeError += abs(layer[layerCount-1]->neuron[i]->output - output[i]);
    }
    //average error
    float averageError = cumulativeError / layer[layerCount-1]->neuronCount;
    //precision
    float precision = 1 - averageError;
    return precision;
}

float NeuralNetwork::test(DataSet *testDataset)
{
    //test network using testing data
    float cumulativePrecision = 0;
    for (int i = 0; i < testDataset->patternCount; i++)
    {
        cumulativePrecision += testOnSample(testDataset->input[i].value,testDataset->output[i].value);
    }
    float precision = cumulativePrecision / testDataset->patternCount;
    return precision;
}

void NeuralNetwork::saveToFile(const char *path)
{
    FILE *file = fopen(path, "w");
    
    fprintf(file, "#Neural network weights configuration file\n\n");
    
    fprintf(file, "#Number of layers\n");
    fprintf(file, "%d\n", layerCount);
    fprintf(file, "#Number of neurons in each layer\n");
    for (int i=0; i < layerCount; i++)
        fprintf(file, "%d ", layer[i]->neuronCount);
    fprintf(file, "\n\n");
    
    for (int h=1; h < layerCount; h++)
    {
        fprintf(file, "#Weights between %d. and %d. layer:\n", h, h+1);
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            for (int j=0; j < layer[h]->neuron[i]->previousCount; j++)
            {
                fprintf(file, "%f ", layer[h]->neuron[i]->weight[j]);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "\n");
        
        fprintf(file, "#Bias on %d. layer:\n", h+1);
        for (int i=0; i < layer[h]->neuronCount; i++)
        {
            fprintf(file, "%f ", layer[h]->neuron[i]->biasWeight);
            fprintf(file, "\n");
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
}
