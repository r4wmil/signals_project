#ifndef CONV_H_
#define CONV_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct conv_trel_t {
	size_t c;   // constraint length
	size_t k;   // input bits
	size_t n;   // output bits
	size_t is;  // number of input symbols
	size_t os;  // number of output symbols
	size_t s;   // number of states
	uint64_t* N; // next states
	uint64_t* O; // outputs
} conv_trel_t;

conv_trel_t conv_trel_gen(size_t c, uint32_t* G, size_t n);
size_t conv_get_enc_len(conv_trel_t trel, size_t ilen);
size_t conv_enc(conv_trel_t trel, bool* inp, size_t ilen, size_t olen, bool* out);
void conv_hard(conv_trel_t trel, bool* inp, size_t ilen, bool* out);

#endif /* CONV_H_ */

#ifdef CONV_IMPLEMENTATION

conv_trel_t conv_trel_gen(size_t c, uint32_t* G, size_t n) {
	conv_trel_t t = {0};
	t.c = c;
	t.k = 1; // TODO: 2D
	t.n = n;
	t.is = 0x1 << t.k;
	t.os = 0x1 << t.n;
	t.s = 0x1 << ((t.c - 1) * t.k);
	t.N = calloc(t.s * t.is, sizeof(*t.N));
	t.O = calloc(t.s * t.is, sizeof(*t.O));
	for (size_t s = 0; s < t.s; s++) {
		for (size_t i = 0; i < t.is; i++) {
			t.N[s * t.is + i] = (s >> t.k) | (i << ((t.c - 2) * t.k));
			uint64_t r = s | (i << ((t.c - 2) * t.k + 1));
			for (size_t g = 0; g < n; g++) {
				t.O[s * t.is + i] <<= 1;
				t.O[s * t.is + i] |= __builtin_parity(r & G[g]);
			}
		}
	}
	return t;
}

size_t conv_get_enc_len(conv_trel_t trel, size_t ilen) {
	return ilen * trel.n;
}

size_t conv_enc(conv_trel_t trel, bool* inp, size_t ilen, size_t olen, bool* out) {
	// TODO: remove olen & return nothing
	size_t state = 0;
	size_t outi = 0;
	for (size_t i = 0; i < ilen; i++) {
		size_t tbli = state * trel.is + inp[i];
		uint64_t tblo = trel.O[tbli];
		for (size_t b = 0; b < trel.n; b++) {
			out[outi++] = (tblo >> (trel.n - b - 1)) & 0x1;
		}
		state = trel.N[tbli];
	}
	return outi;
}

void conv_hard(conv_trel_t trel, bool* inp, size_t ilen, bool* out) {
	size_t steps = ilen / trel.n;
	size_t* path = malloc(trel.s * steps * sizeof(*path));
	uint32_t* metrics = malloc(trel.s * sizeof(*metrics));
	uint32_t* next_metrics = malloc(trel.s * sizeof(*next_metrics));

	for (size_t s = 1; s < trel.s; s++) metrics[s] = 1e9;
	metrics[0] = 0;

	for (size_t t = 0; t < steps; t++) {
		for (size_t s = 0; s < trel.s; s++) next_metrics[s] = 1e9;

		for (size_t s = 0; s < trel.s; s++) {
			if (metrics[s] >= 1e9) continue;
			for (size_t i = 0; i < trel.is; i++) {
				size_t idx = s * trel.is + i;
				size_t next_s = trel.N[idx];
				uint64_t tblo = trel.O[idx];

				uint32_t dist = 0;
				for (size_t b = 0; b < trel.n; b++) {
					bool bit = (tblo >> (trel.n - b - 1)) & 0x1;
					dist += (bit != inp[t * trel.n + b]);
				}

				uint32_t cost = metrics[s] + dist;
				if (cost < next_metrics[next_s]) {
					next_metrics[next_s] = cost;
					path[t * trel.s + next_s] = s;
				}
			}
		}
		uint32_t* tmp = metrics; metrics = next_metrics; next_metrics = tmp;
	}

	size_t best_s = 0;
	for (size_t s = 1; s < trel.s; s++) {
		if (metrics[s] < metrics[best_s]) best_s = s;
	}

	for (size_t t = steps; t > 0; t--) {
		size_t prev_s = path[(t - 1) * trel.s + best_s];
		for (size_t i = 0; i < trel.is; i++) {
			if (trel.N[prev_s * trel.is + i] == best_s) {
				out[t - 1] = i;
				break;
			}
		}
		best_s = prev_s;
	}

	free(path); free(metrics); free(next_metrics);
}

#endif /* CONV_IMPLEMENTATION */
