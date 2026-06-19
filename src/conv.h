#ifndef CONV_H_
#define CONV_H_

#include "stdint.h"
#include "stdlib.h"

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

conv_trel_t conv_trel_gen(size_t c, uint32_t* gen, size_t len) {
	conv_trel_t t = {0};
	t.c = c;
	t.k = 1;
	t.n = len;
	t.is = 0x1 << t.k;
	t.os = 0x1 << t.n;
	t.s = 0x1 << ((c - 1) * t.k);
	t.N = malloc(t.s * (0x1 << t.k));
	t.O = malloc(t.s * (0x1 << t.k));
	return t;
}

#endif /* CONV_IMPLEMENTATION */
