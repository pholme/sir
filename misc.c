// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

// miscellaneous routines for sir

#include "sir.h"

extern NODE *n;
extern GLOBALS g;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// initializing the Mersenne Twister (if its state can't be read from file)

void init_rng () {
	unsigned int i;
	struct timeval t;
	
	// seed it with numbers from the clock (current seconds + microseconds)
	gettimeofday(&t, NULL);
	sfmt_init_gen_rand(&g.sfmt, t.tv_sec + t.tv_usec);
	
	// this is to thermalize the Mersenne Twister . . if seeded by a number
	// it doesn't produce high quality random numbers for a while . .
	for (i = 0; i < NREV; i++) sfmt_fill_array32(&g.sfmt, g.rnd, NRND);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// giving exponential random numbers with mean 'rate'

double exptime (double rate) {
	uint32_t i = 0;

	do {
		RND_CHK(1);
		i = *(g.r++);
	} while (i < 1);

	return fabs(log(sfmt_to_real1(i)) / rate);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// get random int in the range 0 to N

unsigned int randint () {
	unsigned int i;

	do {
		RND_CHK(1);
		i = *(g.r++);
	} while (i > g.cutoff);

	return i % g.n; // you can't just do this :)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// reads the network, assumes an edge list with vertex label 0,N-1
// if your network has nodes with degree zero, make sure that none of them is
// the node with largest index

void read_data (FILE *fp) {
	unsigned int i, me, you;

	g.n = 0;

	// scan the system size
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		if (g.n < me) g.n = me;
		if (g.n < you) g.n = you;
	}

	g.n++;

	n = calloc(g.n, sizeof(NODE));

	rewind(fp);

	// scan the degrees
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		n[me].deg++;
		n[you].deg++;
	}

	// allocate adjacency lists
	for (i = 0; i < g.n; i++) {
		n[i].nb = malloc(n[i].deg * sizeof(unsigned int));
		n[i].deg = 0;
	}

	rewind(fp);

	// fill adjacency lists
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		n[me].nb[n[me].deg++] = you;
		n[you].nb[n[you].deg++] = me;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

