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

// HERE

#endif /* CONV_IMPLEMENTATION */
