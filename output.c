#include <stdio.h>

#include "predprey.h"

#define OUTPUT_INTERVAL 25

void output_init(lattice_t *lattice) {
	// do nothing
}

void output(lattice_t *lattice) {
	if (lattice->time % OUTPUT_INTERVAL == 0) {
		printf("%04f\t%04f\n", predator_density(lattice), prey_density(lattice));
	}
}

void output_teardown(lattice_t *lattice) {
	// do nothing
}