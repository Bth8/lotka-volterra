#ifndef PREDPREY_H
#define PREDPREY_H

typedef enum {
	EMPTY,
	PREDATOR,
	PREY
} occupant_t;

typedef struct {
	occupant_t *lattice;
	float lambda;
	float mu;
	float sigma;
	int size;
	int time;
	int maxtime;
} lattice_t;

void lattice_init(lattice_t *lattice);
void lattice_update(lattice_t *lattice);
occupant_t get_cell(lattice_t *lattice, int x, int y);
float predator_density(lattice_t *lattice);
float prey_density(lattice_t *lattice);
void lattice_teardown(lattice_t *lattice);

void output_init(lattice_t *lattice);
void output(lattice_t *lattice);
void output_teardown(lattice_t *lattice);

void fail(char *msg);

#endif /* PREDPREY_H */