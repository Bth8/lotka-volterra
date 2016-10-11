#include <stdlib.h>
#include <stdio.h>

#include "predprey.h"

#define MOD(a, b) ((((a) % (b)) + (b)) % (b))

#define MU 0.025
#define LAMBDA 0.25
#define SIGMA 1.0
#define RHO_PREY 0.3
#define RHO_PRED 0.3
#define SIZE 512
#define MAXTIME 5000

typedef struct {
	int x;
	int y;
} cell_t;

static inline occupant_t _get_cell(lattice_t *lattice, cell_t *cell) {
	return lattice->lattice[cell->y * lattice->size + cell->x];
}

occupant_t get_cell(lattice_t *lattice, int x, int y) {
	if (lattice == NULL)
		fail("get_cell(): lattice cannot be NULL");

	if (x < 0 || x > lattice->size || y < 0 || y > lattice->size)
		fail("get_cell(): invalid cell specified");

	cell_t cell = {x, y};
	return _get_cell(lattice, &cell);
}

static void set_cell(
		lattice_t *lattice,
		cell_t *cell,
		occupant_t occupant) {

	lattice->lattice[cell->y * lattice->size + cell->x] = occupant;
}

void lattice_init(lattice_t *lattice) {
	if (lattice == NULL)
		fail("lattice_init(): lattice cannot be NULL");

	lattice->size = SIZE;

	lattice->time = 0;
	lattice->maxtime = MAXTIME;

	lattice->lambda = LAMBDA;
	lattice->mu = MU;
	lattice->sigma = SIGMA;

	lattice->lattice = (occupant_t *)malloc(sizeof(occupant_t) * 
			lattice->size * lattice->size);
	if (lattice->lattice == NULL)
		fail("lattice_init(): malloc() failed");

	cell_t cell;
	for (cell.y = 0; cell.y < lattice->size; cell.y++) {
		for (cell.x = 0; cell.x < lattice->size; cell.x++) {
			uint64_t r = next_rand();
			if (r < RHO_PRED * UINT64_MAX) {
				set_cell(lattice, &cell, PREDATOR);
			} else if (r < (RHO_PRED + RHO_PREY) * UINT64_MAX) {
				set_cell(lattice, &cell, PREY);
			} else {
				set_cell(lattice, &cell, EMPTY);
			}
		}
	}
}

double predator_density(lattice_t *lattice) {
	if (lattice == NULL)
		fail("predator_density(): lattice cannot be NULL");

	int predators = 0;
	cell_t cell;
	for (cell.y = 0; cell.y < lattice->size; cell.y++) {
		for (cell.x = 0; cell.x < lattice->size; cell.x++) {
			if (_get_cell(lattice, &cell) == PREDATOR)
				predators++;
		}
	}

	return (double)predators / lattice->size / lattice->size;
}

double prey_density(lattice_t *lattice) {
	if (lattice == NULL)
		fail("prey_density(): lattice cannot be NULL");

	int prey = 0;
	cell_t cell;
	for (cell.y = 0; cell.y < lattice->size; cell.y++) {
		for (cell.x = 0; cell.x < lattice->size; cell.x++) {
			if (_get_cell(lattice, &cell) == PREY)
				prey++;
		}
	}

	return (double)prey / lattice->size / lattice->size;
}

void lattice_teardown(lattice_t *lattice) {
	if (lattice != NULL) {
		free(lattice->lattice);
	}
}

static void assign_random_cell(lattice_t *lattice, cell_t *cell) {
	cell->x = next_rand() % lattice->size;
	cell->y = next_rand() % lattice->size;
}

static void assign_random_neighbor(
		lattice_t *lattice,
		cell_t *cell,
		cell_t *neighbor) {

	int r = next_rand() % 4;

	switch(r) {
		case 0:
			neighbor->x = cell->x;
			neighbor->y = MOD(cell->y + 1, lattice->size);
			break;
		case 1:
			neighbor->x = cell->x;
			neighbor->y = MOD(cell->y - 1, lattice->size);
			break;
		case 2:
			neighbor->x = MOD(cell->x + 1, lattice->size);
			neighbor->y = cell->y;
			break;
		case 3:
			neighbor->x = MOD(cell->x - 1, lattice->size);
			neighbor->y = cell->y;
			break;
	}
}

static void die(lattice_t *lattice, cell_t *cell) {
	if (_get_cell(lattice, cell) == PREDATOR && 
			next_rand() < lattice->mu * UINT64_MAX)
		set_cell(lattice, cell, EMPTY);
}

static void eat(lattice_t *lattice, cell_t *cell) {
	if (_get_cell(lattice, cell) == PREDATOR) {
		cell_t neighbor;
		assign_random_neighbor(lattice, cell, &neighbor);
		if (_get_cell(lattice, &neighbor) == PREY &&
				next_rand() < lattice->lambda * UINT64_MAX)
			set_cell(lattice, &neighbor, PREDATOR);
	}
}

static void reproduce(lattice_t *lattice, cell_t *cell) {
	if (_get_cell(lattice, cell) == PREY) {
		cell_t neighbor;
		assign_random_neighbor(lattice, cell, &neighbor);
		if (_get_cell(lattice, &neighbor) == EMPTY &&
				next_rand() < lattice->sigma * UINT64_MAX)
			set_cell(lattice, &neighbor, PREY);
	}
}

void lattice_update(lattice_t *lattice) {
	if (lattice == NULL)
		fail("lattice_update(): lattice cannot be NULL");

	for (int i = 0; i < lattice->size * lattice->size; i++) {
		cell_t cell;
		assign_random_cell(lattice, &cell);

		int r = next_rand() % 4;
		switch(r) {
			case 0:
				die(lattice, &cell);
				break;
			case 1:
				eat(lattice, &cell);
				break;
			case 2:
				reproduce(lattice, &cell);
				break;
			case 3:
				// do nothing
				break;
		}
	}
}