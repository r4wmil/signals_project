#define CONV_IMPLEMENTATION
#include "conv.h"
#undef CONV_IMPLEMENTATION

#include "stdio.h"
#include "stdint.h"
#include "assert.h"

//size_t c;   // constraint length
//size_t k;   // input bits
//size_t n;   // output bits
//size_t is;  // number of input symbols
//size_t os;  // number of output symbols
//size_t s;   // number of states
//uint64_t N; // next states
//uint64_t O; // outputs

void ut_trel(size_t c, uint32_t* gen, size_t len,
	size_t k, size_t n,
	size_t is, size_t os, size_t s,
	uint64_t* N,
	uint64_t* O) {
	conv_trel_t t = conv_trel_gen(c, gen, len);
	assert(t.c == c);
	assert(t.k == k);
	assert(t.is == is);
	assert(t.os == os);
	assert(t.s == s);
	assert(t.N);
	assert(t.O);
	for (size_t i = 0; i < t.s * t.is; i++) { assert(N[i] == t.N[i]); }
	for (size_t i = 0; i < t.s * t.is; i++) { assert(O[i] == t.O[i]); }
}

void ut_enc(size_t c, uint32_t* gen, size_t len,
	size_t k, size_t n,
	size_t is, size_t os, size_t s,
	uint64_t* N,
	uint64_t* O,
	bool* inp, size_t ilen,
	bool* exp, size_t iexp) {
	conv_trel_t t = conv_trel_gen(c, gen, len);
	assert(t.c == c);
	assert(t.k == k);
	assert(t.is == is);
	assert(t.os == os);
	assert(t.s == s);
	assert(t.N);
	assert(t.O);
	for (size_t i = 0; i < t.s * t.is; i++) { assert(N[i] == t.N[i]); }
	for (size_t i = 0; i < t.s * t.is; i++) { assert(O[i] == t.O[i]); }
	size_t iout = conv_get_enc_len(t, ilen);
	assert(iout == iexp);
}

int main(int argc, char** argv) {
	ut_trel(3, (uint32_t[]){0x5,0x7}, 2,
		1, 2,
		2, 4, 4,
		(uint64_t[]){0,2,0,2,1,3,1,3},
		(uint64_t[]){0,3,3,0,1,2,2,1});
	ut_enc(3, (uint32_t[]){0x5,0x7}, 2,
		1, 2,
		2, 4, 4,
		(uint64_t[]){0,2,0,2,1,3,1,3},
		(uint64_t[]){0,3,3,0,1,2,2,1},
		(bool[]){1,1,0,1}, 4,
		(bool[]){1,1, 1,0, 1,0, 0,0}, 8);
	return 0;
}
