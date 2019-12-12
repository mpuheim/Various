// Program includes
#include <iostream>
// Configuration
#define INPUT_FILE     "in.pat"
#define OUTPUT_FILE    "out.pat"

//line counter
unsigned long count_lines_of_file(char *file_patch) {
    FILE *file = fopen(file_patch, "r");
    if(file == NULL)
    {
        std::cerr << "Error. Cannot open file '" << INPUT_FILE << "'." << std::endl;
        return 0;
    }
    unsigned long line_count = 0;
    while (EOF != (fscanf(file,"%*[^\n]"), fscanf(file,"%*c"))) 
        ++line_count;
    fclose(file);
    return line_count;
}

//entry point function
int main()
{
    //get number of inputs and outputs for training pattern creation
    int nn_inputs;
    std::cout << "Enter desired ANN inputs: ";
    while(!(std::cin >> nn_inputs) || nn_inputs <= 0 || nn_inputs > 100)
    {
        std::cout << "Wrong input. Please enter number between 0 and 100." << std::endl;
        std::cout << "Enter desired ANN inputs: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    int nn_outputs;
    std::cout << "Enter desired ANN outputs: ";
    while(!(std::cin >> nn_outputs) || nn_outputs <= 0 || nn_outputs > 100)
    {
        std::cout << "Wrong input. Please enter number between 0 and 100." << std::endl;
        std::cout << "Enter desired ANN outputs: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //get max and min values of data
    int max_value;
    std::cout << "Enter maximum input/output value: ";
    while(!(std::cin >> max_value) || max_value <= -100 || max_value > 100)
    {
        std::cout << "Wrong input. Please enter number between -100 and 100." << std::endl;
        std::cout << "Enter maximum input/output value: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    int min_value;
    std::cout << "Enter minimum input/output value: ";
    while(!(std::cin >> min_value) || min_value < -100 || min_value >= max_value)
    {
        std::cout << "Wrong input. Please enter number between -100 and " << max_value << "." << std::endl;
        std::cout << "Enter minimum input/output value: ";
        std::cin.clear();
        std::cin.ignore(10000,'\n');
    }
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    //count lines of a input file which contain data
    int numLines = count_lines_of_file(INPUT_FILE)-2;
    //set input and output buffers
    float *inputs = new float[nn_inputs];
    float *outputs = new float[nn_outputs];
    //open input file
    FILE *input_file = fopen(INPUT_FILE, "r");
    if(input_file == NULL)
    {
        std::cerr << "Error. Cannot open file '" << INPUT_FILE << "'." << std::endl;
        return 0;
    }
    //open output file
    FILE *output_file = fopen(OUTPUT_FILE, "w");
    if(output_file == NULL)
    {
        std::cerr << "Error. Cannot open file '" << OUTPUT_FILE << "'." << std::endl;
        return 0;
    }
    //set read buffer for line skipping
    int MAX_LEN = 355;
    char str_buf[355];
    //skip first 2 lines
    fgets(str_buf, MAX_LEN, input_file); //skip line
    fgets(str_buf, MAX_LEN, input_file); //skip line
    //print output file header
    fprintf(output_file, "#Input/output dataset\n\n");
    fprintf(output_file, "%d #Number of samples\n", numLines-nn_inputs-nn_outputs);
    fprintf(output_file, "%d #Inputs per sample\n", nn_inputs);
    fprintf(output_file, "%d #Outputs per sample\n\n", nn_outputs);
    fprintf(output_file, "#Input data values\n");
    fprintf(output_file, "#Output data values\n\n");
    //copy temperatures to output file
    float temp;
    int c;
    for (int i = 0; i < numLines; i++)
    {
        //skip first 6 digits (or 2 spaces)
        while (((c=fgetc(input_file))!=' ')&&(c!=EOF));
        while (((c=fgetc(input_file))!=' ')&&(c!=EOF));
        //read temperature
        if (fscanf(input_file, "%f", &temp) != 1)
        {
            fclose(input_file);
            std::cerr << "Error occured while reading from file'" << INPUT_FILE << "'." << std::endl;
            return 0;
        }
        //skip rest of line
        fgets(str_buf, MAX_LEN, input_file);
        //fill input/output buffers
        if (i < nn_inputs+nn_outputs)
        {
            if (i < nn_inputs)
            {
                inputs[i] = (temp-min_value)/(max_value-min_value); //normalize and save input
                if (inputs[i] > 1)
                    inputs[i] = 1;
                if (inputs[i] < 0)
                    inputs[i] = 0;
            }
            else
            {
                outputs[i-nn_inputs] = (temp-min_value)/(max_value-min_value); //normalize and save output
                if (outputs[i-nn_inputs] > 1)
                    outputs[i-nn_inputs] = 1;
                if (outputs[i-nn_inputs] < 0)
                    outputs[i-nn_inputs] = 0;
            }
            if (i < nn_inputs+nn_outputs-1)
                continue;
        }
        else
        {
            for (int j=0; j < nn_inputs-1; j++)
                inputs[j] = inputs[j+1];
            inputs[nn_inputs-1] = outputs[0];
            for (int j=0; j < nn_outputs-1; j++)
                outputs[j] = outputs[j+1];
            outputs[nn_outputs-1] = (temp-min_value)/(max_value-min_value); //normalize and save output
            if (outputs[nn_outputs-1] > 1)
                outputs[nn_outputs-1] = 1;
            if (outputs[nn_outputs-1] < 0)
                outputs[nn_outputs-1] = 0;
        }
        //write inputs and outputs to file
        for (int j=0; j < nn_inputs; j++)
            fprintf(output_file, "%f ", inputs[j]);
        fprintf(output_file, "\n");
        for (int j=0; j < nn_outputs; j++)
            fprintf(output_file, "%f ", outputs[j]);
        fprintf(output_file, "\n\n");
    }
    //close files
    fclose(input_file);
    fclose(output_file);
    return 0;
}