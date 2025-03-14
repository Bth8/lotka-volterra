#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <math.h>

#include "predprey.h"

// Program usage docstring
static char doc[] = "Simulates a stochastic Lotka-Volterra model on a 2D "
	"lattice according to the dynamics specified in https://arxiv.org/abs/1511.05114 "
	"and outputs an animated gif of the results to FILE";

// Arguments we accept
static char args_doc[] = "FILE";

static struct argp_option options[] = {
	{"quiet",		'q', 0,			0, "Don't output to stdout", 0},
	{"mu",			'm', "RATE",	0, "Predator extinction rate (default 0.025)", 1},
	{"lambda",		'l', "RATE",	0, "Predator predation/reproduction rate (default 0.25)"},
	{"sigma",		's', "RATE",	0, "Prey reproduction rate (default 1.0)"},
	{"initprey",	'i', "DENSITY",	0, "Initial prey density (default 0.3)", 2},
	{"initpred",	'I', "DENSITY",	0, "Initial predator density (default 0.3)"},
	{"size",		'S', "SIZE",	0, "Lattice sites per dimension (default 512)", 3},
	{"duration",	'd', "STEPS",	0, "Total simulation duration (default 3000 steps)", 4},
	{"rrate",		'r', "STEPS",	0, "Number of time steps per report to stdout (default 25, 0 for no reports)"},
	{"ffreq",		'f', "STEPS",	0, "Number of time steps per animation frame (default 50)"},

	{0}
};

// Struct to keep track of values set by command line arguments
struct arguments {
	char *outfile;					// image file to which we ultimately output
	int quiet;						// -q
	double lambda, mu, sigma;		// reaction rates
	double rho_pred, rho_prey;		// initial species densities
	unsigned int size;				// lattice size per dimension
	unsigned int maxtime;			// duration of the simulation
	unsigned int steps_per_report;	// time steps per report to stdout
	unsigned int steps_per_frame;	// time steps per animation frame
};

// Parse a single option
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = (struct arguments *)state->input;
	char *end; // For string conversion error checking

	switch (key) {
		case 'q':
			arguments->quiet = 1;
			break;

		case 'm':
			arguments->mu = strtod(arg, &end);
			if (end == arg || *end != '\0' || !isfinite(arguments->mu) || arguments->mu < 0 || arguments->mu > 1)
				argp_error(state, "Invalid predator extinction rate: %s", arg);
			break;

		case 'l':
			arguments->lambda = strtod(arg, &end);
			if (end == arg || *end != '\0' || !isfinite(arguments->lambda) || arguments->lambda < 0 || arguments->lambda > 1)
				argp_error(state, "Invalid predation rate: %s", arg);
			break;

		case 's':
			arguments->sigma = strtod(arg, &end);
			if (end == arg || *end != '\0' || !isfinite(arguments->sigma) || arguments->sigma < 0 || arguments->sigma > 1)
				argp_error(state, "Invalid prey reproduction rate: %s", arg);
			break;

		case 'i':
			arguments->rho_prey = strtod(arg, &end);
			if (end == arg || *end != '\0' || !isfinite(arguments->rho_prey) || arguments->rho_prey < 0 || arguments->rho_prey > 1)
				argp_error(state, "Invalid initial prey density: %s", arg);
			break;

		case 'I':
			arguments->rho_pred = strtod(arg, &end);
			if (end == arg || *end != '\0' || !isfinite(arguments->rho_pred) || arguments->rho_pred < 0 || arguments->rho_pred > 1)
				argp_error(state, "Invalid initial predator density: %s", arg);
			break;

		case 'S':
			arguments->size = strtoul(arg, &end, 0);
			if (end == arg || *end != '\0')
				argp_error(state, "Invalid lattice size: %s", arg);
			break;

		case 'd':
			arguments->maxtime = strtoul(arg, &end, 0);
			if (end == arg || *end != '\0')
				argp_error(state, "Invalid simulation duration: %s", arg);
			break;

		case 'r':
			arguments->steps_per_report = strtoul(arg, &end, 0);
			if (end == arg || *end != '\0')
				argp_error(state, "Invalid report rate: %s", arg);
			break;

		case 'f':
			arguments->steps_per_frame = strtoul(arg, &end, 0);
			if (end == arg || *end != '\0' || arguments->steps_per_frame == 0)
				argp_error(state, "Invalid frame rate: %s", arg);
			break;

		case ARGP_KEY_ARG:
			// We should only ever have one non-option argument - the outfile
			if (state->arg_num != 0)
				return ARGP_ERR_UNKNOWN;
			arguments->outfile = arg;
			break;

		case ARGP_KEY_NO_ARGS:
			argp_usage(state);

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

void fail(char *msg) {
	if (msg != NULL)
		fprintf(stderr, "Error %s\n", msg);

	exit(-1);
}

int main(int argc, char **argv) {
	struct arguments arguments;

	// Default arguments
	arguments.outfile = "-";
	arguments.quiet = 0;
	arguments.lambda = 0.25;
	arguments.mu = 0.025;
	arguments.sigma = 1.0;
	arguments.rho_pred = 0.3;
	arguments.rho_prey = 0.3;
	arguments.size = 512;
	arguments.maxtime = 3000;
	arguments.steps_per_report = 25;
	arguments.steps_per_frame = 50;

	// Parse command line arguments
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	lattice_t *lattice = (lattice_t *)malloc(sizeof(lattice_t));
	if (lattice == NULL)
		fail("main(): malloc() failed");

	seed_rand();

	lattice_init(lattice, arguments.mu, arguments.lambda, arguments.sigma,
		arguments.rho_pred, arguments.rho_prey, arguments.size,
		arguments.maxtime);
	output_init(lattice, arguments.quiet, arguments.steps_per_report,
		*argv, arguments.outfile, arguments.steps_per_frame);

	for(lattice->time = 0; lattice->time < lattice->maxtime; lattice->time++) {
		output(lattice);
		lattice_update(lattice);
	}

	output_teardown(lattice);
	lattice_teardown(lattice);

	return 0;
}
