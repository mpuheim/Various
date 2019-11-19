/******************************************************************************/
/* * *                                                                    * * */
/* * *  Aplikacia metódy TLD na sledovanie objektov v stereoviznom obraze * * */
/* * *                                                                    * * */
/* * *                           Diplomova Praca                          * * */
/* * *                                                                    * * */
/* * *                            neuralnet.h                             * * */
/* * *                            verzia 1.0.0                            * * */ 
/* * *                                                                    * * */
/* * *                            Michal Puheim                           * * */
/* * *                             april 2013                             * * */
/* * *                                                                    * * */
/******************************************************************************/

#ifndef NEURALNET_H_
#define NEURALNET_H_

//macros and definitions
#define AF(X)   (1.0/(1.0+exp((-1.0)*X))) //activation function of neurons
#define DF(X)   (exp((-1.0)*X)/((1.0+exp((-1.0)*X))*(1.0+exp((-1.0)*X)))) //derivative of activation function
#define BIAS    -1.0 //bias contribution
#define WEIGHT_INIT_MAX   0.1
#define WEIGHT_INIT_MIN  -0.1

//struct
struct Pattern
{
    float *value;
};

//classes
class DataSet
{
public:
    int patternCount;
    int inputCount;
    int outputCount;
    Pattern *input;
    Pattern *output;
    
    DataSet(int patternCount, int inputCount, int outputCount);
    DataSet(const char *path); //load from file
    void saveToFile(const char *path);
    virtual ~DataSet();
};

class Neuron
{
public:
    float input;
    float activation;
    float output;
    float delta;
    int previousCount;
    Neuron **previousNeuron; //pointers to neurons in front of this neuron
    float *weight; //weights in front of the neuron
    float biasWeight;
    
    Neuron();
    Neuron(Neuron **previous, int previousCount);
    void calculateOutput();
    virtual ~Neuron();
};

class Layer
{
public:
    Layer *previous;
    Neuron **neuron;
    int neuronCount;
    
    Layer(int size);
    Layer(int size, Layer *previous);
    void calculateOutput();
    virtual ~Layer();
};

class NeuralNetwork
{
public:
    Layer **layer;
    int layerCount;
    
    NeuralNetwork(int layerCount, int *neuronCount);
    NeuralNetwork(const char *path); //load from file
    void run(float *input);
    void calculateOutput(float *input, float *output);
    void trainOnSample(float *input, float *output, float gamma);
    void train(DataSet *trainDataset, float gamma);
    float testOnSample(float *input, float *output);
    float test(DataSet *testDataset);
    void saveToFile(const char *path);
    virtual ~NeuralNetwork();
};

#endif // NEURALNET_H_