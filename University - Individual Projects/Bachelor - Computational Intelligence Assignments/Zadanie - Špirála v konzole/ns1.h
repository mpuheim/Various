/*********************************************************
  ns1.h
  --------------------------------------------------------
  generated at Tue Sep 27 11:29:14 2011
  by snns2c ( Bernward Kett 1995 ) 
*********************************************************/

extern int ns1(float *in, float *out, int init);

static struct {
  int NoOfInput;    /* Number of Input Units  */
  int NoOfOutput;   /* Number of Output Units */
  int(* propFunc)(float *, float*, int);
} ns1REC = {2,2,ns1};
