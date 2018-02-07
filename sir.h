// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <limits.h>
#include "../SFMT/SFMT.h"

#define NAVG 100000 // number of runs for averages
#define NRND 0x100000 // number of random numbers per batch
#define NREV 1000 // number of iterations to thermalize the RNG

#define SUSCEPTIBLE 0
#define INFECTED 1
#define RECOVERED 2
#define NONE UINT_MAX

#define S(x) (n[(x)].state == SUSCEPTIBLE)
#define I(x) (n[(x)].state == INFECTED)
#define R(x) (n[(x)].state == RECOVERED)

// if rng needs a refill within x new random numbers
#define RND_CHK(x) if ((x) + (g.r - g.rnd) >= NRND) {sfmt_fill_array32(&g.sfmt, g.rnd, NRND); g.r = g.rnd;}

// auxiliary macro
#define SQ(x) ((x) * (x))

typedef struct GLOBALS {
	// INPUT PARAMETERS
	double beta; // infection rate
	// NETWORK SPECS
	unsigned int n;
	// OTHER GLOBALS
	unsigned int nheap, *heap;
	// OUTBREAK STATS
	unsigned int s;
	double t;
	// FOR RNG
	uint32_t *rnd, *r; // arrays for random numbers
	unsigned int cutoff; // to get the probabilities right . .
	sfmt_t sfmt; // state of random number generator
} GLOBALS;

typedef struct NODE {
	unsigned int deg, *nb; // degree and network neighbors
	unsigned int heap, state;
	double time;
} NODE;

// heap.c
extern void up_heap (unsigned int);
extern void down_heap (unsigned int);
extern void del_root ();

// misc.c
extern void init_rng ();
extern void read_data (FILE *);
extern double exptime (double);
extern unsigned int randint ();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
