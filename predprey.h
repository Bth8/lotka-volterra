#ifndef PREDPREY_H
#define PREDPREY_H

#include <stdint.h>

typedef enum {
	EMPTY,
	PREDATOR,
	PREY
} occupant_t;

typedef struct {
	occupant_t *lattice;
	double lambda;
	double mu;
	double sigma;
	unsigned int size;
	unsigned int time;
	unsigned int maxtime;
} lattice_t;

void lattice_init(lattice_t *lattice, double mu, double lambda, double sigma,
	double rho_pred, double rho_prey, unsigned int size, unsigned int maxtime);
void lattice_update(lattice_t *lattice);
occupant_t get_cell(lattice_t *lattice, int x, int y);
double predator_density(lattice_t *lattice);
double prey_density(lattice_t *lattice);
void lattice_teardown(lattice_t *lattice);

void output_init(lattice_t *lattice, int quiet, unsigned int steps_per_report,
	char *path, char *outfile, unsigned int steps_per_frame);
void output(lattice_t *lattice);
void output_teardown(lattice_t *lattice);

void seed_rand(void);
uint64_t next_rand(void);

void fail(char *msg);

#endif /* PREDPREY_H */
