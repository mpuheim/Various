/*********************************************************
  meteo_net.c
  --------------------------------------------------------
  generated at Tue Oct 04 11:26:11 2011
  by snns2c ( Bernward Kett 1995 ) 
*********************************************************/

#include <math.h>
#include <stdio.h>
#include ".\meteo_net.h"

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
  static UnitType Units[46];
  /* Sources definition section */
  static pUnit Sources[] =  {
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, 
Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, 
Units + 21, Units + 22, Units + 23, Units + 24, 
Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34, 

Units + 35, Units + 36, Units + 37, Units + 38, Units + 39, Units + 40, Units + 41, Units + 42, Units + 43, Units + 44, 


  };

  /* Weigths definition section */
  static float Weights[] =  {
-0.177280, -0.157630, -0.174990, -0.251880, -0.339430, -0.307290, -0.210430, -0.068600, 0.036750, 0.075810, 
-0.073220, 0.104340, 0.157130, 0.277800, 0.422070, 0.464690, 0.564040, 0.561040, 0.619240, 0.908450, 
1.159950, 1.484230, 1.683150, 1.958740, 
-0.210620, -0.281720, -0.221590, -0.230750, -0.340640, -0.467170, -0.335580, -0.231480, -0.082910, -0.088410, 
-0.328260, 0.024840, 0.022130, 0.214820, 0.427210, 0.424650, 0.471650, 0.472860, 0.496010, 0.742110, 
1.048640, 1.421520, 1.787040, 2.154350, 
-0.140270, -0.097410, -0.131320, -0.222380, -0.310520, -0.242800, -0.155460, -0.003170, 0.097700, 0.148280, 
0.023520, 0.162390, 0.236210, 0.344100, 0.478010, 0.532090, 0.636610, 0.635360, 0.699510, 0.995660, 
1.225810, 1.529650, 1.687030, 1.937100, 
-0.148070, -0.109290, -0.140880, -0.230220, -0.316380, -0.254780, -0.166070, -0.015940, 0.084850, 0.133760, 
0.005940, 0.150000, 0.220290, 0.330030, 0.465210, 0.517740, 0.621990, 0.620000, 0.682820, 0.976110, 
1.210500, 1.517590, 1.681080, 1.935880, 
0.223360, -1.496060, -1.043160, -0.742420, 0.618120, 0.705770, 0.650760, -0.102080, 0.129190, 0.219300, 
1.674630, 0.238380, -0.069160, -1.058500, 0.301300, 1.177410, 1.268350, -0.314630, -0.702390, -1.240610, 
-0.870460, -1.256800, 0.365210, 3.508630, 
-0.114890, -0.057580, -0.098300, -0.191380, -0.294530, -0.206260, -0.121130, 0.042100, 0.147130, 0.201240, 
0.079670, 0.209620, 0.293780, 0.395410, 0.524330, 0.579600, 0.685100, 0.687710, 0.756910, 1.053120, 
1.282300, 1.580570, 1.728190, 1.969430, 
-0.149140, -0.110860, -0.142090, -0.231180, -0.317270, -0.256350, -0.167550, -0.017670, 0.083160, 0.131840, 
0.003540, 0.148270, 0.218160, 0.328230, 0.463600, 0.516120, 0.619870, 0.618090, 0.680960, 0.973520, 
1.209250, 1.516620, 1.680170, 1.935930, 
-0.164480, -0.135160, -0.160260, -0.244090, -0.329800, -0.281980, -0.189560, -0.043710, 0.058440, 0.102720, 
-0.034830, 0.124510, 0.186240, 0.301100, 0.440220, 0.488630, 0.590290, 0.588440, 0.649050, 0.940010, 
1.181090, 1.496390, 1.677100, 1.942550, 
1.365860, -0.113600, 0.400870, 0.693180, 0.516180, -0.052340, -0.145030, -0.705250, -0.560330, -0.833800, 
-1.007940, -0.520910, -0.824020, -0.657870, 0.085830, 0.740520, 0.691890, 0.245640, 0.122040, -0.368800, 
0.135400, 0.090050, 1.478510, 2.448670, 
-0.110250, -0.049520, -0.091600, -0.184640, -0.292520, -0.199450, -0.114640, 0.051760, 0.158420, 0.213030, 
0.091130, 0.220250, 0.306580, 0.406810, 0.535810, 0.589760, 0.695340, 0.698510, 0.769230, 1.067010, 
1.296160, 1.593390, 1.740780, 1.979470, 
0.039150, 0.191150, 0.007190, 0.009930, 2.090930, 0.019450, 0.010420, 0.022410, 1.084120, 0.026400, 

0.925260, 0.985180, 0.897750, 0.902470, 0.872670, 0.885070, 0.903130, 0.914000, 0.845340, 0.883090, 

0.040440, 0.190810, 0.009950, 0.012340, 2.066460, 0.023560, 0.012790, 0.024170, 1.080950, 0.030830, 

0.053120, 0.198780, 0.025790, 0.027480, 1.930100, 0.041930, 0.027850, 0.037920, 1.066690, 0.049730, 

1.014650, 1.078850, 0.979260, 0.986100, 0.804150, 0.956740, 0.987050, 1.001250, 0.850880, 0.952200, 

0.042960, 0.191600, 0.013730, 0.015850, 2.033780, 0.028490, 0.016260, 0.027120, 1.076370, 0.035980, 

0.040510, 0.193800, 0.006710, 0.009910, 2.089020, 0.016880, 0.010470, 0.023240, 1.080650, 0.023300, 

0.040270, 0.190800, 0.009650, 0.012070, 2.068950, 0.023190, 0.012530, 0.023990, 1.081430, 0.030440, 

0.043180, 0.191680, 0.014050, 0.016130, 2.030890, 0.028860, 0.016550, 0.027370, 1.076200, 0.036380, 

0.625280, 0.748310, 0.593460, 0.596910, 1.236260, 0.598460, 0.597440, 0.609480, 1.112600, 0.602710, 

1.852100, 2.956070, 1.837760, 1.739600, 3.455730, 1.813970, 1.839480, 1.839330, 1.811930, 2.111580, 


  };

  /* unit definition section (see also UnitType) */
  static UnitType Units[46] = 
  {
    { 0.0, 0.0, 0, NULL , NULL },
    { /* unit 1 (Old: 1) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 2 (Old: 2) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 3 (Old: 3) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 4 (Old: 4) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 5 (Old: 5) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 6 (Old: 6) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 7 (Old: 7) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 8 (Old: 8) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 9 (Old: 9) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 10 (Old: 10) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 11 (Old: 11) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 12 (Old: 12) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 13 (Old: 13) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 14 (Old: 14) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 15 (Old: 15) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 16 (Old: 16) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 17 (Old: 17) */
      0.0, 0.999980, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 18 (Old: 18) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 19 (Old: 19) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 20 (Old: 20) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 21 (Old: 21) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 22 (Old: 22) */
      0.0, 1.000000, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 23 (Old: 23) */
      0.0, 0.999970, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 24 (Old: 24) */
      0.0, 0.999990, 0,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 25 (Old: 25) */
      0.0, -0.371850, 24,
       &Sources[0] , 
       &Weights[0] , 
      },
    { /* unit 26 (Old: 26) */
      0.0, -0.350840, 24,
       &Sources[24] , 
       &Weights[24] , 
      },
    { /* unit 27 (Old: 27) */
      0.0, -0.310890, 24,
       &Sources[48] , 
       &Weights[48] , 
      },
    { /* unit 28 (Old: 28) */
      0.0, -0.329230, 24,
       &Sources[72] , 
       &Weights[72] , 
      },
    { /* unit 29 (Old: 29) */
      0.0, -2.768920, 24,
       &Sources[96] , 
       &Weights[96] , 
      },
    { /* unit 30 (Old: 30) */
      0.0, -0.207410, 24,
       &Sources[120] , 
       &Weights[120] , 
      },
    { /* unit 31 (Old: 31) */
      0.0, -0.331490, 24,
       &Sources[144] , 
       &Weights[144] , 
      },
    { /* unit 32 (Old: 32) */
      0.0, -0.357640, 24,
       &Sources[168] , 
       &Weights[168] , 
      },
    { /* unit 33 (Old: 33) */
      0.0, -0.990610, 24,
       &Sources[192] , 
       &Weights[192] , 
      },
    { /* unit 34 (Old: 34) */
      0.0, -0.177440, 24,
       &Sources[216] , 
       &Weights[216] , 
      },
    { /* unit 35 (Old: 35) */
      0.0, -2.169610, 10,
       &Sources[240] , 
       &Weights[240] , 
      },
    { /* unit 36 (Old: 36) */
      0.0, -2.738270, 10,
       &Sources[250] , 
       &Weights[250] , 
      },
    { /* unit 37 (Old: 37) */
      0.0, -2.166610, 10,
       &Sources[260] , 
       &Weights[260] , 
      },
    { /* unit 38 (Old: 38) */
      0.0, -2.137500, 10,
       &Sources[270] , 
       &Weights[270] , 
      },
    { /* unit 39 (Old: 39) */
      0.0, -3.021980, 10,
       &Sources[280] , 
       &Weights[280] , 
      },
    { /* unit 40 (Old: 40) */
      0.0, -2.160210, 10,
       &Sources[290] , 
       &Weights[290] , 
      },
    { /* unit 41 (Old: 41) */
      0.0, -2.158910, 10,
       &Sources[300] , 
       &Weights[300] , 
      },
    { /* unit 42 (Old: 42) */
      0.0, -2.167090, 10,
       &Sources[310] , 
       &Weights[310] , 
      },
    { /* unit 43 (Old: 43) */
      0.0, -2.159760, 10,
       &Sources[320] , 
       &Weights[320] , 
      },
    { /* unit 44 (Old: 44) */
      0.0, -2.186720, 10,
       &Sources[330] , 
       &Weights[330] , 
      },
    { /* unit 45 (Old: 45) */
      0.0, -12.427700, 10,
       &Sources[340] , 
       &Weights[340] , 
      }

  };



int meteo_net(float *in, float *out, int init)
{
  int member, source;
  float sum;
  enum{OK, Error, Not_Valid};
  pUnit unit;


  /* layer definition section (names & member units) */

  static pUnit Input[24] = {Units + 1, Units + 2, Units + 3, Units + 4, Units + 5, Units + 6, Units + 7, Units + 8, Units + 9, Units + 10, Units + 11, Units + 12, Units + 13, Units + 14, Units + 15, Units + 16, Units + 17, Units + 18, Units + 19, Units + 20, Units + 21, Units + 22, Units + 23, Units + 24}; /* members */

  static pUnit Hidden1[10] = {Units + 25, Units + 26, Units + 27, Units + 28, Units + 29, Units + 30, Units + 31, Units + 32, Units + 33, Units + 34}; /* members */

  static pUnit Hidden2[10] = {Units + 35, Units + 36, Units + 37, Units + 38, Units + 39, Units + 40, Units + 41, Units + 42, Units + 43, Units + 44}; /* members */

  static pUnit Output1[1] = {Units + 45}; /* members */

  static int Output[1] = {45};

  for(member = 0; member < 24; member++) {
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

  for (member = 0; member < 10; member++) {
    unit = Hidden2[member];
    sum = 0.0;
    for (source = 0; source < unit->NoOfSources; source++) {
      sum += unit->sources[source]->act
             * unit->weights[source];
    }
    unit->act = Act_Logistic(sum, unit->Bias);
  };

  for (member = 0; member < 1; member++) {
    unit = Output1[member];
    sum = 0.0;
    for (source = 0; source < unit->NoOfSources; source++) {
      sum += unit->sources[source]->act
             * unit->weights[source];
    }
    unit->act = Act_Logistic(sum, unit->Bias);
  };

  for(member = 0; member < 1; member++) {
    out[member] = Units[Output[member]].act;
  }

  return(OK);
}

main()
{
    char c;
    int i;
    int time;
    int time_list[10000];
    float temp;
    float temperature_list[10000];
    float prediction[1];
    int temperature_count;
    
    //Open file
    FILE *f;
    if((f = fopen("graf.txt", "r")) == NULL)
    {
        printf("Cannot open file.\n");
        getchar();
        exit(1);
    }
    
    //Read temperature data from file
    c = 'a';
    temperature_count = 0;
    while (c!=EOF)
    {
        //read time
        if (fscanf(f,"%d",&time)==1)
            time_list[temperature_count] = time;
        //skip 2 spaces
        while (((c=fgetc(f))!=' ')&&(c!=EOF));
        while (((c=fgetc(f))!=' ')&&(c!=EOF));
        //read temperature
        if (fscanf(f,"%f",&temp)==1)
        {
            temperature_list[temperature_count] = temp;
            temperature_count++;
        }
        //skip 4 lines (1 hour)
        for (i=0; i<4; i++)
            while (((c=fgetc(f))!='\n')&&(c!=EOF));
    }
    
    //Close file
    if( fclose( f )) 
        printf("File close error.\n");
    
    //Print loaded temperature list
    printf("Temperature list:\n");
    c = 248;
    for (i=0;i<temperature_count;i++)
        printf("Time: %2d:00, Temperature: %7.3f %cC.\n",time_list[i],temperature_list[i],c);
        
    //Normalize values from interval <-20,38> to <0,1>
    for (i=0;i<temperature_count;i++)
        temperature_list[i] = (temperature_list[i]+20)/58;
    
    //Compute and print prediction
    meteo_net(temperature_list,prediction,0);
    printf("Prediction for next hour: %7.3f %cC.\n",58*prediction[0]-20,c);
    getchar();
}
