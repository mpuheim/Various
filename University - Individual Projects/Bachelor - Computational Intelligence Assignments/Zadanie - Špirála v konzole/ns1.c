/*********************************************************
  ns1.c
  --------------------------------------------------------
  generated at Tue Sep 27 11:29:14 2011
  by snns2c ( Bernward Kett 1995 ) 
*********************************************************/

#include <math.h>
#include <stdio.h>
#include ".\ns1.h"

#define Act_Logistic(sum, bias)  ( (sum+bias<10000.0) ? ( 1.0/(1.0 + exp(-sum-bias) ) ) : 0.0 )
#ifndef NULL
#define NULL (void *)0
#endif

typedef struct UT {
          float act;         /* Activation       */
          float Bias;        /* Bias of the Unit */
          int   NoOfSources; /* Number of predecessor units */
   struct UT   **sources; /* predecessor units */
          float *weights; /* weights from predecessor units */
        } UnitType, *pUnit;

  /* Forward Declaration for all unit types */
  static UnitType Units[21];
  /* Sources definition section */
  static pUnit Sources[] =  {
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 1, Units + 2, 
Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, 

Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, 
Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, 

  };

  /* Weigths definition section */
  static float Weights[] =  {
69.478447, -63.917980, 
61.377491, 53.907230, 
14.085430, -74.938164, 
27.837490, 50.459911, 
61.365631, -12.524250, 
42.804581, 48.700211, 
36.844219, -43.012730, 
-54.401371, -50.611431, 
21.849480, -7.466090, 
-33.005489, 56.636230, 
-3.401550, -18.494921, -33.955341, -18.885111, -19.825380, -16.573259, -6.605860, -33.706631, -8.192200, -25.531811, 

-5.064830, -46.531170, -25.283051, -56.139500, -3.695540, -6.909480, -19.513741, -32.741928, 5.685330, -28.875360, 

9.476680, -18.143230, -18.033991, -32.231739, -18.477880, -25.476101, -13.816460, -8.092330, -7.847350, -10.891020, 

-18.444510, -21.411119, -0.766140, -29.932489, -9.368130, -29.043671, -0.293740, -25.401360, -1.688080, 1.751950, 

-41.551441, -10.705600, -30.195629, -27.851641, -21.724791, -29.317310, 13.081720, -21.876801, 6.726580, 17.897900, 

25.662991, -7.585970, 8.356660, -8.819970, -8.716030, -12.210450, 24.102051, -8.606780, 11.970660, -0.886980, 

14.150060, 14.728820, -16.730820, -16.849489, 13.004150, 6.002260, 
-14.149720, -14.728470, 16.730810, 16.849569, -13.004690, -6.002110, 

  };

  /* unit definition section (see also UnitType) */
  static UnitType Units[21] = 
  {
    { 0.0, 0.0, 0, NULL , NULL },
    { /* unit 1 (Old: 1) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 2 (Old: 2) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 3 (Old: 3) */
      0.0, -45.135948, 2,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 4 (Old: 4) */
      0.0, -96.552818, 2,
       &Sources[2] , 
       &Weights[2] , 
      },
    { /* unit 5 (Old: 5) */
      0.0, -8.497510, 2,
       &Sources[4] , 
       &Weights[4] , 
      },
    { /* unit 6 (Old: 6) */
      0.0, -78.363007, 2,
       &Sources[6] , 
       &Weights[6] , 
      },
    { /* unit 7 (Old: 7) */
      0.0, -56.561390, 2,
       &Sources[8] , 
       &Weights[8] , 
      },
    { /* unit 8 (Old: 8) */
      0.0, -90.939468, 2,
       &Sources[10] , 
       &Weights[10] , 
      },
    { /* unit 9 (Old: 9) */
      0.0, -6.283110, 2,
       &Sources[12] , 
       &Weights[12] , 
      },
    { /* unit 10 (Old: 10) */
      0.0, 15.232900, 2,
       &Sources[14] , 
       &Weights[14] , 
      },
    { /* unit 11 (Old: 11) */
      0.0, -17.053040, 2,
       &Sources[16] , 
       &Weights[16] , 
      },
    { /* unit 12 (Old: 12) */
      0.0, -36.371460, 2,
       &Sources[18] , 
       &Weights[18] , 
      },
    { /* unit 13 (Old: 13) */
      0.0, 29.006250, 10,
       &Sources[20] , 
       &Weights[20] , 
      },
    { /* unit 14 (Old: 14) */
      0.0, 27.302750, 10,
       &Sources[30] , 
       &Weights[30] , 
      },
    { /* unit 15 (Old: 15) */
      0.0, 32.631359, 10,
       &Sources[40] , 
       &Weights[40] , 
      },
    { /* unit 16 (Old: 16) */
      0.0, 28.015079, 10,
       &Sources[50] , 
       &Weights[50] , 
      },
    { /* unit 17 (Old: 17) */
      0.0, 23.239050, 10,
       &Sources[60] , 
       &Weights[60] , 
      },
    { /* unit 18 (Old: 18) */
      0.0, -31.250360, 10,
       &Sources[70] , 
       &Weights[70] , 
      },
    { /* unit 19 (Old: 19) */
      0.0, -7.339240, 6,
       &Sources[80] , 
       &Weights[80] , 
      },
    { /* unit 20 (Old: 20) */
      0.0, 7.339020, 6,
       &Sources[86] , 
       &Weights[86] , 
      }

  };



int ns1(float *in, float *out, int init)
{
  int member, source;
  float sum;
  enum{OK, Error, Not_Valid};
  pUnit unit;


  /* layer definition section (names & member units) */

  static pUnit Input[2] = {Units + 1, Units + 2}; /* members */

  static pUnit Hidden1[10] = {Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12}; /* members */

  static pUnit Hidden2[6] = {Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18}; /* members */

  static pUnit Output1[2] = {Units + 19, Units + 20}; /* members */

  static int Output[2] = {19, 20};

  for(member = 0; member < 2; member++) {
    Input[member]->act = in[member];
  }

  for (member = 0; member < 10; member++) {
    unit = Hidden1[member];
    sum = 0.0;
    for (source = 0; source < unit->NoOfSources; source++) {
      sum += unit->sources[source]->act
             * unit->weights[source];
    }
    unit->act = Act_Logistic(sum, unit->Bias);
  };

  for (member = 0; member < 6; member++) {
    unit = Hidden2[member];
    sum = 0.0;
    for (source = 0; source < unit->NoOfSources; source++) {
      sum += unit->sources[source]->act
             * unit->weights[source];
    }
    unit->act = Act_Logistic(sum, unit->Bias);
  };

  for (member = 0; member < 2; member++) {
    unit = Output1[member];
    sum = 0.0;
    for (source = 0; source < unit->NoOfSources; source++) {
      sum += unit->sources[source]->act
             * unit->weights[source];
    }
    unit->act = Act_Logistic(sum, unit->Bias);
  };

  for(member = 0; member < 2; member++) {
    out[member] = Units[Output[member]].act;
  }

  return(OK);
}

int main()
{
    int i;
    char c;
    float **netInput, **netOutput;
    float in1,in2,out1,out2;
    int pat_count;
    
    //Open file
    FILE *f;
    if((f = fopen("spiral_test.pat", "r")) == NULL)
    {
        printf("Cannot open file.\n");
        getchar();
        exit(1);
    }
    
    //Read pattern count from file
    if (fscanf(f,"%d",&pat_count)!=1)
    {
        printf("File format error. (1)\n");
        getchar();
        exit(1);
    }
    printf("Pattern count: %d\n",pat_count);
    
    //Allocate memory for pointers
    if ((netInput = (float**)malloc(pat_count * sizeof(float*))) == NULL)
        printf("Memory allocation error.");
    if ((netOutput = (float**)malloc(pat_count * sizeof(float*))) == NULL)
        printf("Memory allocation error.");
            
    //Allocate memory for values
    for (i=0; i<pat_count; i++)
        netInput[i] = (float*)malloc(ns1REC.NoOfInput * sizeof(float));
    for (i=0; i<pat_count; i++)
        netOutput[i] = (float*)malloc(ns1REC.NoOfOutput * sizeof(float));
    
    //Skip 5 lines
    for (i=0; i<5;i++)
        while (((c=fgetc(f))!='\n')&&(c!=EOF));
    
    //Read pattern data from file
    printf("Loading patterns from file... ");
    for (i=0; i<pat_count; i++)
    {
        if (fscanf(f,"%f %f %f %f",&in1,&in2,&out1,&out2)==4)
        {
            //Read inputs from file
            netInput[i][0] = in1;
            netInput[i][1] = in2;
            //Proces inputs in neural network and get outputs
            netOutput[i][0] = out1;
            netOutput[i][1] = out2;
            //ns1(netInput[i],netOutput[i],0);
        }
        else
        {
            printf("File format error (2).\n");
            getchar();
            exit(1);
        }
        //DEBUG printf("%f\n%f\n",netInput[i][0],netInput[i][1]);
        //DEBUG printf("%f\n%f\n\n",netOutput[i][0],netOutput[i][1]);
    }
    
    printf("Completed.\n");
    
    //Draw spiral to console
    printf("Drawing grid:\n\n");
    int x,y;
    int width = 45;
    int heigth = 78;
    int printed;
    for (y=0; y<width; y++)
    {
        printf(" ");
        for (x=0; x<heigth; x++)
        {
            printed = 0;
            for (i=0; i<pat_count; i++)
            {
                if (((int)(heigth*netInput[i][0]) == x) && ((int)(width*netInput[i][1]) == y))
                {
                    if ((int)(netOutput[i][1]) == 0)
                    {
                        printf("o");
                        printed = 1;
                        break;
                    }
                }
            }
            if (printed == 0)
                printf(" ");
        }
        printf("\n");
    }    
    
    getchar();
}
