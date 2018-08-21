// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include "sir.h"

GLOBALS g;
NODE *n;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this routine does the bookkeeping for an infection event

void infect () {
	unsigned int i, you, me = g.heap[1];
	float t, now = n[me].time;

	del_root();
	n[me].heap = I_OR_R;
	// get the recovery time
	n[me].time += g.rexp[pcg_16()] * g.beta; // bcoz g.rexpr has a / g.beta factor
	if (n[me].time > g.t) g.t = n[me].time;
	g.s++;

	// go through the neighbors of the infected node . .
	for (i = 0; i < n[me].deg; i++) {
		you = n[me].nb[i];
		if (n[you].heap != I_OR_R) { // if you is S, you can be infected
			t = now + g.rexp[pcg_16()]; // get the infection time

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
	unsigned int i, source;
	
	g.t = 0.0;
	g.s = 0;
	
	// initialize
	for (i = 0; i < g.n; i++) {
		n[i].heap = NONE;
		n[i].time = DBL_MAX; // to a large value
	}

	// get & infect the source
	source = pcg_32_bounded(g.n);
	n[source].time = 0.0;
	n[source].heap = 1;
	g.heap[g.nheap = 1] = source;

	// run the outbreak
	while (g.nheap) infect();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main function handling input

int main (int argc, char *argv[]) {
	unsigned int i;
	double st1 = 0.0, st2 = 0.0, ss1 = 0.0, ss2 = 0.0; // for averages
	FILE *fp;
#ifdef TIME
	struct timespec t0, t1;
#endif
	
	// just a help message
	if ((argc < 3) || (argc > 4)) {
		fprintf(stderr, "usage: ./sir [nwk file] [beta] <seed>\n");
		return 1;
	}

	if (argc == 4) g.state = (uint64_t) strtoull(argv[3], NULL, 10);
	else pcg_init();
	
	// initialize parameters
	g.beta = atof(argv[2]);
	 
	// read network data file
	fp = fopen(argv[1], "r");
	if (!fp) {
		fprintf(stderr, "can't open '%s'\n", argv[1]);
		return 1;
	}
	read_data(fp);
	fclose(fp);

	// allocating the heap (N + 1) because it's indices are 1,...,N
	g.heap = malloc((g.n + 1) * sizeof(unsigned int));

	for (i = 0; i < 0x10000; i++)
		g.rexp[i] = -log((i + 1) / (double) 0x10000) / g.beta;

#ifdef TIME
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
#endif

	// run the simulations and summing for averages
	for (i = 0; i < NAVG; i++) {
		sir();
		// saving stats for averages
		ss1 += (double) g.s;
		ss2 += SQ((double) g.s);
		st1 += g.t;
		st2 += SQ(g.t);
	}

#ifdef TIME
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
#endif

	// make averages
	ss1 /= NAVG;
	ss2 /= NAVG;
	st1 /= NAVG;
	st2 /= NAVG;

	// print result
	printf("avg. outbreak size: %g (%g)\n", ss1, sqrt((ss2 - SQ(ss1)) / (NAVG - 1)));
	printf("avg. time to extinction: %g (%g)\n", st1, sqrt((st2 - SQ(st1)) / (NAVG - 1)));
#ifdef TIME
	printf("\ntime per outbreak (s): %g\n", ((t1.tv_sec - t0.tv_sec) + 1.0e-9 * (t1.tv_nsec - t0.tv_nsec)) / NAVG);
#endif

	// cleaning up
	for (i = 0; i < g.n; i++) free(n[i].nb);
	free(n); free(g.heap);
	 
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
