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
	t.N = malloc(t.s * t.is * sizeof(*t.N));
	t.O = malloc(t.s * t.is * sizeof(*t.O));
	for (size_t s = 0; s < t.s; s++) {
		for (size_t i = 0; i < t.is; i++) {
			t.N[s * t.is + i] = (s >> t.k) | (i << ((t.c - 2) * t.k));
			t.O[s * t.is + i] = 0;
			//for (size_t g = 0; g < len; g++) {
			//	t.O[s * t.is + i] = G[g];
			//}
		}
	}
	return t;
}

#endif /* CONV_IMPLEMENTATION */
