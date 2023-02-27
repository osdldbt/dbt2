/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>

#include "pcg_variants.h"
#include "entropy.h"

int main(int argc, char *argv[])
{
	unsigned long long seed = 1;
	pcg64f_random_t rng;
	int64_t min, max;
	int p;

	if (argc != 4) {
		printf("usage: %s <min> <max> <precision>\n", argv[0]);
		return 1;
	}

	entropy_getbytes((void *) &seed, sizeof(seed));
	pcg64f_srandom_r(&rng, seed);

	sscanf(argv[1], "%" SCNd64, &min);
	sscanf(argv[2], "%" SCNd64, &max);
	p = atoi(argv[3]);

	if (p) {
		int i;
		double r;
		char fmt[16];
		snprintf(fmt, 15, "%%0.%df\n", p);
		for (i = 0; i < p; i++) {
			min *= 10.0;
			max *= 10.0;
		}
		r = (double) (min + ((max - min + 1) * (pcg64f_random_r(&rng) >> 11) *
				(1.0 / 9007199254740992.0)));
		for (i = 0; i < p; i++) {
			r /= 10.0;
		}
		printf(fmt, r);
	} else {
		int64_t r;
		r = min + (int64_t) ((max - min + 1) * (pcg64f_random_r(&rng) >> 11) *
				(1.0 / 9007199254740992.0));
		printf("%" SCNd64 "\n", r);
	}

	return 0;
}
