// MLP2 - TWO-LAYER MULTILAYER PERCEPTRON - SAMPLE IMPLEMENTATION (WITH THE XOR DATA)
// compile: gcc -Wall -std=gnu99 -O3 -ffast-math -funroll-loops -s -o MLP2 MLP2.c -lm
// Version 1.0 ----------------------------------------- Copyleft R.JAKSA 2009, GPLv3

#define Nin 2		// no. of inputs
#define Nh1 2		// no. of hidden units
#define Nou 1		// no. of outputs
#define Gamma 0.2	// learning rate
#define Epochs 40000	// no. of training epochs (cycles)

// ------------------------------------------------------------- END OF CONFIGURATION
#include <math.h>	// fabs, exp
#include <stdlib.h>	// rand, srand
#include <stdio.h>	// printf
#include <sys/timeb.h>	// ftime

#define Nx 1+Nin+Nh1+Nou // no. of units
#define IN1 1		// 1st input
#define IN2 2		// 2nd input
#define INn Nin		// last (n-th) input
#define H11 Nin+1	// 1st hidden
#define H1n Nin+Nh1	// last hidden
#define OU1 Nin+Nh1+1	// 1st output
#define OUn Nin+Nh1+Nou	// last output

typedef struct {	//
  double x[Nx];		// units inputs
  double y[Nx];		// units activations
  double delta[Nx];	// units delta signal
  double w[Nx][Nx];	// weights
  double dv[Nx];	// desired value on output !!! TODO: Nou is enough
} ann_t;		//

#define w(i,j)	ann->w[i][j]
#define x(i)	ann->x[i]
#define y(i)	ann->y[i]
#define delta(i) ann->delta[i]
#define dv(i)	ann->dv[i]

// --------------------------------------- ACTIVATION FUNCTION AND ITS 1st DERIVATION
#define af(X)	(1.0/(1.0+exp((-1.0)*X)))
#define df(X)	(exp((-1.0)*X)/((1.0+exp((-1.0)*X))*(1.0+exp((-1.0)*X))))

// -------------------------------------------------------- SINGLE LAYER WEIGHTS INIT
static void layer_rnd_init(ann,i1,in,j1,jn,min,max)	//
  ann_t *ann; int i1,in,j1,jn;				// output/input block from-to
  double min,max;					// weights init interval size
{ for(int i=i1; i<=in; i++) {				//
    w(i,0) = rand() / (RAND_MAX/(max-min)) + min;	//
    for(int j=j1; j<=jn; j++)				//
      w(i,j) = rand() / (RAND_MAX/(max-min)) + min; }}	//

// ---------------------------------------------------------------- MLP2 WEIGHTS INIT
void MLP2_rnd_init(ann_t *ann, double min, double max) {//
  y(0)=-1.0;						// the input for bias
  for(int i=0; i<Nx; i++) for(int j=0; j<Nx; j++)	//
    w(i,j)=0;						// all weights to zero
  layer_rnd_init(ann,H11,H1n,IN1,INn,min,max);		// in -> h1
  layer_rnd_init(ann,OU1,OUn,H11,H1n,min,max); }	// h1 -> ou

// ----------------------------------------------------------------- SINGLE LAYER RUN
static void layer_run(ann,i1,in,j1,jn)			//
  ann_t *ann; int i1,in,j1,jn;				// output/input block from-to
{ for(int i=i1; i<=in; i++) {				//
    x(i) = w(i,0) * y(0);				// add bias contribution
    for(int j=j1; j<=jn; j++) x(i) += w(i,j) * y(j);	// add main inputs contrib.
    y(i) = af(x(i)); }}					// apply activation function

// ---------------------------------------------------------------------- NETWORK RUN
void MLP2_run(ann_t *ann) {				//
  layer_run(ann,H11,H1n,IN1,INn);			// in -> h1
  layer_run(ann,OU1,OUn,H11,H1n); }			// h1 -> ou

// ------------------------------------------------------ SINGLE LAYER WEIGHTS UPDATE
static void layer_weights_update(ann,i1,in,j1,jn,gamma)	//
  ann_t *ann; int i1,in,j1,jn;				// output/input block from-to
  double gamma;						// learning rate
{ for(int i=i1; i<=in; i++) {				//
    w(i,0) += gamma * delta(i) * y(0);			// bias (weight) update
    for(int j=j1; j<=jn; j++)				//
      w(i,j) += gamma * delta(i) * y(j); }}		// the weights update

// ------------------------------------------------- VANILLA BACKPROPAGATION LEARNING
void MLP2_vanilla_bp(ann_t *ann, double gamma) {	//
  MLP2_run(ann);					// 1st run the network
  for(int i=OU1; i<=OUn; i++)				//
    delta(i) = (dv(i)-y(i)) * df(x(i));			// delta on output layer
  for(int i=H11; i<=H1n; i++) {				//
    double S=0.0; for(int h=OU1; h<=OUn; h++) S += delta(h) * w(h,i);
    delta(i) = S * df(x(i)); }				// delta on hidden layer
  layer_weights_update(ann,OU1,OUn,H11,H1n,gamma);	// h1 -> ou
  layer_weights_update(ann,H11,H1n,IN1,INn,gamma); }	// in -> h1

// ----------------------------------------------------------------------------- MAIN
int main(void) {					//
  ann_t ann[1];						//
  struct timeb t; ftime(&t); srand(t.time);		// time-seed random generator 
  MLP2_rnd_init(ann,-0.1,0.1);				// initialize the network
  printf("\nEpoch:  Output  Des.out. (Error)\n");	//
  printf("--------------------------------\n");		//
  ftime(&t); long t1=t.time*1000+t.millitm;		// start time in milliseconds
  for(int e=0; e<=Epochs; e++) {			// for every epoch
    for(int p=0; p<4; p++) {				// for every pattern
      switch(p) {					// input data XOR
      case 0: y(IN1)=0; y(IN2)=0; dv(OU1)=0; break;	//
      case 1: y(IN1)=0; y(IN2)=1; dv(OU1)=1; break;	//
      case 2: y(IN1)=1; y(IN2)=0; dv(OU1)=1; break;	//
      case 3: y(IN1)=1; y(IN2)=1; dv(OU1)=0; break; }	//
      MLP2_vanilla_bp(ann,Gamma);			// train
      if(e%5000==0) {					// every 5000 ep. print error
	if(p==0 && e!=0) printf("\n");			//
	MLP2_run(ann);					// run network
	double J=fabs(dv(OU1) - y(OU1));		// compute the error
	printf("%5d: %f %f (%.3f)\n",e,y(OU1),dv(OU1),J); }}}
  ftime(&t); long t2=t.time*1000+t.millitm;		// end time (in milliseconds)
  printf("--------------------------------\n");		//
  long con=((Nin+1)*Nh1+(Nh1+1)*Nou)*4*Epochs;		// no. of connect. updated
  int msec=t2-t1; if(!msec) msec=1;			// time in milliseconds
  printf("%ld kCUPS in %.3f sec\n\n",con/msec,(double)msec/1000.0);
  return(0); }						//

// ------------------------------------------------------------------------------ END
