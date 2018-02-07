// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include "sir.h"

GLOBALS g;
NODE *n;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this routine does the bookkeeping for an infection event

void infect () {
	unsigned int i, you, me = g.heap[1];
	double t, now = n[me].time;

	n[me].state = INFECTED;

	n[me].time += exptime(1.0);
	down_heap(1); // this will re-heapify because only the root and it's children can violate the heap property

	// go through the neighbors of the infected node . .
	for (i = 0; i < n[me].deg; i++) {
		you = n[me].nb[i];
		if (S(you)) { // if you is S, you can be infected
			t = now + exptime(g.beta); // get the infection time
			// if the infection time is before when me gets infected,
			// and (if it was already listed for infection) before the
			// previously listed infection event, then list it
			if ((t < n[me].time) && (t < n[you].time)) {
				n[you].time = t;
				if (n[you].heap == NONE) { // if not listed before, then extend the heap
					g.heap[++g.nheap] = you;
					n[you].heap = g.nheap;
				}
				up_heap(n[you].heap); // this works bcoz the only heap relationship that can be violated is the one between you and its parent
			}
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this routine runs one SIR outbreak from a random seed node

void sir () {
	unsigned int i, me, source;
	
	g.t = 0.0;
	g.s = 0;
	
	// initialize
	for (i = 0; i < g.n; i++) {
		n[i].state = SUSCEPTIBLE;
		n[i].heap = NONE;
		n[i].time = DBL_MAX; // to a large value
	}

	// get & infect the source
	RND_CHK(1);
	source = *(g.r++) % g.n;
	n[source].time = 0.0;
	n[source].heap = 1;
	g.heap[g.nheap = 1] = source;

	// run the outbreak
	while (g.nheap) {
		// get the node involved in the next event
		me = g.heap[1];
		if (I(me)) { // is it a recovery event?
			n[me].state = RECOVERED;
			g.t = n[me].time; // to get the extinction time
			del_root();
		} else { // . . or an infection? (note that me cannot be R)
			infect();
			g.s++; // to get the outbreak size
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main function handling input

int main (int argc, char *argv[]) {
	unsigned int i;
	double t1 = 0.0, t2 = 0.0, s1 = 0.0, s2 = 0.0; // for averages
	FILE *fp;
	
	// just a help message
	if (argc != 4) {
		fprintf(stderr, "usage: sir [nwk file] [r.n.g. state file] [beta]\n");
		return 1;
	}
	
	// initialize parameters
	g.beta = atof(argv[3]);
	 
	// read network data file
	fp = fopen(argv[1], "r");
	if (!fp) {
		fprintf(stderr, "can't open '%s'\n", argv[1]);
		return 1;
	}
	read_data(fp);
	fclose(fp);

	// read state or initialize RNG, start - - - - - - - - - - - - - - - - - - - - - - - -
	if (sfmt_get_min_array_size32(&g.sfmt) > NRND) {
		fprintf(stderr, "can't initialize the r.n.g.\n");
		return 1;
	}
	
	// an array for random numbers (slightly faster than generating them one by one)
	g.rnd = malloc(NRND * sizeof(uint32_t));
	
	// check if I can read the RNG state from a file
	fp = fopen(argv[2], "rb");
	if (fp) {
		if (1 != fread(&g.sfmt, sizeof(sfmt_t), 1, fp)) {
			fclose(fp);
			init_rng(); // if reading fails, then re-initialize it
		}
		fclose(fp);
	} else init_rng(); // initialize from the clock
	sfmt_fill_array32(&g.sfmt, g.rnd, NRND);
	g.r = g.rnd;

	g.cutoff = (4294967296 / g.n) * g.n; // to get the epidemic seeds with equal probability
	// read state or initialize RNG, stop  - - - - - - - - - - - - - - - - - - - - - - - -

	// allocating the heap (N + 1) because it's indices are 1,...,N
	g.heap = malloc((g.n + 1) * sizeof(unsigned int));
	
	// run the simulations and summing for averages
	for (i = 0; i < NAVG; i++) {
		sir();
		// saving stats for averages
		s1 += (double) g.s;
		s2 += SQ((double) g.s);
		t1 += g.t;
		t2 += SQ(g.t);
	}

	// make averages
	s1 /= NAVG;
	s2 /= NAVG;
	t1 /= NAVG;
	t2 /= NAVG;

	// print result
	printf("avg. outbreak size: %g (%g)\n", s1, sqrt((s2 - SQ(s1)) / (NAVG - 1)));
	printf("avg. time to extinction: %g (%g)\n", t1, sqrt((t2 - SQ(t1)) / (NAVG - 1)));

	// save the state of the random number generator
	fp = fopen(argv[2], "wb");
	fwrite(&g.sfmt, sizeof(sfmt_t), 1, fp);
	fclose(fp);
	
	// cleaning up
	for (i = 0; i < g.n; i++) free(n[i].nb);
	free(g.rnd); free(n); free(g.heap);
	 
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
