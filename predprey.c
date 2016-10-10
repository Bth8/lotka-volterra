#include <stdio.h>
#include <stdlib.h>

#include "predprey.h"

void fail(char *msg) {
	if (msg != NULL)
		fprintf(stderr, "Error %s\n", msg);

	exit(-1);
}

int main() {
	lattice_t *lattice = (lattice_t *)malloc(sizeof(lattice_t));
	if (lattice == NULL)
		fail("main(): malloc() failed");

	lattice_init(lattice);
	output_init(lattice);

	for(lattice->time = 0; lattice->time < lattice->maxtime; lattice->time++) {
		output(lattice);
		lattice_update(lattice);
	}

	output_teardown(lattice);
	lattice_teardown(lattice);
}