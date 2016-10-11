#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "predprey.h"

uint64_t s[2];

void seed_rand(void) {
	FILE *f = fopen("/dev/urandom", "r");
	if (f == NULL) {
		perror("seed_rand()");
		fail(NULL);
	}

	if (fread(s, sizeof(uint64_t), 2, f) < 2) {
		fail("seed_rand(): fread() failed");
	}

	fclose(f);
}

/* Implementation of xorshift128+ as detailed in 
 * http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf
 */
uint64_t next_rand(void) {
	uint64_t s1 = s[0];
	const uint64_t s0 = s[1];
	const uint64_t result = s0 + s1;

	s[0] = s0;
	s1 ^= s1 << 23;
	s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

	return result;
}