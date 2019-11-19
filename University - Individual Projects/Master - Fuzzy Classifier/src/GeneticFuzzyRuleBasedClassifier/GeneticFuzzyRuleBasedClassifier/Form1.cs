using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Threading;
using System.Text;
using System.Windows.Forms;

namespace GeneticFuzzyRuleBasedClassifier
{
    public partial class Form1 : Form
    {
        //Global constants
        const int MAX_PATTERNS = 1000000;
        const int MAX_POPULATION = 10000;
        const int MAX_RULES = 10000;
        const int INPUTS = 6;
        const int CLASSES = 7;
        const string TRAIN_FILE_PATH = "\\train.pat";
        const string TEST_FILE_PATH = "\\test.pat";
        const string FULL_FILE_PATH = "\\cely.txt";
        const int FS_COUNT = 5;
        const double FS_SUP_L = 0.4;
        const double FS_SUP_LM = 0.4;
        const double FS_SUP_M = 0.4;
        const double FS_SUP_MH = 0.4;
        const double FS_SUP_H = 0.8;
        const double FS_PK_L = 0;
        const double FS_PK_LM = 0.2;
        const double FS_PK_M = 0.4;
        const double FS_PK_MH = 0.6;
        const double FS_PK_H = 1;

        //Global variables
        int       Train_Count, Test_Count, Full_Count;                           // Pattern count of datasets
        double[,] Train_Input = new double[MAX_PATTERNS, INPUTS];                // Training set - Input patterns
        int[]     Train_Output = new int[MAX_PATTERNS];                          // Training set - Outputs (as class corresponding to the Inputs)
        double[]  Train_Weights = new double[MAX_PATTERNS];                      // Training set - Weights
        double[,] Test_Input = new double[MAX_PATTERNS, INPUTS];                 // Testing set - Input patterns
        int[]     Test_Output = new int[MAX_PATTERNS];                           // Testing set - Outputs (as class corresponding to the Inputs)
        double[,] Full_Input = new double[MAX_PATTERNS, INPUTS];                 // Complete data set (training + testing + other) - Input patterns only
        int[]     Full_Output = new int[MAX_PATTERNS];                           // Complete data set (training + testing + other) - Output classes
        int       Population_Size;                                               // Population in genetic algorithm - Size
        double    Population_Elitism;                                            // Population in genetic algorithm - Elitism (relative size of population left without change between generations)
        double    Population_Mutation;                                           // Population in genetic algorithm - Mutation chance
        int       Population_MaxGenerations;                                     // Population in genetic algorithm - Maximum number of generations
        int       Population_GenerationsPerRule;                                 // Population in genetic algorithm - Number generations required to extract rule from population to the rule base
        bool[,]   Population_InputActivation = new bool[MAX_POPULATION, INPUTS]; // Population of genetic algorithm - Input activations
        int[,]    Population_InputLingValue = new int[MAX_POPULATION, INPUTS];   // Population in genetic algorithm - Input lingvistic values
        int[]     Population_Class = new int[MAX_POPULATION];                    // Population in genetic algorithm - Output classes
        double[]  Population_Certainty = new double[MAX_POPULATION];             // Population in genetic algorithm - Output class certainty degrees
        double[]  Population_Fittness = new double[MAX_POPULATION];              // Population in genetic algorithm - Fittness functions
        double    Population_TotalFittness;                                      // Population in genetic algorithm - Sum of Fittness functions of all individuals in population
        int[]     Population_Sorted = new int[MAX_POPULATION];                   // Population in genetic algorithm - Cross indexes to sorted population according to best fittness
        int       Population_Reset;                                              // Population in genetic algorithm - Number of turns with error after which population is reset by initialization
        int       Rules_Count;                                                   // Fuzzy Rule Base - Size
        bool[,]   Rules_InputActivation = new bool[MAX_RULES, INPUTS];           // Fuzzy Rule Base - Input activations
        int[,]    Rules_InputLingValue = new int[MAX_RULES, INPUTS];             // Fuzzy Rule Base - Input linguistic values
        int[]     Rules_Class = new int[MAX_RULES];                              // Fuzzy Rule Base - Output classes
        double[]  Rules_Certainty = new double[MAX_RULES];                       // Fuzzy Rule Base - Output class certainty degrees
        double    Rules_MinCertainty;                                            // Fuzzy Rule Base - Minimal required certainty fo fuzzy rule
        bool      working = false;

        public Form1()
        {
            InitializeComponent();
            print_to_log("Genetic Fuzzy Rule Based Classifier by Michal Puheim. November 2011.",false);
            print_to_log("Waiting for user action...");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //---Disable user actions if a process is running---//
            if (working == true)
            {
                MessageBox.Show("Please wait. Another process is already running.");
                return;
            }
            working = true;
            //---Disable user actions if a process is running---//

            //Load file with training patterns
            print_to_log("Initializing the load of training data...");
            if (load_file(TRAIN_FILE_PATH, ref Train_Input, ref Train_Output, ref Train_Count) != 0)
            {
                print_to_log("File loading failed.");
                print_to_log("Waiting for user action...");
                working = false; //Enable user actions
                return;
            }
            print_to_log(string.Format("{0} training patterns loaded.", Train_Count));

            //Load runtime properties from controls
            print_to_log("Loading runtime properties from user controls:");
            Population_Size = (int)this.numericUpDown3.Value;
            print_to_log(string.Format(" - Population size: {0}", Population_Size));
            Population_MaxGenerations = (int)this.numericUpDown4.Value;
            print_to_log(string.Format(" - Maximum number of generations: {0}", Population_MaxGenerations));
            Population_Mutation = (double)this.numericUpDown5.Value / 100;
            print_to_log(string.Format(" - Mutation chance: {0} %", (decimal)(100 * Population_Mutation)));
            Population_Elitism = (double)this.numericUpDown1.Value / 100;
            print_to_log(string.Format(" - Population elitism: {0} %", (int)(100 * Population_Elitism)));
            Population_GenerationsPerRule = (int)this.numericUpDown2.Value;
            print_to_log(string.Format(" - Number of generations needed to produce rule: {0}", Population_GenerationsPerRule));
            Rules_MinCertainty = (double)this.numericUpDown6.Value / 100;
            print_to_log(string.Format(" - Minimum required certainty of rule: {0}", (int)(100 * Rules_MinCertainty)));
            Population_Reset = (int)this.numericUpDown7.Value;
            print_to_log(string.Format(" - Generations with error after which GA is reset: {0}", Population_Reset));
            print_to_log("Runtime properties loaded.");

            //Build fuzzy rules using genetic algorithm
            print_to_log("Initializing genetic algorithm to create fuzzy rules...");
            Rules_Count = 0;
            Thread thread = new Thread(new ThreadStart(ada_boost));
            thread.Start();
            while (thread.IsAlive)
                Application.DoEvents();
            print_to_log(string.Format("Rule base successfuly established with total count of {0} rules.",Rules_Count));

            //Evaluation
            print_to_log(" ");
            thread = new Thread(new ParameterizedThreadStart(validate_dataset));
            thread.Start((object)("train"));
            while (thread.IsAlive)
                Application.DoEvents();

            print_to_log("Waiting for user action...");
            working = false; //Enable user actions
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //---Disable user actions if a process is running---//
            if (working == true)
            {
                MessageBox.Show("Please wait. Another process is already running.");
                return;
            }
            working = true;
            //---Disable user actions if a process is running---//

            //Check if rule base is established
            if (Rules_Count == 0)
            {
                MessageBox.Show("Cannot evaluate performance because there are no rules in rule base.\nPlease start training first to produce classification rules.");
                working = false;
                return;
            }

            //Load file with testing patterns
            print_to_log("Initializing the load of testing data...");
            if (load_file(TEST_FILE_PATH, ref Test_Input, ref Test_Output, ref Test_Count) != 0)
            {
                print_to_log("File loading failed.");
                print_to_log("Waiting for user action...");
                working = false; //Enable user actions
                return;
            }
            print_to_log(string.Format("{0} testing patterns loaded.", Train_Count));

            //Evaluation
            Thread thread = new Thread(new ParameterizedThreadStart(validate_dataset));
            thread.Start((object)("test"));
            while (thread.IsAlive)
                Application.DoEvents();

            print_to_log("Waiting for user action...");
            working = false; //Enable user actions
        }

        private void button3_Click(object sender, EventArgs e)
        {
            //---Disable user actions if a process is running---//
            if (working == true)
            {
                MessageBox.Show("Please wait. Another process is already running.");
                return;
            }
            working = true;
            //---Disable user actions if a process is running---//

            //Check if rule base is established
            if (Rules_Count == 0)
            {
                MessageBox.Show("Cannot classify data because there are no rules in rule base.\nPlease start training first to produce classification rules.");
                working = false;
                return;
            }

            //Get destination file name from user
            print_to_log("Prompting user to enter destination file name... ");
            this.saveFileDialog1.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            this.saveFileDialog1.Filter = "(*.ppm)|*.ppm";
            if (saveFileDialog1.ShowDialog() != DialogResult.OK)
            {
                print_to_log("Action cancelled by user.");
                print_to_log("Waiting for user action...");
                working = false; //Enable user actions
                return;
            }
            print_to_log("OK.",false);
            print_to_log(string.Format("Exported data will be saved to '{0}'", saveFileDialog1.FileName));

            //Load source file with patterns for export
            print_to_log("Initializing the load of data to export...");
            if (load_file(FULL_FILE_PATH, ref Full_Input, ref Full_Count) != 0)
            {
                print_to_log("File loading failed.");
                print_to_log("Waiting for user action...");
                working = false; //Enable user actions
                return;
            }
            print_to_log(string.Format("{0} export patterns loaded.", Full_Count));

            //Classify patterns
            print_to_log("Initializing data classification process...");
            Thread thread = new Thread(new ThreadStart(classify_dataset));
            thread.Start();
            while (thread.IsAlive)
                Application.DoEvents();
            print_to_log("Classification finished.");

            //Export to file
            print_to_log("Initializing export to file...");
            save_file();
            print_to_log("Data successfuly exported.");

            print_to_log("Waiting for user action...");
            working = false; //Enable user actions
        }

        private void numericUpDown4_change(object sender, EventArgs e)
        {
            this.numericUpDown2.Maximum = this.numericUpDown4.Value;
        }

        private void numericUpDown2_change(object sender, EventArgs e)
        {
            this.numericUpDown4.Minimum = this.numericUpDown2.Value;
        }

        // This delegate enables asynchronous calls for setting properties on a Progressbar control.
        delegate void SetProgressbarCallback(int value);
        private void SetProgressbar(int value)
        {
            // InvokeRequired required compares the thread ID of the calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.progressBar1.InvokeRequired)
            {
                SetProgressbarCallback d = new SetProgressbarCallback(SetProgressbar);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.progressBar1.Value = value;
            }
        }

        // This delegate enables asynchronous calls for printing text on Textbox control.
        delegate void print_to_logCallback(string text, bool newline);
        //Method 'print_to_log' (with optional 'newline' parameter) prints a line of text to the Aplication Log.
        private void print_to_log(string text)
        {
            print_to_log(text, true);
        }
        private void print_to_log(string text, bool newline)
        {
            // InvokeRequired required compares the thread ID of the calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.progressBar1.InvokeRequired)
            {
                print_to_logCallback d = new print_to_logCallback(print_to_log);
                this.Invoke(d, new object[] { text, newline });
            }
            else
            {
                if (newline == true)
                    this.textBox1.AppendText(Environment.NewLine + ">>  " + text);
                else if (newline == false)
                    this.textBox1.AppendText(text);
            }
        }

        //Method 'load_file' loads a file from given path and returns its contents by parameters 'Input', optional 'Output' and 'Count'.
        private int load_file(string Pat_Path, ref double[,] Input, ref int Count)
        {
            int[] blank = new int[MAX_PATTERNS];
            return load_file(Pat_Path, ref Input, ref blank, ref Count);
        }
        private int load_file(string Pat_Path, ref double[,] Input, ref int[] Output, ref int Count)
        {
            //Initialize progress bar
            SetProgressbar(0);
            //Set file path
            string Absolute_Pat_Path;
            Absolute_Pat_Path = System.Windows.Forms.Application.StartupPath + Pat_Path;
            //Open and load a file using given path
            print_to_log(string.Format("Opening source data file in '{0}'...", Absolute_Pat_Path));
            if (System.IO.File.Exists(Absolute_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Absolute_Pat_Path);
                string line;
                // Skipping first 7 lines
                for (int i = 0; i < 7; i++)
                    line = objReader.ReadLine();
                // Reading patterns
                int pattern_number = 0;
                int input_data = 0;
                char[] delimiterChars = { ' ', '\t' };
                double class_maximum;
                print_to_log("Reading from file...");
                while ((line = objReader.ReadLine()) != null)
                {
                    if (line.Length < 2)
                        continue; //Skipping empty lines
                    line = line.Replace(",", ".");
                    try
                    {
                        //print_to_log(string.Format("I/O index: '{0}'.", input_data));
                        //print_to_log(string.Format("Original string: '{0}'.", line));
                        string[] words = line.Split(delimiterChars);
                        //print_to_log(string.Format("{0} words in string:", words.Length));
                        class_maximum = -1;
                        Output[pattern_number] = -1;
                        for (int i = 0; i < words.Length; i++)
                        {
                            //print_to_log(string.Format("{0} ", words[i]), false);
                            //print_to_log(string.Format("{0} ", Convert.ToDouble(words[i])), false);
                            if (input_data == 0)
                                Input[pattern_number, i] = Convert.ToDouble(words[i]); //Save line of data to Input
                            else if (Convert.ToDouble(words[i]) > class_maximum)
                            {
                                Output[pattern_number] = i; //Save class with largest value as Output
                                class_maximum = Convert.ToDouble(words[i]);
                            }
                        }
                        if ((input_data != 0)&&(Output[pattern_number] == -1))
                        {
                            print_to_log("Error. Output classes definition is not found in the file.");
                            return -1;
                        }
                    }
                    catch (FormatException)
                    {
                        print_to_log(string.Format("Error while reading the file. Cannot convert string '{0}' to double.", line));
                        return -1;
                    }
                    catch (OverflowException)
                    {
                        print_to_log(string.Format("Error while reading the file. Values in '{0}' are out of bound of double.", line));
                        return -1;
                    }
                    if (input_data++ >= 1)
                    {
                        input_data = 0;
                        pattern_number++;
                    }
                    //Set progress bar
                    if ((pattern_number % 2000 == 0) && (progressBar1.Value < progressBar1.Maximum - 1))
                        SetProgressbar(progressBar1.Value + 1);
                    //Check for user actions in application gui. (Temporary solution to avoid possible freezing of application gui.)
                    Application.DoEvents();
                }
                objReader.Close();
                Count = pattern_number;
                SetProgressbar(progressBar1.Maximum);
                print_to_log("File reading is complete. ");
                return 0;
            }
            else
            {
                print_to_log(string.Format("Error. File was not found in '{0}'", Absolute_Pat_Path));
                return -1;
            }
        }

        //Method 'initialize_pattern_weights' sets weights for all patterns in training dataset to 1
        private void initialize_pattern_weights()
        {
            print_to_log("Initializing training pattern weights... ");
            for (int i = 0; i < Train_Count; i++)
                Train_Weights[i] = 1;
            print_to_log("Completed.", false);
        }

        //Method 'best_ling_value' finds lingvistic value with highest membership grade value for given input
        private int best_ling_value(double input)
        {
            //Calculate membership grade for all lingvistic values
            double[] mem_grade = new double[FS_COUNT];
            for (int i = 0; i < FS_COUNT; i++)
                mem_grade[i] = membership_grade(i, input);
            //Find lingvistic value with largest membership grade
            double max_grade = mem_grade[0];
            int best_ling_val = 0;
            for (int i = 1; i < FS_COUNT; i++)
                if (max_grade < mem_grade[i])
                {
                    max_grade = mem_grade[i];
                    best_ling_val = i;
                }
            return best_ling_val;
        }

        //Method 'generate_initial_population' generates new population of random rules for further processing in genetic algorithm
        private void generate_initial_population(int Size)
        {
            print_to_log("Generating new population... ");
            Random r = new Random();
            for (int member = 0; member < Size; member++)
            {
                //Initialization of the variable part
                bool inactive = true;
                while (inactive == true)
                {
                    for (int i = 0; i < INPUTS; i++)
                    {
                        if (r.NextDouble() > 0.5) //Generate random number from 0.0 to 1.0
                        {
                            Population_InputActivation[member, i] = true;
                            inactive = false;
                        }
                        else
                            Population_InputActivation[member, i] = false;
                    }
                }
                //Initialization of the value part
                int rand = r.Next(Train_Count); //Generate random number from 0 to Train_Count (Pick a random sample from training dataset)
                while (Train_Weights[rand] < 0.2)
                {
                    rand = r.Next(Train_Count);
                }
                for (int i = 0; i < INPUTS; i++)
                {
                    Population_InputLingValue[member, i] = best_ling_value(Train_Input[rand, i]);
                }
                //Set class which the rule points to
                Population_Class[member] = Train_Output[rand];
            }
            //Evaluate fittness
            sort_by_fittness();
            print_to_log("Completed.", false);
        }

        //Method 'membership_grade' calculates membership grade of linguistic value to input variable
        private double membership_grade(int LingValue, double input)
        {
            double result;
            switch (LingValue)
            {
                case 4:
                    result = 1 + (2 * (input - FS_PK_H)) / FS_SUP_H;
                    break;
                case 3:
                    if (input > FS_PK_MH)
                        result = 1 + (2 * (FS_PK_MH - input)) / FS_SUP_MH;
                    else
                        result = 1 + (2 * (input - FS_PK_MH)) / FS_SUP_MH;
                    break;
                case 2:
                    if (input > FS_PK_M)
                        result = 1 + (2 * (FS_PK_M - input)) / FS_SUP_M;
                    else
                        result = 1 + (2 * (input - FS_PK_M)) / FS_SUP_M;
                    break;
                case 1:
                    if (input > FS_PK_LM)
                        result = 1 + (2 * (FS_PK_LM - input)) / FS_SUP_LM;
                    else
                        result = 1 + (2 * (input - FS_PK_LM)) / FS_SUP_LM;
                    break;
                case 0:
                    result = 1 + (2 * (FS_PK_L - input)) / FS_SUP_L;
                    break;
                default:
                    result = 0;
                    break;
            }
            if (result < 0)
                return 0;
            if (result > 1)
                return 1;
            return result;
        }

        //Method 'T_norm' computes T-norm as minimum of membership grades of linguistic values participaring in the formulation of the rule
        private double T_norm(int rule, int pattern, bool in_rule_base, string dataset)
        {
            double tmp = 2;
            double min = 2;
            //Computation in final rule base (for final classifier)
            if (in_rule_base == true)
            {
                for (int i = 0; i < INPUTS; i++)
                {
                    if (Rules_InputActivation[rule, i] == true)
                    {
                        switch (dataset) //Switch pattern set according to given value
                        {
                            case "full":
                                tmp = membership_grade(Rules_InputLingValue[rule, i], Full_Input[pattern, i]);
                                break;
                            case "train":
                                tmp = membership_grade(Rules_InputLingValue[rule, i], Train_Input[pattern, i]);
                                break;
                            case "test":
                                tmp = membership_grade(Rules_InputLingValue[rule, i], Test_Input[pattern, i]);
                                break;
                        }
                        if (tmp < min)
                            min = tmp;
                    }
                }
            }
            //Computation in population (for training stage)
            else
            {
                for (int i = 0; i < INPUTS; i++)
                {
                    if (Population_InputActivation[rule, i] == true)
                    {
                        switch (dataset) //Switch pattern set according to given value
                        {
                            case "full":
                                tmp = membership_grade(Population_InputLingValue[rule, i], Full_Input[pattern, i]);
                                break;
                            case "train":
                                tmp = membership_grade(Population_InputLingValue[rule, i], Train_Input[pattern, i]);
                                break;
                            case "test":
                                tmp = membership_grade(Population_InputLingValue[rule, i], Test_Input[pattern, i]);
                                break;
                        }
                        if (tmp < min)
                            min = tmp;
                    }
                }
            }
            if (min <= 1)
                return min;
            else
                return 0;
        }

        //Method 'evaluate_matching_degree' computer matching degree for given rule and pattern
        private double evaluate_matching_degree(int rule, int pattern)
        {
            return evaluate_matching_degree(rule, pattern, false, "train");
        }
        private double evaluate_matching_degree(int rule, int pattern, bool in_rule_base, string dataset)
        {
            return T_norm(rule, pattern, in_rule_base, dataset);
        }

        //Method 'positive_examples' returns a fuzzy number of examples positively classified by the rule while taking into consideration each example’s matching degree with the rule and its weight
        private double positive_examples(int rule)
        {
            double sum = 0;
            for (int pattern = 0; pattern < Train_Count; pattern++)
            {
                if (Population_Class[rule] == Train_Output[pattern])
                    sum += Train_Weights[pattern] * evaluate_matching_degree(rule,pattern);
            }
            return sum;
        }

        //Method 'negative_examples' returns a fuzzy number of examples negatively classified by the rule while taking into consideration each example’s matching degree with the rule and its weight
        private double negative_examples(int rule)
        {
            double sum = 0;
            for (int pattern = 0; pattern < Train_Count; pattern++)
            {
                if (Population_Class[rule] != Train_Output[pattern])
                    sum += Train_Weights[pattern] * evaluate_matching_degree(rule, pattern);
            }
            return sum;
        }

        //Method 'fittness' evaluates rule based on concepts of consistency and (class) coverage
        private double fittness(int rule)
        {
            double positive = positive_examples(rule);
            double negative = negative_examples(rule);
            if (positive == 0)
                return 0;

            //Rule consistency demands a rule to cover a high number of positive examples and few negative ones
            double consistency;
            if (positive < negative)
                consistency = 0;
            else
                consistency = 1 - (negative / positive);

            //The coverage criterion measures the number of training patterns covered by the  k-th rule,
            // compared to the overall number of training patterns belonging to the rule’s class.
            //DEBUG double coverage = 1;
            double coverage;
            double sum = 0;
            for (int pattern = 0; pattern < Train_Count; pattern++)
            {
                if (Population_Class[rule] == Train_Output[pattern])
                    sum += Train_Weights[pattern];
            }
            if (sum == 0)
                return 0;
            coverage = positive / sum;
            //Rule certainty degree measures relevance of rule classification into its output class
            Population_Certainty[rule] = rule_certainty_degree(rule);

            //Final fittness function is product of consistency, coverage and certainty degree
            return (consistency * coverage * Population_Certainty[rule]);
        }

        //Method 'build_fittness_table' creates table with fittness values of all rules in population (also counts sum of fittnes of entire population)
        private void build_fittness_table()
        {
            Population_TotalFittness = 0;
            for (int i = 0; i < Population_Size; i++)
            {
                //Calculate fittness of individual
                Population_Fittness[i] = fittness(i);
                //Calculate total fittness of the population
                Population_TotalFittness += Population_Fittness[i];
            }
        }

        //Method 'sort_by_fittness' creates indexed lookup table for population rules sorted by fittness
        private void sort_by_fittness()
        {
            int i, j;
            //Initialize fittness table
            build_fittness_table();
            //Initialize lookup table
            for (i = 0; i < Population_Size; i++)
                Population_Sorted[i] = i;
            //Sort lookup table by fittness (insertsort)
            double value;
            int index;
            for (j = 1; j < Population_Size; j++)
            {
                value = Population_Fittness[Population_Sorted[j]];
                index = Population_Sorted[j];
                i = j - 1;
                while ((i >= 0) && (Population_Fittness[Population_Sorted[i]] < value))
                {
                    Population_Sorted[i + 1] = Population_Sorted[i];
                    i -= 1;
                }
                Population_Sorted[i + 1] = index;
            }
        }

        //Method 'candidate_selection' creates a table of parents fit for reproduction in genetic algorithm (based on roulettewheel selection)
        private int[,] candidate_selection()
        {
            //Calculate number of candidate pairs (as total population minus elitism percentage)
            int candidate_count = Population_Size - (int)((double)Population_Size * Population_Elitism);
            //Pick candidates
            Random r = new Random();
            double wheel_turn;
            int candidate_index;
            int[,] candidates = new int[candidate_count, 2];
            for (int i = 0; i < candidate_count; i++)
            {
                for (int parent = 0; parent < 2; parent++)
                {
                    candidate_index = -1;
                    wheel_turn = Population_TotalFittness * r.NextDouble(); //Turn the roulette
                    do
                    {
                        candidate_index++;
                        wheel_turn -= Population_Fittness[Population_Sorted[candidate_index]];  //Find member of population whose turn it is
                    } while ((wheel_turn > 0) && (candidate_index < Population_Size - 1));
                    candidates[i, parent] = candidate_index;
                }
                //print_to_log(string.Format("Created candidate pair: {0}, {1}", candidates[i, 0],candidates[i, 1]));
            }
            return candidates;
        }

        //Method 'reproduction' processes individuals from parent list through stages of crossover and mutation and creates their offspring
        private void reproduction(int[,] parent_list, int Offsprings_Count, ref bool[,] Offsprings_InputActivation, ref int[,] Offsprings_InputLingValue)
        {
            Random r = new Random();
            int crosspoint;
            bool active;

            for (int i = 0; i < Offsprings_Count; i++)
            {
                //Crossover (1 point) - Input activations
                crosspoint = r.Next(INPUTS - 1);
                for (int j = 0; j < INPUTS; j++)
                {
                    if (j <= crosspoint)
                        Offsprings_InputActivation[i, j] = Population_InputActivation[Population_Sorted[parent_list[i, 0]], j];
                    else
                        Offsprings_InputActivation[i, j] = Population_InputActivation[Population_Sorted[parent_list[i, 1]], j];
                }
                //Crossover (1 point) - Input linvistic values
                crosspoint = r.Next(INPUTS - 1);
                for (int j = 0; j < INPUTS; j++)
                {
                    if (j <= crosspoint)
                        Offsprings_InputLingValue[i, j] = Population_InputLingValue[Population_Sorted[parent_list[i, 0]], j];
                    else
                        Offsprings_InputLingValue[i, j] = Population_InputLingValue[Population_Sorted[parent_list[i, 1]], j];
                }
                //Mutation
                for (int j = 0; j < INPUTS; j++)
                {
                    //Activation
                    if (Population_Mutation > r.NextDouble())
                    {
                        if (Offsprings_InputActivation[i, j] == true)
                            Offsprings_InputActivation[i, j] = false;
                        else
                            Offsprings_InputActivation[i, j] = true;
                    }
                    //Lingvistic value
                    if (Population_Mutation > r.NextDouble())
                    {
                        if (r.NextDouble() > 0.5)
                            Offsprings_InputLingValue[i, j] += 1;
                        else
                            Offsprings_InputLingValue[i, j] -= 1;
                        //Check for overflow
                        if (Offsprings_InputLingValue[i, j] < 0)
                            Offsprings_InputLingValue[i, j] = 0;
                        if (Offsprings_InputLingValue[i, j] >= FS_COUNT)
                            Offsprings_InputLingValue[i, j] = FS_COUNT - 1;
                    }
                }
                //Check if at least one Input Activation is active, otherwise set one to active
                active = false;
                for (int j = 0; j < INPUTS; j++)
                {
                    if (Offsprings_InputActivation[i, j] == true)
                        active = true;
                }
                if (active == false)
                {
                    Offsprings_InputActivation[i, r.Next(INPUTS)] = true;
                }
            }
        }

        //Method 'determine_rule_class' determines rule class as class dominating among the training instances covered by the rule antecedent
        private int determine_rule_class(int rule)
        {
            //Compute sums for all classes
            double[] class_sum = new double[CLASSES];
            for (int i = 0; i < CLASSES; i++)
            {
                //Accumulate sum for class
                class_sum[i] = 0;
                for (int j = 0; j < Train_Count; j++)
                {
                    if (Train_Output[j] == i)
                        class_sum[i] += evaluate_matching_degree(rule, j) * Train_Output[j];
                }
            }
            //Find class with largest sum
            int max = 0;
            for (int i = 0; i < CLASSES; i++)
            {
                if (class_sum[i] > class_sum[max])
                    max = i;
            }
            return max;
        }

        //Method 'rule_certainty_degree' computes relevance of rule classification into its output class
        private double rule_certainty_degree(int rule)
        {
            double all_sum = 0;
            double rule_class_sum = 0;
            double pattern_match_degree;
            for (int pat = 0; pat < Train_Count; pat++)
            {
                pattern_match_degree = evaluate_matching_degree(rule, pat);
                all_sum += Train_Weights[pat] * pattern_match_degree;
                if (Population_Class[rule] == Train_Output[pat])
                    rule_class_sum += Train_Weights[pat] * pattern_match_degree;
            }
            return rule_class_sum / all_sum;
        }

        //Method 'new_generation' creates new generation of population through means of genetic algorithm
        private void new_generation()
        {
            //Selection of candidates for reproduction (based on roulettewheel selection)
            int[,] parent_list = candidate_selection();
            //Reproduction of candidates
            int Offsprings_Count = parent_list.Length / 2;                           // Number of offsprings
            bool[,] Offsprings_InputActivation = new bool[Offsprings_Count, INPUTS]; // Offsprings - Input activations
            int[,] Offsprings_InputLingValue = new int[Offsprings_Count, INPUTS];    // Offsprings - Input lingvistic values
            reproduction(parent_list, Offsprings_Count, ref Offsprings_InputActivation, ref Offsprings_InputLingValue);
            //Replace worst individuals in population by new ones
            int j = Population_Size - 1;
            for (int i = 0; i < Offsprings_Count; i++)
            {
                for (int input = 0; input < INPUTS; input++)
                {
                    Population_InputActivation[Population_Sorted[j], input] = Offsprings_InputActivation[i, input];
                    Population_InputLingValue[Population_Sorted[j], input] = Offsprings_InputLingValue[i, input];
                    //print_to_log(string.Format("Replacing pop {0}...", j));
                }
                Population_Class[Population_Sorted[j]] = determine_rule_class(i);
                j--;
            }
            //Evaluate fittness
            sort_by_fittness();
        }

        //Method 'ada_boost' utilizes genetic algorithm to iteratively create fuzzy rules
        private void ada_boost()
        {
            //Initialize progress bar
            SetProgressbar(0);
            //Initialize the initial pattern weights and the population of genetic algorithm
            initialize_pattern_weights();
            generate_initial_population(Population_Size);
            print_to_log("Starting rule generation algorithm...");
            double rule_error, match_degree, down_factor;
            int add_rule_counter = 0;
            for (int generation = 0; generation < Population_MaxGenerations; generation++)
            {
                //Print information to application log
                print_to_log(" ");
                print_to_log(string.Format("Generation: {0}", generation + 1));
                print_to_log(string.Format(" Total Fittness: {0}",(decimal)Population_TotalFittness));
                print_to_log(string.Format(" Best Fittness: {0}", (decimal)Population_Fittness[Population_Sorted[0]]));
                //Check for population fittness - if it is zero, terminate rule generation process
                if (Population_TotalFittness < 0.00001)
                {
                    print_to_log("Total population fittness decreased to zero.");
                    print_to_log("Trying to restart genetic algorithm...");
                    generate_initial_population(Population_Size);
                    add_rule_counter = 0;
                }
                //Generate new generation of rules in the population of genetic algorithm
                new_generation();
                //Add best rule from population to the fuzzy rule base every n-generation
                add_rule_counter++;
                if (add_rule_counter >= Population_GenerationsPerRule)
                {
                    print_to_log(" ");
                    print_to_log("Adding new rule to final rule base.");
                    for (int input = 0; input < INPUTS; input++)
                    {
                        Rules_InputActivation[Rules_Count, input] = Population_InputActivation[Population_Sorted[0], input];
                        Rules_InputLingValue[Rules_Count, input] = Population_InputLingValue[Population_Sorted[0], input];
                    }
                    Rules_Class[Rules_Count] = Population_Class[Population_Sorted[0]];
                    //Compute rule certainty and error
                    Rules_Certainty[Rules_Count] = Population_Certainty[Population_Sorted[0]];
                    rule_error = 1 - Rules_Certainty[Rules_Count];
                    //Print information to application log
                    print_to_log(string.Format(" Rule Error: {0} ", rule_error));
                    print_to_log(string.Format(" Rule Certainty: {0} ", Rules_Certainty[Rules_Count]));
                    //Reject rule if there is high error
                    if (Rules_Certainty[Rules_Count] < Rules_MinCertainty)
                    {
                        print_to_log("Rule rejected because of low certainty.");
                        if (add_rule_counter >= Population_Reset + Population_GenerationsPerRule)
                        {
                            print_to_log(" ");
                            print_to_log("Cannot find new rule. Trying to restart genetic algorithm...");
                            generate_initial_population(Population_Size);
                            add_rule_counter = 0;
                        }
                    }
                    //Otherwise add rule to rule base (by increasing total rule counter)
                    else
                    {
                        print_to_log("Rule accepted.");
                        //Increase rules counter
                        Rules_Count++;
                        //Reset counter for adding new rule
                        add_rule_counter = 0;
                        //Change weights on training patterns
                        double DEBUG_sum = 0;
                        for (int p = 0; p < Train_Count; p++)
                        {
                            if (Train_Output[p] == Population_Class[Population_Sorted[0]])
                            {
                                match_degree = evaluate_matching_degree(Population_Sorted[0], p);
                                down_factor = System.Math.Pow(rule_error / (1 - rule_error), match_degree);
                                Train_Weights[p] *= down_factor;
                                //Train_Weights[p] = 0;
                            }
                            DEBUG_sum += Train_Weights[p];
                        }
                        print_to_log("");
                        print_to_log(string.Format(" Sum of pattern weights: {0}", DEBUG_sum));
                        print_to_log(string.Format(" Average pattern weight: {0}", DEBUG_sum / Train_Count));
                        //Evaluate fittness of population (with changed pattern weights)
                        sort_by_fittness();
                    }
                }
                //Set progress bar
                SetProgressbar((int)(100 * (double)generation / (double)Population_MaxGenerations) + 1);
            }
            SetProgressbar(this.progressBar1.Maximum);
            print_to_log(" ");
            print_to_log("Rule generation algorithm finished.");
        }

        //Method 'classify_pattern' classifies pattern using fuzzy interference through rule base
        private int classify_pattern(int pattern, string dataset)
        {
            //Compute sums for all classes
            double[] class_sum = new double[CLASSES];
            for (int i = 0; i < CLASSES; i++)
            {
                //Accumulate sum for class
                class_sum[i] = 0;
                for (int rule = 0; rule < Rules_Count; rule++)
                {
                    if (i == Rules_Class[rule])
                    {
                        switch (dataset) //Switch patterns according to dataset
                        {
                            case "train":
                                class_sum[i] += evaluate_matching_degree(rule, pattern, true, "train") * Rules_Certainty[rule];
                                break;
                            case "test":
                                class_sum[i] += evaluate_matching_degree(rule, pattern, true, "test") * Rules_Certainty[rule];
                                break;
                            case "full":
                                class_sum[i] += evaluate_matching_degree(rule, pattern, true, "full") * Rules_Certainty[rule];
                                break;
                        }
                    }
                }
            }
            //Find class with largest sum
            int max = 1;
            for (int i = 0; i < CLASSES; i++)
            {
                if (class_sum[i] > class_sum[max])
                    max = i;
            }
            return max;
        }

        //Method 'classify_dataset' creates list of output classes for Complete Dataset
        private void classify_dataset()
        {
            int progress;
            int announce = (int)Full_Count / 10;
            int announce_counter = 0;
            //Initialize progress bar
            SetProgressbar(0);
            for (int i = 0; i < Full_Count; i++)
            {
                Full_Output[i] = classify_pattern(i, "full");
                //Set progress bar
                progress = (int)(100 * (double)i / (double)Full_Count);
                SetProgressbar((int)progress);
                //Print information to application log
                if (announce_counter == 0)
                {
                    print_to_log(string.Format("Classifying... Progress: {0} %.", (int)progress + 1));
                    announce_counter = announce;
                }
                announce_counter--;
            }
            SetProgressbar(this.progressBar1.Maximum);
        }

        //Method 'validate_dataset' evaluates classifier accuracy by comparing classifier outputs with pattern classes
        private void validate_dataset(object dataset_obj)
        {
            int progress;
            int announce;
            int announce_counter = 0;
            int count = 0;
            int[] dataset_output;
            string dataset = (string)dataset_obj;
            //Initialize progress bar
            SetProgressbar(0);
            //Select dataset on which evaluation will be performed
            if (dataset == "train")
            {
                print_to_log("Starting evaluation on training dataset...");
                count = Train_Count;
                announce = (int)Train_Count / 10;
                dataset_output = Train_Output;
                //error check
                for (int i = 0; i < Train_Count; i++)
                {
                    if (dataset_output[i] != Train_Output[i])
                    {
                        print_to_log("Eror copying reference to Train output data.");
                        return;
                    }
                }
            }
            else if (dataset == "test")
            {
                print_to_log("Starting evaluation on testing dataset...");
                count = Test_Count;
                announce = (int)Test_Count / 10;
                dataset_output = Test_Output;
                //error check
                for (int i = 0; i < Train_Count; i++)
                {
                    if (dataset_output[i] != Test_Output[i])
                    {
                        print_to_log("Eror copying reference to Test output data.");
                        return;
                    }
                }
            }
            else
            {
                print_to_log("Error in evaluation. Wrong dataset name.");
                return;
            }
            //Start evaluation
            int positive = 0;
            for (int i = 0; i < count; i++)
            {
                if (dataset_output[i] == classify_pattern(i, dataset))
                    positive++;
                //Set progress bar
                progress = (int)(100 * (double)i / (double)count);
                SetProgressbar((int)progress);
                //Print information to application log
                if (announce_counter == 0)
                {
                    print_to_log(string.Format("Validating... Progress: {0} %.", (int)progress + 1));
                    announce_counter = announce;
                }
                announce_counter--;
            }
            print_to_log("Validation is complete.");
            print_to_log(string.Format("{0} of total {1} patterns classified correctly.",positive,count));
            print_to_log(string.Format("Classification accuracy: {0:0.00} %.", 100 * (double)positive / (double)count));
            SetProgressbar(this.progressBar1.Maximum);
        }

        //Method 'save_file' exports data obtained by classification of Full dataset to file
        private void save_file()
        {
            SetProgressbar(0);
            print_to_log("Opening file...");
            System.IO.StreamWriter write = new System.IO.StreamWriter(saveFileDialog1.FileName);
            print_to_log("Writing to file... ");
            write.Write("P3\n775 475\n255\n");
            //MessageBox.Show(string.Format("DEBUG {0}", saveFileDialog1.FileName));
            for (int pattern = 0; pattern < Full_Count; pattern++)
            {
                Application.DoEvents();
                SetProgressbar((int)(100 * (double)pattern / (double)Full_Count) + 1);
                switch(Full_Output[pattern])
                {
                    case 0:
                        write.Write("210 26 26\n");
                        continue;
                    case 1:
                        write.Write("0 0 0\n");
                        continue;
                    case 2:
                        write.Write("30 232 25\n");
                        continue;
                    case 3:
                        write.Write("162 126 29\n");
                        continue;
                    case 4:
                        write.Write("243 236 45\n");
                        continue;
                    case 5:
                        write.Write("49 124 47\n");
                        continue;
                    case 6:
                        write.Write("26 26 188\n");
                        continue;
                    default:
                        write.Write("255 255 255\n");
                        continue;
                }
            }
            write.Close();
            SetProgressbar(this.progressBar1.Maximum);
            print_to_log("Completed.", false);
        }
    }
}
