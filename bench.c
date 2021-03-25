#include "dynarr.h"
#include <stdio.h>
#include <time.h>

int* grow(int size) {
	int *a = da_init();
	//static volatile int b[1000000];
	for (int i = 0; i < size; ++i)
		da_push(&a, i);
		//b[i] = i;
	return a;
}

double bench(int reps) {
	// `<=` is intentional
	const int ireps = 10000;
	double total = 0.0;
	for (int i = 0; i <= reps; ++i) {
		struct timespec tic, toc;
		clock_gettime(CLOCK_MONOTONIC, &tic);
		int *p = grow(ireps);
		clock_gettime(CLOCK_MONOTONIC, &toc);
		da_free(&p);
		double elapsed = (toc.tv_sec - tic.tv_sec) + 1e-9 * (toc.tv_nsec - tic.tv_nsec);
		if (i > 0) total += elapsed;
	}
	return total / reps / ireps;
}

int main() {
	printf("%g nsec per da_push()\n", 1e9 * bench(100000));
	return 0;
}
