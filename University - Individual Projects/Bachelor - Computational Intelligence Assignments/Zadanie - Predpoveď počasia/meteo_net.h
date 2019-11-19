/*********************************************************
  ../downloads/meteo_net.h
  --------------------------------------------------------
  generated at Tue Oct 04 11:26:11 2011
  by snns2c ( Bernward Kett 1995 ) 
*********************************************************/

extern int meteo_net(float *in, float *out, int init);

static struct {
  int NoOfInput;    /* Number of Input Units  */
  int NoOfOutput;   /* Number of Output Units */
  int(* propFunc)(float *, float*, int);
} meteo_netREC = {24,1,meteo_net};
